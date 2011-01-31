/*
 *
 *    standMelCepstrum.cpp
 *                              (c) HAL 2010-           
 *
 *  This file is a part of STAND Library.
 * standMelCepstrum is a structure that contains
 * frequency transform functions and spectral filter.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#include "standMelCepstrum.h"

double  getMelScale(double freq){
    double ret = 1127.01048 * log(1 + freq / 700);
    return ret;
}

double  getFrequency(double mel){
    double ret = 700 * (exp(mel / 1127.01048) - 1);
    return ret;
}


standMelCepstrum::standMelCepstrum()
{
    this->f0 = NULL;
    this->cepstrumLength = 0;
    this->cepstrumNumber = 0;
    this->melCepstrum = NULL;
    this->framePeriod = framePeriod;
}

standMelCepstrum::~standMelCepstrum()
{
    this->destroy();
}

void standMelCepstrum::destroy()
{
    this->cepstrumLength = 0;
    if(this->melCepstrum){
        for(int i = 0; i < this->cepstrumNumber; i++){
            SAFE_DELETE_ARRAY(this->melCepstrum[i]);
        }
        SAFE_DELETE_ARRAY(this->melCepstrum);
    }
    this->cepstrumNumber = 0;
    this->framePeriod = framePeriod;
    SAFE_DELETE_ARRAY(this->f0);
}

bool standMelCepstrum::writeMelCepstrum(string_t output)
{
    if(!this->melCepstrum) return false;

    string output_s;
    mb_conv(output, output_s);

    bool ret = false;
    FILE *fp = fopen(output_s.c_str(), "wb");
    if(fp){
        fwrite(&this->framePeriod, sizeof(float), 1, fp);
        fwrite(&this->cepstrumNumber, sizeof(int), 1, fp);
        fwrite(&this->cepstrumLength, sizeof(int), 1, fp);
        for(int j = 0; j < this->cepstrumNumber; j++){
            fwrite(&this->f0[j], sizeof(float), 1, fp);
            for(int i = 0; i < this->cepstrumLength; i++){
                fwrite(&this->melCepstrum[j][i].re, sizeof(float), 1, fp);
                fwrite(&this->melCepstrum[j][i].im, sizeof(float), 1, fp);
            }
        }
        ret = true;
    }
    return ret;
}

bool standMelCepstrum::readMelCepstrum(string_t input)
{
    bool ret = false;
    string input_s;
    mb_conv(input, input_s);
    FILE *fp = fopen(input_s.c_str(), "rb");
    if(fp){
        size_t c = 0;
        this->destroy();
        c += fread(&this->framePeriod, sizeof(int), 1, fp);
        c += fread(&this->cepstrumNumber, sizeof(int), 1, fp);
        c += fread(&this->cepstrumLength, sizeof(int), 1, fp);
        this->f0 = new float[cepstrumNumber];
        this->melCepstrum = new standComplex*[this->cepstrumNumber];
        for(int j = 0; j < this->cepstrumNumber; j++){
            this->melCepstrum[j] = new standComplex[this->cepstrumLength];
            c += fread(&this->f0[j], sizeof(float), 1, fp);
            for(int i = 0; i < this->cepstrumLength; i++){
                c += fread(&this->melCepstrum[j][i].re, sizeof(float), 1, fp);
                c += fread(&this->melCepstrum[j][i].im, sizeof(float), 1, fp);
            }
        }
        // 読み込んだ個数が正しければ真を返す．
        if(c == 3 + 2 * this->cepstrumLength * this->cepstrumNumber + this->cepstrumNumber){
            ret = true;
        }
    }
    return ret;
}

void stretchToMelScale(fftw_complex *melSpectrum, const double *spectrum, int spectrumLength, int maxFrequency)
{
    double tmp = getMelScale(maxFrequency);
    for(int i = 0; i < spectrumLength; i++)
    {
        double dIndex = getFrequency((double)i / (double)spectrumLength * tmp) / (double)maxFrequency * (double)spectrumLength;
        if(dIndex <= spectrumLength-1.0){
            melSpectrum[i][0] = interpolateArray(dIndex, spectrum);
        }else{
            melSpectrum[i][0] = spectrum[spectrumLength - 1];
        }
    }
}

void stretchFromMelScale(double *spectrum, const fftw_complex *melSpectrum, int spectrumLength, int maxFrequency)
{
    double tmp = getMelScale(maxFrequency);
    double *tempArray = new double[spectrumLength];
    for(int i = 0; i < spectrumLength; i++){
        tempArray[i] = melSpectrum[i][0];
    }
    for(int i = 0; i < spectrumLength; i++)
    {
        double dIndex = getMelScale((double)i / (double)spectrumLength * (double)maxFrequency) / tmp * (double)spectrumLength;
        if(dIndex <= spectrumLength-1.0){
            spectrum[i] = interpolateArray(dIndex, tempArray);
        }else{
            spectrum[i] = melSpectrum[spectrumLength - 1][0];
        }
    }
    delete[] tempArray;
}

void standMelCepstrum::calculateMelCepstrum(int cepstrumLength, const double *f0, double **sourceSpecgram, int spectrumNumber, int spectrumLength, int maxFrequency)
{
    // ケプストラムの最大長はエルミート対称性を考えてスペクトル長の半分を超えない．
    if(!f0 || !sourceSpecgram || cepstrumLength <= 0 || spectrumLength <= 0 || spectrumNumber <= 0 || cepstrumLength > spectrumLength / 2){
        return;
    }
    fftw_complex *melSpectrum = new fftw_complex[spectrumLength];
    fftw_complex *tmpCepstrum = new fftw_complex[spectrumLength];

    if(melSpectrum && tmpCepstrum){
        fftw_plan inverseFFT;
        this->destroy();
        this->cepstrumNumber = spectrumNumber;
        this->melCepstrum = new standComplex*[cepstrumNumber];
        this->f0 = new float[cepstrumNumber];
        for(int i = 0; i < cepstrumNumber; i++){
            this->melCepstrum[i] = new standComplex[cepstrumLength];
        }
        this->cepstrumLength = cepstrumLength;
#ifdef STND_MULTI_THREAD
        if(hFFTWMutex)
            stnd_mutex_lock(hFFTWMutex);
#endif
        inverseFFT = fftw_plan_dft_1d(spectrumLength, melSpectrum, tmpCepstrum, FFTW_BACKWARD, FFTW_ESTIMATE);
#ifdef STND_MULTI_THREAD
        if(hFFTWMutex)
            stnd_mutex_unlock(hFFTWMutex);
#endif
        // メルスケールへ展開
        for(int j = 0; j < spectrumNumber; j++){
            this->f0[j] = f0[j];
            stretchToMelScale(melSpectrum, sourceSpecgram[j], spectrumLength / 2 + 1, maxFrequency / 2);
            // FFTW は N 点の DFT, IDFT 後は N 倍された値が格納されるので先に割ってしまう．
            for(int i = 0; i <= spectrumLength / 2; i++){
                melSpectrum[i][0] = log(melSpectrum[i][0]) / (double)spectrumLength;
                melSpectrum[i][1] = 0.0;
            }
            for(int i = spectrumLength / 2 + 1; i < spectrumLength; i++){
                melSpectrum[i][0] = melSpectrum[spectrumLength-i][0];
                melSpectrum[i][1] = 0.0;
            }
            // log とって IDFT というか DFT．
            fftw_execute(inverseFFT);

            // float に落とすのは単にライブラリとしての容量の問題．
            for(int i = 0; i < cepstrumLength; i++){
                this->melCepstrum[j][i].re = tmpCepstrum[i][0];
                this->melCepstrum[j][i].im = tmpCepstrum[i][1];
            }
        }

#ifdef STND_MULTI_THREAD
        if(hFFTWMutex)
            stnd_mutex_lock(hFFTWMutex);
#endif
        fftw_destroy_plan(inverseFFT);
#ifdef STND_MULTI_THREAD
        if(hFFTWMutex)
            stnd_mutex_unlock(hFFTWMutex);
#endif
    }
    SAFE_DELETE_ARRAY(melSpectrum);
    SAFE_DELETE_ARRAY(tmpCepstrum);
}

standComplex *standMelCepstrum::getMelCepstrum(double msTime, int *length)
{
    standComplex *ret = NULL;
    *length = 0;
    int index = msTime / this->framePeriod;
    if(index < 0) index = 0;
    if(index >= this->cepstrumNumber) index = this->cepstrumNumber - 1;

    if(this->melCepstrum){
        ret = melCepstrum[index];
        *length = this->cepstrumLength;
    }
    return ret;
}

double standMelCepstrum::getF0(double msTime)
{
    int index = msTime / this->framePeriod;
    if(index < 0) index = 0;
    if(index >= this->cepstrumNumber) index = this->cepstrumNumber - 1;
    return this->f0[index];
}
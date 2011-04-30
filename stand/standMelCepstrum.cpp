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
#include "matching.h"

double  standMelCepstrum::getMelScale(double freq){
    double ret = 1127.01048 * log(1 + freq / 700);
    return ret;
}

double  standMelCepstrum::getFrequency(double mel){
    double ret = 700 * (exp(mel / 1127.01048) - 1);
    return ret;
}


standMelCepstrum::standMelCepstrum()
{
    this->noiseRatio = this->t = this->f0 = NULL;
    this->cepstrumLength = 0;
    this->timeLength = 0;
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
        for(int i = 0; i < this->timeLength; i++){
            SAFE_DELETE_ARRAY(this->melCepstrum[i]);
        }
        SAFE_DELETE_ARRAY(this->melCepstrum);
    }
    this->timeLength = 0;
    this->framePeriod = framePeriod;
    SAFE_DELETE_ARRAY(this->f0);
    SAFE_DELETE_ARRAY(this->t);
    SAFE_DELETE_ARRAY(this->noiseRatio);
}

void standMelCepstrum::setTimeLength(int timeLength, int cepstrumLength)
{
    if(timeLength <= 0) return;
    this->destroy();
    this->timeLength = timeLength;
    this->melCepstrum = new standComplex*[timeLength];
    this->f0 = new float[timeLength];
    this->t  = new float[timeLength];
    this->noiseRatio = new float[timeLength];
    for(int i = 0; i < timeLength; i++){
        this->melCepstrum[i] = new standComplex[cepstrumLength];
    }
    this->cepstrumLength = cepstrumLength;
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
        fwrite(&this->timeLength, sizeof(int), 1, fp);
        fwrite(&this->cepstrumLength, sizeof(int), 1, fp);
        fwrite(this->t, sizeof(float), this->timeLength, fp);
        fwrite(this->f0, sizeof(float), this->timeLength, fp);
        fwrite(this->noiseRatio, sizeof(float), this->timeLength, fp);
        for(int j = 0; j < this->timeLength; j++){
            for(int i = 0; i < this->cepstrumLength; i++){
                fwrite(&this->melCepstrum[j][i].re, sizeof(float), 1, fp);
            }
        }
        fclose(fp);
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
        c += fread(&this->timeLength, sizeof(int), 1, fp);
        c += fread(&this->cepstrumLength, sizeof(int), 1, fp);

        this->setTimeLength(this->timeLength, this->cepstrumLength);

        c += fread(this->t, sizeof(float), this->timeLength, fp);
        c += fread(this->f0, sizeof(float), this->timeLength, fp);
        c += fread(this->noiseRatio, sizeof(float), this->timeLength, fp);

        for(int j = 0; j < this->timeLength; j++){
            for(int i = 0; i < this->cepstrumLength; i++){
                c += fread(&this->melCepstrum[j][i].re, sizeof(float), 1, fp);
            }
        }
        // 読み込んだ個数が正しければ真を返す．
        if(c == 3 + this->cepstrumLength * this->timeLength + 2 * this->timeLength){
            ret = true;
        }
        fclose(fp);
    }
    return ret;
}

void standMelCepstrum::stretchToMelScale(double *melSpectrum, const double *spectrum, int spectrumLength, int maxFrequency)
{
    double tmp = getMelScale(maxFrequency);
    for(int i = 0; i < spectrumLength; i++)
    {
        double dIndex = getFrequency((double)i / (double)spectrumLength * tmp) / (double)maxFrequency * (double)spectrumLength;
        if(dIndex <= spectrumLength-1.0){
            melSpectrum[i] = interpolateArray(dIndex, spectrum);
        }else{
            melSpectrum[i] = spectrum[spectrumLength - 1];
        }
    }
}

void standMelCepstrum::stretchFromMelScale(double *spectrum, const double *melSpectrum, int spectrumLength, int maxFrequency)
{
    double tmp = getMelScale(maxFrequency);
    for(int i = 0; i < spectrumLength; i++)
    {
        double dIndex = getMelScale((double)i / (double)spectrumLength * (double)maxFrequency) / tmp * (double)spectrumLength;
        if(dIndex <= spectrumLength-1.0){
            spectrum[i] = interpolateArray(dIndex, melSpectrum);
        }else{
            spectrum[i] = melSpectrum[spectrumLength - 1];
        }
    }
}

void standMelCepstrum::calculateMelCepstrum(int cepstrumLength, const double *t, const double *f0, const double *noiseRatio,
                                            double **sourceSpecgram, double **dstSpecgram,
                                            int timeLength, int spectrumLength, int maxFrequency, double framePeriod)
{
    // ケプストラムの最大長はエルミート対称性を考えてスペクトル長の半分を超えない．
    if(!f0 || !sourceSpecgram || cepstrumLength <= 0 || spectrumLength <= 0 || timeLength <= 0 || cepstrumLength > spectrumLength / 2){
        return;
    }
    double *melSpectrum = new double[spectrumLength];
    double *tmpSpectrum = new double[spectrumLength];
    fftw_complex *tmpCepstrum = new fftw_complex[spectrumLength];
    this->framePeriod = framePeriod;

    if(melSpectrum && tmpCepstrum){
        fftw_plan inverseFFT;

        this->setTimeLength(timeLength, cepstrumLength);

#ifdef STND_MULTI_THREAD
        if(hFFTWMutex){
            stnd_mutex_lock(hFFTWMutex);
        }
#endif
        inverseFFT = fftw_plan_dft_r2c_1d(spectrumLength, melSpectrum, tmpCepstrum, FFTW_ESTIMATE);
#ifdef STND_MULTI_THREAD
        if(hFFTWMutex){
            stnd_mutex_unlock(hFFTWMutex);
        }
#endif
        // メルスケールへ展開
        for(int j = 0; j < timeLength; j++){
            this->f0[j] = f0[j];
            this->t[j]  = t[j];
            this->noiseRatio[j] = noiseRatio[j];
            stretchToMelScale(melSpectrum, sourceSpecgram[j], spectrumLength / 2 + 1, maxFrequency / 2);
            stretchToMelScale(tmpSpectrum, dstSpecgram[j], spectrumLength / 2 + 1, maxFrequency / 2);
            // FFTW は N 点の DFT, IDFT 後は N 倍された値が格納されるので先に割ってしまう．
            for(int i = 0; i <= spectrumLength / 2; i++){
                melSpectrum[i] = (log(tmpSpectrum[i]) - log(melSpectrum[i])) / (double)spectrumLength;
            }
            for(int i = spectrumLength / 2 + 1; i < spectrumLength; i++){
                melSpectrum[i] = melSpectrum[spectrumLength-i];
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
        if(hFFTWMutex){
            stnd_mutex_lock(hFFTWMutex);
        }
#endif
        fftw_destroy_plan(inverseFFT);
#ifdef STND_MULTI_THREAD
        if(hFFTWMutex){
            stnd_mutex_unlock(hFFTWMutex);
        }
#endif
    }
    SAFE_DELETE_ARRAY(tmpSpectrum);
    SAFE_DELETE_ARRAY(melSpectrum);
    SAFE_DELETE_ARRAY(tmpCepstrum);
}

standComplex *standMelCepstrum::getMelCepstrum(double msTime, int *length)
{
    standComplex *ret = NULL;
    *length = 0;
    int index = msTime / framePeriod; //this->framePeriod;
    if(index < 0) index = 0;
    if(index >= this->timeLength) index = this->timeLength - 1;

    if(this->melCepstrum){
        ret = melCepstrum[index];
        *length = this->cepstrumLength;
    }
    return ret;
}

double standMelCepstrum::getF0(double msTime)
{
    int index = msTime / framePeriod;
    if(index < 0) index = 0;
    if(index >= this->timeLength) index = this->timeLength - 1;
    return this->f0[index];
}

double standMelCepstrum::getStretchedPosition(double msTime)
{
    int index = msTime / framePeriod;
    if(index < 1) index = 1;
    if(index >= this->timeLength) index = this->timeLength - 1;
    return this->t[index];
}

double standMelCepstrum::getNoiseRatio(double msTime)
{
    int index = msTime / framePeriod;
    if(index < 0) index = 0;
    if(index >= this->timeLength) index = this->timeLength - 1;
    return this->noiseRatio[index];
}

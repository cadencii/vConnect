#include "vConnectPhoneme.h"

#include <stdio.h>
#include "vConnectUtility.h"
#include "world/world.h"
#include "waveFileEx/waveFileEx.h"
#include "worldParameters.h"
#include "Configuration.h"

vConnectPhoneme::vConnectPhoneme()
{
    timeLength = 0;
    cepstrumLength = 0;
    vorbisSize = 0;
    framePeriod = 0.0f;

    melCepstrum = NULL;
    baseTimeAxis = t = f0 = NULL;
    vorbisData = NULL;

    wave = NULL;
    pulseLocations = NULL;
    mode = VCNT_UNKNOWN;
    waveLength = 0;
    waveOffset = 0;
    baseTimeLength = 0;
}

vConnectPhoneme::~vConnectPhoneme()
{
    destroy();
}

void vConnectPhoneme::destroy()
{
    if(melCepstrum) {
        for(int i = 0; i < cepstrumLength; i++) {
            delete[] melCepstrum[i];
        }
        delete[] melCepstrum;
    }
    delete[] f0;
    delete[] t;
    delete[] baseTimeAxis;
    delete[] vorbisData;

    delete[] (wave + waveOffset);
    delete[] pulseLocations;

    timeLength = 0;
    baseTimeLength = 0;
    cepstrumLength = 0;
    vorbisSize = 0;
    framePeriod = 0.0f;
    waveLength = 0;
    waveOffset = 0;

    melCepstrum = NULL;
    baseTimeAxis = t = f0 = NULL;
    mode = VCNT_UNKNOWN;
    vorbisData = NULL;
    wave = NULL;
    pulseLocations = NULL;
}

bool vConnectPhoneme::writePhoneme(const char* path)
{
    if( mode != VCNT_COMPRESSED || !melCepstrum || !f0 || !vorbisData
        || timeLength <= 0 || cepstrumLength <= 0 || vorbisSize <= 0) {
            return false;
    }

    FILE *fp = fopen(path, "wb");
    if(!fp) {
        return false;
    }

    fwrite(&timeLength, sizeof(int), 1, fp);
    fwrite(&cepstrumLength, sizeof(float), 1, fp);
    fwrite(&framePeriod, sizeof(int), 1, fp);

    for(int i = 0; i < timeLength; i++) {
        fwrite(melCepstrum[i], sizeof(float), cepstrumLength, fp);
    }
    fwrite(f0, sizeof(float), timeLength, fp);
    fwrite(t , sizeof(float), timeLength, fp);

    fwrite(&vorbisSize, sizeof(int), 1, fp);
    fwrite(vorbisData, vorbisSize, 1, fp);

    if(baseTimeAxis)
    {
        fwrite(&baseTimeLength, sizeof(int), 1, fp);
        fwrite(baseTimeAxis, sizeof(float), baseTimeLength, fp);
    }

    fclose(fp);

    return true;
}

bool vConnectPhoneme::readPhoneme(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if(!fp) {
        return false;
    }

    destroy();
    fread(&timeLength, sizeof(int), 1, fp);
    fread(&cepstrumLength, sizeof(float), 1, fp);
    fread(&framePeriod, sizeof(int), 1, fp);

    f0 = new float[timeLength];
    t  = new float[timeLength];
    melCepstrum = new float*[timeLength];
    for(int i = 0; i < timeLength; i++) {
        melCepstrum[i] = new float[cepstrumLength];
    }

    for(int i = 0; i < timeLength; i++) {
        fread(melCepstrum[i], sizeof(float), cepstrumLength, fp);
    }
    fread(f0, sizeof(float), timeLength, fp);
    fread(t , sizeof(float), timeLength, fp);

    fread(&vorbisSize, sizeof(int), 1, fp);
    vorbisData = new char[vorbisSize];
    fread(vorbisData, vorbisSize, 1, fp);

    if(fread(&baseTimeLength, sizeof(int), 1, fp) == 1 && baseTimeLength > 0)
    {
        baseTimeAxis = new float[baseTimeLength];
		fread(baseTimeAxis, sizeof(float), baseTimeLength, fp);
    }
    else
    {
        baseTimeLength = 0;
    }

    fclose(fp);
    mode = VCNT_COMPRESSED;

    return true;
}

int vConnectPhoneme::computeWave(double *wave, int length, int fs, double framePeriod, int cepstrumLength)
{
    if(!wave || length < 0) { return -1; }

    int fftLength;
    destroy();

    this->framePeriod = (float)framePeriod;
    this->timeLength = getSamplesForDIO(fs, length, framePeriod);
    this->cepstrumLength = cepstrumLength;
    fftLength = getFFTLengthForStar(fs);

    double *spectrum = new double[fftLength];
    fftw_complex *cepstrum = new fftw_complex[fftLength];
    fftw_plan forward = fftw_plan_dft_r2c_1d(fftLength, spectrum, cepstrum, FFTW_ESTIMATE);
    fftw_plan inverse = fftw_plan_dft_c2r_1d(fftLength, cepstrum, spectrum, FFTW_ESTIMATE);

    double *tmpF0 = new double[timeLength];
    double *tmpT = new double[timeLength];
    double **specgram = new double*[timeLength];
    double **residual = new double*[timeLength];

    this->f0 = new float[timeLength];
    this->t = new float[timeLength];
    this->melCepstrum = new float*[timeLength];
    for(int i = 0; i < timeLength; i++) {
        this->melCepstrum[i] = new float[cepstrumLength];
    }

    for(int i = 0; i < timeLength; i++) {
        specgram[i] = new double[fftLength];
        residual[i] = new double[fftLength];
        memset(specgram[i], 0, sizeof(double) * fftLength);
        memset(residual[i], 0, sizeof(double) * fftLength);
    }

    /****************************************************/
    /* まず WORLD による分析を行う．                    */
    /* ただしスペクトルの代わりにメルケプを使用する．   */
    /****************************************************/
    dio(wave, length, fs, framePeriod, tmpT, tmpF0);
    star(wave, length, fs, tmpT, tmpF0, specgram, false);

    for(int i = 0; i < timeLength; i++) {
        this->f0[i] = (float)tmpF0[i];
        this->t[i] = (float)tmpT[i];
    }

    /* melCepstrum の計算 → specgram へ再度展開 */
    // 展開する場合は melCepstrum の歪み分を残差に持たせる形になる． */
    for(int i = 0; i < timeLength; i++) {
        int j;
        vConnectUtility::stretchToMelScale(spectrum, specgram[i], fftLength / 2 + 1, fs / 2);

        for(j = 0; j <= fftLength / 2; j++) {
            spectrum[j] = log(spectrum[j]) / fftLength;
        }
        for(; j < fftLength; j++) {
            spectrum[j] = spectrum[fftLength - j];
        }

        fftw_execute(forward);

        for(j = 0; j < cepstrumLength; j++) {
            melCepstrum[i][j] = (float)cepstrum[j][0];
            cepstrum[j][1] = 0.0;
        }
        for(; j <= fftLength / 2; j++) {
            cepstrum[j][0] = cepstrum[j][1] = 0.0;
        }

        fftw_execute(inverse);

        for(j = 0; j < fftLength; j++) {
            spectrum[j] = exp(spectrum[j]);
        }
        vConnectUtility::stretchFromMelScale(specgram[i], spectrum, fftLength / 2 + 1, fs / 2);
    }

    platinum_v4(wave, length, fs, tmpT, tmpF0, specgram, residual);

    /****************************************************/
    /* 次に残差スペクトルを波形の形に持ち直す．         */
    /* 保存形式は Ogg Vorbis                            */
    /****************************************************/
    double *residualWave = new double[fftLength * timeLength];

    // 残差波形を計算
    for(int i = 0; i < timeLength; i++) {
        vConnectUtility::extractResidual(cepstrum, residual[i], fftLength);

        fftw_execute(inverse);

        for(int j = 0, k = i * fftLength; j < fftLength; j++, k++) {
            residualWave[k] = spectrum[j] / (double)fftLength;
        }
    }

    vConnectUtility::newOggVorbis(&vorbisData, &vorbisSize, residualWave, fftLength * timeLength, fs);

    delete[] residualWave;

    fftw_destroy_plan(forward);
    fftw_destroy_plan(inverse);
    delete[] spectrum;
    delete[] cepstrum;
    delete[] tmpF0;
    delete[] tmpT;
    for(int i = 0; i < timeLength; i++) {
        delete[] specgram[i];
        delete[] residual[i];
    }
    delete[] specgram;
    delete[] residual;

    mode = VCNT_COMPRESSED;

    return 0;
}

float *vConnectPhoneme::getMelCepstrum(int index, int *length)
{
    if(index < 0) {
        index = 0;
    } else if(index > timeLength) {
        index = timeLength;
    }
    *length = cepstrumLength;
    return melCepstrum[index];
}

float vConnectPhoneme::getF0(int index)
{
    if(index < 0) {
        index = 0;
    } else if(index > timeLength) {
        index = timeLength;
    }
    return f0[index];
}

size_t vConnectPhoneme::vorbisRead(void *dst, size_t size, size_t maxCount, void *vp)
{
    V_FILE *p = (V_FILE*)vp;

    size_t ret = (p->pos + maxCount * size >= p->size) ? p->size - p->pos : maxCount * size;
    memcpy(dst, p->p + p->pos, ret);
    p->pos += ret;

    return ret / size;
}

int vConnectPhoneme::vorbisSeek(void *vp, ogg_int64_t offset, int flag)
{
    V_FILE *p = (V_FILE*)vp;

    switch(flag) {
        case SEEK_CUR:
            p->pos += offset;
            break;
        case SEEK_END:
            p->pos = p->size + offset;
            break;
        case SEEK_SET:
            p->pos = offset;
            break;
        default:
            return -1;
    }

    if(p->pos > p->size) {
        p->pos = p->size;
        return -1;
    } else if(p->pos < 0) {
        p->pos = 0;
        return -1;
    }

    return 0;
}

long vConnectPhoneme::vorbisTell(void *vp)
{
    return ((V_FILE*)vp)->pos;
}

int vConnectPhoneme::vorbisClose( void *vp )
{
    V_FILE *p = (V_FILE *)vp;
    delete p;
    return 0;
}

bool vConnectPhoneme::vorbisOpen(OggVorbis_File *ovf)
{
    // 保持形式が圧縮形式でない場合．
    if(mode != VCNT_COMPRESSED)
    {
        return false;
    }

    ov_callbacks callbacks = {
        &vConnectPhoneme::vorbisRead,
        &vConnectPhoneme::vorbisSeek,
        &vConnectPhoneme::vorbisClose,
        &vConnectPhoneme::vorbisTell
    };

    V_FILE *vp = new V_FILE();
    vp->p = this->vorbisData;
    vp->size = this->vorbisSize;
    vp->pos = 0;

    if(ov_open_callbacks(vp, ovf, 0, 0, callbacks) != 0) {
        return false;
    }

    return true;
}

void vConnectPhoneme::getOneFrameWorld(double *starSpec,
                                       fftw_complex *residualSpec,
                                       double t, int fftLength,
                                       double *waveform,
                                       fftw_complex *spectrum,
                                       fftw_complex *cepstrum,
                                       fftw_plan forward_r2c,
                                       fftw_plan forward,
                                       fftw_plan inverse)
{
    if( mode != VCNT_RAW )
    {
        for(int i = 0; i < fftLength; i++) {
            starSpec[i] = 1.0;
            residualSpec[i][0] = residualSpec[i][1] = 0.0;
        }
        return;
    }
    int index = (int)(t / this->framePeriod * 1000.0);
    if(index < 0)
    {
        index = 0;
    }
    if(index >= this->timeLength)
    {
        index = timeLength - 1;
    }

    ////// 各バッファと FFT の対応は以下．
    // fftw_plan forward_r2c = fftw_plan_dft_r2c_1d(fftLength, waveform, cepstrum, FFTW_ESTIMATE);
    // fftw_plan forward = fftw_plan_dft_1d(fftLength, spectrum, cepstrum, FFTW_FORWARD, FFTW_ESTIMATE);
    // fftw_plan inverse = fftw_plan_dft_1d(fftLength, cepstrum, spectrum, FFTW_BACKWARD, FFTW_ESTIMATE);

    // STAR スペクトルを計算する．
    //  残差分を作業用バッファとして使いまわし．
    double currentF0 = (f0[index] == 0.0)? DEFAULT_F0 : f0[index];
    int sampleRate = Configuration::getDefaultSampleRate();
    // starGeneralBody(x, xLen, fs, currentF0, timeAxis[i], fftl, specgram[i], waveform, powerSpec, ySpec,&forwardFFT);
    starGeneralBody(wave, waveLength, sampleRate, currentF0, this->t[index], fftLength, starSpec, waveform, waveform, cepstrum, &forward_r2c);

    // PLATINUM 残差スペクトルを計算する．
    double T0 = (double)sampleRate / currentF0;
    int wLen = (int)(0.5 + T0*2.0);
    int pulseIndex = pulseLocations[index];

    // 波形終了位置を越えてしまっている．
    if(wLen+pulseIndex-(int)(0.5+T0) >= waveLength)
    {
        for(int i = 0;i <= fftLength / 2;i++)
        {
            residualSpec[i][0] = residualSpec[i][1] = 0.0;
        }
    } else {
        int i;

        for(i = 0; i < wLen;i++)
        {
            int tmpIndex = i + pulseIndex - (int)(0.5+T0);
            waveform[i] = wave[max(waveOffset, tmpIndex)] *
            (0.5 - 0.5*cos(2.0*PI*(double)(i+1)/((double)(wLen+1))));
        }
        for(;i < fftLength;i++)
        {
            waveform[i] = 0.0;
        }

        // 最小位相スペクトルを求める．
        getMinimumPhaseSpectrum(starSpec, spectrum, cepstrum, fftLength, forward, inverse);

        // 実波形スペクトルを求める．
        fftw_execute(forward_r2c);

        // 最小位相はゼロ点を持たないので除算して残差スペクトルを得る．
        for(i = 0;i <= fftLength/2;i++)
        {
            double tmp = spectrum[i][0]*spectrum[i][0] + spectrum[i][1]*spectrum[i][1];
            residualSpec[i][0] = ( spectrum[i][0]*cepstrum[i][0] + spectrum[i][1]*cepstrum[i+1][1])/tmp;
            residualSpec[i][1] = (-spectrum[i][1]*cepstrum[i][0] + spectrum[i][0]*cepstrum[i+1][1])/tmp;
        }
    }
}

bool vConnectPhoneme::readRawWave(string dir_path, const UtauParameter *utauParams, double framePeriod)
{
    if(!utauParams)
    {
        return false;
    }
    waveFileEx waveFile;
    string fileName = utauParams->fileName;
    fileName = dir_path + fileName;

    if(waveFile.readWaveFile(fileName + ".wav") == 1)
    {
        worldParameters worldParams;
        double *waveBuffer;
        int waveLength;
        waveBuffer = waveFile.getWavePointer(&waveLength);

        // 事前分析データが無いのであれば事前分析を行いファイルとして保存する．
        if(worldParams.readParameters((fileName + ".wpd").c_str()) == false)
        {
            int sampleRate = Configuration::getDefaultSampleRate();
            if(worldParams.computeWave(waveBuffer, waveLength, sampleRate, framePeriod))
            {
                /* 要らない…？
                // 同名ファイルの書き込み中に読み込まれるといけない．
#ifdef STND_MULTI_THREAD
                if(hMutex)
                {
                    hMutex->lock();
                }
#endif
                */
                worldParams.writeParameters((fileName + ".wpd").c_str());
                /*
#ifdef STND_MULTI_THREAD
                if(hMutex)
                {
                    hMutex->unlock();
                }
#endif
                */
            }
            else
            {
                // 事前分析に失敗した．
                return false;
            }
        }
        destroy();
        double beginTime = utauParams->msLeftBlank / 1000.0;
        double endTime = (utauParams->msRightBlank < 0) ?
                         (beginTime - utauParams->msRightBlank / 1000.0) :
                         ((double)waveLength / (double)waveFile.getSamplingFrequency() - utauParams->msRightBlank / 1000.0);

        // 読み込みその他終了したので，波形とパラメタを取り出す．
        waveOffset = 0;
        timeLength = (int)((endTime - beginTime) / framePeriod * 1000.0 + 0.5);
        f0 = new float[timeLength];
        t  = new float[timeLength];
        pulseLocations = new int[timeLength];

        worldParams.getParameters(f0, t, pulseLocations, waveFile.getSamplingFrequency(), beginTime, timeLength, framePeriod);
        for(int i = 0; i < timeLength; i++)
        {
            int tmp = (int)(waveFile.getSamplingFrequency() * 1.0 / ((f0[i] == 0.0) ? DEFAULT_F0 : f0[i]) + 0.5);
            waveOffset = min(waveOffset, pulseLocations[i] - tmp);
        }

        int sampleLength = (int)((endTime - beginTime) * waveFile.getSamplingFrequency() + 0.5 - waveOffset);
        wave = new double[sampleLength];

        int i, j;

        // 波形の値を取り出す．
        for(i = 0, j = (int)(beginTime * waveFile.getSamplingFrequency() + waveOffset); j < 0; i++, j++)
        {
            wave[i] = 0.0;
        }
        for(; i < sampleLength; i++, j++)
        {
            if(j > waveLength) {
                break;
            }
            wave[i] = waveBuffer[j];
        }
        for(; i < sampleLength; i++)
        {
            wave[i] = 0.0;
        }
        wave -= waveOffset;
        mode = VCNT_RAW;
        this->framePeriod = framePeriod;
        this->waveLength = sampleLength + waveOffset;

        // 波形の正規化を行う．
        double sum1 = 0.0, sum2 = 0.0;

        for(i = 0; i < 2048; i++)
        {
            sum1 += wave[i] * wave[i];
        }
        float wavePos = utauParams->msLeftBlank + utauParams->msFixedLength;
        for(i = 0, j = (int)(wavePos / 1000.0 * waveFile.getSamplingFrequency()); i < 2048 && j < waveLength - waveOffset; i++, j++)
        {
            sum2 += waveBuffer[j] * waveBuffer[j];
        }
        sum1 = max(sum1, sum2);
        sum1 = VOL_NORMALIZE / sqrt( (sum1 / 2048.0) );
        for(i = waveOffset; i < sampleLength + waveOffset; i++)
        {
            wave[i] *= sum1;
        }
    }

    return true;
}

void vConnectPhoneme::setTimeAxis(double *t, int length)
{
    if(length != timeLength)
    {
        return;
    }
    for(int i = 0; i < length; i++)
    {
        this->t[i] = t[i];
    }
}

void vConnectPhoneme::setBaseTimeAxis(double *base, int length)
{
    if(length <= 0)
    {
        return;
    }
    delete[] baseTimeAxis;
    baseTimeAxis = new float[length];
    baseTimeLength = length;
    for(int i = 0; i < length; i++)
    {
        this->baseTimeAxis[i] = base[i];
    }
}

double vConnectPhoneme::getFrameTime(int index)
{
    index = max(0, min(index, timeLength - 1));
    return t[index];
}

double vConnectPhoneme::getBaseFrameTime(int index)
{
    if(baseTimeAxis == NULL)
    {
        return (double)index * framePeriod / 1000.0;
    }
    index = max(0, min(index, baseTimeLength - 1));
    return baseTimeAxis[index];
}

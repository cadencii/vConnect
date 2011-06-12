#include "vConnectPhoneme.h"

#include <stdio.h>
#include "vConnectUtility.h"
#include "world/world.h"

vConnectPhoneme::vConnectPhoneme()
{
    timeLength = 0;
    cepstrumLength = 0;
    vorbisSize = 0;
    framePeriod = 0.0f;

    melCepstrum = NULL;
    t = f0 = NULL;
    vorbisData = NULL;
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
    delete[] vorbisData;

    timeLength = 0;
    cepstrumLength = 0;
    vorbisSize = 0;
    framePeriod = 0.0f;

    melCepstrum = NULL;
    t = f0 = NULL;
    vorbisData = NULL;
}

bool vConnectPhoneme::writePhoneme(const char* path)
{
    if(!melCepstrum || !f0 || !vorbisData
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

    fclose(fp);

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
    delete vp;
    return 0;
}

bool vConnectPhoneme::vorbisOpen(OggVorbis_File *ovf)
{
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

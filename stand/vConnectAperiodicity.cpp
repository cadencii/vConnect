#include "vConnectAperiodicity.h"

#include <stdlib.h>
#include <math.h>
#include "standSpecgram.h"
#include "standFrame.h"

#ifndef PI
#define PI 3.1415926535897932384
#endif

vConnectAperiodicity::vConnectAperiodicity()
{
    wave = NULL;
    apLength = apIndex = NULL;
    tLen = waveLength = 0;
}

void vConnectAperiodicity::destroy(void)
{
    delete[] wave;
    delete[] apLength;
    delete[] apIndex;
    tLen = waveLength = 0;
}

void vConnectAperiodicity::createBuffer(int waveLength, int tLen)
{
    if(waveLength > 0 && tLen > 0)
    {
        destroy();
        apLength = new int[tLen];
        apIndex = new int[tLen];
        wave = new double[waveLength];
        this->tLen = tLen;
        this->waveLength = waveLength;
    }
}

bool vConnectAperiodicity::writeAperiodicityWave(string output)
{
    // 暫定
    bool ret = false;
    FILE *fp = fopen(output.c_str(), "wb");

    if(fp && wave && waveLength > 0)
    {
        short *data = new short[waveLength];
        double fmax = 0.0;

        for(int i = 0; i < waveLength; i++)
        {
            fmax = (fmax > fabs(wave[i])) ? fmax : fabs(wave[i]);
        }
        for(int i = 0; i < waveLength; i++)
        {
            data[i] = wave[i] / fmax * ((1 << 15) - 1);
        }

        fwrite(&tLen, sizeof(int), 1, fp);
        fwrite(&waveLength, sizeof(int), 1, fp);
        fwrite(&fmax, sizeof(double), 1, fp);
        fwrite(apLength, sizeof(int), tLen, fp);
        fwrite(apIndex, sizeof(int), tLen, fp);
        fwrite(data, sizeof(short), waveLength, fp);

        delete[] data;

        ret = true;
    }
    return ret;
}

bool vConnectAperiodicity::readAperiodicityWave(string input)
{
    bool ret = false;
    FILE *fp = fopen(input.c_str(), "rb");

    if(fp)
    {
        destroy();
        double fmax;
        fread(&tLen, sizeof(int), 1, fp);
        fread(&waveLength, sizeof(int), 1, fp);
        fread(&fmax, sizeof(double), 1, fp);

        createBuffer(waveLength, tLen);
        short *data = new short[waveLength];

        fread(apLength, sizeof(int), tLen, fp);
        fread(apIndex, sizeof(int), tLen, fp);
        fread(data, sizeof(short), waveLength, fp);

        for(int i = 0; i < waveLength; i++)
        {
            wave[i] = data[i] * fmax / ((1 << 15) - 1);
        }

        delete[] data;
        ret = true;
    }
    return ret;
}

void vConnectAperiodicity::getOneFrameSegment(double *dst, int index, int fftl)
{
    int begin, end, i;
    index = (index < 0) ? 0 : (index > tLen) ? tLen : index;

    begin = apIndex[index] - apLength[index];
    end = apIndex[index] + apLength[index];
    if(end > waveLength)
    {
        memset(dst, 0, sizeof(double) * fftl);
        return;
    }
    for(i = 0; i < fftl / 2 - apLength[index]; i++)
    {
        dst[i] = 0.0;
    }
    for(int j = begin; j < end; j++, i++)
    {
        dst[i] = wave[j] * (0.5 + 0.5 * cos(PI * (double)(j - apIndex[index]) / apLength[index]));
    }
    for(; i < fftl; i++)
    {
        dst[i] = 0.0;
    }
}

void vConnectAperiodicity::computeExciteWave(standSpecgram *specgram)
{
    if(!specgram){ return; }

    int index = 0, prevIndex = 0;
    double f0, T, currentTime = 0.0;
    standFrame srcFrame;

    fftw_complex *tmpSpectrum = new fftw_complex[specgram->getFFTLength()];
    double *tmpWave = new double[specgram->getFFTLength()];

#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_lock( hFFTWMutex );
    }
#endif
    fftw_plan inverse = fftw_plan_dft_c2r_1d(specgram->getFFTLength(), tmpSpectrum, tmpWave, FFTW_ESTIMATE);
#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_unlock( hFFTWMutex );
    }
#endif

    destroy();
    createBuffer(specgram->getTimeLength() * framePeriod * fs / 1000.0, specgram->getTimeLength());

    for(; index < specgram->getTimeLength(); index = currentTime / framePeriod * 1000.0)
    {
        specgram->getFramePointer(index, srcFrame);
        f0 = *srcFrame.f0;
        if(f0 == 0.0){
            f0 = DEFAULT_F0;
        }
        T = 1.0 / f0;

        tmpSpectrum[0][0] = srcFrame.aperiodicity[0];
        tmpSpectrum[0][1] = 0.0;
        for(int i = 1; i < srcFrame.fftl / 2; i++)
        {
            tmpSpectrum[i][0] = srcFrame.aperiodicity[i*2];
            tmpSpectrum[i][1] = srcFrame.aperiodicity[i*2-1];
        }
        tmpSpectrum[srcFrame.fftl/2][0] = srcFrame.aperiodicity[srcFrame.fftl-1];
        tmpSpectrum[srcFrame.fftl/2][1] = 0.0;

        fftw_execute(inverse);

        // FFT による係数の補正はここで行ってしまおう．
        for(int i = 0, j = currentTime * fs; i < srcFrame.fftl && j < waveLength; i++, j++)
        {
            wave[j] += tmpWave[i] / (double)srcFrame.fftl;
        }

        for(int i = prevIndex; i < index; i++){
            apIndex[i] = currentTime * fs + srcFrame.fftl / 2;
            apLength[i] = T * fs;
        }
        currentTime += T;
        prevIndex = index;
    }

    delete[] tmpWave;
    delete[] tmpSpectrum;
#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_lock( hFFTWMutex );
    }
#endif
    fftw_destroy_plan(inverse);
#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_unlock( hFFTWMutex );
    }
#endif
}

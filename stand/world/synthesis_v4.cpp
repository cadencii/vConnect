/*
 *
 * Synthesis API for WORLD 0.0.4 by M. Morise
 *
 *    synthesis_v4.cpp
 *                        (c) M. Morise 2010-
 *                            edit and comment by HAL, kbinani
 *
 *  This file is a part of WORLD system.
 * WORLD needs FFTW. Please install FFTW to use these files.
 * FFTW is here; http://www.fftw.org/
 *
 * notice: this comment is added by HAL.
 *         Original files are on the web page below;
 *         http://www.aspl.is.ritsumei.ac.jp/morise/world/
 * (this file is from WORLD 0.0.4
 *  functions are renamed by HAL to support both 0.0.1 & 0.0.4.)
 *
 *  synthesis_v4.cpp includes a set of functions
 * that supports synthesis on WORLD 0.0.4.
 *  Notice that aperiodicity of 0.0.1 is much different
 * from that of 0.0.4. Please use platinum_v4 function
 * for synthesis_v4 function.
 *
 */
#include "world.h"
#include "../stand.h"

#include <stdio.h> // for debug
#include <stdlib.h>

// 特定時刻の応答を取得する．
void getOneFrameSegment_v4(double *f0, int tLen, double **specgram, double **residualSpecgram, int fftl, double framePeriod, double currentTime, int fs, double defaultF0,
                        fftw_complex *spectrum, fftw_complex *cepstrum, 
                        double *response, int xLen, fftw_plan *inverseFFT_RP, fftw_plan minForward, fftw_plan minInverse)
{
    int i;
    double real, imag, tmp;

    int currentFrame, currentPosition;

    currentFrame = (int)(currentTime/(framePeriod/1000.0) + 0.5);    
    currentPosition = (int)(currentTime*(double)fs);

    tmp = currentTime + 1.0/(f0[currentFrame] == 0.0 ? defaultF0 : f0[currentFrame]);
    
    // 値を取り出す
    getMinimumPhaseSpectrum(specgram[currentFrame], spectrum, cepstrum, fftl, minForward, minInverse);
    
    spectrum[0][0] *= residualSpecgram[currentFrame][0];
    for(i = 1;i < fftl/2;i++)
    {
        real = spectrum[i][0]*residualSpecgram[currentFrame][(i-1)*2+1] - spectrum[i][1]*residualSpecgram[currentFrame][i*2];
        imag = spectrum[i][0]*residualSpecgram[currentFrame][i*2] + spectrum[i][1]*residualSpecgram[currentFrame][(i-1)*2+1];
        spectrum[i][0] = real;
        spectrum[i][1] = imag;
    }
    spectrum[fftl/2][0] = spectrum[fftl/2][0] * residualSpecgram[currentFrame][fftl-1];
    fftw_execute(*inverseFFT_RP);

}


void synthesis_v4(double *f0, int tLen, double **specgram, double **residualSpecgram, int fftl, double framePeriod, int fs, 
               double *synthesisOut, int xLen)
{
    int i,j;
    double *impulseResponse;
    impulseResponse = (double *)malloc(sizeof(double) * fftl);
    fftw_complex        *cepstrum, *spectrum;    // ケプストラムとスペクトル
    cepstrum = (fftw_complex *)malloc(sizeof(fftw_complex) * fftl);
    spectrum = (fftw_complex *)malloc(sizeof(fftw_complex) * fftl);
    fftw_plan    inverseFFT_RP, minForward, minInverse;                // FFTセット
#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_lock( hFFTWMutex );
    }
#endif
    inverseFFT_RP = fftw_plan_dft_c2r_1d(fftl, spectrum, impulseResponse ,  FFTW_ESTIMATE);
    minForward = fftw_plan_dft_1d(fftl, spectrum, cepstrum, FFTW_FORWARD, FFTW_ESTIMATE);
    minInverse = fftw_plan_dft_1d(fftl, cepstrum, spectrum, FFTW_BACKWARD, FFTW_ESTIMATE);
#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_unlock( hFFTWMutex );
    }
#endif

    double currentTime = 0.0;
    int currentPosition = 0;//currentTime / framePeriod;
    int currentFrame = 0;
    for(i = 0;;i++)
    {
        // 完全な無音は飛ばす．
        if(currentFrame < tLen && f0[currentFrame] < 0.0){
            currentFrame++;
            currentTime = (double)currentFrame * framePeriod / 1000.0;
            currentPosition = (int)(currentTime * (double)fs);
            if(currentFrame == tLen) break;
            continue;
        }
        for(j = 0;j < fftl;j++) impulseResponse[j] = 0.0; // 配列は毎回初期化

        getOneFrameSegment_v4(f0, tLen, specgram, residualSpecgram, fftl, framePeriod, currentTime, fs, DEFAULT_F0,
                        spectrum, cepstrum, impulseResponse, xLen, &inverseFFT_RP, minForward, minInverse);

        currentPosition = (int)(currentTime*(double)fs);
//        for(j = 0;j < fftl/2;j++)
        for(j = 0;j < 3*fftl/4;j++)
        {
            if(j+currentPosition >= xLen) break;
            synthesisOut[j+currentPosition] += impulseResponse[j];
        }

        // 更新
        currentTime += 1.0/(f0[currentFrame] == 0.0 ? DEFAULT_F0 : f0[currentFrame]);
        currentFrame = (int)(currentTime/(framePeriod/1000.0) + 0.5);
        currentPosition = (int)(currentTime*(double)fs);
        if(j+currentPosition >= xLen || currentFrame >= tLen) break;
    }

#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_lock( hFFTWMutex );
    }
#endif
    fftw_destroy_plan(inverseFFT_RP);
    fftw_destroy_plan(minForward);
    fftw_destroy_plan(minInverse);
#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_unlock( hFFTWMutex );
    }
#endif
    free(cepstrum); free(spectrum);
    free(impulseResponse);
    return;
}

/*
 *
 * Spectrum 推定法 STAR by M. Morise
 *
 *    star.cpp
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
 *  star.cpp includes a set of functions
 * that supports STAR, spectrum estimation algorhythm.
 *
 */
#include "world.h"

#include <stdio.h> // for debug
#include <stdlib.h>

// calculate fft length from sampling frequency.
// this function ignores the lowest f0.
int getFFTLengthForStar(int fs)
{
    return (int)pow(2.0, 1.0+(int)(log(3.0*fs/FLOOR+1) / log(2.0)));
}

// Spectrum extraction based on STAR
// x    : Input signal whose length is xLen [sample]
// xLen : Length of the input signal.
// f0    : Estimated F0 contour
void star(double *x, int xLen, int fs, double *timeAxis, double *f0,
         double **specgram, bool mode)
{
    int i,j;
    double framePeriod = (timeAxis[1]-timeAxis[0])*1000.0;
    double f0LowLimit = FLOOR; // F0がFLOOR Hz以下の場合は無声音として扱う
    double defaultF0 = 300; // 
    int    index;

    int    fftl = (int)pow(2.0, 1.0+(int)(log(3.0*fs/f0LowLimit+1) / log(2.0)));
    int tLen = getSamplesForDIO(fs, xLen, framePeriod);

    double *sliceSTAR;
    // 波形のスペクトルを計算
    double                *waveform;
    double                *powerSpec;
    double                 currentF0;
    waveform  = (double *)malloc(sizeof(double) * fftl);
    powerSpec = (double *)malloc(sizeof(double) * fftl);

    fftw_plan            forwardFFT;    // FFTセット
    fftw_complex        *ySpec;        // スペクトル
    ySpec = (fftw_complex *)malloc(sizeof(fftw_complex) * fftl);
#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_lock( hFFTWMutex );
    }
#endif
    forwardFFT = fftw_plan_dft_r2c_1d(fftl, waveform, ySpec, FFTW_ESTIMATE);
#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_unlock( hFFTWMutex );
    }
#endif
    sliceSTAR = (double *)malloc(sizeof(double) * fftl);

    if(mode){
        for(i = 0;i < tLen;i++)
        {
            if(f0[i] == 0)
            {
                index = (int)((double)i*(timeAxis[1]-timeAxis[0])*(double)fs);
                for(j = 0;j <= fftl/2;j++)
                {
                    if(j+index >= xLen) break;
                    specgram[i][j] = x[j+index];
                }
            }
            else
            {
    		    currentF0 = f0[i] <= FLOOR_F0 ? DEFAULT_F0 : f0[i];
    		    starGeneralBody(x, xLen, fs, currentF0, timeAxis[i], fftl, specgram[i], waveform, powerSpec, ySpec,&forwardFFT);
            }
        }
    }else{
        for(i = 0;i < tLen;i++)
        {
  		    currentF0 = f0[i] <= FLOOR_F0 ? DEFAULT_F0 : f0[i];
   		    starGeneralBody(x, xLen, fs, currentF0, timeAxis[i], fftl, specgram[i], waveform, powerSpec, ySpec,&forwardFFT);
        }
    }

    free(ySpec);
#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_lock( hFFTWMutex );
    }
#endif
    fftw_destroy_plan(forwardFFT);
#ifdef STND_MULTI_THREAD
    if( hFFTWMutex ){
        stnd_mutex_unlock( hFFTWMutex );
    }
#endif
    free(waveform);
    free(powerSpec);
    free(sliceSTAR);
}

void starGeneralBody(double *x, int xLen, int fs, double f0, double t, int fftl,
                               double * sliceSTAR, double *waveform, double *powerSpec, fftw_complex *ySpec, fftw_plan *forwardFFT)
{
    int i,j;
    double t0 = 1.0 / f0;

    int *baseIndex, *index; // i付きのも包含する (Matlab版参照)
    int nFragment = (int)(0.5 + 3.0*(double)fs/f0/2.0);
    baseIndex = (int *)malloc(sizeof(int) * (nFragment*2+1));
    index  = (int *)malloc(sizeof(int) * (nFragment*2+1));

    for(i = -nFragment, j = 0;i <= nFragment;i++, j++) 
        baseIndex[j] = i;
    for(i = 0;i <= nFragment*2;i++) 
        index[i]  = min(xLen, max(1, c_round(t*(double)fs+1+baseIndex[i]) ) ) - 1;

    double *segment, *window;
    double position, average;
    segment  = (double *)malloc(sizeof(double) * (nFragment*2+1));
    window   = (double *)malloc(sizeof(double) * (nFragment*2+1));
    average  = 0.0;
    for(i = 0;i <= nFragment*2;i++)
    {
        segment[i]  = x[index[i]];
        position  = (double)(baseIndex[i]/(double)fs/(3.0/2.0) ) + 
            (t*(double)fs - (double)(c_round(t*(double)fs))) / (double)fs;
        window[i]  = 0.5*cos(PI*position*f0) +0.5;
        average  += window[i]*window[i];
    }
    average  = sqrt(average);
    for(i = 0;i <= nFragment*2;i++) window[i]  /= average;

    // パワースペクトルの計算
    for(i = 0;i <= nFragment*2;i++) 
        waveform[i] = segment[i] * window[i];
    for(;i < fftl;i++) 
        waveform[i] = 0.0;
    fftw_execute(*forwardFFT); // FFTの実行
    for(i = 1;i <= fftl/2;i++) 
        powerSpec[i] = ySpec[i][0]*ySpec[i][0] + ySpec[i][1]*ySpec[i][1];
    powerSpec[0] = powerSpec[1];

    free(window);
    free(segment);
    free(baseIndex); free(index);

    // adroit smoothing
    // 無駄を仕分け．
    int limit;
    limit = (int)(f0 / (fs/(double)fftl))+1;
    double *dSpectrum, dFrequencyAxis, dShift;
    dSpectrum        = (double *)malloc(sizeof(double) * (fftl+limit*2 + 1) );

    // 計算コストを少しでも減らす
    dFrequencyAxis = -((double)limit-0.5)*(double)fs/(double)fftl;
    dShift = (double)fs/(double)fftl;

    for(i = 0;i < limit;i++) 
        dSpectrum[i] = powerSpec[limit-i];
    for(j = 0;i < fftl/2+limit;i++,j++) 
        dSpectrum[i] = powerSpec[j];
    for(j=1;i < fftl/2+limit*2+1;i++,j++) 
        dSpectrum[i] = powerSpec[fftl/2-j];

    int tmp = (int)(f0*fftl/(double)fs);
    double *dSegment, *centers;
    dSegment    = (double *)malloc(sizeof(double) * fftl*2);
    centers        = (double *)malloc(sizeof(double) * (fftl/2 + 1) );

    // 加算は暫定処置
    dSegment[0]    = log(dSpectrum[0] + 1.0e-17)*(double)fs/(double)fftl;
    for(i = 1;i < fftl/2+limit*2+1;i++)
        dSegment[i] = log(dSpectrum[i] + 1.0e-17)*(double)fs/(double)fftl + dSegment[i-1];

    for(i = 0;i <= fftl/2;i++)
        centers[i] = (double)i / (double)fftl * (double)fs - f0/2.0;

    double *lowLevels, *highLevels;
    lowLevels  = (double *)malloc(sizeof(double) * (fftl/2+1));
    highLevels = (double *)malloc(sizeof(double) * (fftl/2+1));
    interp1Q(dFrequencyAxis, dShift, dSegment, fftl/2+limit*2+1, centers, (fftl/2 + 1), lowLevels);
    for(i = 0;i <= fftl/2;i++)
        centers[i] += f0;
    interp1Q(dFrequencyAxis, dShift, dSegment, fftl/2+limit*2+1, centers, (fftl/2 + 1), highLevels);

    for(i = 0;i <= fftl/2;i++)
        sliceSTAR[i] = exp( (highLevels[i]-lowLevels[i])/f0);

    free(lowLevels); free(highLevels);
    free(dSegment); free(centers);
    free(dSpectrum);
}

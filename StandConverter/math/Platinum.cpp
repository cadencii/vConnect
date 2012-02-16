/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file Platinum.cpp
 *  @brief Excitation signal estimation method PLATINUM
 *         Original source codes are below.
 *         http://www.aspl.is.ritsumei.ac.jp/morise/world/
 *         This code is redistributed to support thread safety with Qt Library.
 *  @author HAL@shurabaP
 */
#include "Platinum.h"
#include "World.h"
#include "Dio.h"
#include "MatlabFunctions.h"
#include "MinimumPhaseSpectrum.h"
#include "FFTSet.h"

#include <stdio.h> // for debug
#include <stdlib.h>
#include <math.h>

using namespace stand::math;
using namespace stand::math::world;

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef max
#define max(a,b) (((a)<(b))?(b):(a))
#endif

namespace stand
{
namespace math
{
namespace world
{
namespace platinumsup
{

static void getOneFrameResidualSpec(const double *x, int xLen, int fs, int positionIndex, double framePeriod, double f0, double *specgram, int fftl, double *pulseLocations, int pCount,
                            double *residualSpec, FFTSet *forwardR2C, FFTSet *forward, FFTSet *inverse)
{
    int i;
    double *tmpWave;
    double T0;
    int index, tmpIndex, wLen;

    double tmp;
    double tmpValue = 100000.0; // safeGuard
    for(i = 0;i < pCount;i++)
    {
        tmp = fabs(pulseLocations[i] - (double)positionIndex*framePeriod);
        if(tmp < tmpValue)
        {
            tmpValue = tmp;
            tmpIndex = i;
        }
        index = 1+(int)(0.5+pulseLocations[tmpIndex]*fs);
    }

    T0 = (double)fs/f0;
    wLen = (int)(0.5 + T0*2.0);
    tmpWave = (double *)(forwardR2C->in());

    // 区間内の波形を FFT しておく．
    if(wLen+index-(int)(0.5+T0) >= xLen)
    {
        for(i = 0;i < fftl;i++) tmpWave[i] = 0;
        return;
    }

    for(i = 0;i < wLen;i++)
    {
        tmpIndex = i+index - (int)(0.5+T0);
        tmpWave[i] = x[min( xLen-1, max(0, tmpIndex))] *
        (0.5 - 0.5*cos(2.0*PI*(double)(i+1)/((double)(wLen+1))));
    }
    for(;i < fftl;i++)
        tmpWave[i] = 0.0;

    forwardR2C->execute();

    fftw_complex        *tmpSpec, *starSpec;        // スペクトル
    tmpSpec        = (fftw_complex *)forwardR2C->out();
    starSpec    = (fftw_complex *)forward->in();

    stand::math::world::minimumPhaseSpectrum(specgram, fftl, forward, inverse);

    residualSpec[0] = tmpSpec[0][0]/starSpec[0][0];
    for(i = 0;i < fftl/2-1;i++)
    {
        tmp = starSpec[i+1][0]*starSpec[i+1][0] + starSpec[i+1][1]*starSpec[i+1][1];
        residualSpec[i*2+1] = ( starSpec[i+1][0]*tmpSpec[i+1][0] + starSpec[i+1][1]*tmpSpec[i+1][1])/tmp;
        residualSpec[i*2+2] = (-starSpec[i+1][1]*tmpSpec[i+1][0] + starSpec[i+1][0]*tmpSpec[i+1][1])/tmp;
    }
    residualSpec[fftl-1] = tmpSpec[fftl/2][0]/starSpec[fftl/2][0];
}

static int getPulseLocations(const double *x, int xLen, double *totalPhase, int vuvNum, int *stList, int *edList, int fs, double framePeriod, int *wedgeList, double *pulseLocations)
{
    int i, j;
    int stIndex, edIndex;

    int pCount = 0;
    int numberOfLocation;
    double *tmpPulseLocations, *basePhase;
    tmpPulseLocations    = (double *)malloc(sizeof(double) * xLen);
    basePhase            = (double *)malloc(sizeof(double) * xLen);

    double tmp;
    for(i = 0;i < vuvNum;i++)
    {
        stIndex = max(0, (int)((double)fs*(stList[i])*framePeriod/1000.0));
        edIndex = min(xLen-1, (int)((double)fs*(edList[i]+1)*framePeriod/1000.0+0.5) -1);
        tmp = totalPhase[wedgeList[i]];

        for(j = stIndex;j < edIndex-1;j++) basePhase[j] = fmod(totalPhase[j+1]-tmp, 2*PI) - fmod(totalPhase[j]-tmp, 2*PI);

        basePhase[0] = 0; numberOfLocation = 0;
        for(j = stIndex;j < edIndex;j++)
        {
            if(fabs(basePhase[j]) > PI/2.0)
            {
                tmpPulseLocations[numberOfLocation++] = (double)j/(double)fs;
            }
        }
        for(j = 0;j < numberOfLocation;j++) pulseLocations[pCount++] = tmpPulseLocations[j];
    }

    free(tmpPulseLocations);
    return pCount;
}

static void getWedgeList(const double *x, int xLen, int vuvNum, int *stList, int *edList, int fs, double framePeriod, double *f0, int *wedgeList)
{
    int i, j;
    double LowestF0 = 40.0;
    int center, T0;
    double peak;
    int peakIndex = 0;
    double *tmpWav;
    double currentF0;
    tmpWav = (double *)malloc(sizeof(double) * (int)(fs*2/LowestF0));

    for(i = 0;i < vuvNum;i++)
    {
        center        = (int)((stList[i]+edList[i]+1)/2);
        currentF0    = f0[center] == 0.0 ? DEFAULT_F0 : f0[center];
        T0            = (int)((fs / currentF0)+0.5);
        peakIndex = (int)(((1+center)*framePeriod*fs/1000.0)+0.5);
        for(j = 0;j < T0*2;j++)
        {
//            tmpWav[j] = x[peakIndex-T0+j-1];
            tmpWav[j] = x[max(0, min(xLen-1, peakIndex-T0+j-1))];
        }
        peak = 0.0;
        peakIndex = 0;
        for(j = 0;j < T0*2+1;j++)
        {
            if(fabs(tmpWav[j]) > peak)
            {
                peak = tmpWav[j];
                peakIndex = j;
            }
        }
        wedgeList[i] = max(0, min(xLen-1, (int)(0.5 + ((center+1)*framePeriod*fs/1000.0)-T0+peakIndex+1.0) - 1));
    }
    free(tmpWav);
}

}
}
}
}

using namespace stand::math::world::platinumsup;


// PLATINUM Version 0.0.4. 恐らくこの仕様で確定です．
// Aperiodicity estimation by PLATINUM
// x                : Input signal
// xLen                : Length of x [sample]
// fs                : Sampling frequency [Hz]
// timeAxis            : Time axis [sec].
// f0                : F0 [Hz]
// specgram            : Spectrogram.
// residualSpecgram : Extracted excitation signal
//                      - "Residual signal" is old name. It will be replaced by the current name.
//                      - Time-frequency representation is employed to speed up synthesis.
//                      - Time-Time representation may be useful for voice morphing.
void stand::math::world::platinum(const double *x, int xLen, int fs, double *timeAxis, double *f0, double **specgram, double **residualSpecgram)
{
    int i, j, index;
    double framePeriod = (timeAxis[1]-timeAxis[0])*1000.0;

    int    fftl = (int)pow(2.0, 1.0+(int)(log(3.0*fs/FLOOR_F0+1) / log(2.0)));
    int tLen = samplesForDio(fs, xLen, framePeriod);

    int vuvNum;
    vuvNum = 0;
    for(i = 1;i < tLen;i++)
    {
        if(f0[i]!=0.0 && f0[i-1]==0.0) vuvNum++;
    }
    vuvNum+=vuvNum-1; // 島数の調整 (有声島と無声島)
    if(f0[0] == 0) vuvNum++;
    if(f0[tLen-1] == 0) vuvNum++;

    int stCount, edCount;
    int *stList, *edList;
    stList = (int *)malloc(sizeof(int) * vuvNum);
    edList = (int *)malloc(sizeof(int) * vuvNum);
    edCount = 0;

    stList[0] = 0;
    stCount = 1;
    index = 1;
    if(f0[0] != 0)
    {
        for(i = 1;i < tLen;i++)
        {
            if(f0[i]==0 && f0[i-1]!=0)
            {
                edList[0] = i-1;
                edCount++;
                stList[1] = i;
                stCount++;
                index = i;
            }
        }
    }

    edList[vuvNum-1] = tLen-1;
    for(i = index;i < tLen;i++)
    {
        if(f0[i]!=0.0 && f0[i-1]==0.0)
        {
            edList[edCount++] = i-1;
            stList[stCount++] = i;
        }
        if(f0[i]==0.0 && f0[i-1]!=0.0)
        {
            edList[edCount++] = i-1;
            stList[stCount++] = i;
        }
    }

    int *wedgeList;
    wedgeList = (int *)malloc(sizeof(int) * vuvNum);
    getWedgeList(x, xLen, vuvNum, stList, edList, fs, framePeriod, f0, wedgeList);

    double *signalTime, *f0interpolatedRaw, *totalPhase;
    double *fixedF0;
    fixedF0                = (double *)malloc(sizeof(double) * tLen);
    signalTime            = (double *)malloc(sizeof(double) * xLen);
    f0interpolatedRaw    = (double *)malloc(sizeof(double) * xLen);
    totalPhase            = (double *)malloc(sizeof(double) * xLen);

    for(i = 0;i < tLen;i++) fixedF0[i] = f0[i] == 0 ? DEFAULT_F0 : f0[i];
    for(i = 0;i < xLen;i++) signalTime[i] = (double)i / (double)fs;
    interp1(timeAxis, fixedF0, tLen, signalTime, xLen, f0interpolatedRaw);
    totalPhase[0] = f0interpolatedRaw[0]*2*PI/(double)fs;
    for(i = 1;i < xLen;i++) totalPhase[i] = totalPhase[i-1] + f0interpolatedRaw[i]*2*PI/(double)fs;

    double *pulseLocations;
    pulseLocations        = (double *)malloc(sizeof(double) * xLen);
    int pCount;
    pCount = getPulseLocations(x, xLen, totalPhase, vuvNum, stList, edList, fs, framePeriod, wedgeList, pulseLocations);

    FFTSet forwardR2C(fftl, FFTSet::FFTW_R2C);
    FFTSet forward(fftl, FFTSet::FFTW_C2C_FORWARD);
    FFTSet inverse(fftl, FFTSet::FFTW_C2C_BACKWARD);
    double currentF0;
    for(j = 0;j < fftl;j++) residualSpecgram[0][j] = 0.0;
    for(i = 1;i < tLen;i++)
    {
        currentF0 = f0[i] <= FLOOR_F0 ? DEFAULT_F0 : f0[i];
        getOneFrameResidualSpec(x, xLen, fs, i, framePeriod/1000.0, currentF0, specgram[i], fftl, pulseLocations, pCount,
                        residualSpecgram[i], &forwardR2C, &forward, &inverse);
    }

    free(fixedF0);
    free(pulseLocations);
    free(totalPhase); free(f0interpolatedRaw); free(signalTime);
    free(wedgeList);
    free(edList); free(stList);
    return;
}


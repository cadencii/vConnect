/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file WorldSynthesis.cpp
 *  @brief Synthesis for WORLD data.
 *         Original source codes are below.
 *         http://www.aspl.is.ritsumei.ac.jp/morise/world/
 *         This code is redistributed to support thread safety with Qt Library.
 *  @author HAL@shurabaP
 */
#include "WorldSynthesis.h"

#include "FFTSet.h"
#include "MinimumPhaseSpectrum.h"

using namespace stand::math::world;
using namespace stand::math;

#include <stdio.h> // for debug
#include <stdlib.h>

// 特定時刻の応答を取得する．
void stand::math::world::oneFrameSegment(double *f0, double **specgram, double **residualSpecgram, int fftl, double framePeriod, double currentTime, int fs, double defaultF0,
                        FFTSet *forward, FFTSet *inverse, FFTSet *toWave)
{
    double tmp;

    int currentFrame, currentPosition;

    currentFrame = (int)(currentTime/(framePeriod/1000.0) + 0.5);
    currentPosition = (int)(currentTime*(double)fs);

    tmp = currentTime + 1.0/(f0[currentFrame] == 0.0 ? defaultF0 : f0[currentFrame]);

    // 値を取り出す
    minimumPhaseSpectrum(specgram[currentFrame], fftl, forward, inverse);
    fftw_complex *spectrum = (fftw_complex *)(forward->in());
    fftw_complex *dst = (fftw_complex *)toWave->in();

    dst[0][0] = spectrum[0][0] * residualSpecgram[currentFrame][0];
    dst[0][1] = 0.0;
    for(int i = 1; i < fftl / 2; i++)
    {
        dst[i][0] = spectrum[i][0]*residualSpecgram[currentFrame][(i-1)*2+1] - spectrum[i][1]*residualSpecgram[currentFrame][i*2];
        dst[i][1] = spectrum[i][0]*residualSpecgram[currentFrame][i*2] + spectrum[i][1]*residualSpecgram[currentFrame][(i-1)*2+1];
    }
    dst[fftl/2][0] = spectrum[fftl/2][0] * residualSpecgram[currentFrame][fftl-1];
    dst[fftl/2][1] = 0.0;
    toWave->execute();
}

void stand::math::world::synthesis(double *f0, double **specgram, double **residual, int tLen, int fftl, double framePeriod, int fs, double *synthesisOut, int xLen)
{
    int i,j;

    FFTSet toWave(fftl, FFTSet::FFTW_C2R);
    FFTSet forward(fftl, FFTSet::FFTW_C2C_FORWARD);
    FFTSet inverse(fftl, FFTSet::FFTW_C2C_BACKWARD);

    double *impulseResponse = (double *)toWave.out();

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
//        for(j = 0;j < fftl;j++) impulseResponse[j] = 0.0; // 配列は毎回初期化

        oneFrameSegment(f0, specgram, residual, fftl, framePeriod, currentTime, fs, DEFAULT_F0,
                        &forward, &inverse, &toWave);

        currentPosition = (int)(currentTime*(double)fs);
//        for(j = 0;j < fftl/2;j++)
        for(j = 0;j < 3*fftl/4;j++)
        {
            if(j+currentPosition >= xLen) break;
            synthesisOut[j+currentPosition] += impulseResponse[j] / fftl;
        }

        // 更新
        currentTime += 1.0/(f0[currentFrame] == 0.0 ? DEFAULT_F0 : f0[currentFrame]);
        currentFrame = (int)(currentTime/(framePeriod/1000.0) + 0.5);
        currentPosition = (int)(currentTime*(double)fs);
        if(j+currentPosition >= xLen || currentFrame >= tLen) break;
    }

    return;
}

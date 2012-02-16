/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file WorldSynthesis.h
 *  @brief Synthesis for WORLD data.
 *         Original source codes are below.
 *         http://www.aspl.is.ritsumei.ac.jp/morise/world/
 *         This code is redistributed to support thread safety with Qt Library.
 *  @author HAL@shurabaP
 */
#ifndef WORLDSYNTHESIS_H
#define WORLDSYNTHESIS_H

#include "World.h"

namespace stand
{
namespace math
{
namespace world
{

void oneFrameSegment(double *f0, double **specgram, double **residualSpecgram, int fftl, double framePeriod, double currentTime, int fs, double defaultF0,
                        FFTSet *forward, FFTSet *inverse, FFTSet *toWave);
void synthesis(double *f0, double **specgram, double **residual, int tLen, int fftl, double framePeriod, int fs,
               double *synthesisOut, int xLen);

}
}
}

#endif // WORLDSYNTHESIS_H

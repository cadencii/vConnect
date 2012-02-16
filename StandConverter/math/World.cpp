/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file World.cpp
 *  @author HAL@shurabaP
 */
#include "World.h"

#include <stdio.h>

using namespace stand::math::world;

void stand::math::world::extractResidual(fftw_complex *dst, const double *src, int fftLength)
{
    dst[0][0] = src[0];
    dst[0][1] = 0.0;
    for(int i = 1; i < fftLength / 2; i++) {
        dst[i][0] = src[2*i-1];
        dst[i][1] = src[2*i];
    }
    dst[fftLength/2][0] = src[fftLength-1];
    dst[fftLength/2][1] = 0.0;
}

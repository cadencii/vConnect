/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file World.h
 *  @brief Voice Analysis and Synthesis System WORLD.
 *         Original source codes are below.
 *         http://www.aspl.is.ritsumei.ac.jp/morise/world/
 *         This code is redistributed to support thread safety with Qt Library.
 *  @author HAL@shurabaP
 */
#ifndef WORLD_H
#define WORLD_H

#include <fftw3.h>

#include "Dio.h"
#include "Star.h"
#include "Platinum.h"
#include "MinimumPhaseSpectrum.h"
#include "WorldSynthesis.h"
#include "WorldSet.h"

namespace stand
{
namespace math
{
const double PI = 3.1415926535897932384;
namespace world
{
const unsigned int MAX_FFT_LENGTH = 2048;
const double FLOOR_F0 = 71.0;
const double DEFAULT_F0 = 150.0;
const double LOW_LIMIT = 65.0;

/// <summary>WORLDの非周期性指標をFFTWで使用する形へ展開します．</summary>
/// <param name="dst">FFTWの複素数配列．</param>
/// <param name="src">WORLD非周期性指標．</param>
/// <param name="fftLength">FFT長．</param>
void extractResidual(fftw_complex *dst, const double *src, int fftLength);


}
}
}

#endif // WORLD_H

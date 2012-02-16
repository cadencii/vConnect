/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file MinimumPhaseSpectrum.h
 *  @brief Functions that calculates minimum phase spectrum.
 *         Original source codes are below.
 *         http://www.aspl.is.ritsumei.ac.jp/morise/world/
 *         This code is redistributed to support thread safety with Qt Library.
 *  @author HAL@shurabaP
 */
#ifndef MINIMUMPHASESPECTRUM_H
#define MINIMUMPHASESPECTRUM_H

#include <fftw3.h>

namespace stand
{
namespace math
{
class FFTSet;
namespace world
{
fftw_complex *minimumPhaseSpectrum(double *inputSpec, int fftl, FFTSet *forward, stand::math::FFTSet *inverse);
}
}
}

#endif // MINIMUMPHASESPECTRUM_H

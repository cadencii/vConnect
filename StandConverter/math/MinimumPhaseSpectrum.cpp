/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file MinimumPhaseSpectrum.cpp
 *  @brief Functions that calculates minimum phase spectrum.
 *         Original source codes are below.
 *         http://www.aspl.is.ritsumei.ac.jp/morise/world/
 *         This code is redistributed to support thread safety with Qt Library.
 *  @author HAL@shurabaP
 */
#include "MinimumPhaseSpectrum.h"
#include "FFTSet.h"

#include <math.h>

using namespace stand::math;
using namespace stand::math::world;

fftw_complex *stand::math::world::minimumPhaseSpectrum(double *inputSpec, int fftl, FFTSet *forward, FFTSet *inverse)
{
    int i;

    fftw_complex *spectrum = (fftw_complex *)forward->in(), *dst = spectrum;
    fftw_complex *cepstrum = (fftw_complex *)inverse->in();
    fftw_complex *tmp = (fftw_complex *)forward->out();
    // 値を取り出す
    for(i = 0;i <= fftl/2;i++)
    {
        spectrum[i][0] = log(inputSpec[i] + 1.0e-55)/2.0; // add safe guard
        spectrum[i][1] = 0.0;
    }
    for(;i < fftl;i++)
    {
        spectrum[i][0] = spectrum[fftl-i][0];
        spectrum[i][1] = 0.0;
    }
    forward->execute();
    cepstrum[0][0] = tmp[0][0];
    cepstrum[0][1] = tmp[0][1];
    for(i = 1;i < fftl/2;i++)
    {
        cepstrum[i][0] = 0.0;
        cepstrum[i][1] = 0.0;
    }
    for(;i < fftl;i++)
    {
        cepstrum[i][0] = tmp[i][0] * 2.0;
        cepstrum[i][1] = tmp[i][1] * 2.0;
    }
    inverse->execute();
    spectrum = (fftw_complex *)inverse->out();
    for(i = 0;i < fftl;i++)
    {
        spectrum[i][0] /= (double)fftl;
        spectrum[i][1] /= (double)fftl;
        dst[i][0] = exp(spectrum[i][0])*cos(spectrum[i][1]);
        dst[i][1] = exp(spectrum[i][0])*sin(spectrum[i][1]);
    }
    return dst;
}

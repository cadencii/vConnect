/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file Star.h
 *  @brief Specgram estimation method STAR
 *         Original source codes are below.
 *         http://www.aspl.is.ritsumei.ac.jp/morise/world/
 *         This code is redistributed to support thread safety with Qt Library.
 *  @author HAL@shurabaP
 */
#ifndef STAR_H
#define STAR_H

namespace stand
{
namespace math
{
namespace world
{
/// <summary> Spectral envelop estimation by STAR </summary>
void star(const double *x, int xLen, int fs, double *timeAxis, double *f0, double **specgram);

int FFTLengthForStar(int fs);
}
}
}

#endif // STAR_H

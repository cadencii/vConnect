/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file Platinum.h
 *  @brief Excitation signal estimation method PLATINUM
 *         Original source codes are below.
 *         http://www.aspl.is.ritsumei.ac.jp/morise/world/
 *         This code is redistributed to support thread safety with Qt Library.
 *  @author HAL@shurabaP
 */
#ifndef PLATINUM_H
#define PLATINUM_H

namespace stand
{
namespace math
{
namespace world
{
void platinum(const double *x, int xLen, int fs, double *timeAxis, double *f0, double **specgram, double **residualSpecgram);
}
}
}

#endif // PLATINUM_H

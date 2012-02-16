/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file SmoothMatching.h
 *  @brief Functions that calculates matching between two arrays.
 *  @author HAL@shurabaP
 */
#ifndef SMOOTMATCHING_H
#define SMOOTMATCHING_H

namespace stand
{
namespace math
{
void smoothMatching(double* dst_to_src, double* src_to_dst, double* src_s, double* dst_s, int length);
void applyStretching(double *T, double* target, int length);
}
}

#endif // SMOOTMATCHING_H

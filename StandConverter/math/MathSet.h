/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file MathSet.h
 *  @author HAL@shurabaP
 */
#ifndef MATHSET_H
#define MATHSET_H

#include <math.h>

#include "FFTSet.h"
#include "MFCCSet.h"
#include "World.h"
#include "SmootMatching.h"

namespace stand
{
namespace math
{
/// <summary>配列の中間値を線形補間します．</summary>
/// <param name="x">配列のインデックス．</param>
/// <param name="p">配列</param>
/// <returns>配列の中間値を返します．</returns>
double interpolateArray(double x, const double *p);

/*!
 *  @brief 自己相関関数を計算する．
 *  @param[out] a 計算結果．
 *  @param[in] aLen 自己相関関数の長さ
 *  @param[in] y 信号
 *  @param[in] yLen 信号長
 */
void autoCorrelation(double *a, int aLen, const double *y, int yLen);

}
}

#endif // MATHSET_H

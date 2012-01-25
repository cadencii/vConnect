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

}
}

#endif // MATHSET_H

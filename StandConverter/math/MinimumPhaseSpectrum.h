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
void minimumPhaseSpectrum(double *inputSpec, int fftl, FFTSet *forward, stand::math::FFTSet *inverse);
}
}
}

#endif // MINIMUMPHASESPECTRUM_H
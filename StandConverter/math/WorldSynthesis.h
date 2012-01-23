#ifndef WORLDSYNTHESIS_H
#define WORLDSYNTHESIS_H

#include "World.h"

namespace stand
{
namespace math
{
namespace world
{

void synthesis(double *f0, double **specgram, double **residual, int tLen, int fftl, double framePeriod, int fs,
               double *synthesisOut, int xLen);

}
}
}

#endif // WORLDSYNTHESIS_H

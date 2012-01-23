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

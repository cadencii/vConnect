#ifndef MATLABFUNCTIONS_H
#define MATLABFUNCTIONS_H

namespace stand
{
namespace math
{
namespace world
{
// Matlab functions
double matlab_std(double *x, int xLen); //
void inv(double **r, int n, double **invr);
void fftfilt(double *x, int xlen, double *h, int hlen, int fftl, double *y);
float randn(void);
void histc(double *x, int xLen, double *y, int yLen, int *index);
void interp1(double *t, double *y, int iLen, double *t1, int oLen, double *y1);
long decimateForF0(const double *x, int xLen, double *y, int r);
void filterForDecimate(double *x, int xLen, double *y, int r);
int matlab_round(double x);
void diff(double *x, int xLength, double *ans);
void interp1Q(double x, double shift, double *y, int xLength, double *xi, int xiLength, double *ans);

}
}
}

#endif // MATLABFUNCTIONS_H

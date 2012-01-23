#include "MatlabFunctions.h"
#include "FFTSet.h"
#include "World.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

using namespace stand::math;
using namespace stand::math::world;

// Matlabから移植した関数の寄せ集め

// histc based on Matlab
// This function is hidden.
// length of i (Index) and y is the same.
void stand::math::world::histc(double *x, int xLen, double *y, int yLen, int *index)
{
    int i;
    int count = 1;

    for(i = 0;i < yLen;i++)
    {
        index[i] = 1;
        if(y[i] >= x[0])
            break;
    }
    for(;i < yLen;i++)
    {
        if(y[i] < x[count]) index[i] = count;
        else
        {
            index[i] = count++;
            i--;
        }
        if(count == xLen) break;
    }
    count--;
    for(i++;i < yLen;i++) index[i] = count;
}

// interp1 by using linear interpolation
// This function is based on Matlab function that has the same name
void stand::math::world::interp1(double *t, double *y, int iLen, double *t1, int oLen, double *y1)
{
    int i;
    double *h, *p, *s;
    int *k;
    h = (double *)malloc(sizeof(double) * (iLen-1));
    p = (double *)malloc(sizeof(double) * oLen);
    s = (double *)malloc(sizeof(double) * oLen);
    k = (int   *)malloc(sizeof(int)   * oLen);

    // 初期設定
    for(i = 0;i < iLen-1;i++) h[i] = t[i+1]-t[i];
    for(i = 0;i < oLen;i++) {p[i] = i; k[i] = 0;}

    histc(t, iLen, t1, oLen, k);

    for(i = 0;i < oLen;i++) s[i] = (t1[i] - t[k[i]-1]) / h[k[i]-1];

    for(i = 0;i < oLen;i++)
        y1[i] = y[k[i]-1] + s[i]*(y[k[i]] - y[k[i]-1]);

    free(k);
    free(s);
    free(p);
    free(h);
}



// decimate by using IIR and FIR filter coefficients
// x: Input signal whose length is xLen [sample]
// y: Output signal
long stand::math::world::decimateForF0(const double *x, int xLen, double *y, int r)
{
//	int r = 11;
    int nfact = 9; // 多分これは固定でOK
    double *tmp1, *tmp2;
    tmp1 = (double *)malloc(sizeof(double) * (xLen + nfact*2));
    tmp2 = (double *)malloc(sizeof(double) * (xLen + nfact*2));

    int i;
    for(i = 0;i < nfact;i++) tmp1[i] = 2*x[0] - x[nfact-i];
    for(i = nfact;i < nfact+xLen;i++) tmp1[i] = x[i-nfact];
    for(i = nfact+xLen;i < 2*nfact+xLen;i++) tmp1[i] = 2*x[xLen-1] - x[xLen-2 - (i-(nfact+xLen))];

    filterForDecimate(tmp1, 2*nfact+xLen, tmp2, r);
    for(i = 0;i < 2*nfact+xLen;i++) tmp1[i] = tmp2[2*nfact+xLen - i - 1];
    filterForDecimate(tmp1, 2*nfact+xLen, tmp2, r);
    for(i = 0;i < 2*nfact+xLen;i++) tmp1[i] = tmp2[2*nfact+xLen - i - 1];

    int nout = (int)(xLen/r) + 1;
    int nbeg = r - (r*nout - xLen);
    int count;

    for(i = nbeg, count = 0;i < xLen+nfact;i+=r, count++) y[count] = tmp1[i+nfact-1];

    free(tmp1); free(tmp2);
    return count;
}

// filter based on Matlab function
// x: Input signal whose length is xLen [sample]
// y: Output signal
void stand::math::world::filterForDecimate(double *x, int xLen, double *y, int r)
{
    double w[3], wt;
    w[0] = w[1] = w[2] = 0.0;
    double a[3], b[2]; // フィルタ係数 (r依存)

    switch(r)
    {
    case 11: // fs : 44100
        a[0] = 2.450743295230728;
        a[1] = -2.06794904601978;
        a[2] = 0.59574774438332101;
        b[0] = 0.0026822508007163792;
        b[1] = 0.0080467524021491377;
        break;
    case 12: // fs : 48000
        a[0] = 2.4981398605924205;
        a[1] = -2.1368928194784025;
        a[2] = 0.62187513816221485;
        b[0] = 0.0021097275904709001;
        b[1] = 0.0063291827714127002;
        break;
    case 8: // fs : 32000
        a[0] = 2.2357462340187593;
        a[1] = -1.7780899984041358;
        a[2] = 0.49152555365968692;
        b[0] = 0.0063522763407111993;
        b[1] = 0.019056829022133598;
        break;
    case 6: // fs : 24000 and 22050
        a[0] = 1.9715352749512141;
        a[1] = -1.4686795689225347;
        a[2] = 0.3893908434965701;
        b[0] = 0.013469181309343825;
        b[1] = 0.040407543928031475;
        break;
    case 4: // fs : 16000
        a[0] = 1.4499664446880227;
        a[1] = -0.98943497080950582;
        a[2] = 0.24578252340690215;
        b[0] = 0.036710750339322612;
        b[1] = 0.11013225101796784;
        break;
    case 2: // fs : 8000
        a[0] = 0.041156734567757189;
        a[1] = -0.42599112459189636;
        a[2] = 0.041037215479961225;
        b[0] = 0.16797464681802227;
        b[1] = 0.50392394045406674;
    }

    for(int i = 0;i < xLen;i++)
    {
        wt = x[i] + a[0]*w[0]
                  + a[1]*w[1]
                  + a[2]*w[2];

        y[i] = b[0]*wt
             + b[1]*w[0]
             + b[1]*w[1]
             + b[0]*w[2];

        w[2] = w[1];
        w[1] = w[0];
        w[0] = wt;
    }
}

// matlabに順ずる丸め
int stand::math::world::matlab_round(double x)
{
    if(x > 0)
        return (int)(x+0.5);
    else
        return (int)(x-0.5);
}

// 差分
void stand::math::world::diff(double *x, int xLength, double *ans)
{
    for(int i = 0;i < xLength-1;i++)
    {
        ans[i] = x[i+1] - x[i];
    }
    return;
}


// サンプリング間隔が等間隔に限定し高速に動作するinterp1．
// 基本的には同じだが，配列の要素数を明示的に指定する必要がある．
void stand::math::world::interp1Q(double x, double shift, double *y, int xLength, double *xi, int xiLength, double *ans)
{
    double deltaX;
    double *xiFraction, *deltaY;
    int *xiBase;
    int i;

    xiFraction = (double *)malloc(xiLength*sizeof(double));
    deltaY = (double *)malloc(xLength*sizeof(double));
    xiBase = (int *)malloc(xiLength*sizeof(int));

    deltaX = shift;
    for(i = 0;i < xiLength;i++)
    {
        xiBase[i] = (int)floor((xi[i] - x) / deltaX);
        xiFraction[i] = (double)(xi[i]-x)/deltaX - (double)xiBase[i];
    }
    diff(y, xLength, deltaY);
    deltaY[xLength-1] = 0.0;

    for(i = 0;i < xiLength;i++)
    {
        ans[i] = y[xiBase[i]] + deltaY[xiBase[i]]*xiFraction[i];
    }

    free(xiFraction);
    free(xiBase);
    free(deltaY);
}

// xorshift法と中心極限定理との組み合わせ
float stand::math::world::randn(void)
{
    static unsigned int x = 123456789;
    static unsigned int y = 362436069;
    static unsigned int z = 521288629;
    static unsigned int w = 88675123;
    unsigned int t;
    t = x ^ (x << 11);
    x = y; y = z; z = w;

    int i;
    unsigned int tmp;

    tmp = 0;
    for(i = 0;i < 12;i++)
    {
        t = x ^ (x << 11);
        x = y; y = z; z = w;
        w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
        tmp += w>>4;
    }
    return (float)tmp / 268435456.0f - 6.0f;
}

// fftfilt関数の移植
// yは，fftl分の長さを確保すること．
void stand::math::world::fftfilt(double *x, int xlen, double *h, int hlen, int fftl, double *y)
{
    int i;
    FFTSet forward1(fftl, FFTSet::FFTW_R2C);
    FFTSet forward2(fftl, FFTSet::FFTW_R2C);
    FFTSet inverse(fftl, FFTSet::FFTW_C2R);
    fftw_complex *specx, *spech;
    double *input1, *input2;

    input1 = (double *)      forward1.in();
    input2 = (double *)      forward2.in();
    specx  = (fftw_complex *)forward1.out();
    spech  = (fftw_complex *)forward2.out();
    fftw_complex *specx2 = (fftw_complex *)inverse.in();

    for(i = 0;i < xlen;i++) input1[i] = x[i]/(double)fftl;
    for(; i < fftl;i++) input1[i] = 0;
    for(i = 0;i < hlen;i++) input2[i] = h[i];
    for(; i < fftl;i++) input2[i] = 0;

    forward1.execute();
    forward2.execute();

    for(i = 0;i <= fftl/2;i++)
    {
        specx2[i][0] = specx[i][0]*spech[i][0] - specx[i][1]*spech[i][1];
        specx2[i][1] = specx[i][0]*spech[i][1] + specx[i][1]*spech[i][0];
    }
    inverse.execute();
    memcpy(y, inverse.out(), sizeof(double) * fftl);
}

// 2次元配列 (n*n)の逆行列を計算．メモリは確保しておくこと
void stand::math::world::inv(double **r, int n, double **invr)
{
    int i,j,k;
    double tmp;

    for(i = 0;i < n;i++)
    {
        for(j = 0;j < n;j++)
        {
            invr[i][j] = 0.0;
        }
    }
    for(i = 0;i < n;i++) invr[i][i] = 1.0;

    // 配列の初期化
    //
    for(i = 0;i < n;i++)
    {
        tmp = r[i][i]; r[i][i] = 1.0;
        for(j = 0;j <= i;j++) invr[i][j] /= tmp;
        for(;j < n;j++) r[i][j]	/= tmp;
        for(j = i+1;j < n;j++)
        {
            tmp = r[j][i];
            for(k = 0;k <= i;k++) invr[j][k] -= invr[i][k]*tmp;
            for(k--;k < n;k++) r[j][k] -= r[i][k]*tmp;
        }
    }

    // これで半分完了
    for(i = n-1;i >= 0;i--)
    {
        for(j = 0;j < i;j++)
        {
            tmp = r[j][i];
            for(k = 0;k < n;k++) invr[j][k] -= invr[i][k]*tmp;
        }
    }
}

double stand::math::world::matlab_std(double *x, int xLen)
{
    int i;
    double average, s;
    average = 0.0;
    for(i = 0;i < xLen;i++)
        average += x[i];
    average /= (double)xLen;

    s = 0.0;
    for(i = 0;i < xLen;i++)
        s += pow(x[i] - average, 2.0);
    s /= (double)(xLen-1);

    return sqrt(s);
}

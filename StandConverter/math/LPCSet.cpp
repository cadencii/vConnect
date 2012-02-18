/*!
 *  @file LPCSet.cpp
 *  @brief LPCSet supports LPC analysis,
 *         and keeps data as specgram.
 *         These functions are redistribution of SPTK.
 *         Original source codes are below.
 *         http://sp-tk.sourceforge.net/
 *  @author HAL@shurabaP
 */
#include "LPCSet.h"
#include "MathSet.h"
#include "FFTSet.h"

#include <math.h>
#include <float.h>

using namespace stand::math;

#ifdef WIN32
#define isnan(x) _isnan(x)
#endif

const double LPCSet::EPSILON = 1.0e-6;
const int LPCSet::DEFAULT_DIMENSION = 64;

LPCSet::LPCSet(const SpecgramSetting *s) :
    SpecgramSet(s)
{
    _dim = DEFAULT_DIMENSION;
}

LPCSet::~LPCSet()
{
}

void LPCSet::compute(const double *x, int xLen, const SpecgramSetting *s)
{
    int aLen = 32;
    if(!x || xLen <= 0)
    {
        return;
    }
    if(s)
    {
        _setting = *s;
    }

    _tLen = (double)xLen / _setting.fs * 1000.0 / _setting.framePeriod + 1;
    _fftl = 2048;
    _createSpecgram(_tLen, _fftl);

    FFTSet inverse(_fftl, FFTSet::FFTW_C2R);
    fftw_complex *in = (fftw_complex *)inverse.in();
    double *out = (double *)inverse.out();
    double *a = new double[aLen];

    for(int t = 0; t < _tLen; t++)
    {
        int begin = t * _setting.framePeriod / 1000.0 * _setting.fs - _fftl / 2;
        for(int j = 0; j < _fftl; j++)
        {
            int index = begin + j;
            if(index < 0)
            {
                index = 0;
            }
            else if(index >= xLen)
            {
                index = xLen - 1;
            }
            out[j] = x[index] * sin(PI * j / (double)_fftl);
        }
        lpc(a, aLen, out, _fftl);
        lpcToCepstrum(in, _fftl / 2 + 1, a, aLen);
        inverse.execute();
        for(int j = 0; j < _fftl / 2 + 1; j++)
        {
            _specgram[t][j] = exp(out[j]);
        }
    }
    delete[] a;
}

int LPCSet::dimension()
{
    return _dim;
}

void LPCSet::setDimension(int d)
{
    if(d <= 0)
    {
        return;
    }
    _dim = d;
}

void LPCSet::lpc(double *a, int aLen, const double *y, int yLen)
{
    double *r = new double[aLen + 1];
    double *c = new double[aLen + 1];

    // 自己相関関数を計算する
    autoCorrelation(r, aLen + 1, y, yLen);

    // LPC 係数を求める．
    double mue;
    double rmd = r[0];
    for(int i = 0; i < aLen; i++)
    {
        c[i] = 0;
    }
    a[0] = 0.0;
    for(int i = 1; i < aLen; i++)
    {
        mue = -r[i];
        for(int j = 1; j < i; j++)
        {
            mue -= c[j] * r[i - j];
        }
        mue = mue / rmd;

        for(int j = 1; j < i; j++)
        {
            a[j] = c[j] + mue * c[i - j];
        }
        a[i] = mue;
        rmd = (1.0 - mue * mue) * rmd;

        if(fabs(rmd) < EPSILON || isnan(rmd))
        {
            qDebug("LPCSet::lpc(); // Abnormal signal");
            for(int j = 0; j < aLen; i++)
            {
                a[i] = 1.0;
                return;
            }
        }
        if(fabs(mue) >= 1.0)
        {
            qDebug("LPCSet::lpc(); // Unstable lpc");
        }
        for(int j = 0; j <= i; j++)
        {
            c[j] = a[j];
        }
    }
    a[0] = sqrt(rmd);

    delete[] c;
    delete[] r;
}

void LPCSet::lpcToCepstrum(fftw_complex *c, int cLen, const double *a, int aLen)
{
    c[0][0] = log(a[0]);
    c[1][0] = -a[1];
    c[0][1] = c[1][1] = 0.0;
    for(int k = 2; k < cLen; k++)
    {
        int upper = (k > cLen) ? cLen + 1 : k;
        double d = 0;
        for(int i = (k > aLen) ? k - aLen : 1; i < upper; i++)
        {
            d += i * c[i][0] * a[k - i];
        }
        c[k][0] = -d / k;
        c[k][1] = 0.0;
        if(k <= aLen)
        {
            c[k][0] -= a[k];
        }
    }
}

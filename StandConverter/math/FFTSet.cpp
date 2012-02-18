/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file FFTSet.cpp
 *  @brief FFTSet class supports fft calculation
 *         and keep data for FFT.
 *         This code supports thread safety with Qt Library.
 *  @author HAL@shurabaP
 */
#include "FFTSet.h"
#include "World.h"

using namespace stand::math;

#ifdef USE_FFTW
QMutex FFTSet::_mutex;
#endif

FFTSet::FFTSet(unsigned int l, FFTtype type)
{
    _length = 0;
    _in = _out = NULL;
#ifdef USE_FFTW
    _plan = NULL;
    setFFT(l, type);
#else
    _sineTable = NULL;
    _sineTableSize = 0;
    setFFT(l, type);
#endif
}

FFTSet::~FFTSet()
{
    _destroy();
}

void FFTSet::_destroy()
{
    delete[] _in;
    delete[] _out;
    _in = _out = NULL;
    _length = 0;

#ifdef USE_FFTW
    if(_plan)
    {
        _mutex.lock();
        fftw_destroy_plan(_plan);
        _mutex.unlock();
    }
    _plan = NULL;
#else
    delete[] _sineTable;
    _sineTable = NULL;
    _sineTableSize = 0;
#endif
}

#ifdef USE_FFTW
void FFTSet::setFFT(unsigned int l, FFTtype type)
{
    _destroy();
    if(l == 0)
    {
        return;
    }
    _length = l;
    switch(_type = type)
    {
    case FFTW_R2R_FORWARD:
        _in  = (void *)(new double[l]);
        _out = (void *)(new double[l]);
        _mutex.lock();
        _plan = fftw_plan_r2r_1d(l, (double *)_in, (double *)_out, FFTW_REDFT11, FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
        _mutex.unlock();
        break;
    case FFTW_R2R_BACKWARD:
        _in  = (void *)(new double[l]);
        _out = (void *)(new double[l]);
        _mutex.lock();
        _plan = fftw_plan_r2r_1d(l, (double *)_in, (double *)_out, FFTW_REDFT11, FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
        _mutex.unlock();
        break;
    case FFTW_R2C:
        _in  = (void *)(new double[l]);
        _out = (void *)(new fftw_complex[l]);
        _mutex.lock();
        _plan = fftw_plan_dft_r2c_1d(l, (double *)_in, (fftw_complex *)_out, FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
        _mutex.unlock();
        break;
    case FFTW_C2R:
        _in  = (void *)(new fftw_complex[l]);
        _out = (void *)(new double[l]);
        _mutex.lock();
        _plan = fftw_plan_dft_c2r_1d(l, (fftw_complex *)_in, (double *)_out, FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
        _mutex.unlock();
        break;
    case FFTW_C2C_FORWARD:
        _in  = (void *)(new fftw_complex[l]);
        _out = (void *)(new fftw_complex[l]);
        _mutex.lock();
        _plan = fftw_plan_dft_1d(l, (fftw_complex *)_in, (fftw_complex *)_out, FFTW_FORWARD, FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
        _mutex.unlock();
        break;
    case FFTW_C2C_BACKWARD:
        _in  = (void *)(new fftw_complex[l]);
        _out = (void *)(new fftw_complex[l]);
        _mutex.lock();
        _plan = fftw_plan_dft_1d(l, (fftw_complex *)_in, (fftw_complex *)_out, FFTW_BACKWARD, FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
        _mutex.unlock();
        break;
    }
}
#else

int stand::math::sptkCheckm(int m)
{
    int k;

    for (k = 4; k <= m; k <<= 1) {
       if (k == m)
          return (0);
    }

    return (-1);
}

void FFTSet::setFFT(unsigned int length, FFTtype type)
{
    _destroy();
    if(length == 0)
    {
        return;
    }
    _length = length;
    switch(_type = type)
    {
    case FFTW_R2C:
        _in = (void *)(new double[length]);
        _out = (void *)(new fftw_complex[length]);
        _func = FFTSet::sptk_fftr;
        break;
    case FFTW_C2R:
        _in = (void *)(new fftw_complex[length]);
        _out = (void *)(new double[length]);
        _func = FFTSet::sptk_fft;
        break;
    case FFTW_C2C_FORWARD:
        _in = (void *)(new fftw_complex[length]);
        _out = (void *)(new fftw_complex[length]);
        _func = FFTSet::sptk_fft;
        break;
    case FFTW_C2C_BACKWARD:
        _in = (void *)(new fftw_complex[length]);
        _out = (void *)(new fftw_complex[length]);
        _func = FFTSet::sptk_ifft;
        break;
    }

}

int FFTSet::sptk_ifft(double *x, double *y, int m)
{
    int retval = sptk_fft(x, y, m);

    if(retval == -1)
    {
        return retval;
    }
    for(int i = 0; i < m; i++)
    {
        x[i] /= m;
        y[i] /= m;
    }
    return 0;
}

int FFTSet::sptk_ifftr(double *x, double *y, int m)
{
    int retval = sptk_fftr(x, y, m);
    if(retval == -1)
    {
        return retval;
    }
    for(int i = 0; i < m; i++)
    {

    }
}

int FFTSet::sptk_fftr(double *x, double *y, int m)
{
    if(sptk_fft(x, y, m / 2) == -1)
    {
        return -1;
    }
    int i, j;
    int maxfftsize = m;
    int mv2 = m / 2;
    int n = maxfftsize / m;
    double *sinp = _sineTable;
    double *cosp = _sineTable + maxfftsize / 4;

    double *xp = x;
    double *yp = y;
    double *xq = xp + m;
    double *yq = yp + m;
    *(xp + mv2) = *xp - *yp;
    *xp = *xp + *yp;
    *(yp + mv2) = *yp = 0;

    for (i = mv2, j = mv2 - 2; --i; j -= 2) {
       ++xp;
       ++yp;
       sinp += n;
       cosp += n;
       yt = *yp + *(yp + j);
       xt = *xp - *(xp + j);
       *(--xq) = (*xp + *(xp + j) + *cosp * yt - *sinp * xt) * 0.5;
       *(--yq) = (*(yp + j) - *yp + *sinp * yt + *cosp * xt) * 0.5;
    }

    xp = x + 1;
    yp = y + 1;
    xq = x + m;
    yq = y + m;

    for (i = mv2; --i;) {
       *xp++ = *(--xq);
       *yp++ = -(*(--yq));
    }
    return 0;
}

#endif

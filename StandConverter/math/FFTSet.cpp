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

using namespace stand::math;

QMutex FFTSet::_mutex;

FFTSet::FFTSet(unsigned int l, FFTtype type)
{
    _length = 0;
    _in = _out = NULL;
    _plan = NULL;
    setFFT(l, type);
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
    if(_plan)
    {
        _mutex.lock();
        fftw_destroy_plan(_plan);
        _mutex.unlock();
    }
    _plan = NULL;
}

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

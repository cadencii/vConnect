/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file FFTSet.h
 *  @brief FFTSet class supports fft calculation
 *         and keep data for FFT.
 *         This code supports thread safety with Qt Library.
 *  @author HAL@shurabaP
 */
#ifndef FFTSET_H
#define FFTSET_H

#include <QMutex>
#include "../stand.h"

#ifdef USE_FFTW
#include <fftw3.h>
#else
typedef double fftw_complex[2];
#endif

namespace stand
{
namespace math
{
/*!
 *  @brief FFTSet class supports FFT by FFTW.
 */
class FFTSet
{
public:
    //! @brief FFTType
    enum FFTtype
    {
        FFTW_UNKNOWN = -1,
        FFTW_R2R_FORWARD,
        FFTW_R2R_BACKWARD,
        FFTW_R2C,
        FFTW_C2R,
        FFTW_C2C_FORWARD,
        FFTW_C2C_BACKWARD
    };

    FFTSet(unsigned int length, FFTtype type);
    ~FFTSet();

    void setFFT(unsigned int length, FFTtype type);


    unsigned int length() const
    {
        return _length;
    }

    void *in()
    {
        return _in;
    }

    void *out()
    {
        return _out;
    }

    void execute()
    {
#ifdef USE_FFTW
        fftw_execute(_plan);
#else
        _func((double *)_in, (double *)_out, _length);
#endif
    }

private:
    void _destroy();

    FFTtype _type;
    void *_in;
    void *_out;
    unsigned int _length;

#ifdef USE_FFTW
    fftw_plan _plan;
    static QMutex _mutex;
#else
    int (*_func)(double *, double *, int);
    static int sptk_fft(double *x, double *y, int m);
    static int sptk_fftr(double *x, double *y, int m);
    static int sptk_ifft(double *x, double *y, int m);
    static int sptk_ifftr(double *x, double *y, int m);
    double *_sineTable;
    int _sineTableSize;
#endif
};

}
}

#endif // FFTSET_H

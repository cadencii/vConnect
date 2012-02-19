#ifndef CONFIGURE_H
#define CONFIGURE_H

/*
 * use FFTW or SPTK FFT ?
 * If you want to use FFTW, define USE_FFTW.
 * otherwise, undef USE_FFTW.
 */
#define USE_FFTW

namespace stand
{
const double A4Frequency = 440.0;
const int A4NoteNumber = 69;
const double DefaultFramePeriod = 2.0;
}

#endif // CONFIGURE_H

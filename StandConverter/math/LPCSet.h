/*!
 *  @file LPCSet.h
 *  @brief LPCSet supports LPC analysis,
 *         and keeps data as specgram.
 *         These functions are redistribution of SPTK.
 *         Original source codes are below.
 *         http://sp-tk.sourceforge.net/
 *  @author HAL@shurabaP
 */
#ifndef LPCSET_H
#define LPCSET_H

#include "SpecgramSet.h"
#include "FFTSet.h"

namespace stand
{
namespace math
{

class LPCSet : public SpecgramSet
{
public:
    explicit LPCSet(const SpecgramSetting *s = &SPECGRAM_DEFAULT_SETTING);
    ~LPCSet();

    void compute(const double *x, int xLen, const SpecgramSetting *s = NULL);

    int dimension();
    void setDimension(int d);

    static void lpcToCepstrum(fftw_complex *c, int cLen, const double *a, int aLen);
    static void lpc(double *a, int aLen, const double *y, int yLen);
private:
    int _dim;

    const static double EPSILON;
    const static int DEFAULT_DIMENSION;
};

}
}

#endif // LPCSET_H

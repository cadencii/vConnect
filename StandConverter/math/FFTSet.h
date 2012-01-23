#ifndef FFTSET_H
#define FFTSET_H

#include <QMutex>
#include <fftw3.h>

namespace stand
{
namespace math
{
class FFTSet
{
public:
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
        fftw_execute(_plan);
    }

private:
    void _destroy();

    static QMutex _mutex;

    FFTtype _type;
    void *_in;
    void *_out;
    unsigned int _length;

    fftw_plan _plan;
};

}
}

#endif // FFTSET_H

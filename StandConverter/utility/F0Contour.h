#ifndef F0CONTOUR_H
#define F0CONTOUR_H

#include "../configure.h"

namespace stand
{
namespace utility
{

class F0Contour
{
public:
    explicit F0Contour(double *f0 = 0, int length = 0, double framePeriod = 0);

    const double *f0() const
    {
        return _f0;
    }

    int length() const
    {
        return _length;
    }

    double framePeriod() const
    {
        return _framePeriod;
    }

    double at(double ms);

    void set(double *f0, int length, double framePeriod);

private:
    double *_f0;
    int _length;
    double _framePeriod;
};

}
}

#endif // F0CONTOUR_H

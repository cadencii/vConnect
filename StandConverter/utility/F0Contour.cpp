#include "F0Contour.h"

// for debug
#include <qglobal.h>

using namespace stand::utility;

F0Contour::F0Contour(double *f0, int length, double framePeriod)
{
    set(f0, length, framePeriod);
}

void F0Contour::set(double *f0, int length, double framePeriod)
{
    if(f0)
    {
        _f0 = f0;
        _length = length;
        _framePeriod = framePeriod;
        if(_framePeriod <= 0.0)
        {
            _framePeriod = stand::DefaultFramePeriod;
        }
    }
}

double F0Contour::at(double ms)
{
    if(!_f0)
    {
        return -1.0;
    }
    int index = ms / _framePeriod;
    if(index < 0)
    {
        index = 0;
    }
    else if(index >= _length)
    {
        index = _length - 1;
    }
    return _f0[index];
}

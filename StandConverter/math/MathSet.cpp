#include "MathSet.h"

double stand::math::interpolateArray( double x, const double *p )
{
    int t = (int)x;
    double r = x - (double)t;
    return ( p[t] * ( 1.0 - r ) + p[t+1] * r );
}

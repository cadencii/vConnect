/*!
 * Stand Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License
 *
 * Stand Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @file MathSet.cpp
 *  @author HAL@shurabaP
 */
#include "MathSet.h"

double stand::math::interpolateArray( double x, const double *p )
{
    int t = (int)x;
    double r = x - (double)t;
    return ( p[t] * ( 1.0 - r ) + p[t+1] * r );
}

void stand::math::autoCorrelation(double *a, int aLen, const double *y, int yLen)
{
    for(int i = 0; i < aLen; i++)
    {
        a[i] = 0;
        for(int j = 0; j < yLen -i; j++)
        {
            a[i] += y[j] * y[j + i];
        }
    }
}

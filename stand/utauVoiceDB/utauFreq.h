/*
 *
 *    utauFreq.h
 *                              (c) HAL 2010-           
 *
 *  This file is a part of v.Connect.
 * utauFreq contains UTAU frequency table.
 * It provides convert function between
 * WORLD pitch contour and UTAU one.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __utauFreq_h__
#define __utauFreq_h__

#include "../stand.h"

class utauFreq{
public:
    bool readFrqFile( string_t input );
    void getF0Contour( double* f0, double msLeftBlank, double msRightBlank, int length );
    void getDynamics( double* dynamics, double msLeftBlank, double msRightBlank, int length );
protected:
    vector<double> f0;
    vector<double> volume;
    int        sampleInterval;
    double    averageFrequency;
    string    header;
};


#endif

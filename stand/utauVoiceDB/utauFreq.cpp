/*
 *
 *    utauFreq.cpp
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
#include "utauFreq.h"

#define BUFLEN 80

bool utauFreq::readFrqFile( string input )
{
    bool ret = false;
    FILE* fp;
    char buf[BUFLEN];
    int numPoints;
    double f,v;
    
    string s;
    mb_conv( input, s );
    fp = fopen( s.c_str(), "rb" );
    if( fp ){
        fread( buf, sizeof(char), 8, fp );
        buf[8] = '\0';
        header = buf;
        fread( (void*)&sampleInterval, sizeof(int), 1, fp );
        fread( (void*)&averageFrequency, sizeof(double), 1, fp );
        fread( buf, sizeof(char), 16, fp );    // 空白文字
        fread( (void*)&numPoints, sizeof(int), 1, fp );

        f0.resize( numPoints, 0.0 ); volume.resize( numPoints, 0.0 );

        for( int i = 0; i < numPoints; i++ ){
            fread( &f, sizeof(double), 1, fp );
            fread( &v, sizeof(double), 1, fp );
            f0[i] = f; volume[i] = v;
            if( f0[i] < 56.0 )
                f0[i] = 0.0;
        }
        fclose( fp );
        ret = true;
    }

    return ret;
}

void utauFreq::getF0Contour( double* f0, double msLeftBlank, double msRightBlank, int length )
{
    double x;
    double step = (double)fs * framePeriod / 1000.0 / (double)sampleInterval;
    x = msLeftBlank / 1000.0 * (double)fs / (double)sampleInterval;

    for( long i = 0; i < length; i++ ){
        if( 0 <= x && x < this->f0.size() )
            f0[i] = this->f0[(long)x];
        else
            f0[i] = 0.0;
        x += step;
    }
}

void utauFreq::getDynamics( double* dynamics, double msLeftBlank, double msRightBlank, int length )
{
    double x;
    double step = (double)fs * framePeriod / 1000.0 / (double)sampleInterval;
    x = msLeftBlank / 1000.0 * (double)fs / (double)sampleInterval;

    for( long i = 0; i < length; i++ ){
        if( 0 <= x && x < volume.size() )
            dynamics[i] = volume[(long)x];
        else
            dynamics[i] = 0.0;
        x += step;
    }
}


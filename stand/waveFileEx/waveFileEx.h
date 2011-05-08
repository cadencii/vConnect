/*
 *
 *    waveFileEx.cpp                                         
 *                              (c) HAL 2010-           
 *
 *  This file is a part of STAND Library.
 * STAND Library is a wrapper library of WORLD.
 * It provides I/O functions for wave files.
 * This class contains 64-bit wave buffer.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#ifndef __waveFileEx_h__
#define __waveFileEx_h__

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <string.h>

using namespace std;

#include "waveFormatEx.h"

class waveFileEx{
public:
    waveFileEx();
    ~waveFileEx(){destroy();}
    /* If the data is stereo, only left-channel will be read. */
    int    readWaveFile( string fileName );
    int    writeWaveFile( string fileName );

    int    setWaveBuffer( vector<double>& srcBuffer );
    int    setWaveBuffer( double* srcBuffer, unsigned long bufferLength );

    int    getWaveBuffer( vector<double>& dstBuffer );
    int    getWaveBuffer( double* dstBuffer, unsigned long* bufferLength );

    int    getWaveBuffer( vector<double>& dstBuffer, double leftBlank, double rightBlank );
    int    getWaveBuffer( double *dstBuffer, double leftBlank, double rightBlank, int length );

    long   getWaveLength( void ){ return waveLength; }
    long   getWaveLength( double leftBlank, double rightBlank );

    int    setOffset( double secOffset );
    void   normalize( void );
    void   applyDynamics( vector<double>& dynamics, int sample_rate, double frame_period );

protected:
private:
    void        destroy(void){delete[] waveBuffer; waveLength = 0; secOffset = 0.0;}
    void        createBuffer(int length){destroy(); waveBuffer = new double[length]; waveLength = length;}
    bool        readWaveHeader( FILE* fp );
    bool        readWaveData( FILE* fp );

    void        setDefaultFormat( void );

    static void outputError( string s );

    double          *waveBuffer;
    int             waveLength;
    // 空白時間を持つことにした．
    double          secOffset;
    waveFormatEx    format;
};

#endif

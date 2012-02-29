/*
 *
 *    WaveBuffer.h
 *                              (c) HAL 2010-, kbinani 2012
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

#ifndef __WaveBuffer_h__
#define __WaveBuffer_h__

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

namespace vconnect
{
    class RiffChunk;
}

class WaveBuffer{
public:
    struct Format {
        int16_t  formatID;
        uint16_t numChannels;
        uint32_t samplePerSecond;
        uint32_t bytesPerSecond;
        uint16_t blockAlign;
        uint16_t bitsPerSample;
    };

    WaveBuffer();
    ~WaveBuffer(){destroy();}

    /* If the data is stereo, only left-channel will be read. */
    int    readWaveFile( const std::string &fileName );
    int    writeWaveFile( const std::string &fileName );
    static int writeWaveFile( const std::string &fileName, const double *wave, int length, double secOffset = 0.0, const Format *format = NULL );

    int    setWaveBuffer( std::vector<double>& srcBuffer );
    int    setWaveBuffer( double* srcBuffer, unsigned long bufferLength );

    int    getWaveBuffer( std::vector<double>& dstBuffer );
    int    getWaveBuffer( double* dstBuffer, unsigned long* bufferLength );

    int    getWaveBuffer( std::vector<double>& dstBuffer, double leftBlank, double rightBlank );
    int    getWaveBuffer( double *dstBuffer, double leftBlank, double rightBlank, int length );

    int    getSamplingFrequency() { return format.samplePerSecond; }

    double *getWavePointer(int *length)
    {
        if(length)
        {
            *length = waveLength;
        }
        return waveBuffer;
    }

    long   getWaveLength( void ){ return waveLength; }
    long   getWaveLength( double leftBlank, double rightBlank );

    int    setOffset( double secOffset );
    void   normalize( void );
    void   applyDynamics( std::vector<double>& dynamics, int sample_rate, double frame_period );

protected:
private:
    void        destroy(void){delete[] waveBuffer; waveLength = 0; secOffset = 0.0;}
    void        createBuffer(int length){destroy(); waveBuffer = new double[length]; waveLength = length;}

    void _setHeader(const vconnect::RiffChunk *fmt);
    void _setData(const vconnect::RiffChunk *data);

    void        setDefaultFormat( void );

    static void outputError( std::string s );

    double          *waveBuffer;
    int             waveLength;
    // 空白時間を持つことにした．
    double          secOffset;
    Format    format;

    static const Format DefaultFormat;
};

#endif

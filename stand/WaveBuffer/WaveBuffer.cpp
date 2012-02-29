/*
 *
 *    WaveBuffer.cpp                                         
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
#include "WaveBuffer.h"

#include "RiffFile.h"
#include "stdint.h"

using namespace std;
using namespace vconnect;

const WaveBuffer::Format WaveBuffer::DefaultFormat =
{
    1,
    1,
    44100,
    88200,
    2,
    16,
};

void WaveBuffer::outputError( string s ){
    cout << s << endl;
}

WaveBuffer::WaveBuffer()
{
    setDefaultFormat();
    waveLength = 0;
    waveBuffer = NULL;
    secOffset = 0.0;
}

void WaveBuffer::setDefaultFormat( void )
{
    /* WaveFileFormat will be 16 bits, 44,100Hz, mono Liniar PCM */
    format = DefaultFormat;
}

int WaveBuffer::getWaveBuffer( vector<double>& dstBuffer )
{
    int result = 0;

    if( waveBuffer && waveLength > 0 ){
        dstBuffer.resize( waveLength );
        for( unsigned long i = 0; i < waveLength; i++ ){
            dstBuffer[i] = waveBuffer[i];
        }
        result = 1;
    }
    return result;
}

int WaveBuffer::getWaveBuffer( double* dstBuffer, unsigned long* bufferLength )
{
    int result = 0;

    if( dstBuffer && bufferLength && waveBuffer && waveLength > 0 ){
        int length = (*bufferLength <waveLength ) ? *bufferLength : waveLength;

        memcpy(dstBuffer, waveBuffer, sizeof(double) * length);
        result = 1;
    }
    return result;
}

int WaveBuffer::setWaveBuffer( vector<double>& srcBuffer )
{
    int result = 0;

    if( !( srcBuffer.empty() ) ){
        createBuffer(srcBuffer.size());
        for( unsigned long i = 0; i < waveLength; i++ ){
            waveBuffer[i] = srcBuffer[i];
        }
        result = 1;
    }
    return result;
}

int WaveBuffer::setWaveBuffer( double* srcBuffer, unsigned long bufferLength )
{
    int result = 0;
    
    if( srcBuffer && bufferLength > 0 ){
        createBuffer(bufferLength);
        memcpy(waveBuffer, srcBuffer, sizeof(double) * bufferLength);
        result = 1;
    }
    return result;
}

int WaveBuffer::readWaveFile( const string &fileName )
{
    RiffFile f;
    if(!f.read(fileName.c_str()))
    {
        return 0;
    }
    if(!f.is("WAVE"))
    {
        outputError(" WaveBuffer::readWaveFile(" + fileName + "); // This is not WAVE file");
        return 0;
    }
    const RiffChunk *c;
    if((c = f.chunk("fmt ")) == NULL)
    {
        outputError(" WaveBuffer::readWaveFile(" + fileName + "); // \"fmt \" chunk not found");
        return 0;
    }
    _setHeader(c);


    if((c = f.chunk("data")) == NULL)
    {
        outputError(" WaveBuffer::readWaveFile(" + fileName + "); // \"data\" chunk not found");
        return 0;
    }
    _setData(c);


    return 1;
}

void WaveBuffer::_setHeader(const RiffChunk *fmt)
{
    if(fmt->length() < 16)
    {
        format = DefaultFormat;
    }
    else
    {
        memcpy(&format, fmt->data(), sizeof(Format));
    }
}

void WaveBuffer::_setData(const RiffChunk *data)
{
    int l = data->length() / format.numChannels / (format.bitsPerSample / 8);
    createBuffer(l);
    switch(format.bitsPerSample)
    {
    case 8:
        {
            for(int i = 0; i < data->length(); i += format.numChannels)
            {
                waveBuffer[i / format.numChannels] = (data->data()[i] - 128) / 128.0;
            }
        }
        break;
    case 16:
        {
            const int16_t *p = (int16_t *)(data->data());
            for(int i = 0; i < data->length() / 2; i += format.numChannels)
            {
                waveBuffer[i / format.numChannels] = p[i] / (double)(1 << 15);
            }
        }
        break;
    case 24:
        {
            for(int i = 0; i < data->length(); i += format.numChannels * (24 / 8))
            {
                int32_t val = 0;
                char *p = (char *)(&val);
                p[3] = data->data()[i + 2];
                p[2] = data->data()[i + 1];
                p[1] = data->data()[i + 0];
                waveBuffer[i / 3 / format.numChannels] = val / (double)(1 << 31);
            }
        }
        break;
    case 32:
        {
            const int32_t *p = (int32_t *)(data->data());
            for(int i = 0; i < data->length() / 4; i += format.numChannels)
            {
                waveBuffer[i / format.numChannels] = data->data()[i] / (double)(1 << 31);
            }
        }
        break;
    default:
        return;
    }
}


int    WaveBuffer::writeWaveFile(const string &fileName, const double *wave, int length, double secOffset, const Format *format )
{
    int result = 2;
    FILE *fp;

    if( !wave || length <= 0)
    {
        return 0;
    }

    if( !format )
    {
        format = &DefaultFormat;
    }

#ifdef __GNUC__
    fp = fopen( fileName.c_str(), "wb" );
#else
    fopen_s( &fp, fileName.c_str(), "wb" );
#endif
    
    if( fp )
    {
        /* This code may occur an error on big-endian CPU. */
        /* write Header */
        unsigned int offset = (unsigned int)(secOffset * format->samplePerSecond);
        unsigned int waveSize = (offset + length) * ( format->bitsPerSample / 8 ) * format->numChannels;
        unsigned int fileSize = waveSize + 44;
        unsigned int chunkSize = 16;

        short *data = new short[length + offset];
        for(int i = 0; i < offset; i++)
        {
            data[i] = 0;
        }
        for(unsigned long i = 0, j = offset; i < length; i++, j++)
        {
            data[j] = (short)( 32767.0 * wave[i] ); 
        }

        fprintf( fp, "RIFF" );
        fwrite( (void*)&fileSize, 4, 1, fp );
        fprintf( fp, "WAVEfmt " );
        fwrite( (void*)&chunkSize, 4, 1, fp );
        fwrite( (void*)&(format->formatID), 2, 1, fp );
        fwrite( (void*)&(format->numChannels), 2, 1, fp );
        fwrite( (void*)&(format->samplePerSecond), 4, 1, fp );
        fwrite( (void*)&(format->bytesPerSecond), 4, 1, fp );
        fwrite( (void*)&(format->blockAlign), 2, 1, fp );
        fwrite( (void*)&(format->bitsPerSample), 2, 1, fp );
        fprintf( fp, "data" );

        fwrite( (void*)&waveSize, 4, 1, fp );        
        fwrite( (void*)data, 2, length + offset, fp);

        delete[] data;

        result = 1;
        fclose( fp );
    }
    return result;
}

int    WaveBuffer::writeWaveFile(const string &fileName )
{
    return writeWaveFile( fileName, waveBuffer, waveLength, secOffset, &format);
}

void    WaveBuffer::normalize( void )
{
    double temp;
    double max = 1.0;
    for( unsigned long i = 0; i < waveLength; i++ ){
        temp = fabs( waveBuffer[i] );
        if( temp > max ){
            max = temp;
        }
    }
    if( max != 0.0 ){
        for( unsigned long i = 0; i < waveLength; i++ ){
            waveBuffer[i] /= max;
        }
    }
}

void    WaveBuffer::applyDynamics( vector<double>& dynamics, int sample_rate, double framePeriod ){
    long frameIndex;
    double rate;

    for( unsigned long index = 0; index < waveLength; index++ ){
        rate = (double)index / (framePeriod * (double)sample_rate / 1000.0);
        frameIndex = (long)rate;
        rate -= (double)frameIndex;
        if( frameIndex < (long)dynamics.size() - 1 ){
            waveBuffer[index] *= ( 1.0 - rate ) * dynamics[frameIndex] + rate * dynamics[frameIndex+1];
        }else if( frameIndex == dynamics.size() - 1 ){
            waveBuffer[index] *= dynamics[frameIndex];
        }else{
            waveBuffer[index] *= 1.0;
        }
    }
}

int    WaveBuffer::getWaveBuffer( vector<double>& dstBuffer, double leftBlank, double rightBlank )
{
    int ret = 0;
    if( waveLength != 0 ){
        long beginFrame = (long)( (leftBlank / 1000.0) * format.samplePerSecond );
        long endFrame;

        if( rightBlank < 0 ){
            endFrame = beginFrame + (long)( (-rightBlank / 1000.0) * format.samplePerSecond );
        }else{
            endFrame = waveLength - (long)( rightBlank / 1000.0 * format.samplePerSecond );
        }

        if( endFrame > beginFrame ){
            dstBuffer.resize( endFrame - beginFrame );
        }

        for( long i = beginFrame; i < endFrame; i++ ){
            if( 0 <= i && i < (long)waveLength ){
                dstBuffer[i-beginFrame] = waveBuffer[i];
            }else{
                dstBuffer[i-beginFrame] = 0;
            }
        }
        ret = 1;
    }

    return ret;
}

long   WaveBuffer::getWaveLength( double leftBlank, double rightBlank )
{
    long length = (long)( leftBlank / 1000.0 * (double)format.samplePerSecond );
    long endIndex;
    if( rightBlank < 0 ){
        endIndex = length - (long)( rightBlank / 1000.0 * (double)format.samplePerSecond );
    } else {
        endIndex = waveLength - (long)( rightBlank / 1000.0 * (double)format.samplePerSecond );
    }
    return ( endIndex - length );
}

int    WaveBuffer::getWaveBuffer( double *dstBuffer, double leftBlank, double rightBlank, int length )
{
    int ret = 0;
    if( !length || !dstBuffer ) return ret;

    if( waveLength != 0 ){
        long beginIndex = (long)( (leftBlank / 1000.0) * format.samplePerSecond );
        long endIndex;

        if( rightBlank < 0 ){
            endIndex = beginIndex + (long)( (-rightBlank / 1000.0) * format.samplePerSecond );
        }else{
            endIndex = waveLength - (long)( rightBlank / 1000.0 * format.samplePerSecond );
        }

        if( endIndex < beginIndex && endIndex - beginIndex < 0 ){
            return ret;
        }

        int i, cpyLength;
        endIndex = min<int>(waveLength, endIndex);
        cpyLength = min<int>(waveLength, endIndex - beginIndex);

        for( i = beginIndex; i < 0; i++){
            dstBuffer[i-beginIndex] = 0.0;
        }

        memcpy(&dstBuffer[i-beginIndex], waveBuffer + i, sizeof(double) * cpyLength);
        i += cpyLength;

        for( ; i < length; i++){
            dstBuffer[i-beginIndex] = 0.0;
        }
        ret = 1;
    }

    return ret;
}


int WaveBuffer::setOffset( double secOffset )
{
    int ret = 1;

    this->secOffset = secOffset;

    return ret;    
}

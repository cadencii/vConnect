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
#include "waveFileEx.h"

const waveFileEx::waveFormatEx waveFileEx::defaultFormat =
{
    1,
    0,
    1,
    1,
    44100,
    88200,
    2,
    16,
};

void waveFileEx::outputError( string s ){
    cout << s << endl;
}

waveFileEx::waveFileEx()
{
    setDefaultFormat();
    waveLength = 0;
    waveBuffer = NULL;
    secOffset = 0.0;
}

void waveFileEx::setDefaultFormat( void )
{
    /* WaveFileFormat will be 16 bits, 44,100Hz, mono Liniar PCM */
    format.bitsPerSample = 16;
    format.blockAlign = 2;
    format.bytesPerSecond = 88200;
    format.chunkID = 1;    /* Linear PCM */
    format.formatTag = 1;
    format.numChannels = 1;
    format.samplePerSecond = 44100;
}

int waveFileEx::getWaveBuffer( vector<double>& dstBuffer )
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

int waveFileEx::getWaveBuffer( double* dstBuffer, unsigned long* bufferLength )
{
    int result = 0;

    if( dstBuffer && bufferLength && waveBuffer && waveLength > 0 ){
        int length = (*bufferLength <waveLength ) ? *bufferLength : waveLength;

        memcpy(dstBuffer, waveBuffer, sizeof(double) * length);
        result = 1;
    }
    return result;
}

int waveFileEx::setWaveBuffer( vector<double>& srcBuffer )
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

int waveFileEx::setWaveBuffer( double* srcBuffer, unsigned long bufferLength )
{
    int result = 0;
    
    if( srcBuffer && bufferLength > 0 ){
        createBuffer(bufferLength);
        memcpy(waveBuffer, srcBuffer, sizeof(double) * bufferLength);
        result = 1;
    }
    return result;
}

int waveFileEx::readWaveFile( string fileName ){
    int result = 2;
    FILE *fp;

#ifdef __GNUC__
    fp = fopen( fileName.c_str(), "rb" );
#else
    fopen_s( &fp, fileName.c_str(), "rb" );
#endif

    if( fp ){
        if( readWaveHeader( fp ) ){
            if( readWaveData( fp ) ){
                result = 1;
            }else{
                result = -1;
            }
        }else{
            result = 0;
        }
        fclose( fp );
    }

    return result;
}

bool    waveFileEx::readWaveHeader( FILE *fp )
{
    bool result = false;
    char buffer[5];
    string temp;
    unsigned int fileSize;
    waveFormatEx formatChunk;

    /* read 4 byte for RIFF chunk */
    fread( (void*)buffer, sizeof(char), 4, fp );
    buffer[4] = '\0';
    temp = buffer;

    if( temp.compare( "RIFF" ) == 0 ){
        /* read format chunk */
        fread( (void*)&fileSize, sizeof(int), 1, fp );
        fread( (void*)buffer, sizeof(char), 4, fp ); 
        buffer[4] = '\0';
        temp = buffer;
        if( temp.compare( "WAVE" ) == 0 ){
            fread( (void*)buffer, sizeof(char), 4, fp ); 
            buffer[4] = '\0';
            temp = buffer;
            if( temp.compare( "fmt " ) == 0 ){
                int chunkSize;
                fread( (void*)&chunkSize, 4, 1, fp );
                fread( (void*)&(formatChunk.chunkID), 2, 1, fp );
                fread( (void*)&(formatChunk.numChannels), 2, 1, fp );
                fread( (void*)&(formatChunk.samplePerSecond), 4, 1, fp );
                fread( (void*)&(formatChunk.bytesPerSecond), 4, 1, fp );
                fread( (void*)&(formatChunk.blockAlign), 2, 1, fp );
                fread( (void*)&(formatChunk.bitsPerSample), 2, 1, fp );
                if( chunkSize > 16 ){
                    for( int i = 0; i < 16 - chunkSize; i++ ){
                        fgetc( fp );
                    }
                }

                if( formatChunk.bitsPerSample == 16 && formatChunk.chunkID == 1 ){
                    result = true;
                    memcpy( &format, &formatChunk, sizeof(waveFormatEx) );
                } else {
                    outputError( "WaveFileEx: WaveFileEx can use only for 16bits PCM Audio" );
                }
            }
        }
    }
    return result;
}

bool    waveFileEx::readWaveData( FILE* fp )
{
    bool result = false;
    char buffer[5];
    string temp;
    int dataSize;
    unsigned long maxNum = 0;

    // その場しのぎ．
    buffer[0] = 0;
    while( fread( (void*)buffer, sizeof(char), 1, fp ) == 1 )  {
        if( buffer[0] == 'd' ){
            fread( (void*)(buffer+1),sizeof(char),3,fp );
            break;
        }
    }

//    fread( (void)*buffer, sizeof(char), 4, fp );
    buffer[4] = '\0';
    temp = buffer;

    if( temp.compare( "data" ) == 0 ){
        char *data;
        fread( (void*)&dataSize, sizeof(int), 1, fp );
        maxNum = dataSize / format.numChannels / ( format.bitsPerSample / 8 );
        data = new char[dataSize];
        fread(data, sizeof(char), dataSize, fp);

        createBuffer(maxNum);

        for(unsigned int index = 0, waveIndex = 0; waveIndex < maxNum; index += format.numChannels, waveIndex++)
        {
            // short 固定．いずれ拡張したいけど．．．
            int value = ((short*)data)[index];
            // これ半分になってね？とりあえず今は放置．
            waveBuffer[waveIndex] = (double)value / (double)(1 << format.bitsPerSample);
        }

        delete[] data;

        result = true;
    }else{
        outputError( "This wave file does not contain any data!" );
    }

    return result;
}

int    waveFileEx::writeWaveFile( string fileName, const double *wave, int length, double secOffset, const waveFormatEx *format )
{
    int result = 2;
    FILE *fp;

    if( !wave || length <= 0)
    {
        return 0;
    }

    if( !format )
    {
        format = &defaultFormat;
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
        unsigned int offset = secOffset * format->samplePerSecond;
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
        fwrite( (void*)&(format->chunkID), 2, 1, fp );
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

int    waveFileEx::writeWaveFile( string fileName )
{
    return writeWaveFile( fileName, waveBuffer, waveLength, secOffset, &format);
}

void    waveFileEx::normalize( void )
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

void    waveFileEx::applyDynamics( vector<double>& dynamics, int sample_rate, double framePeriod ){
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

int    waveFileEx::getWaveBuffer( vector<double>& dstBuffer, double leftBlank, double rightBlank )
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

long   waveFileEx::getWaveLength( double leftBlank, double rightBlank )
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

int    waveFileEx::getWaveBuffer( double *dstBuffer, double leftBlank, double rightBlank, int length )
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


int waveFileEx::setOffset( double secOffset )
{
    int ret = 1;

    this->secOffset = secOffset;

    return ret;    
}

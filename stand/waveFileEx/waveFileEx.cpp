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

void waveFileEx::outputError( string s ){
    cout << s << endl;
}

waveFileEx::waveFileEx()
{
    setDefaultFormat();
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

    if( !( waveBuffer.empty() ) ){
        dstBuffer.resize( waveBuffer.size() );
        for( unsigned long i = 0; i < waveBuffer.size(); i++ )
            dstBuffer[i] = waveBuffer[i];
        result = 1;
    }
    return result;
}

int waveFileEx::getWaveBuffer( double* dstBuffer, unsigned long* bufferLength )
{
    int result = 0;

    if( dstBuffer && bufferLength ){
        if( *bufferLength >= waveBuffer.size() ){
            for( unsigned long i = 0; i < waveBuffer.size(); i++ )
                dstBuffer[i] = waveBuffer[i];
            *bufferLength = waveBuffer.size();
            result = 1;
        }
    }
    return result;
}

int waveFileEx::setWaveBuffer( vector<double>& srcBuffer )
{
    int result = 0;

    if( !( srcBuffer.empty() ) ){
        waveBuffer.resize( srcBuffer.size() );
        for( unsigned long i = 0; i < waveBuffer.size(); i++ )
            waveBuffer[i] = srcBuffer[i];
        result = 1;
    }
    return result;
}

int waveFileEx::setWaveBuffer( double* srcBuffer, unsigned long bufferLength )
{
    int result = 0;
    
    if( srcBuffer && bufferLength > 0 ){
        waveBuffer.resize( bufferLength );
        for( unsigned long i = 0; i < waveBuffer.size(); i++ )
            waveBuffer[i] = srcBuffer[i];
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
        if( readWaveHeader( fp ) )
            if( readWaveData( fp ) )
                result = 1;
            else
                result = -1;
        else
            result = 0;
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
                if( chunkSize > 16 )
                    for( int i = 0; i < 16 - chunkSize; i++ )
                        fgetc( fp );

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

        waveBuffer.resize( maxNum );

        for(unsigned int index = 0; index < maxNum; index += format.numChannels)
        {
            // short 固定．いずれ拡張したいけど．．．
            int value = ((short*)data)[index];
            waveBuffer[index] = (double)value / (double)(1 << format.bitsPerSample);
        }

        delete[] data;

        result = true;
    }else{
        outputError( "This wave file does not contain any data!" );
    }

    return result;
}

int    waveFileEx::writeWaveFile( string fileName ){
    int result = 2;
    FILE* fp;

#ifdef __GNUC__
    fp = fopen( fileName.c_str(), "wb" );
#else
    fopen_s( &fp, fileName.c_str(), "wb" );
#endif

    if( fp ){
        if( waveBuffer.empty() ){
            result = 0;
        }else{
            setDefaultFormat();
            /* This code may occur an error on big-endian CPU. */
            /* write Header */
            unsigned int waveSize = waveBuffer.size() * ( format.bitsPerSample / 8 ) * format.numChannels;
            unsigned int fileSize = waveSize + 44;
            unsigned int chunkSize = 16;

            short *data = new short[waveBuffer.size()];
            for(unsigned long i = 0; i < waveBuffer.size(); i++)
            {
                data[i] = (short)( 32767.0 * waveBuffer[i] ); 
            }

            fprintf( fp, "RIFF" );
            fwrite( (void*)&fileSize, 4, 1, fp );
            fprintf( fp, "WAVEfmt " );
            fwrite( (void*)&chunkSize, 4, 1, fp );
            fwrite( (void*)&(format.chunkID), 2, 1, fp );
            fwrite( (void*)&(format.numChannels), 2, 1, fp );
            fwrite( (void*)&(format.samplePerSecond), 4, 1, fp );
            fwrite( (void*)&(format.bytesPerSecond), 4, 1, fp );
            fwrite( (void*)&(format.blockAlign), 2, 1, fp );
            fwrite( (void*)&(format.bitsPerSample), 2, 1, fp );
            fprintf( fp, "data" );

            fwrite( (void*)&waveSize, 4, 1, fp );        
            fwrite( (void*)data, 2, waveBuffer.size(), fp);

            delete[] data;

            result = 1;
        }
        fclose( fp );
    }
    return result;
}

void    waveFileEx::normalize( void )
{
    double temp;
    double max = 1.0;
    for( unsigned long i = 0; i < waveBuffer.size(); i++ ){
        temp = fabs( waveBuffer[i] );
        if( temp > max )
            max = temp;
    }
    if( max != 0.0 )
        for( unsigned long i = 0; i < waveBuffer.size(); i++ )
            waveBuffer[i] /= max;
}

void    waveFileEx::applyDynamics( vector<double>& dynamics, int sample_rate, double framePeriod ){
    long frameIndex;
    double rate;

    for( unsigned long index = 0; index < waveBuffer.size(); index++ ){
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
    if( waveBuffer.size() != 0 ){
        long beginFrame = (long)( (leftBlank / 1000.0) * format.samplePerSecond );
        long endFrame;

        if( rightBlank < 0 )
            endFrame = beginFrame + (long)( (-rightBlank / 1000.0) * format.samplePerSecond );
        else
            endFrame = waveBuffer.size() - (long)( rightBlank / 1000.0 * format.samplePerSecond );

        if( endFrame > beginFrame )
            dstBuffer.resize( endFrame - beginFrame );

        for( long i = beginFrame; i < endFrame; i++ )
            if( 0 <= i && i < (long)waveBuffer.size() )
                dstBuffer[i-beginFrame] = waveBuffer[i];
            else
                dstBuffer[i-beginFrame] = 0;
        ret = 1;
    }

    return ret;
}

long   waveFileEx::getWaveLength( double leftBlank, double rightBlank )
{
    long length = (long)( leftBlank / 1000.0 * (double)format.samplePerSecond );
    long endFrame;
    if( rightBlank < 0 ){
        endFrame = length - (long)( rightBlank / 1000.0 * (double)format.samplePerSecond );
    } else {
        endFrame = waveBuffer.size() - (long)( rightBlank / 1000.0 * (double)format.samplePerSecond );
    }
    return ( endFrame - length );
}

int    waveFileEx::getWaveBuffer( double *dstBuffer, double leftBlank, double rightBlank, int length )
{
    int ret = 0;
    if( !length || !dstBuffer ) return ret;

    if( waveBuffer.size() != 0 ){
        long beginFrame = (long)( (leftBlank / 1000.0) * format.samplePerSecond );
        long endFrame;

        if( rightBlank < 0 )
            endFrame = beginFrame + (long)( (-rightBlank / 1000.0) * format.samplePerSecond );
        else
            endFrame = waveBuffer.size() - (long)( rightBlank / 1000.0 * format.samplePerSecond );

        if( endFrame < beginFrame && endFrame - beginFrame < 0 ){
            return ret;
        }

        for( long i = beginFrame; i < endFrame; i++ )
            if( 0 <= i && i < (long)waveBuffer.size() )
                dstBuffer[i-beginFrame] = waveBuffer[i];
            else
                dstBuffer[i-beginFrame] = 0;
        ret = 1;
    }

    return ret;
}


int waveFileEx::setOffset( double secOffset )
{
    int ret = 1;
    unsigned long size, offset = 0;
    vector<double> temp;
    size = waveBuffer.size();

    if( secOffset > 0.0 )
        offset = (unsigned long)( secOffset * format.samplePerSecond );

    temp.resize( size + offset, 0.0 );

    for( unsigned long i = 0; i < size; i++ )
        temp[offset+i] = waveBuffer[i];

    waveBuffer.swap( temp );

    return ret;    
}

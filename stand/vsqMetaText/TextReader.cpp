/**
 * TextReader.cpp
 *
 * Copyright (C) 2011 kbinani.
 */
#include "TextReader.h"

TextReader::TextReader( MB_FILE *fp )
{
    this->mFile = fp;
    this->mSocket = 0;
}

TextReader::TextReader( Socket socket )
{
    this->mFile = 0;
    this->mSocket = socket;
}

TextReader::~TextReader()
{
    this->mFile = 0;
    this->mSocket = 0;
}

int TextReader::getNextLine( string &buffer )
{
    buffer = "";
    if( 0 == mFile ) return -1;
    if( 0 == mSocket ) return -1;
    if( mFile )
    {
        if( mb_fgets( buffer, mFile ) )
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
    }
}

int TextReader::getNextLine( wstring &buffer )
{
    buffer = L"";
    if( 0 == mFile ) return -1;
    if( 0 == mSocket ) return -1;
    if( mFile )
    {
        if( mb_fgets( buffer, mFile ) )
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
    }
}

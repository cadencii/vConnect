/**
 * vsqHandle.cpp
 * Copyright (C) 2010 HAL, kbinani
 *
 * This file is a part of v.Connect,
 * implements the class 'vsqHandle' for handling singer, vibrato, and lyric
 * data in VSQ meta-text.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "vsqHandle.h"

void vsqLyric::setLyric( string right )
{
    lyric = right.substr( 0, right.find( "," ) );
    lyric = lyric.substr( lyric.find( "\"" ) + 1, lyric.rfind( "\"" ) - 1 );
    right = right.substr( right.find( "," ) + 1 );
    pronounce = right.substr( 0, right.find( "," ) );
    pronounce = pronounce.substr( pronounce.find( "\"" ) + 1, pronounce.rfind( "\"" ) - 1 );

    right = right.substr( right.find( "," ) + 1 );
    lyricDelta = atoi( right.substr( 0, right.find( "," ) ).c_str() );

    right = right.substr( right.find( "," ) + 1 );
    consonantAdjustment = atoi( right.substr( 0, right.find( "," ) ).c_str() );

    right = right.substr( right.find( "," ) + 1 );
    protectFlag = atoi( right.substr( 0, right.find( "," ) ).c_str() );
}

void vsqHandle::setParameter( string left, string right )
{
    if( left.compare( "IconID" ) == 0 ){
        iconID = right;
    }else if( left.compare( "IDS" ) == 0 ){
        IDS = right;
    }else if( left.compare( "Caption" ) == 0 ){
        caption = right;
    }else if( left.compare( "Length" ) == 0 ){
        length = atoi( right.c_str() );
    }else if( left.compare( "Language" ) == 0 ){
        language = atoi( right.c_str() );
    }else if( left.compare( "Program" ) == 0 ){
        program = atoi( right.c_str() );
    }else if( left.compare( "Original" ) == 0 ){
        original = atoi( right.c_str() );
    }else if( left.compare( "StartDepth" ) == 0 ){
        vsqVibratoBP point;
        point.value = atoi( right.c_str() );
        point.position = -1.0;
        if( vibratoDepth.size() <= 0 )
        {
            vibratoDepth.resize( 1 );
        }
        vibratoDepth[0] = point;
    }else if( left.compare( "DepthBPNum" ) == 0 ){
        parseBPNum( vibratoDepth, right );
    }else if( left.compare( "DepthBPX" ) == 0 ){
        parseBP( vibratoDepth, right, true );
    }else if( left.compare( "DepthBPY" ) == 0 ){
        parseBP( vibratoDepth, right, false );
    }else if( left.compare( "StartRate" ) == 0 ){
        vsqVibratoBP point;
        point.value = atoi( right.c_str() );
        point.position = -1.0;
        if( vibratoRate.size() <= 0 )
        {
            vibratoRate.resize( 1 );
        }
        vibratoRate[0] = point;
    }else if( left.compare( "RateBPNum" ) == 0 ){
        parseBPNum( vibratoRate, right );
    }else if( left.compare( "RateBPX" ) == 0 ){
        parseBP( vibratoRate, right, true );
    }else if( left.compare( "RateBPY" ) == 0 ){
        parseBP( vibratoRate, right, false );
    }else if( left.find( "L" ) != string::npos ){
        lyric.setLyric( right );
    }else{
        string Message = "warning: unknown Property in VsqHandle : " + left;
        cout << Message << endl;
    }
}

void vsqHandle::parseBPNum( vector<vsqVibratoBP> &list, string str )
{
    vector<vsqVibratoBP>::size_type new_size = atoi( str.c_str() ) + 1;
    list.resize( new_size );
}

void vsqHandle::parseBP( vector<vsqVibratoBP> &list, string str, bool parse_x )
{
    string s;
    string::size_type indx_comma = str.find( "," );
    int indx = 1;
    while( indx_comma != string::npos )
    {
        s = str.substr( 0, indx_comma );
        if( list.size() <= indx ){
            list.resize( indx + 1 );
        }

        if( parse_x ){
            list[indx].position = (float)atof( s.c_str() );
        }else{
            list[indx].value = atoi( s.c_str() );
        }
        str = str.substr( indx_comma + 1 );
        indx_comma = str.find( "," );
        indx++;
    }
    if( str.size() > 0 ){
        if( list.size() <= indx ){
            list.resize( indx + 1 );
        }
        s = str;
        if( parse_x ){
            list[indx].position = (float)atof( s.c_str() );
        }else{
            list[indx].value = atoi( s.c_str() );
        }
    }
}

short vsqHandle::getVibratoDepth( double position )
{
    short ret;
    vector<vsqVibratoBP>::size_type i;
    vector<vsqVibratoBP>::size_type c = vibratoDepth.size();
    for( i = 0; i < c; i++ )
    {
        if( vibratoDepth[i].position > position )
        {
            break;
        }
    }
    if( i != 0 )
    {
        i--;
        ret = vibratoDepth[i].value;
    }
    else
    {
        ret = 0;
    }

    return ret;
}

short vsqHandle::getVibratoRate( double position )
{
    short ret = 0;
    vector<vsqVibratoBP>::size_type i;
    vector<vsqVibratoBP>::size_type c = vibratoRate.size();
    for( i = 0; i < c; i++ )
    {
        if( vibratoRate[i].position > position )
        {
            break;
        }
    }
    if( i != 0 )
    {
        i--;
        ret = vibratoRate[i].value;
    }
    else
    {
        ret = 0;
    }

    return ret;
}

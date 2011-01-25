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

void vsqLyric::setLyric( string_t right )
{
    lyric = right.substr( 0, right.find( _T(",") ) );
    lyric = lyric.substr( lyric.find( _T("\"") ) + 1, lyric.rfind( _T("\"") ) - 1 );
    right = right.substr( right.find( _T(",") ) + 1 );
    //pronounce = right.substr( 0, right.find( "," ) ); //BEFORE
    mb_conv( right.substr( 0, right.find( _T(",") ) ), pronounce ); //AFTER
    pronounce = pronounce.substr( pronounce.find( _T("\"") ) + 1, pronounce.rfind( _T("\"") ) - 1 );

    right = right.substr( right.find( _T(",") ) + 1 );
    string s;
    mb_conv( right.substr( 0, right.find( _T(",") ) ), s );
    lyricDelta = atoi( s.c_str() );

    right = right.substr( right.find( _T(",") ) + 1 );
    mb_conv( right.substr( 0, right.find( _T(",") ) ), s );
    consonantAdjustment = atoi( s.c_str() );

    right = right.substr( right.find( _T(",") ) + 1 );
    mb_conv( right.substr( 0, right.find( _T(",") ) ), s );
    protectFlag = atoi( s.c_str() );
}

void vsqHandle::setParameter( string_t left, string_t right ){
    string s;
    if( left.compare( _T("IconID") ) == 0 ){
        iconID = right;
    }else if( left.compare( _T("IDS") ) == 0){
        IDS = right;
    }else if( left.compare( _T("Caption") ) == 0 ){
        caption = right;
    }else if( left.compare( _T("Length") ) == 0 ){
        mb_conv( right, s );
        length = atoi( s.c_str() );
    }else if( left.compare( _T("Language") ) == 0 ){
        mb_conv( right, s );
        language = atoi( s.c_str() );
    }else if( left.compare( _T("Program") ) == 0 ){
        mb_conv( right, s );
        program = atoi( s.c_str() );
    }else if( left.compare( _T("Original") ) == 0 ){
        mb_conv( right, s );
        original = atoi( s.c_str() );
    }else if( left.compare( _T("StartDepth") ) == 0 ){
        vsqVibratoBP point;
        mb_conv( right, s );
        point.value = atoi( s.c_str() );
        point.position = -1.0;
        if( vibratoDepth.size() <= 0 ){
            vibratoDepth.resize( 1 );
        }
        vibratoDepth[0] = point;
    }else if( left.compare( _T("DepthBPNum") ) == 0 ){
        parseBPNum( vibratoDepth, right );
    }else if( left.compare( _T("DepthBPX") ) == 0 ){
        parseBP( vibratoDepth, right, true );
    }else if( left.compare( _T("DepthBPY") ) == 0 ){
        parseBP( vibratoDepth, right, false );
    }else if( left.compare( _T("StartRate") ) == 0 ){
        vsqVibratoBP point;
        mb_conv( right, s );
        point.value = atoi( s.c_str() );
        point.position = -1.0;
        if( vibratoRate.size() <= 0 ){
            vibratoRate.resize( 1 );
        }
        vibratoRate[0] = point;
    }else if( left.compare( _T("RateBPNum") ) == 0 ){
        parseBPNum( vibratoRate, right );
    }else if( left.compare( _T("RateBPX") ) == 0 ){
        parseBP( vibratoRate, right, true );
    }else if( left.compare( _T("RateBPY") ) == 0 ){
        parseBP( vibratoRate, right, false );
    }else if( left.find( _T("L") ) != string_t::npos ){
        lyric.setLyric( right );
    }else{
        mb_conv( left, s );
        string Message = "warning: unknown Property in VsqHandle : " + s;
        outputError( Message.c_str() );
    }
}

void vsqHandle::parseBPNum( vector<vsqVibratoBP>& list, string_t str ){
    string s;
    mb_conv( str, s );
    vector<vsqVibratoBP>::size_type new_size = atoi( s.c_str() ) + 1;
    list.resize( new_size );
}

void vsqHandle::parseBP( vector<vsqVibratoBP>& list, string_t str, bool parse_x ){
    string s;
    string_t::size_type indx_comma = str.find( _T(",") );
    int indx = 1;
    while( indx_comma != string_t::npos ){
        mb_conv( str.substr( 0, indx_comma ), s );
        if( list.size() <= indx ){
            list.resize( indx + 1 );
        }
        if( parse_x ){
            list[indx].position = (float)atof( s.c_str() );
        }else{
            list[indx].value = atoi( s.c_str() );
        }
        str = str.substr( indx_comma + 1 );
        indx_comma = str.find( _T(",") );
        indx++;
    }
    if( str.size() > 0 ){
        if( list.size() <= indx ){
            list.resize( indx + 1 );
        }
        mb_conv( str, s );
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
    for( i = 0; i < c; i++ ){
        if( vibratoDepth[i].position > position ){
            break;
        }
    }
    if( i != 0 ){
        i--;
        ret = vibratoDepth[i].value;
    }else{
        ret = 0;
    }

    return ret;
}

short vsqHandle::getVibratoRate( double position )
{
    short ret = 0;
    vector<vsqVibratoBP>::size_type i;
    vector<vsqVibratoBP>::size_type c = vibratoRate.size();
    for( i = 0; i < c; i++ ){
        if( vibratoRate[i].position > position ){
            break;
        }
    }
    if( i != 0 ){
        i--;
        ret = vibratoRate[i].value;
    }else{
        ret = 0;
    }

    return ret;
}

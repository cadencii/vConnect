/*
 *
 *    vsqEventEx.cpp
 *                        (c) HAL 2010-
 *
 *  This files is a part of v.Connect.
 * vsqEventEx contains vsq-event's properties
 * and some UTAU properties such as preutterance and voice-overlap.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#include "vsqEventEx.h"

void vsqEventEx::setParameter( string_t left, string_t right ){
    string s;
    if( left.compare( _T("Type") ) == 0 ){
        type = right;
    }else if( left.compare( _T("Length") ) == 0 ){
        mb_conv( right, s );
        length = atoi( s.c_str() );
    }else if( left.compare( _T("Note#") ) == 0 ){
        mb_conv( right, s );
        note = atoi( s.c_str() );
    }else if( left.compare( _T("Dynamics") ) == 0 ){
        mb_conv( right, s );
        velocity = atoi( s.c_str() );
    }else if( left.compare( _T("PMBendDepth") ) == 0 ){
        mb_conv( right, s );
        portamentoDepth = atoi( s.c_str() );
    }else if( left.compare( _T("PMBendLength") ) == 0 ){
        mb_conv( right, s );
        portamentoLength = atoi( s.c_str() );
    }else if( left.compare( _T("PMbPortamentoUse") ) == 0 ){
        mb_conv( right, s );
        portamentoUse = atoi( s.c_str() );
    }else if( left.compare( _T("DEMdecGainRate") ) == 0 ){
        mb_conv( right, s );
        decay = atoi( s.c_str() );
    }else if( left.compare( _T("DEMaccent") ) == 0 ){
        mb_conv( right, s );
        attack = atoi( s.c_str() );
    }else if( left.compare( _T("VibratoDelay") ) == 0 ){
        mb_conv( right, s );
        vibratoDelay = atoi( s.c_str() );
    }else if( left.compare( _T("PreUtterance") ) == 0 ){
        mb_conv( right, s );
        utauSetting.msPreUtterance = (float)atof( s.c_str() );
    }else if( left.compare( _T("VoiceOverlap") ) == 0 ){
        mb_conv( right, s );
        utauSetting.msVoiceOverlap = (float)atof( s.c_str() );
    }else if( left.compare( _T("LyricHandle") ) == 0 ){
        right = _T("[") + right + _T("]");
        objectMap.insert( make_pair( right, (vsqBase*)&lyricHandle ) );
    }else if( left.compare( _T("VibratoHandle") ) == 0 ){
        right = _T("[") + right + _T("]");
        objectMap.insert( make_pair( right, (vsqBase*)&vibratoHandle ) );
    }else if( left.compare( _T("IconHandle") ) == 0 ){
        right = _T("[") + right + _T("]");
        objectMap.insert( make_pair( right, (vsqBase*)&iconHandle ) );
    }else{
        mb_conv( left, s );
        string message = "warning: unknown Property in VsqEvent : " + s;
        outputError( message.c_str() );
    }

    return;
}

/*
 * vsqFileEx.cpp
 * Copyright (C) 2009-2011 HAL,
 * Copyright (C) 2011-2012 kbinani.
 */
#include "vsqFileEx.h"

void vsqFileEx::setParamOtoIni( vsqPhonemeDB *target, string_t singerName, string_t otoIniPath )
{
    // 名前登録して
    singerMap.insert( make_pair( singerName, target->singerIndex ) );
    // 中身読んで
    UtauDB *p = new UtauDB;
    p->read( otoIniPath, target->_codepage_otoini.c_str() );
    // リストに追加
    UtauDB::dbRegist( p );
    target->singerIndex++;
}

void vsqFileEx::setParamEvent( vsqEventEx *target, string_t left, string_t right )
{
    string s;
    if( left.compare( _T("Type") ) == 0 )
    {
        target->type = right;
    }
    else if( left.compare( _T("Length") ) == 0 )
    {
        mb_conv( right, s );
        target->length = atoi( s.c_str() );
    }
    else if( left.compare( _T("Note#") ) == 0 )
    {
        mb_conv( right, s );
        target->note = atoi( s.c_str() );
    }
    else if( left.compare( _T("Dynamics") ) == 0 )
    {
        mb_conv( right, s );
        target->velocity = atoi( s.c_str() );
    }
    else if( left.compare( _T("PMBendDepth") ) == 0 )
    {
        mb_conv( right, s );
        target->portamentoDepth = atoi( s.c_str() );
    }
    else if( left.compare( _T("PMBendLength") ) == 0 )
    {
        mb_conv( right, s );
        target->portamentoLength = atoi( s.c_str() );
    }
    else if( left.compare( _T("PMbPortamentoUse") ) == 0 )
    {
        mb_conv( right, s );
        target->portamentoUse = atoi( s.c_str() );
    }
    else if( left.compare( _T("DEMdecGainRate") ) == 0 )
    {
        mb_conv( right, s );
        target->decay = atoi( s.c_str() );
    }
    else if( left.compare( _T("DEMaccent") ) == 0 )
    {
        mb_conv( right, s );
        target->attack = atoi( s.c_str() );
    }
    else if( left.compare( _T("VibratoDelay") ) == 0 )
    {
        mb_conv( right, s );
        target->vibratoDelay = atoi( s.c_str() );
    }
    else if( left.compare( _T("PreUtterance") ) == 0 )
    {
        mb_conv( right, s );
        target->utauSetting.msPreUtterance = (float)atof( s.c_str() );
    }
    else if( left.compare( _T("VoiceOverlap") ) == 0 )
    {
        mb_conv( right, s );
        target->utauSetting.msVoiceOverlap = (float)atof( s.c_str() );
    }
    else if( left.compare( _T("LyricHandle") ) == 0 )
    {
        right = _T("[") + right + _T("]");
        mMapHandles.insert( make_pair( right, &target->lyricHandle ) );
    }
    else if( left.compare( _T("VibratoHandle") ) == 0 )
    {
        right = _T("[") + right + _T("]");
        mMapHandles.insert( make_pair( right, &target->vibratoHandle ) );
    }
    else if( left.compare( _T("IconHandle") ) == 0 )
    {
        right = _T("[") + right + _T("]");
        mMapHandles.insert( make_pair( right, &target->iconHandle ) );
    }
    else
    {
        mb_conv( left, s );
        string message = "warning: unknown Property in VsqEvent : " + s;
        cout << message << endl;//outputError( message.c_str() );
    }

    return;
}

long vsqFileEx::getEndTick()
{
    return this->events.endTick;
}

double vsqFileEx::getTempo()
{
    return this->vsqTempoBp.getTempo();
}

vsqFileEx::vsqFileEx()
{
    string_t temp;

    controlCurves.resize( CONTROL_CURVE_NUM );

    for( unsigned int i = 0; i < controlCurves.size(); i++ )
    {
        //objectMap.insert( make_pair( controlCurveName[i], (vsqBase *)&(controlCurves[i]) ) );
        /* The note on 0 tick can be allocated before 0 tick, because of its preutterance. */
        controlCurves[i].setParameter( -10000, controlCurveDefaultValue[i] );
        mMapCurves.insert( make_pair( controlCurveName[i], &controlCurves[i] ) );
    }

    //temp = _T("[EventList]");
    //objectMap.insert( make_pair( temp, (vsqBase *)&events ) );
    //temp = _T("[Tempo]");
    //objectMap.insert( make_pair( temp, (vsqBase *)&vsqTempoBp ) );
    //temp = _T("[oto.ini]");
    //objectMap.insert( make_pair( temp, (vsqBase *)&voiceDataBase ) );
}

bool vsqFileEx::read( string_t file_name, runtimeOptions options )
{
    bool result = false;
    voiceDataBase.setRuntimeOptions( options );

    MB_FILE *fp = mb_fopen( file_name, options.encodingVsqText.c_str() );

    bool ret = readCore( fp, file_name );
    mb_fclose( fp );
    return ret;
}

bool vsqFileEx::readCore( MB_FILE *fp, string vsqFilePath )
{
    if( !fp ) return false;

    string_t temp, search, left, right;
    //map_t<string_t, vsqBase *>::iterator i;

    while( true )
    {
        bool ret = mb_fgets( temp, fp );
#if defined( _DEBUG )
        if( !ret )
        {
            cout << "vsqFileEx::readCore; mb_fgets returned false" << endl;
        }
        string s;
        mb_conv( temp, s );
        cout << "vsqFileEx::readCore; temp=" << s << endl;
#endif
        if( !ret ) break;

        // 空文字の場合は次へ
        if( temp.size() <= 0 ) continue;

        if( temp.find( _T("[") ) == 0 )
        {
            search = temp;
            continue;
        }
        string_t::size_type indx_equal = temp.find( _T("=") );
        if( indx_equal == string_t::npos )
        {
            left = temp;
            right = _T("");
        }
        else
        {
            left = temp.substr( 0, indx_equal );
            right = temp.substr( indx_equal + 1 );
        }

#if 1
        if( search.compare( OBJ_NAME_EVENT_LIST ) == 0 )
        {
            // [EventList]
            string_t::size_type indx_comma = right.find( _T(",") );
            while( indx_comma != string_t::npos )
            {
                // コンマが見つからなくなるまでループ
                string_t tright = right.substr( 0, indx_comma );
                this->events.setParameter( left, tright, mMapIDs );
                right = right.substr( indx_comma + 1 );
                indx_comma = right.find( _T(",") );
            }
            this->events.setParameter( left, right, mMapIDs );
        }
        else if( search.compare( OBJ_NAME_OTOINI ) == 0 )
        {
            // [oto.ini]
            string_t::size_type index = left.find( _T( "\t" ) );
            string_t singerName, otoIniPath;

            if( index == string_t::npos ){
                singerName = _T( "" );
                otoIniPath = left;
            }else{
                singerName = left.substr( 0, index );
                otoIniPath = left.substr( index + 1 );
            }
            if( false == Path::exists( otoIniPath ) ){
                string directory = Path::getDirectoryName( vsqFilePath );
                otoIniPath = Path::combine( directory, otoIniPath );
            }
            this->setParamOtoIni( &this->voiceDataBase, singerName, otoIniPath );
        }
        else if( search.compare( OBJ_NAME_TEMPO ) == 0 )
        {
            // [Tempo]
            this->vsqTempoBp.setParameter( left, right );
        }
        else if( search.find( _T( "[ID#" ) ) == 0 )
        {
            // ID
            map_t<string_t, vsqEventEx *>::iterator i;
            i = mMapIDs.find( search );
            if( i != mMapIDs.end() && i->second )
            {
                setParamEvent( i->second, left, right );
            }
        }
        else if( search.find( _T( "[h#" ) ) == 0 )
        {
            // handle
            map_t<string_t, vsqHandle *>::iterator i;
            i = mMapHandles.find( search );
            if( i != mMapHandles.end() && i->second )
            {
                i->second->setParameter( left, right );
            }
        }
        else
        {
            // たぶんコントロールカーブ
            map_t<string_t, vsqBPList *>::iterator i;
            i = mMapCurves.find( search );
            if( i != mMapCurves.end() && i->second )
            {
                i->second->setParameter( left, right );
            }
            else
            {
                // そんなセクション名知らねー
                string message;
                string t_search;
                mb_conv( search, t_search );
                message = "vsqFileEx::readCore; not found: " + t_search;
                cout << message << endl;
            }
        }

#else
        i = objectMap.find( search );
        if( i != objectMap.end() )
        {
            if( i->second )
            {
                if( search.compare( _T("[EventList]") ) == 0 )
                {
                    string_t::size_type indx_comma = right.find( _T(",") );
                    while( indx_comma != string_t::npos )
                    {
                        // コンマが見つからなくなるまでループ
                        string_t tright = right.substr( 0, indx_comma );
                        i->second->setParameter( left, tright );
                        right = right.substr( indx_comma + 1 );
                        indx_comma = right.find( _T(",") );
                    }
                }
#if defined( _DEBUG )
                string s1;
                mb_conv( left, s1 );
                cout << "vsqFileEx::readCore; left=" << s1 << endl;
                mb_conv( right, s1 );
                cout << "vsqFileEx::readCore; right=" << s1 << endl;
#endif
                i->second->setParameter( left, right );
            }
        }
        else
        {
            string message;
            string t_search;
            mb_conv( search, t_search );
            message = "vsqFileEx::readCore; not found: " + t_search;
            vsqBase::outputError( message.c_str() );
        }
#endif
    }

    // utau音源が無ければ合成しようがないので false.
    int size = UtauDB::dbSize();

#if defined( _DEBUG )
    dumpEvents();
    dumpMapIDs();
    dumpMapHandles();
    cout << "vsqFileEx::readCore; size=" << size << endl;
#endif

    return (size > 0);
}

double vsqFileEx::getEndSec()
{
     return this->vsqTempoBp.tickToSecond( getEndTick() );
}

int vsqFileEx::getSingerIndex( string_t t_search )
{
    int ret = 0;
    map_t<string_t, int>::iterator i = singerMap.find( t_search );
    if( i != singerMap.end() )
    {
        ret = i->second;
    }
    return ret;
}

/*
 * vsqFileEx.cpp
 * Copyright (C) 2009-2011 HAL,
 * Copyright (C) 2011 kbinani.
 */
#include "vsqFileEx.h"

vsqFileEx::vsqFileEx()
{
    string_t temp;

    controlCurves.resize( CONTROL_CURVE_NUM );

    for( unsigned int i = 0; i < controlCurves.size(); i++ )
    {
        objectMap.insert( make_pair( controlCurveName[i], (vsqBase *)&(controlCurves[i]) ) );
        /* The note on 0 tick can be allocated before 0 tick, because of its preutterance. */
        controlCurves[i].setParameter( -10000, controlCurveDefaultValue[i] );
    }

    temp = _T("[EventList]");
    objectMap.insert( make_pair( temp, (vsqBase *)&events ) );
    temp = _T("[Tempo]");
    objectMap.insert( make_pair( temp, (vsqBase *)&vsqTempoBp ) );
    temp = _T("[oto.ini]");
    objectMap.insert( make_pair( temp, (vsqBase *)&voiceDataBase ) );
}

bool vsqFileEx::read( string_t file_name, runtimeOptions options )
{
#ifdef _DEBUG
    cout << "vsqFileEx::readVsqFile" << endl;
#endif
    bool result = false;
    voiceDataBase.setRuntimeOptions( options );

    MB_FILE *fp;
#ifdef _DEBUG
    cout << "vsqFileEx::readVsqFile; calling mb_fopen...";
#endif
    fp = mb_fopen( file_name, options.encodingVsqText.c_str() );
#ifdef _DEBUG
    cout << " done" << endl;
    cout << "vsqFileEx::readVsqFile; (fp==NULL)=" << (fp == NULL ? "true" : "false") << endl;
#endif

    if( fp )
    {
        string_t temp, search, left, right;
        map_t<string_t, vsqBase *>::iterator i;

        while( mb_fgets( temp, fp ) )
        {
#ifdef _DEBUG
            string s;
            mb_conv( temp, s );
            cout << "vsqFileEx::readVsqFile; temp=" << s << endl;
#endif
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
                    i->second->setParameter( left, right );
                }
            }
            else
            {
                string message;
                string t_search;
                mb_conv( search, t_search );
                message = "vsqFileEx::readVsqFile; not found: " + t_search;
                outputError( message.c_str() );
            }
        }
        mb_fclose( fp );

        // utau音源が無ければ合成しようがないので false.
        result = (UtauDB::dbSize() > 0);
    }
#ifdef _DEBUG
    cout << "type any key to continue...";
    int i;
    scanf( "%d", &i );
#endif
    return result;
}

double vsqFileEx::getEndSec()
{
     return this->vsqTempoBp.tickToSecond( endTick );
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

/*
 * UtauDB.cpp
 *
 * Copyright (C) 2009-2011 HAL,
 * Copyright (C) 2011 kbinani.
 */
#include "UtauDB.h"

vector<UtauDB *> UtauDB::mDBs;

map_t<string_t, utauParameters *>::iterator UtauDB::begin()
{
    return mSettingMap.begin();
}

map_t<string_t, utauParameters *>::iterator UtauDB::end()
{
    return mSettingMap.end();
}

int UtauDB::dbSize()
{
    return (int)UtauDB::mDBs.size();
}

UtauDB *UtauDB::dbGet( int index )
{
    return UtauDB::mDBs[index];
}

void UtauDB::dbRegist( UtauDB *db )
{
    UtauDB::mDBs.push_back( db );
}

void UtauDB::dbClear()
{
    for( unsigned int i = 0; i < UtauDB::mDBs.size(); i++ )
    {
        SAFE_DELETE( UtauDB::mDBs[i] );
    }
    UtauDB::mDBs.clear();
}

UtauDB::~UtauDB()
{
    list<utauParameters*>::iterator i;
    for( i = mSettingList.begin(); i != mSettingList.end(); i++ )
    {
        if( (*i) )
        {
            delete (*i);
        }
    }
}

int UtauDB::read( string_t path_oto_ini, const char *codepage )
{
    int result = 2;
    MB_FILE *fp;
    //char buf[LINEBUFF_LEN];
    string_t temp;
    int index;
#ifdef _DEBUG
    string s2;
    mb_conv( path_oto_ini, s2 );
    cout << "utauVoiceDataBase::readUtauVoiceDataBase; before normalize; path_oto_ini=" << s2 << endl;
#endif
    normalize_path_separator( path_oto_ini );
#ifdef _DEBUG
    string s;
    mb_conv( path_oto_ini, s );
    cout << "utauVoiceDataBase::readUtauVoiceDataBase; after normalize; path_oto_ini=" << s << endl;
#endif
    
    fp = mb_fopen( path_oto_ini, codepage );

#ifdef _DEBUG
    cout << "utauVoiceDataBase::readUtauVoiceDataBase; (fp==NULL)=" << (fp == NULL ? "true" : "false") << endl;
#endif
    if( fp )
    {
        mDBPath = path_oto_ini.substr( 0, path_oto_ini.rfind( PATH_SEPARATOR ) + 1 );

        while( mb_fgets( temp, fp ) )
        {
#ifdef _DEBUG
            string ts;
            mb_conv( temp, ts );
            //cout << "utauVoiceDataBase::readUtauVoiceDataBase; temp=" << ts << endl;
#endif
            utauParameters* current = new utauParameters;
            if( current )
            {
                if( ( index = temp.find( _T(".wav") ) ) == string_t::npos )
                {
                    if( ( index = temp.find( _T(".stf") ) ) == string_t::npos )
                    {
                        index = temp.find( _T("=") );
                    }
                    current->isWave = false;
                }
                else
                {
                    current->isWave = true;
                }

                normalize_path_separator( current->fileName );
                current->fileName = temp.substr( 0, index );

                /* When no lyric symbol exists, voiceDB will use fileName instead. */
                if( temp.find( _T("=") ) + 1 == temp.find( _T(",") ) )
                {
                    current->lyric = current->fileName;
                }
                else
                {
                    current->lyric = temp.substr( temp.find( _T("=") ) + 1 );
                    current->lyric = current->lyric.substr( 0, current->lyric.find( _T(",") ) );
                }

                string t;
                temp = temp.substr( temp.find( _T(",") ) + 1 );
                mb_conv( temp, t );
                current->msLeftBlank = (float)atof( t.c_str() );

                temp = temp.substr( temp.find( _T(",") ) + 1 );
                mb_conv( temp, t );
                current->msFixedLength = (float)atof( t.c_str() );

                temp = temp.substr( temp.find( _T( "," ) ) + 1 );
                mb_conv( temp, t );
                current->msRightBlank = (float)atof( t.c_str() );

                temp = temp.substr( temp.find( _T( "," ) ) + 1 );
                mb_conv( temp, t );
                current->msPreUtterance = (float)atof( t.c_str() );

                temp = temp.substr( temp.find( _T( "," ) ) + 1 );
                mb_conv( temp, t );
                current->msVoiceOverlap = (float)atof( t.c_str() );

                mSettingMap.insert( make_pair( current->lyric, current ) );
                if( current->lyric.compare( current->fileName ) != 0 )
                {
                    mSettingMap.insert( make_pair( current->fileName, current ) );
                }
                mSettingList.push_back( current );

            }
            else
            {
                result = 3;
                break;
            }
        }
        if( result != 3 )
        {
            result = 1;
        }
        mb_fclose( fp );
    }
    return result;
}

int UtauDB::getParams( utauParameters &parameters, string_t search )
{
    int    result = 0;
    map_t<string_t, utauParameters *>::iterator i = mSettingMap.find( search );
    if( i != mSettingMap.end() )
    {
        if( i->second )
        {
            parameters.fileName = i->second->fileName;
            parameters.lyric = i->second->lyric;
            parameters.msFixedLength = i->second->msFixedLength;
            parameters.msLeftBlank = i->second->msLeftBlank;
            parameters.msPreUtterance = i->second->msPreUtterance;
            parameters.msRightBlank = i->second->msRightBlank;
            parameters.msVoiceOverlap = i->second->msVoiceOverlap;
            parameters.isWave = i->second->isWave;
            result = 1;
        }
    }
    return result;
}

int UtauDB::getDBPath( string_t &dst )
{
    dst = mDBPath; 
    return 1;
}

bool UtauDB::empty()
{
    return mSettingMap.empty();
}

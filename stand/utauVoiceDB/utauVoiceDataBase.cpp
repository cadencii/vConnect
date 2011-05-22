/*
 *
 *    utauVoiceDataBase.cpp
 *                        (c) HAL 2009-
 *
 * This files is a part of v.Connect.
 * utauVoiceDataBase is a container class of contains UTAU oto.ini
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#include "utauVoiceDataBase.h"

vector<utauVoiceDataBase *> utauVoiceDataBase::mDBs;

int utauVoiceDataBase::dbSize()
{
    return (int)utauVoiceDataBase::mDBs.size();
}

utauVoiceDataBase *utauVoiceDataBase::dbGet( int index )
{
    return utauVoiceDataBase::mDBs[index];
}

void utauVoiceDataBase::dbRegist( utauVoiceDataBase *db )
{
    utauVoiceDataBase::mDBs.push_back( db );
}

void utauVoiceDataBase::dbClear()
{
    for( unsigned int i = 0; i < utauVoiceDataBase::mDBs.size(); i++ )
    {
        SAFE_DELETE( utauVoiceDataBase::mDBs[i] );
    }
    utauVoiceDataBase::mDBs.clear();
}

utauVoiceDataBase::~utauVoiceDataBase()
{
    list<utauParameters*>::iterator i;
    for( i = settingList.begin(); i != settingList.end(); i++ )
        if( (*i) )
            delete (*i);
}

int utauVoiceDataBase::read( string_t path_oto_ini, const char *codepage )
{
    int result = 2;
    MB_FILE* fp;
    //char buf[LINEBUFF_LEN];
    string_t temp;
    int index;
#ifdef _DEBUG
    string s2;
    mb_conv( fileName, s2 );
    cout << "utauVoiceDataBase::readUtauVoiceDataBase; before normalize; path_oto_ini=" << s2 << endl;
#endif
    normalize_path_separator( path_oto_ini );
#ifdef _DEBUG
    string s;
    mb_conv( fileName, s );
    cout << "utauVoiceDataBase::readUtauVoiceDataBase; after normalize; path_oto_ini=" << s << endl;
#endif
    
    fp = mb_fopen( path_oto_ini, codepage );

#ifdef _DEBUG
    cout << "utauVoiceDataBase::readUtauVoiceDataBase; (fp==NULL)=" << (fp == NULL ? "true" : "false") << endl;
#endif
    if( fp ){
        voicePath = path_oto_ini.substr( 0, path_oto_ini.rfind( PATH_SEPARATOR ) + 1 );

        while( mb_fgets( temp, fp ) ){
#ifdef _DEBUG
            string ts;
            mb_conv( temp, ts );
            //cout << "utauVoiceDataBase::readUtauVoiceDataBase; temp=" << ts << endl;
#endif
            utauParameters* current = new utauParameters;
            if( current ){
                if( ( index = temp.find( _T(".wav") ) ) == string_t::npos ){
                    if( ( index = temp.find( _T(".stf") ) ) == string_t::npos ){
                        index = temp.find( _T(".") );
                    }
                }

                normalize_path_separator( current->fileName );
                current->fileName = temp.substr( 0, index );

                /* When no lyric symbol exists, voiceDB will use fileName instead. */
                if( temp.find( _T("=") ) + 1 == temp.find( _T(",") ) ){
                    current->lyric = current->fileName;
                }else{
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

                settingMap.insert( make_pair( current->lyric, current ) );
                if( current->lyric.compare( current->fileName ) != 0 ){
                    settingMap.insert( make_pair( current->fileName, current ) );
                }
                settingList.push_back( current );

            }else{
                result = 3;
                break;
            }
        }
        if( result != 3 ){
            result = 1;
        }
        mb_fclose( fp );
    }
    return result;
}

int utauVoiceDataBase::getUtauParameters( utauParameters &parameters, string_t search )
{
    int    result = 0;
    map_t<string_t, utauParameters*>::iterator i = settingMap.find( search );
    if( i != settingMap.end() ){
        if( i->second ){
            parameters.fileName = i->second->fileName;
            parameters.lyric = i->second->lyric;
            parameters.msFixedLength = i->second->msFixedLength;
            parameters.msLeftBlank = i->second->msLeftBlank;
            parameters.msPreUtterance = i->second->msPreUtterance;
            parameters.msRightBlank = i->second->msRightBlank;
            parameters.msVoiceOverlap = i->second->msVoiceOverlap;
            result = 1;
        }
    }
    return result;
}

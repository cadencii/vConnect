/*
 *
 *    vsqHandle.cpp
 *                        (c) HAL 2010-
 *
 *  This files is a part of v.Connect.
 * vsqPhonemeDB inherits utauVoiceDataBase class.
 * This class reads UTAU oto.ini in vsq meta-text extention.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __vsqPhonemeDB_h__
#define __vsqPhonemeDB_h__

#include "vsqBase.h"
#include "../runtimeOptions.h"

class vsqPhonemeDB : public vsqBase{
public:
    vsqPhonemeDB(){
        isInitialized = false;
        singerIndex = 0;
    }

    ~vsqPhonemeDB(){
        UtauDB::dbClear();
        /*for( unsigned int i = 0; i < voiceDBs.size(); i++ ){
            SAFE_DELETE( voiceDBs[i] );
        }
        voiceDBs.clear();*/
    }

    void setParameter( string_t key, string_t value ){
        string_t::size_type indx_tab = key.find( _T( "\t" ) );
        string_t singer_name, path_otoini;

        if( indx_tab == string_t::npos ){
            singer_name = _T( "" );
            path_otoini = key;
        }else{
            singer_name = key.substr( 0, indx_tab );
            path_otoini = key.substr( indx_tab + 1 );
        }
    
        // –¼‘O“o˜^‚µ‚Ä
        singerMap.insert( make_pair( singer_name, singerIndex ) );
        // ’†g“Ç‚ñ‚Å
        UtauDB *p = new UtauDB;
        p->read( path_otoini, _codepage_otoini.c_str() );
        // ƒŠƒXƒg‚É’Ç‰Á
        UtauDB::dbRegist( p );
        singerIndex++;
    }

    void setRuntimeOptions( runtimeOptions options ){
        _codepage_otoini = options.encodingOtoIni;
    }

private:
    bool isInitialized;
    string _codepage_otoini;

    int singerIndex;

};

#endif

/*
 * vsqPhonemeDB.h
 * Copyright (C) 2010- HAL,
 * Copyright (C) 2011 kbinani.
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
#include "../RuntimeOption.h"

using namespace vconnect;

class vsqPhonemeDB// : public vsqBase
{

public:

    vsqPhonemeDB()
    {
        isInitialized = false;
        singerIndex = 0;
    }

    ~vsqPhonemeDB(){
        //UtauDB::dbClear();
        /*for( unsigned int i = 0; i < voiceDBs.size(); i++ ){
            SAFE_DELETE( voiceDBs[i] );
        }
        voiceDBs.clear();*/
    }

    void setRuntimeOption( RuntimeOption &option )
    {
        _codepage_otoini = option.getEncodingOtoIni();
    }


public:

    int singerIndex;

    string _codepage_otoini;


private:

    bool isInitialized;


};

#endif

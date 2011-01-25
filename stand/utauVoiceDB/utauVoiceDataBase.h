/*
 *
 *    utauVoiceDataBase.h
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
#ifndef __utauVoiceDataBase_h__
#define __utauVoiceDataBase_h__

#include "utauParameters.h"
#include "utauFreq.h"
#include "../stand.h"

class utauVoiceDataBase {
public:
    utauVoiceDataBase(){
    }

    ~utauVoiceDataBase();

    int readUtauVoiceDataBase( string_t fileName, const char *codepage );

    int getUtauParameters( utauParameters& parameters, string_t search );

    int getVoicePath( string_t& dst ){
        dst = voicePath; 
        return 1;
    }

    bool empty( void ){
        return settingMap.empty();
    }
protected:
private:
    string_t voicePath;
    map_t<string_t, utauParameters*>    settingMap;
    list<utauParameters*>    settingList;
};

#endif

/*
 *
 *    corpusManager.h
 *                        (c) HAL 2010-
 *
 *  This files is a part of v.Connect.
 * corpusManager is a class that controls corpus based on UTAU.
 * This class convert UTAU WAVE corpus into WORLD specgrams.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __corpusManager_h__
#define __corpusManager_h__

#include "standSpecgram.h"
#include "standMelCepstrum.h"
#include "vowelTable.h"
#include "utauVoiceDB/UtauDB.h"
#include "standData.h"
#include "vConnectSetting.h"

class corpusManager
{
public:
    ~corpusManager();
    void setUtauDB( UtauDB *p, runtimeOptions &options );
    
    void analyze();

    standData *getStandData( string_t lyric, runtimeOptions &options );
    bool checkEnableExtention();

private:
    map_t<string_t, standData *> objectMap;
    map_t<string_t, standMelCepstrum *> textureMap;
    UtauDB *mUtauDB;
    string_t mDBPath;
    vowelTable vowels;

    vConnectSetting setting;
    bool enableExtention;
};

#endif // __corpusManager_h__

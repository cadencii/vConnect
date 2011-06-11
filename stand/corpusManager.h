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

#include "vConnectSetting.h"
#include "runtimeOptions.h"
#include <list>

class vConnectPhoneme;
class vsqEventEx;

class corpusManager
{
public:
    ~corpusManager();

    class phoneme {
    public:
        phoneme()
        {
            p = NULL;
            isProcessing = false;
            isValid = false;
            waitHandle = NULL;
            fixedLength = 0;
        }
        vConnectPhoneme *p;
        bool isProcessing;
        bool isValid;
        float fixedLength;
        mutex_t waitHandle;
    };

    struct itemForAnalyze {
        list<vsqEventEx*> itemList;
    };

    void setUtauDB( UtauDB *p, runtimeOptions &options );
    phoneme *corpusManager::getPhoneme( string_t lyric );

    /// <summary>
    /// 今のところ分析済みファイルの読み込みを行っています．
    /// 動作が変わる可能性も．
    /// </summary>
    /// <param name="p"> vsqEventList へのポインタ．</param>
    void analyze(void *p);

    bool checkEnableExtention();

private:

    map_t<string_t, phoneme *> objectMap;
    UtauDB *mUtauDB;
    string_t mDBPath;

    vConnectSetting setting;
    bool enableExtention;
};

#endif // __corpusManager_h__

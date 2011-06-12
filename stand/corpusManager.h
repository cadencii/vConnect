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
#if defined( STND_MULTI_THREAD )
            waitHandle = NULL;
#endif
            fixedLength = 0;
        }
        vConnectPhoneme *p;
        bool isProcessing;
        bool isValid;
        float fixedLength;
#if defined( STND_MULTI_THREAD )
        mutex_t waitHandle;
#endif
    };

    struct itemForAnalyze {
        list<vsqEventEx*> itemList;
    };

    void setUtauDB( UtauDB *p, runtimeOptions &options );
    phoneme *getPhoneme( string_t lyric );

    /// <summary>
    /// 今のところ分析済みファイルの読み込みを行っています．
    /// 動作が変わる可能性も．
    /// </summary>
    /// <param name="p">解析する音素のリスト．</param>
    void analyze( vector<string_t> &phonemes );

    bool checkEnableExtention();

private:

    map_t<string_t, phoneme *> objectMap;
    UtauDB *mUtauDB;
    string_t mDBPath;

    vConnectSetting setting;
    bool enableExtention;
};

#endif // __corpusManager_h__

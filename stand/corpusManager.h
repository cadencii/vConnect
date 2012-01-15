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

#include <list>
#include "stand.h"
#include "utau/UtauDB.h"
#include "vConnectSetting.h"
#include "RuntimeOption.h"
#include "vsq/Sequence.h"
#include "Mutex.h"

using namespace vconnect;

class vConnectPhoneme;

class corpusManager
{
public:
    corpusManager();
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
            brightness = 64;
            enableBrightness = false;
            frequency = 0.0f;
            enableFrequency = false;
            children = NULL;
        }
        vConnectPhoneme *p;
        bool isProcessing;
        bool isValid;
        float fixedLength;

        int brightness;
        bool enableBrightness;

        float frequency;
        bool enableFrequency;
        phoneme *children;
#if defined( STND_MULTI_THREAD )
        Mutex *waitHandle;
#endif
    };

    struct itemForAnalyze {
        list<Event*> itemList;
    };

    void setUtauDB( UtauDB *p, RuntimeOption &option );

    /// <summary>
    /// 歌詞にマッチする音素片を得ます．
    /// </summary>
    /// <param name="lyric"> 検索する音素片に対応する歌詞 </param>
    /// <returns> 検索に成功したとき該当する音素片へのポインタ，それ以外のときはNULLを返します． </returns>
    phoneme *getPhoneme( string lyric );

    /// <summary>
    /// 歌詞にマッチする音素片を引数で指定したリストへ追加します．
    /// 追加データベースを使用する場合はこちらの関数を使用してください．
    /// </summary>
    /// <param name="lyric"> 検索する音素片に対応する歌詞 </param>
    /// <param name="phonemeList"> 音素片を追加するリスト </param>
    phoneme *getPhoneme(string lyric, list<phoneme*> &phonemeList);

    /// <summary>
    /// 今のところ分析済みファイルの読み込みを行っています．
    /// 動作が変わる可能性も．
    /// </summary>
    /// <param name="p">解析する音素のリスト．</param>
    void analyze( vector<string> &phonemes );

    void setCorpusSetting(librarySetting *setting);

    bool checkEnableExtention();

    void setIsAppend(bool isAppend){ mIsAppend = isAppend; }
    void setBrightness(int bri){ if(0 <= bri && bri <= 128) mBrightness = bri; }

private:

    Map<string, phoneme *> objectMap;
    vector<corpusManager *> mAppendCorpus;
    UtauDB *mUtauDB;
    string mDBPath;

    vConnectSetting setting;
    bool mEnableExtention;
    int mBrightness;
    bool mEnableBrightness;
    float mFrequency;
    bool mEnableFrequency;

    bool mIsAppend;
};

#endif // __corpusManager_h__

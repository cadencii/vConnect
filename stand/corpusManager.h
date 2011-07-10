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
            brightness = 64;
            enableBrightness = false;
            frequency = 0.0f;
            enableFrequency = false;
        }
        vConnectPhoneme *p;
        bool isProcessing;
        bool isValid;
        float fixedLength;

        int brightness;
        bool enableBrightness;

        float frequency;
        bool enableFrequency;
#if defined( STND_MULTI_THREAD )
        mutex_t waitHandle;
#endif
    };

    struct itemForAnalyze {
        list<vsqEventEx*> itemList;
    };

    void setUtauDB( UtauDB *p, runtimeOptions &options );

    /// <summary>
    /// 歌詞にマッチする音素片を得ます．
    /// </summary>
    /// <param name="lyric"> 検索する音素片に対応する歌詞 </param>
    /// <returns> 検索に成功したとき該当する音素片へのポインタ，それ以外のときはNULLを返します． </returns>
    phoneme *getPhoneme( string_t lyric );

    /// <summary>
    /// 歌詞にマッチする音素片を引数で指定したリストへ追加します．
    /// 追加データベースを使用する場合はこちらの関数を使用してください．
    /// </summary>
    /// <param name="lyric"> 検索する音素片に対応する歌詞 </param>
    /// <param name="phonemeList"> 音素片を追加するリスト </param>
    void getPhoneme(string_t lyric, list<phoneme*> &phonemeList);

    /// <summary>
    /// 今のところ分析済みファイルの読み込みを行っています．
    /// 動作が変わる可能性も．
    /// </summary>
    /// <param name="p">解析する音素のリスト．</param>
    void analyze( vector<string_t> &phonemes );

    void setCorpusSetting(librarySetting *setting);

    bool checkEnableExtention();

private:

    map_t<string_t, phoneme *> objectMap;
    vector<corpusManager *> mAppendCorpus;
    UtauDB *mUtauDB;
    string_t mDBPath;

    vConnectSetting setting;
    bool mEnableExtention;
    int mBrightness;
    bool mEnableBrightness;
    float mFrequency;
    bool mEnableFrequency;
};

#endif // __corpusManager_h__

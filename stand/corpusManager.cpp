/*
 * corpusManager.cpp
 * Copyright (C) 2010- HAL,
 * Copyright (C) 2011-2012 kbinani.
 *
 *  This files is a part of v.Connect.
 * corpusManager is a class that controls corpus based on UTAU.
 * This class convert UTAU WAVE corpus into WORLD specgrams.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "corpusManager.h"
#include "utauVoiceDB/UtauDB.h"
#include "vConnectPhoneme.h"
#include "vsqMetaText/Sequence.h"
#include "Configuration.h"

corpusManager::corpusManager()
{
    mUtauDB = NULL;
    mBrightness = 64;
    mFrequency = 440.0;
}

void corpusManager::analyze( vector<string> &phonemes )
{
    int size = phonemes.size();
    for( int i = 0; i < size; i++ ){
        string lyric = phonemes[i];
        getPhoneme( lyric );
        cout << "corpusManager::analyze; lyric=" << lyric << endl;
    }
    for( int i = 0; i < mAppendCorpus.size(); i++ ){
        if( mAppendCorpus[i] ){
            mAppendCorpus[i]->analyze(phonemes);
        }
    }

    mIsAppend = mEnableExtention = mEnableBrightness = mEnableFrequency = false;
}

corpusManager::~corpusManager()
{
    map_t<string, phoneme *>::iterator i;
    for( i = objectMap.begin(); i != objectMap.end(); i++ )
    {
        SAFE_DELETE( i->second->p );
        SAFE_DELETE( i->second );
    }
    for( int j = 0; j < mAppendCorpus.size(); j++)
    {
        delete mAppendCorpus[j];
    }

    // うーん…
    if(mIsAppend)
    {
        delete mUtauDB;
    }
}

corpusManager::phoneme *corpusManager::getPhoneme( string lyric )
{
    phoneme *ret = NULL;
    map_t<string, phoneme *>::iterator i;
    string alphabet, vtd_path;

#ifdef STND_MULTI_THREAD
    if( hMutex )
    {
        stnd_mutex_lock( hMutex );
    }
#endif

    // まず vConnectPhoneme の有無をチェック．
    i = objectMap.find( lyric );

    if( i != objectMap.end() )         // 希望のデータが存在するのでそれを返す．
    {
#ifdef STND_MULTI_THREAD
        if( hMutex )
        {
            stnd_mutex_unlock( hMutex );
        }
        if( i->second->isProcessing )        // あるにはあるけど分析中なので待機．
        {
            if( i->second->waitHandle )
            {
                stnd_mutex_lock( i->second->waitHandle );
                // ロックが取得できたってことは分析終了なので即解放
                stnd_mutex_unlock( i->second->waitHandle );
                stnd_mutex_destroy( i->second->waitHandle );
            }
            i->second->waitHandle = NULL;
        }
#endif

        if( i->second->isValid )
        {             // 有効性を見る．
            ret = i->second;
        }
        else
        {
            ret = NULL;
        }
    }
    else
    {                              // 希望するデータが存在しないので作成する．
        UtauParameter parameters;
        phoneme *target = new phoneme;  // ハッシュには先に突っ込んでしまう．
        objectMap.insert( make_pair( lyric, target ) );

#ifdef STND_MULTI_THREAD
        target->isProcessing = true;
        target->waitHandle = stnd_mutex_create();// CreateEvent(NULL,TRUE,FALSE,NULL);
        stnd_mutex_lock( target->waitHandle );
        if( hMutex )
        {
            stnd_mutex_unlock( hMutex );
        }
#endif

        // UTAU の原音設定が無ければ読み込みを行わない．
        if( mUtauDB->getParams( parameters, lyric ) )
        {
            target->p = new vConnectPhoneme;
            string path = mDBPath + parameters.fileName;
            bool bResult = false;
            if( parameters.isWave ){
                double framePeriod = Configuration::getMilliSecondsPerFrame();
                bResult = target->p->readRawWave( mDBPath, &parameters, framePeriod );
            }else{
                bResult = target->p->readPhoneme( path.c_str() );
            }
            if( bResult ){
                target->isValid = true;
                target->fixedLength = parameters.msFixedLength;
                ret = target;
                target->brightness = mBrightness;
                target->frequency = mFrequency;
                target->enableBrightness = mEnableBrightness;
                target->enableFrequency = mEnableFrequency;
            }
        }
        target->isProcessing = false;

#ifdef STND_MULTI_THREAD
        if( target->waitHandle )
        {
            stnd_mutex_unlock( target->waitHandle );
        }
#endif
    }
    return ret;
}


corpusManager::phoneme *corpusManager::getPhoneme(string lyric, list<phoneme*> &phonemeList)
{
    phoneme *p = NULL;

    // 有効な音素なら追加する．
    if((p = getPhoneme(lyric)) && p->isValid && p->p)
    {
        p->enableBrightness = mEnableBrightness;
        p->enableFrequency = mEnableFrequency;
        phonemeList.push_back(p);
    }

    // アペンドがあるならそれを追加．
    for(int i = 0; i < mAppendCorpus.size(); i++)
    {
        if(mAppendCorpus[i] && p)
        {
            p->children = mAppendCorpus[i]->getPhoneme(lyric, phonemeList);
        }
    }
    return p;
}

void corpusManager::setUtauDB( UtauDB *p, RuntimeOption &options )
{
    string tmp;
    mUtauDB = p;
    if( p )
    {
        p->getDBPath( mDBPath );
    }
    tmp = "vConnect.ini";
    mEnableExtention =  setting.readSetting( mDBPath, tmp, options.getEncodingOtoIni().c_str()); // 文字コード指定は暫定処置

    if(mEnableExtention)
    {
        setCorpusSetting(setting.getLibrarySetting(SETTING_BASE));
        librarySetting *brightnessSetting = setting.getLibrarySetting(SETTING_BRIGHTNESS);
        if(mEnableBrightness && brightnessSetting)
        {
            UtauDB *db = new UtauDB();
            mAppendCorpus.resize(1, NULL);
            mAppendCorpus[0] = new corpusManager();
            db->read(
                Path::combine( brightnessSetting->path, "oto.ini" ),
                options.getEncodingOtoIni().c_str()
            );
            mAppendCorpus[0]->setBrightness(brightnessSetting->brightness);
            mAppendCorpus[0]->setUtauDB(db, options);
            mAppendCorpus[0]->setIsAppend(true);
        }
    }
}

bool corpusManager::checkEnableExtention()
{
    return mEnableExtention;
}

void corpusManager::setCorpusSetting(librarySetting *setting)
{
    if(!setting)
    {
        mEnableBrightness = mEnableFrequency = false;
        return;
    }
    mEnableBrightness = true;
    mBrightness = setting->brightness;

    // 暫定．
    mEnableFrequency = false;
    mFrequency = setting->frequency;
}

/*
 *
 *    corpusManager.cpp
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
#include "corpusManager.h"
#include "utauVoiceDB/UtauDB.h"
#include "vConnectPhoneme.h"
#include "vsqMetaText/vsqFileEx.h"

void corpusManager::analyze(void *p)
{
    itemForAnalyze *items = (itemForAnalyze*)p;
    map_t<string_t, utauParameters *>::iterator itr;
    for( list<vsqEventEx*>::iterator i = items->itemList.begin(); i != items->itemList.end(); i++ )
    {
        string_t lyric = (*i)->lyricHandle.getLyric();
        this->getPhoneme( lyric );
        cout << "corpusManager::analyze; lyric=" << lyric << endl;
    }
}

corpusManager::~corpusManager()
{
    map_t<string_t, phoneme *>::iterator i;
    for( i = objectMap.begin(); i != objectMap.end(); i++ )
    {
        SAFE_DELETE( i->second->p );
        SAFE_DELETE( i->second );
    }
}

corpusManager::phoneme *corpusManager::getPhoneme( string_t lyric )
{
    phoneme *ret = NULL;
    map_t<string_t, phoneme *>::iterator i;
    string_t alphabet, vtd_path;

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
        utauParameters parameters;
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
            if( target->p->readPhoneme( (mDBPath + parameters.fileName).c_str() ) )
            {
                target->isValid = true;
                target->fixedLength = parameters.msFixedLength;
                ret = target;
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


void corpusManager::setUtauDB( UtauDB *p, runtimeOptions &options )
{
    string_t tmp;
    mUtauDB = p;
    if( p )
    {
        p->getDBPath( mDBPath );
    }
    tmp = _T("vConnect.ini");
    enableExtention = setting.readSetting( mDBPath, tmp, options.encodingOtoIni.c_str()); // 文字コード指定は暫定処置
}

bool corpusManager::checkEnableExtention()
{
    return enableExtention;
}

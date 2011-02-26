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

corpusManager::~corpusManager()
{
    map_t<string_t, standData*>::iterator i;
    map_t<string_t, standMelCepstrum*>::iterator j;
    for( i = objectMap.begin(); i != objectMap.end(); i++ ){
        SAFE_DELETE( (i->second)->specgram );
        SAFE_DELETE( i->second );
    }
    for( j = textureMap.begin(); j != textureMap.end(); j++ ){
        SAFE_DELETE( j->second );
    }
    
}

standData* corpusManager::getStandData( string_t lyric, runtimeOptions& options)
{
    standData *ret = NULL;
    map_t<string_t, standData*>::iterator i;
    map_t<string_t, standMelCepstrum*>::iterator j;
    string_t alphabet, vtd_path;
    bool fast = options.fast;

#ifdef STND_MULTI_THREAD
    if( hMutex ){
        stnd_mutex_lock( hMutex );
    }
#endif

    // まず standData の有無をチェック．
    i = objectMap.find( lyric );

    if( i != objectMap.end() ){         // 希望のデータが存在するのでそれを返す．
#ifdef STND_MULTI_THREAD
        // うーん．
        if( hMutex ){
            stnd_mutex_unlock( hMutex );
        }
        if( i->second->isProcessing ){        // あるにはあるけど分析中なので待機．
            if( i->second->waitHandle ){
                stnd_mutex_lock( i->second->waitHandle );
                // ロックが取得できたってことは分析終了なので即解放
                stnd_mutex_unlock( i->second->waitHandle );
                stnd_mutex_destroy( i->second->waitHandle );
                //WaitForSingleObject( i->second->waitHandle, INFINITE );
                //CloseHandle( i->second->waitHandle );
            }
            i->second->waitHandle = NULL;
        }
#endif

        if(i->second->isValid){             // 有効性を見る．
            ret = i->second;
        }else{
            ret = NULL;
        }
    }else{                              // 希望するデータが存在しないので作成する．
        utauParameters parameters;
        standData* target = new standData;  // ハッシュには先に突っ込んでしまう．
        objectMap.insert( make_pair( lyric, target ) );

#ifdef STND_MULTI_THREAD
        target->isProcessing = true;
        target->waitHandle = stnd_mutex_create();// CreateEvent(NULL,TRUE,FALSE,NULL);
        stnd_mutex_lock( target->waitHandle );
        if( hMutex ){
            stnd_mutex_unlock( hMutex );
        }
#endif

        if(voiceDB->getUtauParameters( parameters, lyric )){
            target->specgram = new standSpecgram;
            if(target->specgram->computeWaveFile( voicePath + parameters.fileName, parameters, fast )){
                target->isValid = true;
                ret = target;
            }
            // 設定が読み込めていてかつ拡張ライブラリを一つでも読み込めた場合は拡張を有効化する．
            target->enableExtention = target->readMelCepstrum(setting, lyric);
        }
        target->isProcessing = false;

#ifdef STND_MULTI_THREAD
        if( target->waitHandle ){
            stnd_mutex_unlock( target->waitHandle );
        }
#endif
    }
    return ret;
}

void corpusManager::setVoiceDB( utauVoiceDataBase* p, runtimeOptions& options )
{
    string_t tmp;
    voiceDB = p;
    if(p){
        p->getVoicePath( voicePath );
    }
    tmp = _T("vConnect.ini");
    enableExtention = setting.readSetting(voicePath, tmp, options.encodingOtoIni.c_str()); // 文字コード指定は暫定処置
    //tmp = voicePath + _T("vowelTable.txt");
    //vowels.readVowelTable( tmp, options );
}

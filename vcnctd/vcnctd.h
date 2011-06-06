/**
 * vcnctd.h
 *
 * Copyright © 2011 kbinani.
 */

#ifndef __vcnctd_h__
#define __vcnctd_h__

#if defined( WIN32 )
    #ifdef _DEBUG
        #define _CRTDBG_MAP_ALLOC
        #include <stdlib.h>
        #include <new>
        #include <crtdbg.h>
        /*#ifndef DBG_NEW
            #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
            #define new DBG_NEW
        #endif*/
    #endif  // _DEBUG
#endif

#include "../stand/standData.h"
#include "../stand/mb_text.h"
#include "../stand/vConnect.h"
#include "../stand/corpusManager.h"
#include "../stand/standFrame.h"
#include "../stand/vsqMetaText/vsqBase.h"
#include "../stand/vsqMetaText/vsqTempo.h"
#include "../stand/vsqMetaText/vsqPhonemeDB.h"
#include "../stand/vsqMetaText/vsqHandle.h"
#include "../stand/vsqMetaText/vsqFileEx.h"
#include "../stand/utauVoiceDB/UtauDB.h"
#include "../stand/vsqMetaText/vsqEventList.h"
#include "../stand/vsqMetaText/vsqLyric.h"
#include "../stand/vsqMetaText/vsqEventEx.h"
#include "../stand/vowelTable.h"
#include "../stand/vsqMetaText/vsqBPList.h"
#include "../stand/standSpecgram.h"
#include "../stand/world/world.h"
#include "../stand/waveFileEx/waveFileEx.h"
#include "../stand/matching.h"
#include "../stand/vConnectSetting.h"
#include "../stand/standMelCepstrum.h"
#include "../stand/stand.h"

namespace vcnctd
{
    
    namespace vsq
    {
        typedef vsqEventList EventList;
        typedef vsqEventEx Event;
        typedef vsqLyric Lyric;
        typedef vsqHandle Handle;
        typedef vsqFileEx Sequence;
    }
    
    namespace stand
    {
        typedef vConnect Synth;
        typedef runtimeOptions Options;
        typedef standMelCepstrum MelCeps;
        typedef standSpecgram Specgram;
        typedef standFrame Frame;
        typedef standData SynthSource;
        typedef corpusManager CorpusManager;
        typedef librarySetting librarySetting;
    }

}

#endif

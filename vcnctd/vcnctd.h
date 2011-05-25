/**
 * vcnctd.h
 *
 * Copyright © 2011 kbinani.
 */

#ifndef __vcnctd_h__

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
        typedef vConnect Synthesizer;
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

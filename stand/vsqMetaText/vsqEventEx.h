/*
 *
 *    vsqEventEx.h
 *                        (c) HAL 2010-
 *
 *  This files is a part of v.Connect.
 * vsqEventEx contains vsq-event's properties
 * and some UTAU properties such as preutterance and voice-overlap.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __vsqEventEx_h__
#define __vsqEventEx_h__

#include "vsqBase.h"
#include "vsqHandle.h"

class vsqEventEx : public vsqBase {
public:

    vsqEventEx(){
        utauSetting.fileName = _T("");
        utauSetting.lyric = _T("");
        utauSetting.msLeftBlank = 0.0f;
        utauSetting.msFixedLength = 0.0f;
        utauSetting.msRightBlank = 0.0f;
        utauSetting.msPreUtterance = 0.0f;
        utauSetting.msVoiceOverlap = 0.0f;
        vibratoDelay = 0;
        isContinuousBack = false;
        isRest = false;

        portamentoDepth = 8;
        portamentoLength = 0;
        portamentoUse = 0;
        decay = 50;
        attack = 50;

        singerIndex = 0;
    }
    void    setParameter( string_t left, string_t right );

    vsqHandle    vibratoHandle;
    vsqHandle    lyricHandle;
    vsqHandle    iconHandle;

    string_t            type;
    long            tick;
    long            length;
    long            vibratoDelay;
    unsigned char    note;
    char            velocity;
    unsigned char    portamentoDepth;
    unsigned char    portamentoLength;
    unsigned char    portamentoUse;
    unsigned char    decay;
    unsigned char    attack;

    bool            isVCV;
    bool            isContinuousBack;
    bool            isRest;

    int             singerIndex;

    long            beginFrame;
    long            endFrame;

    utauParameters    utauSetting;
};

#endif

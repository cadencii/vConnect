/*
 *
 *    vsqBase.h
 *                        (c) HAL 2010-
 *
 *  This files is a part of v.Connect.
 * vsq*.* are codes for vsqMetaText.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __vsqBase_h__
#define __vsqBase_h__

#include "../stand.h"
#include "../utau/UtauDB.h"

#define TICK_PER_BEAT 480
#define CONTROL_CURVE_NUM 7
#define A4_PITCH    440.0
#define A4_NOTE        69

/*  If __UTAU_OFSET__ is defined, preutterance and voiceoverlap */
/* will be treated as offset. So sequence's value will be added */
/* by the value in phoneme DataBase.                            */
//#define __UTAU_OFFSET__

const string controlCurveName[]={
    "[PitchBendBPList]",
    "[PitchBendSensBPList]",
    "[DynamicsBPList]",
    "[EpRResidualBPList]",            /* BRE Curve */
    "[EpRESlopeBPList]",            /* BRI Curve */
    "[EpRESlopeDepthBPList]",        /* CLE Curve */
    "[GenderFactorBPList]"            /* GEN Curve */
};
const int controlCurveDefaultValue[] = {
    0,
    2,
    64,
    0,
    64,
    0,
    64
};
enum{
    PITCH_BEND=0,
    PITCH_BEND_SENS,
    DYNAMICS,
    BRETHINESS,
    BRIGHTNESS,
    CLEARNESS,
    GENDER
};

const string OBJ_NAME_OTOINI = "[oto.ini]";
const string OBJ_NAME_EVENT_LIST = "[EventList]";
const string OBJ_NAME_TEMPO = "[Tempo]";

#endif

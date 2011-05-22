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
#include "../utauVoiceDB/utauVoiceDataBase.h"

#define TICK_PER_BEAT 480
#define CONTROL_CURVE_NUM 7
#define A4_PITCH    440.0
#define A4_NOTE        69

/*  If __UTAU_OFSET__ is defined, preutterance and voiceoverlap */
/* will be treated as offset. So sequence's value will be added */
/* by the value in phoneme DataBase.                            */
//#define __UTAU_OFFSET__

const string_t controlCurveName[]={
    _T("[PitchBendBPList]"),
    _T("[PitchBendSensBPList]"),
    _T("[DynamicsBPList]"),
    _T("[EpRResidualBPList]"),            /* BRE Curve */
    _T("[EpRESlopeBPList]"),            /* BRI Curve */
    _T("[EpRESlopeDepthBPList]"),        /* CLE Curve */
    _T("[GenderFactorBPList]")            /* GEN Curve */
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

class vsqBase{
public:
    virtual ~vsqBase(){}
    virtual void                setParameter( string_t left, string_t right ){}
protected:
    static    double                tempo;
    static    long                endTick;
    //static    vector<utauVoiceDataBase*> voiceDBs;
    static    map_t<string_t, vsqBase*> objectMap;
    static    map_t<string_t, int> singerMap;

    static    void    outputError( const char* p ){ cout << p << endl; }
};

#endif

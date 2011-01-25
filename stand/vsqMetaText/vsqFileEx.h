/*======================================================*/
/*                                                      */
/*    vsqFileEx.h                                       */
/*                        (c) HAL 2009-                 */
/*         vsqFileEx class is the I/O class             */
/*                        of VOCALOID sequence file.    */
/*                                                      */
/*======================================================*/
#ifndef __vsqFileEx_h__
#define __vsqFileEx_h__

#include "vsqEventList.h"
#include "vsqBPList.h"
#include "vsqTempo.h"
#include "vsqPhonemeDB.h"
#include "../runtimeOptions.h"

class vsqFileEx : public vsqBase {
public:
    vsqFileEx();

    int    readVsqFile( string_t fileName, runtimeOptions options );

    vector<utauVoiceDataBase*>* getVoiceDBs( void ){ return &voiceDBs; }
    double                getEndSec( void ){ return vsqTempoBp.tickToSecond( endTick ); }
    int                   getSingerIndex( string_t t_search );

    vector<vsqBPList>    controlCurves;
    vsqEventList        events;
    vsqTempo            vsqTempoBp;
protected:
private:
    vsqPhonemeDB        voiceDataBase;
};

#endif

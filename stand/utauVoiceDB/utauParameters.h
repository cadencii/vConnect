/*======================================================*/
/*                                                      */
/*    utauParameters.h                                  */
/*                        (c) HAL 2009-                 */
/*        utauParameters structure contains             */
/*            the data of utau Voice Settings.          */
/*                                                      */
/*======================================================*/
#ifndef __utauParameters_h__
#define __utauParameters_h__

#include "../stand.h"

struct utauParameters {
    string_t    lyric; // こっちはワイド文字切り替え
    string_t    fileName; // ファイル名はマルチバイト文字で固定
    float    msLeftBlank;
    float    msFixedLength;
    float    msRightBlank;
    float    msPreUtterance;
    float    msVoiceOverlap;
};

#endif

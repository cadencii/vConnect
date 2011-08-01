/**
 * runtimeOptions.h
 * Copyright (C) 2010 kbinani, HAL
 *
 *  This files is a part of v.Connect.
 * runtimeOptions contains data that is necessary in v.Connect.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __runtimeOptions_h__
#define __runtimeOptions_h__

#include <string.h>

/**
 * vConnectでの合成時のオプション
 */
class runtimeOptions{
public:
    runtimeOptions(){
        convert = false;
        transcribe = false;
        encodingOtoIni = "Shift_JIS";
        encodingVsqText = "Shift_JIS";
        encodingVowelTable = "Shift_JIS";
        encodingVoiceTexture = "Shift_JIS";
    }

    string encodingOtoIni;
    string encodingVsqText;
    string encodingVowelTable;
    string encodingVoiceTexture;

    bool convert;
    bool transcribe;
};

#endif // __runtimeOptions_h__

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
        fast = true;
        wspMode = false;
        f0Transform = true;
        volumeNormalization = true;
        encodingOtoIni = "Shift_JIS";
        encodingVsqText = "Shift_JIS";
        encodingVowelTable = "Shift_JIS";
        encodingVoiceTexture = "Shift_JIS";
    }

    bool fast;
    bool wspMode;
    bool f0Transform;
    bool volumeNormalization;
    string encodingOtoIni;
    string encodingVsqText;
    string encodingVowelTable;
    string encodingVoiceTexture;
};

#endif // __runtimeOptions_h__

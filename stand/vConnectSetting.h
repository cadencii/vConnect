/*
 * vConnectSetting.h
 * Copyright © 2011 HAL, kbinani
 *
 * This file is part of vConnect-STAND.
 *
 * org.kbinani.cadencii is free software; you can redistribute it and/or
 * modify it under the terms of the GPL License.
 *
 * org.kbinani.cadencii is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef __vConnectSetting_h__
#define __vConnectSetting_h__

#include "stand.h"
#include "vsqMetaText/vsqBase.h"

enum vConnectSettingList{
    SETTING_BASE = 0,
    SETTING_BRIGHTNESS = 1,
    SETTING_LOW = 2,
    SETTING_HI = 3,
    SETTING_END = 4,
};

class librarySetting{
public:
    librarySetting(){
        enabled = false;
    }
    bool readSetting( string_t left, string_t right );

    bool enabled;
    string_t path;
    int brightness;
    int noteNumber;
    double frequency;
};

class vConnectSetting{
public:
    vConnectSetting();
    ~vConnectSetting();
    bool readSetting(string_t path, string_t fileName, const char *code);
    librarySetting *getLibrarySetting(vConnectSettingList settingKind){return libraryArray[settingKind];}
private:
    map_t<string_t, librarySetting*> libraryMap;
    vector<librarySetting*>          libraryArray;
    string path;
};

#endif

#ifndef __vConnectSetting_h__
#define __vConnectSetting_h__

#include "stand.h"

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
    bool readSetting(string left_t, string_t right);

    bool enabled;
    string_t path;
    int brightness;
    int noteNumber;
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

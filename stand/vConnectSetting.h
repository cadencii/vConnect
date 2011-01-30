#ifndef __vConnectSetting_h__
#define __vConnectSetting_h__

#include "stand.h"

class librarySetting{
public:
    librarySetting(){
        enabled = false;
    }
    bool readSetting(string left, string right);

    bool enabled;
    string path;
    int brightness;
    int noteNumber;
};

class vConnectSetting{
public:
    vConnectSetting();
    bool readSetting(string_t path, const char *code);
private:
    librarySetting base;
    librarySetting brightness;
    librarySetting hi;
    librarySetting low;

    map_t<string_t, librarySetting*> libraryMap;
};

#endif

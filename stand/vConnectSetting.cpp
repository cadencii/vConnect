#include "vConnectSetting.h"

#define SETTING_BUF_LEN 4096

const string_t settingName[] = {
    "[BaseLibrary]",
    "[BrightnessLibrary]",
    "[LowLibrary]",
    "[HiLibrary]",
};

enum{
    SETTING_BASE = 0,
    SETTING_BRIGHTNESS = 1,
    SETTING_LOW =2,
    SETTING_HI =3,
};

bool librarySetting::readSetting(string left, string right)
{
    bool ret = true;
    if(left.compare("Enable")==0){
        enabled = (right.compare("0") == 0);
    }else if(left.compare("Brightness")){
        brightness = atoi(right.c_str());
    }else if(left.compare("NoteNumber")){
        noteNumber = atoi(right.c_str());
    }else if(left.compare("Directory")){
        path = right;
    }else{
        ret = false;
    }
    return ret;
}

vConnectSetting::vConnectSetting()
{
    libraryMap.insert(make_pair(settingName[SETTING_BASE], &base));
    libraryMap.insert(make_pair(settingName[SETTING_BRIGHTNESS], &brightness));
    libraryMap.insert(make_pair(settingName[SETTING_LOW], &low));
    libraryMap.insert(make_pair(settingName[SETTING_HI], &hi));
}

bool vConnectSetting::readSetting(string_t path, const char *code)
{
    bool ret = false;
    string pathString;
    MB_FILE *fp;
    mb_conv(path, pathString);
    fp = mb_fopen(pathString, code);

    if(fp){
        string_t tmp;
        string left, right;
        map_t<string_t, librarySetting*>::iterator currentParse = libraryMap.end();
        int index;
        while(mb_fgets(tmp, fp)){
            if(tmp.find(_T("[")) != string_t::npos){
                currentParse = libraryMap.find(tmp);
                continue;
            }
            index = tmp.find(_T("="));
            if(index != string_t::npos){
                mb_conv(tmp.substr(0, index), left);
                mb_conv(tmp.substr(index+1), right);
            }else{
                mb_conv(tmp, left);
                right = "";
            }
            if(currentParse != libraryMap.end()){
                currentParse->second->readSetting(left, right);
            }
        }
        ret = true;
        mb_fclose(fp);
    }
    return ret;
}

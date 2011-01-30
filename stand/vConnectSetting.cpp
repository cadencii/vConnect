#include "vConnectSetting.h"

#define SETTING_BUF_LEN 4096

const string_t settingName[] = {
    "[BaseLibrary]",
    "[BrightnessLibrary]",
    "[LowLibrary]",
    "[HiLibrary]",
};

bool librarySetting::readSetting(string_t left, string_t right)
{
    bool ret = true;
    if(left.compare(_T("Enable"))==0){
        enabled = (right.compare(_T("0")) == 1);
    }else if(left.compare(_T("Brightness")) == 0){
        brightness = atoi(right.c_str());
    }else if(left.compare(_T("NoteNumber")) == 0){
        noteNumber = atoi(right.c_str());
    }else if(left.compare(_T("Directory")) == 0){
        path += right;
    }else{
        ret = false;
    }
    return ret;
}

vConnectSetting::vConnectSetting()
{
    libraryArray.resize(SETTING_END);
    for(int i = 0; i < SETTING_END; i++){
        libraryArray[i] = new librarySetting;
        libraryMap.insert(make_pair(settingName[i], libraryArray[i]));
    }
}

vConnectSetting::~vConnectSetting()
{
    for(map_t<string_t, librarySetting*>::iterator i = libraryMap.begin(); i != libraryMap.end(); i++){
        delete i->second;
    }
}

bool vConnectSetting::readSetting(string_t path, string_t fileName, const char *code)
{
    bool ret = false;
    string_t iniName = path + fileName;
    string pathString;
    MB_FILE *fp;
    mb_conv(iniName, pathString);
    fp = mb_fopen(pathString, code);

    this->path = path;

    if(fp){
        string_t tmp;
        string left, right;
        map_t<string_t, librarySetting*>::iterator currentParse = libraryMap.end();
        int index;
        for(int i = 0; i < SETTING_END; i++){
            libraryArray[i]->path = this->path;
        }
        while(mb_fgets(tmp, fp)){
            if(tmp.find(_T("[")) != string_t::npos){
                currentParse = libraryMap.find(tmp);
                continue;
            }
            index = tmp.find(_T("="));
            if(index != string_t::npos){
                left = tmp.substr(0, index);
                right = tmp.substr(index + 1);
            }else{
                left = tmp;
                right = _T("");
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

/*
 * vConnectSetting.cpp
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
#include "vConnectSetting.h"
#include "TextInputStream.h"

#define SETTING_BUF_LEN 4096

using namespace vconnect;

const string settingName[] = {
    "[BaseLibrary]",
    "[BrightnessLibrary]",
    "[LowLibrary]",
    "[HiLibrary]",
};

bool librarySetting::readSetting(string left, string right)
{
    bool ret = true;
    if( left.compare( "Enable" ) == 0 ){
        enabled = (right.compare( "0" ) == 1);
    }else if( left.compare( "Brightness" ) == 0 ){
        brightness = atoi( right.c_str() );
    }else if( left.compare( "NoteNumber" ) == 0 ){
        noteNumber = atoi( right.c_str() );
        frequency = A4_PITCH * pow( 2.0, (double)(noteNumber - A4_NOTE) / 12.0 );
    }else if( left.compare( "Directory" ) == 0 ){
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
        libraryArray[i]->brightness = 64;
        libraryArray[i]->enabled = false;
        libraryArray[i]->frequency = A4_PITCH;
        libraryArray[i]->noteNumber = A4_NOTE;
        libraryArray[i]->path = "";
        libraryMap.insert(make_pair(settingName[i], libraryArray[i]));
    }
}

vConnectSetting::~vConnectSetting()
{
    for(Map<string, librarySetting*>::iterator i = libraryMap.begin(); i != libraryMap.end(); i++){
        delete i->second;
    }
}

bool vConnectSetting::readSetting( string path, string fileName, const char *code )
{
    bool ret = false;
    string iniName = path + fileName;
    TextInputStream stream( iniName, code );

    this->path = path;

    if( false == stream.ready() ){
        return ret;
    }

    string tmp;
    string left, right;
    Map<string, librarySetting*>::iterator currentParse = libraryMap.end();
    int index;
    for( int i = 0; i < SETTING_END; i++ ){
        libraryArray[i]->path = this->path;
    }
    while( stream.ready() ){
        tmp = stream.readLine();
        if( tmp.find( "[" ) != string::npos ){
            currentParse = libraryMap.find( tmp );
            continue;
        }
        index = tmp.find( "=" );
        if( string::npos != index ){
            left = tmp.substr( 0, index );
            right = tmp.substr( index + 1 );
        }else{
            left = tmp;
            right = "";
        }
        if( currentParse != libraryMap.end() ){
            currentParse->second->readSetting( left, right );
        }
    }
    ret = true;
    stream.close();

    return ret;
}

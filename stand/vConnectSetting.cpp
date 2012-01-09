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

#define SETTING_BUF_LEN 4096

const string settingName[] = {
    _T( "[BaseLibrary]" ),
    _T( "[BrightnessLibrary]" ),
    _T( "[LowLibrary]" ),
    _T( "[HiLibrary]" ),
};

bool librarySetting::readSetting(string left, string right)
{
    bool ret = true;
    if( left.compare( _T( "Enable" ) ) == 0 ){
        enabled = (right.compare( _T( "0" ) ) == 1);
    }else if( left.compare( _T( "Brightness" ) ) == 0 ){
        brightness = atoi( right.c_str() );
    }else if( left.compare( _T( "NoteNumber" ) ) == 0 ){
        noteNumber = atoi( right.c_str() );
        frequency = A4_PITCH * pow( 2.0, (double)(noteNumber - A4_NOTE) / 12.0 );
    }else if( left.compare( _T( "Directory" ) ) == 0 ){
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
        libraryArray[i]->path = _T( "" );
        libraryMap.insert(make_pair(settingName[i], libraryArray[i]));
    }
}

vConnectSetting::~vConnectSetting()
{
    for(map_t<string, librarySetting*>::iterator i = libraryMap.begin(); i != libraryMap.end(); i++){
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
    map_t<string, librarySetting*>::iterator currentParse = libraryMap.end();
    int index;
    for( int i = 0; i < SETTING_END; i++ ){
        libraryArray[i]->path = this->path;
    }
    while( stream.ready() ){
        tmp = stream.readLine();
        if( tmp.find( _T( "[" ) ) != string::npos ){
            currentParse = libraryMap.find( tmp );
            continue;
        }
        index = tmp.find( _T( "=" ) );
        if( string::npos != index ){
            left = tmp.substr( 0, index );
            right = tmp.substr( index + 1 );
        }else{
            left = tmp;
            right = _T( "" );
        }
        if( currentParse != libraryMap.end() ){
            currentParse->second->readSetting( left, right );
        }
    }
    ret = true;
    stream.close();

    return ret;
}

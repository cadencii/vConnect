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

const string_t settingName[] = {
    _T( "[BaseLibrary]" ),
    _T( "[BrightnessLibrary]" ),
    _T( "[LowLibrary]" ),
    _T( "[HiLibrary]" ),
};

bool librarySetting::readSetting(string_t left, string_t right)
{
    bool ret = true;
    string tmp;
    if( left.compare( _T( "Enable" ) ) == 0 ){
        enabled = (right.compare( _T( "0" ) ) == 1);
    }else if( left.compare( _T( "Brightness" ) ) == 0 ){
        mb_conv( right, tmp ); 
        brightness = atoi( tmp.c_str() );
    }else if( left.compare( _T( "NoteNumber" ) ) == 0 ){
        mb_conv( right, tmp );
        noteNumber = atoi( tmp.c_str() );
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
    mb_conv( iniName, pathString );
    fp = mb_fopen( pathString, code );

    mb_conv( path + _T( "" ), this->path );

    if( !fp ){
        return ret;
    }

    string_t tmp;
    string_t left, right;
    map_t<string_t, librarySetting*>::iterator currentParse = libraryMap.end();
    int index;
    for( int i = 0; i < SETTING_END; i++ ){
        mb_conv( this->path, libraryArray[i]->path );// = this->path;
    }
    while( mb_fgets( tmp, fp ) ){
        if( tmp.find( _T( "[" ) ) != string_t::npos ){
            currentParse = libraryMap.find( tmp );
            continue;
        }
        index = tmp.find( _T( "=" ) );
        if( string_t::npos != index ){
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
    mb_fclose( fp );

    return ret;
}

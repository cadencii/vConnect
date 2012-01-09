/*
 * UtauDB.cpp
 * Copyright © 2009-2012 HAL, 2012 kbinani
 *
 * This file is part of vConnect-STAND.
 *
 * vConnect-STAND is free software; you can redistribute it and/or
 * modify it under the terms of the GPL License.
 *
 * vConnect-STAND is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "UtauDB.h"

vector<UtauDB *> UtauDB::mDBs;

map_t<string, UtauParameter *>::iterator UtauDB::begin()
{
    return mSettingMap.begin();
}

map_t<string, UtauParameter *>::iterator UtauDB::end()
{
    return mSettingMap.end();
}

int UtauDB::dbSize()
{
    return (int)UtauDB::mDBs.size();
}

UtauDB *UtauDB::dbGet( int index )
{
    return UtauDB::mDBs[index];
}

void UtauDB::dbRegist( UtauDB *db )
{
    UtauDB::mDBs.push_back( db );
}

void UtauDB::dbClear()
{
    for( unsigned int i = 0; i < UtauDB::mDBs.size(); i++ )
    {
        SAFE_DELETE( UtauDB::mDBs[i] );
    }
    UtauDB::mDBs.clear();
}

UtauDB::~UtauDB()
{
    list<UtauParameter*>::iterator i;
    for( i = mSettingList.begin(); i != mSettingList.end(); i++ )
    {
        if( (*i) )
        {
            delete (*i);
        }
    }
}

int UtauDB::read( string path_oto_ini, const char *codepage )
{
    int result = 2;

    int index;
    path_oto_ini = Path::normalize( path_oto_ini );

    TextInputStream stream( path_oto_ini, codepage );

    if( false == stream.ready() ){
        return result;
    }

    mDBPath = path_oto_ini.substr( 0, path_oto_ini.rfind( Path::getDirectorySeparator() ) + 1 );

    while( stream.ready() ){
        string line = stream.readLine();
        UtauParameter *current = new UtauParameter( line );
        mSettingMap.insert( make_pair( current->lyric, current ) );
        if( current->lyric.compare( current->fileName ) != 0 ){
            mSettingMap.insert( make_pair( current->fileName, current ) );
        }
        mSettingList.push_back( current );
    }
    if( result != 3 ){
        result = 1;
    }
    stream.close();

    return result;
}

int UtauDB::getParams( UtauParameter &parameters, string search )
{
    int    result = 0;
    map_t<string, UtauParameter *>::iterator i = mSettingMap.find( search );
    if( i != mSettingMap.end() )
    {
        if( i->second )
        {
            parameters.fileName = i->second->fileName;
            parameters.lyric = i->second->lyric;
            parameters.msFixedLength = i->second->msFixedLength;
            parameters.msLeftBlank = i->second->msLeftBlank;
            parameters.msPreUtterance = i->second->msPreUtterance;
            parameters.msRightBlank = i->second->msRightBlank;
            parameters.msVoiceOverlap = i->second->msVoiceOverlap;
            parameters.isWave = i->second->isWave;
            result = 1;
        }
    }
    return result;
}

int UtauDB::getDBPath( string &dst )
{
    dst = mDBPath;
    return 1;
}

bool UtauDB::empty()
{
    return mSettingMap.empty();
}

int UtauDB::size()
{
    return mSettingList.size();
}

int UtauDB::getParams(UtauParameter &parameters, int index)
{
    int ret = 0;
    list<UtauParameter*>::iterator it = mSettingList.begin();
    for(int i = 0; i < size() && it != mSettingList.end(); i++, it++)
    {
        if(index == i)
        {
            ret = 1;
            parameters.fileName = (*it)->fileName;
            parameters.lyric = (*it)->lyric;
            parameters.msFixedLength = (*it)->msFixedLength;
            parameters.msLeftBlank = (*it)->msLeftBlank;
            parameters.msPreUtterance = (*it)->msPreUtterance;
            parameters.msRightBlank = (*it)->msRightBlank;
            parameters.msVoiceOverlap = (*it)->msVoiceOverlap;
            parameters.isWave = (*it)->isWave;
            break;
        }
    }
    if(it!= mSettingList.end())
    {
        parameters = *(*it);
    }
    return ret;
}

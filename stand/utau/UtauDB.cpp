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
#include "../TextInputStream.h"

namespace vconnect
{
    Map<string, UtauParameter *>::iterator UtauDB::begin()
    {
        return mSettingMap.begin();
    }

    Map<string, UtauParameter *>::iterator UtauDB::end()
    {
        return mSettingMap.end();
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

    UtauDB::UtauDB( string path_oto_ini, string codepage )
    {
        int result = 2;

        int index;
        path_oto_ini = Path::normalize( path_oto_ini );

        TextInputStream stream( path_oto_ini, codepage );

        if( false == stream.ready() ){
            return;
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

        return;
    }

    int UtauDB::getParams( UtauParameter &parameters, string search )
    {
        int    result = 0;
        Map<string, UtauParameter *>::iterator i = mSettingMap.find( search );
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

    string UtauDB::getOtoIniPath()
    {
        return this->mDBPath;
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
}

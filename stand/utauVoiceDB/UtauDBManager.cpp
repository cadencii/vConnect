/*
 * UtauDBManager.cpp
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
#include "UtauDBManager.h"

namespace vconnect
{
    vector<UtauDB *> UtauDBManager::mDBs;

    int UtauDBManager::size()
    {
        return (int)UtauDBManager::mDBs.size();
    }

    UtauDB *UtauDBManager::get( int index )
    {
        return UtauDBManager::mDBs[index];
    }

    void UtauDBManager::regist( UtauDB *db )
    {
        UtauDBManager::mDBs.push_back( db );
    }

    void UtauDBManager::clear()
    {
        for( unsigned int i = 0; i < UtauDBManager::mDBs.size(); i++ ){
            if( UtauDBManager::mDBs[i] ){
                delete UtauDBManager::mDBs[i];
                UtauDBManager::mDBs[i] = NULL;
            }
        }
        UtauDBManager::mDBs.clear();
    }
}

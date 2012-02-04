/*
 * BPList.cpp
 * Copyright © 2010-2012 HAL, 2012 kbinani
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
#include "BPList.h"
#include "../Configuration.h"

namespace vconnect
{
    void BPList::setParameter( string left, string right )
    {
        BP current;
        current.tick = atoi( left.c_str() );
        current.value = atoi( right.c_str() );

        data.push_back( current );
    }

    void BPList::setParameter( long tick, int value )
    {
        BP current;
        current.tick = tick;
        current.value = value;
        data.push_back( current );
    }

    void BPList::getList( vector<FrameBP>& dst, TempoList &tempo )
    {
        vector<BP>::size_type size = this->data.size();
        dst.clear();
        dst.resize( size );
        double framePerMilliSecond = 1.0 / Configuration::getMilliSecondsPerFrame();
        for( vector<BP>::size_type i = 0; i < size; i++ ){
            dst[i].value = this->data[i].value;
            dst[i].frameTime = INT_MAX;                // the value will continue till this time.
            if( i ){
                double second = tempo.tickToSecond( this->data[i].tick );
                dst[i - 1].frameTime = 1000.0 * second * framePerMilliSecond;
            }
        }
    }

    BP BPList::get( int index )
    {
        return this->data[index];
    }
}

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
#include "../stand.h"

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

    void BPList::getList( vector<FrameBP>& dst, double tempo )
    {
        vector<BP>::size_type size = this->data.size();
        dst.clear();
        dst.resize( size );
        for( vector<BP>::size_type i = 0; i < size; i++ ){
            dst[i].value = this->data[i].value;
            dst[i].frameTime = INT_MAX;                // the value will continue till this time.
            if( i ){
                dst[i - 1].frameTime = (long)(1000.0 * (double)(this->data[i].tick) / 480.0 * 60.0 / tempo / framePeriod);
            }
        }
    }
}

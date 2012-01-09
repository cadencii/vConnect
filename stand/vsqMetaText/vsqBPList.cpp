/*
 *
 *    vsqBPList.cpp
 *                        (c) HAL 2010-
 *
 *  This files is a part of v.Connect.
 * vsqBPList class contains and reads vsqBP data.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#include "vsqBPList.h"

void vsqBPList::setParameter( string left, string right )
{
    vsqBP current;
    current.tick = atoi( left.c_str() );
    current.value = atoi( right.c_str() );

    data.push_back( current );
}

void vsqBPList::setParameter( long tick, int value )
{
    vsqBP current;
    current.tick = tick;
    current.value = value;
    data.push_back( current );
}

void vsqBPList::getList( vector<standBP>& dst, double tempo )
{
    int size = data.size();
    dst.clear();
    dst.resize( size );
    for( long i = 0; i < size; i++ )
    {
        dst[i].value = data[i].value;
        dst[i].frameTime = INT_MAX;                // the value will continue till this time.
        if( i )
        {
            dst[i - 1].frameTime = (long)(1000.0 * (double)(data[i].tick) / 480.0 * 60.0 / tempo / framePeriod);
        }
    }
}

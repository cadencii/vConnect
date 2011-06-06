/**
 * vsqEventList.cpp
 * Copyright (C) 2009-2011 HAL,
 * Copyright (C) 2011 kbinani
 */
#include "vsqEventList.h"

vsqEventList::~vsqEventList()
{
    for( unsigned long i = 0; i < eventList.size(); i++ )
    {
        if( eventList[i] )
        {
            delete eventList[i];
        }
    }
}

void vsqEventList::setParameter( string_t left, string_t right, map_t<string_t, vsqEventEx *> &id_map )
{
    /* If Back is EOS, Front is End Tick */
    string s;
    if( right.compare( _T("EOS") ) == 0 )
    {
        mb_conv( left, s );
        endTick = atol( s.c_str() );
    }
    else
    {
        vsqEventEx *target = new vsqEventEx();
        right = _T("[") + right + _T("]");
        mb_conv( left, s );
        target->tick = atoi( s.c_str() );
        eventList.push_back( target );
        id_map.insert( make_pair( right, target ) );
    }
}


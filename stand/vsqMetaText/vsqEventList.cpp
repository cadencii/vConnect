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

void vsqEventList::setParameter( string left, string right, map_t<string, vsqEventEx *> &id_map )
{
    /* If Back is EOS, Front is End Tick */
    if( right.compare( _T("EOS") ) == 0 ){
        endTick = atol( left.c_str() );
    }else{
        vsqEventEx *target = new vsqEventEx();
        right = _T("[") + right + _T("]");
        target->tick = atoi( left.c_str() );
        eventList.push_back( target );
        id_map.insert( make_pair( right, target ) );
    }
}


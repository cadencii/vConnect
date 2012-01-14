/**
 * EventList.cpp
 * Copyright (C) 2009-2011 HAL,
 * Copyright (C) 2011-2012 kbinani
 */
#include "EventList.h"

namespace vconnect
{
    EventList::~EventList()
    {
        for( unsigned long i = 0; i < eventList.size(); i++ )
        {
            if( eventList[i] )
            {
                delete eventList[i];
            }
        }
    }

    void EventList::setParameter( string left, string right, Map<string, Event *> &id_map )
    {
        /* If Back is EOS, Front is End Tick */
        if( right.compare( "EOS" ) == 0 ){
            endTick = atol( left.c_str() );
        }else{
            Event *target = new Event();
            right = "[" + right + "]";
            target->tick = atoi( left.c_str() );
            eventList.push_back( target );
            id_map.insert( make_pair( right, target ) );
        }
    }
}

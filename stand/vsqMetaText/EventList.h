/**
 * EventList.h
 * Copyright (C) 2009-2011 HAL,
 * Copyright (C) 2011-2012 kbinani
 */
#ifndef __EventList_h__
#define __EventList_h__

#include "vsqBase.h"
#include "Event.h"

namespace vconnect
{
    class EventList
    {
    public:

        ~EventList();

        void setParameter( string left, string right, map_t<string, Event *> &id_map );


    public:

        vector<Event *> eventList;

        /// <summary>
        /// �V�[�P���X�̒����itick�P�ʁj
        /// </summary>
        long endTick;
    };
}
#endif

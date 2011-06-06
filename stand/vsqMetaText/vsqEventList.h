/**
 * vsqEventList.h
 * Copyright (C) 2009-2011 HAL,
 * Copyright (C) 2011 kbinani
 */
#ifndef __vsqEventList_h__
#define __vsqEventList_h__

#include "vsqBase.h"
#include "vsqEventEx.h"

class vsqEventList// : public vsqBase
{

public:

    ~vsqEventList();

    void setParameter( string_t left, string_t right, map_t<string_t, vsqEventEx *> &id_map );


public:

    vector<vsqEventEx *> eventList;

    /// <summary>
    /// シーケンスの長さ（tick単位）
    /// </summary>
    long endTick;
};

#endif

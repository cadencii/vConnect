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

    void setParameter( string left, string right, map_t<string, vsqEventEx *> &id_map );


public:

    vector<vsqEventEx *> eventList;

    /// <summary>
    /// �V�[�P���X�̒����itick�P�ʁj
    /// </summary>
    long endTick;
};

#endif

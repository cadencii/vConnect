/*
 * BPList.h
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
 * ---------------------------------------------------------------------
 * vsqBPList class contains and reads vsqBP data.
 */
#ifndef __BPList_h__
#define __BPList_h__

#include <string>
#include <vector>
#include "BP.h"
#include "../stand.h"

namespace vconnect
{
    using namespace std;

    class BPList
    {
    public:
        void getList( vector<standBP> &dst, double tempo );

        void setParameter( long tick, int value );

        void setParameter( string left, string right );

    private:
        vector<BP> data;
    };
}
#endif

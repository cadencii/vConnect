/*
 * TempoList.cpp
 * Copyright © 2010-2012 HAL
 * Copyright © 2012 kbinani
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
#include "TempoList.h"
#include "Sequence.h"

namespace vconnect
{
    const double TempoList::DEFAULT_TEMPO = 120.0;

    void TempoList::setParameter( string left, string right )
    {
        tempo = atof( left.c_str() );
    }

    long TempoList::secondToTick( double second )
    {
        /* In This Code, only static tempo is available */
        return (long)(second / 60.0 * tempo * Sequence::getTickPerBeat());
    }

    double TempoList::tickToSecond( long tick )
    {
        return 60.0 / tempo * (double)tick / Sequence::getTickPerBeat();
    }
}

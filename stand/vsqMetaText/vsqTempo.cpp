/*
 *
 *    vsqTempo.cpp
 *                        (c) HAL 2010-
 *
 *  This files is a part of v.Connect.
 * vsqTempo contains tempo data in vsq meta-text extention.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#include "vsqTempo.h"
#include "Sequence.h"

using namespace vconnect;

const double vsqTempo::DEFAULT_TEMPO = 120.0;

void vsqTempo::setParameter( string left, string right )
{
    tempo = atof( left.c_str() );
}

long vsqTempo::secondToTick( double second )
{
    /* In This Code, only static tempo is available */
    return (long)(second / 60.0 * tempo * Sequence::getTickPerBeat());
}

double vsqTempo::tickToSecond( long tick )
{
    return 60.0 / tempo * (double)tick / Sequence::getTickPerBeat();
}

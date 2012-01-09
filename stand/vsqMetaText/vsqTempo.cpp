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

const double vsqTempo::DEFAULT_TEMPO = 120.0;

void vsqTempo::setParameter( string left, string right )
{
    string s;
    mb_conv( left, s );
    tempo = atof( s.c_str() );

}

long vsqTempo::secondToTick( double second )
{
  /* In This Code, only static tempo is available */
  long result = (long)( second / 60.0 * tempo * TICK_PER_BEAT );

  return result;
}

double vsqTempo::tickToSecond( long tick )
{
  double result = 60.0/tempo*(double)tick/TICK_PER_BEAT;

  return result;
}

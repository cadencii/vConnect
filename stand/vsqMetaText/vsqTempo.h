/*
 *
 *    vsqTempo.h
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
#ifndef __vsqTempo_h__
#define __vsqTempo_h__

#include "vsqBase.h"

class vsqTempo// : public vsqBase
{

public:
    vsqTempo()
    {
        tempo = DEFAULT_TEMPO;
    }

    void setParameter( string left, string right );

    long secondToTick( double second );
    double tickToSecond( long tick );

    /// <summary>
    /// �e���|�l���擾���܂��D
    /// </summary>
    double getTempo()
    {
        return tempo;
    }

public:

    /// <summary>
    /// �f�t�H���g�̃e���|�l�ł�
    /// </summary>
    static const double DEFAULT_TEMPO;


private:

    /// <summary>
    /// �e���|�l
    /// </summary>
    double tempo;

};

#endif

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

    void setParameter( string_t left, string_t right );

    long secondToTick( double second );
    double tickToSecond( long tick );

    /// <summary>
    /// テンポ値を取得します．
    /// </summary>
    double getTempo()
    {
        return tempo;
    }

public:

    /// <summary>
    /// デフォルトのテンポ値です
    /// </summary>
    static const double DEFAULT_TEMPO;


private:

    /// <summary>
    /// テンポ値
    /// </summary>
    double tempo;

};

#endif

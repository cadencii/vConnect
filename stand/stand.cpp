/*
 * stand.cpp
 * Copyright (C) 2010-2011 HAL, kbinani
 *
 *  This file is a part of STAND Library.
 * STAND Library is a wrapper library of WORLD.
 * It provides analysis and synthesis of WAVE file(s).
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#include "stand.h"

#ifdef STND_MULTI_THREAD
// あんまり使いたくないグローバル変数．
mutex_t hMutex = NULL;
mutex_t hFFTWMutex = NULL;
#endif

/**
 * str中の文字列searchをreplaceに置換する
 */
void string_replace( string_t& str, string_t search, string_t replace )
{
    int indx = str.find( search, 0 );
    int slen = search.length();
    int rlen = replace.length();
    while( indx != string_t::npos )
    {
        str.replace( indx, slen, replace );
        indx = str.find( search, indx - slen + rlen + 1 );
    }
}

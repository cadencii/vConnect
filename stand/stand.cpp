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

/**
 * 引数に与えられた文字列をパスと見なし、環境に応じたパス区切り文字となるよう正規化する
 */
void normalize_path_separator( string_t& path )
{
    string_t search = _T( "" );
    string_t replace = _T( "" );
    if( PATH_SEPARATOR.compare( PATH_SEPARATOR_SLANT ) == 0 )
    {
        // \\を全て/に置き換える
        search = PATH_SEPARATOR_BACKSLASH;
        replace = PATH_SEPARATOR_SLANT;
    }
    else if( PATH_SEPARATOR.compare( PATH_SEPARATOR_BACKSLASH ) == 0 )
    {
        // /を全て\\に置き換える
        search = PATH_SEPARATOR_SLANT;
        replace = PATH_SEPARATOR_BACKSLASH;
    }

    // yen markをreplaceに置き換える
    if( replace.compare( _T( "" ) ) != 0 )
    {
        int i;
        for( i = 0; i < NUM_PSUDE_PATH_SEPARATOR; i++ )
        {
            string_replace( path, PSUDE_PATH_SEPARATOR[i], replace );
        }

        // 目的の区切り文字を全て置換
        if( search.compare( _T( "" ) ) != 0 )
        {
            string_replace( path, search, replace );
        }
    }
}

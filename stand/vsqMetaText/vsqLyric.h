/**
 * vsqLyric.h
 * Copyright (C) 2010 HAL, kbinani
 *
 * This file is part of v.Connect.
 *
 * v.Connect is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL Lisence.
 *
 * v.Connect is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef __vsqLyric_h__
#define __vsqLyric_h__

#include "../stand.h" // for string_t

class vsqLyric
{
public:

    void setLyric( string_t right );

    string toString()
    {
        string ret;
        string str_lyric;
        string str_pronounce;
        mb_conv( lyric, str_lyric );
        mb_conv( pronounce, str_pronounce );
        ret += "{lyric=" + str_lyric + ",pronounce=" + str_pronounce + "}";
        return ret;
    }

    string_t    lyric;
    string_t    pronounce;
    int        lyricDelta;
    short    consonantAdjustment;
    char    protectFlag;
};

#endif // __vsqLyric_h__

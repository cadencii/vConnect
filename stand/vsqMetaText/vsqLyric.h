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

class    vsqLyric{
public:
    void setLyric( string_t right );

    string_t    lyric;
    string_t    pronounce;
    int        lyricDelta;
    short    consonantAdjustment;
    char    protectFlag;
};

#endif // __vsqLyric_h__

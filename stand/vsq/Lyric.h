/**
 * Lyric.h
 * Copyright (C) 2010 HAL, 2012 kbinani
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
#ifndef __Lyric_h__
#define __Lyric_h__

#include <string>

namespace vconnect
{
    using namespace std;

    class Lyric
    {
    private:
        string lyric;

    public:
        Lyric();

        Lyric( string line );

        string getLyric();

        string toString();
    };
}
#endif

/**
 * Lyric.cpp
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
#include "Lyric.h"

namespace vconnect
{
    Lyric::Lyric()
    {
        this->lyric = "あ";
    }

    Lyric::Lyric( string line )
    {
        this->lyric = line.substr( 0, line.find( "," ) );
        this->lyric = this->lyric.substr( this->lyric.find( "\"" ) + 1, this->lyric.rfind( "\"" ) - 1 );
    }

    string Lyric::getLyric()
    {
        return this->lyric;
    }

    string Lyric::toString()
    {
        string ret;
        ret += "{lyric=" + this->lyric + "}";
        return ret;
    }
}

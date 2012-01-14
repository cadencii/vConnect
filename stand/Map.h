/*
 * Map.h
 * Copyright © 2012 kbinani
 *
 * This file is part of vConnect-STAND.
 *
 * vConnect-STAND is free software; you can redistribute it and/or
 * modify it under the terms of the GPL License.
 *
 * vConnect-STAND is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef __Map_h__
#define __Map_h__

#ifdef __GNUC__
    #include <map>
#else
    #include <hash_map>
#endif

namespace vconnect
{
#ifdef __GNUC__
    using namespace std;
#else
    using namespace stdext;
#endif

    template<typename K, typename V>
    class Map
#ifdef __GNUC__
        : public map<K, V>
#else
        : public hash_map<K, V>
#endif
    {
    };
}
#endif

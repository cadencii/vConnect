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

#include <map>

namespace vconnect
{
    using namespace std;

    template<
        typename _Key,
        typename _Tp,
        typename _Compare = std::less<_Key>,
        typename _Alloc = std::allocator<std::pair<const _Key, _Tp> >
    >
    class Map : public map<_Key, _Tp, _Compare, _Alloc>
    {
    };
}
#endif

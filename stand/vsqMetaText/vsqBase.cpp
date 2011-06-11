/*
 *
 *    vsqBase.cpp
 *                        (c) HAL 2010-
 *
 *  This files is a part of v.Connect.
 * vsq*.* are codes for vsqMetaText.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#include "vsqBase.h"

long                       vsqBase::endTick = 0;
double                     vsqBase::tempo = 120.0;
//vector<utauVoiceDataBase*> vsqBase::voiceDBs;
map_t<string_t, int>       vsqBase::singerMap;
map_t<string_t, vsqBase*>  vsqBase::objectMap;

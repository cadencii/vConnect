/*
 *
 *    vowelTable.h
 *                        (c) HAL 2010-
 *
 *  This files is a part of v.Connect.
 * vowelTable is an extention for UTAU databases.
 * It contains corresponding vowel-table to UTAU aliases.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#ifndef __vowelTable_h__
#define __vowelTable_h__

#include "stand.h"
#include "runtimeOptions.h"

class vowelTable{
public:
    ~vowelTable();

    string_t    getAlphabet( string_t lyric );
    bool    readVowelTable( string_t input, runtimeOptions& options );
private:
    map_t<string_t, string_t*> vowel_map;
};


#endif

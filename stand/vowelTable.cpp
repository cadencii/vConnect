/**
 *
 *    vowelTable.cpp
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
#include "vowelTable.h"

vowelTable::~vowelTable()
{
    map_t<string_t, string_t*>::iterator i;
    for( i = vowel_map.begin(); i != vowel_map.end(); i++ )
        SAFE_DELETE( i->second );
}

bool vowelTable::readVowelTable( string_t input, runtimeOptions& options )
{
    MB_FILE *fp;
    bool ret = false;
    //char buf[LINEBUFF_LEN];
    string_t left;
    string_t *right;

    //TODO: Shift_JISはruntimeOptionsから取るようにする
    fp = mb_fopen( input.c_str(), options.encodingVowelTable.c_str() );

    if( fp ){
        while( mb_fgets( left, fp ) ){
            right = new string_t;
            //left = buf;
            // BEFORE=>
            //*right = left.substr( left.find( "\t" ) + 1 );
            // <=
            // AFTER=>
            mb_conv( left.substr( left.find( _T("\t") ) + 1 ), *right ); // *rightの書き方これでイイのかわかんねぇ・・・
            // <=
            left = left.substr( 0, left.find( _T("\t") ) );

            vowel_map.insert( make_pair( left, right ) );
        }
        mb_fclose( fp );
        ret = true;
    }
    return ret;
}

string_t vowelTable::getAlphabet( string_t lyric )
{
    map_t<string_t, string_t*>::iterator i;
    string_t s;
    int indx_space = lyric.find( _T(" ") );
    if( indx_space != string_t::npos ){
        lyric = lyric.substr( indx_space + 1 );
    }

    i = vowel_map.find( lyric );
    if( i != vowel_map.end() )
        s = *(i->second);
    return s;
}

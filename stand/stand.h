/**
 * stand.h
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
#ifndef __stand_h__
#define __stand_h__

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#if __GNUC__
#include <sys/stat.h>
#else
#include <direct.h>
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>

#include "world/world.h"
#include "waveFileEx/waveFileEx.h"
#include "Mutex.h"

using namespace std;

#if defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#  define STND_OS_MAC
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define STND_OS_WIN
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  define STND_OS_WIN
#elif defined(__MWERKS__) && defined(__INTEL__)
#  define STND_OS_WIN
#else
#  define STND_OS_OTHER
#endif

#if defined( _UNICODE ) || defined( UNICODE )
#define USE_WSTRING
#endif

//Compile-time parameters summary
//  OS: Windows, MacOSX, openSUSE   -> STND_OS_WIN, STND_OS_MAC, STND_OTHER
//  iconv: used or not used         -> MB_USE_ICONV (in mb_text.h)
//  wcstombs: used or not used      -> MB_USE_STDLIB (in mb_text.cpp)
//  UNICODE: defined or not defined -> USE_WSTRING (just below in this file)
//Run-time parameters
//  Text-encoding: Shift_JIS, UTF-8, UTF-16LE, UTF-16BE, UTF-32LE, UTF-32BE

//Our code condition in these compile-time parameters
// ( These tables were tested with UTF-8. )
// ( We should make MORE and MORE tables for Run-time parameters, ooops!! )
//(1) In case STND_OS_WIN (VC++)
//    (1-1) In case MB_USE_ICONV defined
//
//                               |  MB_USE_STDLIB defined  |  MB_USE_STDLIB not defined
//        -------------------------------------------------------------------------------
//          UNICODE defined      |    O.K.                 |    O.K. (*1)
//        -------------------------------------------------------------------------------
//          UNICODE not defined  |    O.K. (*1)            |    O.K. (*1)
//
//
//    (1-2) In case MB_USE_ICONV not defined
//                               |  MB_USE_STDLIB defined  |  MB_USE_STDLIB not defined
//        -------------------------------------------------------------------------------
//          UNICODE defined      |    O.K.                 |    N.G.
//        -------------------------------------------------------------------------------
//          UNICODE not defined  |    N.G.                 |    N.G.
//
//
//(2) In case STND_OS_WIN && __GNUC__
//    (2-1) In case MB_USE_ICONV defined
//
//                               |  MB_USE_STDLIB defined  |  MB_USE_STDLIB not defined
//        -------------------------------------------------------------------------------
//          UNICODE defined      |    O.K.                 |    O.K.
//        -------------------------------------------------------------------------------
//          UNICODE not defined  |    O.K.                 |    O.K.
//
//
//    (2-2) In case MB_USE_ICONV not defined
//                               |  MB_USE_STDLIB defined  |  MB_USE_STDLIB not defined
//        -------------------------------------------------------------------------------
//          UNICODE defined      |    N.G.                 |    N.G.
//        -------------------------------------------------------------------------------
//          UNICODE not defined  |    N.G.                 |    N.G.
//
//
//(3) In case STND_OS_MAC
//    (3-1) In case MB_USE_ICONV defined
//
//                               |  MB_USE_STDLIB defined  |  MB_USE_STDLIB not defined
//        -------------------------------------------------------------------------------
//          UNICODE defined      |    O.K.                 |    O.K.
//        -------------------------------------------------------------------------------
//          UNICODE not defined  |    O.K.                 |    O.K.
//
//
//    (3-2) In case MB_USE_ICONV not defined
//                               |  MB_USE_STDLIB defined  |  MB_USE_STDLIB not defined
//        -------------------------------------------------------------------------------
//          UNICODE defined      |    N.G. (*2)            |    N.G. (*2)
//        -------------------------------------------------------------------------------
//          UNICODE not defined  |    N.G. (*2)            |    N.G. (*2)
//
//
//(4) In case STND_OS_OTHER
//    (4-1) In case MB_USE_ICONV defined
//
//                               |  MB_USE_STDLIB defined  |  MB_USE_STDLIB not defined
//        -------------------------------------------------------------------------------
//          UNICODE defined      |    O.K.                 |    O.K.
//        -------------------------------------------------------------------------------
//          UNICODE not defined  |    O.K.                 |    O.K.
//
//
//    (4-2) In case MB_USE_ICONV not defined
//                               |  MB_USE_STDLIB defined  |  MB_USE_STDLIB not defined
//        -------------------------------------------------------------------------------
//          UNICODE defined      |    N.G. (*2)            |    N.G. (*2)
//        -------------------------------------------------------------------------------
//          UNICODE not defined  |    N.G. (*2)            |    N.G. (*2)
//
// *1 patched version only.
//    http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.9.1.bin.woe32.zip would not work
// *2 libiconv is required in MacOSX and Linux to convert character set.
//

// std::map、stdext::hash_mapのどちらを使うかを決める
// g++では、hash_map<string_t ...>が出来ないのでこれを回避する
#ifdef __GNUC__
#include <map>
using namespace std;
#define map_t map
#else
#include <hash_map>
using namespace stdext;
#define map_t hash_map
#endif

#define LINEBUFF_LEN 4096

#ifdef STND_MULTI_THREAD
extern vconnect::Mutex *hMutex;
extern vconnect::Mutex *hFFTWMutex;
#endif

//#define DEBUG
#define ST_PI (3.141592653589793238462643383279)
#define SAFE_DELETE_ARRAY(x) if(x){ delete[] (x); x = NULL; }
#define SAFE_DELETE(x) if(x){ delete(x); x = NULL; }

const int    fs = 44100;

#endif

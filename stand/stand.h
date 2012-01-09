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

#include "mb_text.h"
#include "world/world.h"
#include "waveFileEx/waveFileEx.h"

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

// ファイルから読み込んだもの(音符名など)をwstringで扱うかどうか
// 内部のstring_tが全てwstringに変わるわけではないので注意
// 将来的には、USE_WSTRING -> _UNICODEな感じで！
#ifdef USE_WSTRING
typedef wchar_t char_t;
#else
typedef char char_t;
#endif
typedef std::basic_string<char_t,
                          char_traits<char_t>,
                          allocator<char_t> >
string_t;

// winnt.hにあるTEXTマクロと同じ
#ifndef _T
#ifdef USE_WSTRING
#define _T(x) L##x
#else
#define _T(x) x
#endif
#endif

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

// サポートするcharset
#ifdef MB_USE_ICONV
static unsigned int CHARSET_SUPPORT[] = { 932, 51932, 50220, 1208, 1202, 1200, 1232, 1234 };
#else
#ifdef _WIN32
static unsigned int CHARSET_SUPPORT[] = { CP_UTF8, CP_ACP };
#else
static unsigned int CHARSET_SUPPORT[] = {};
#endif
#endif

#define LINEBUFF_LEN 4096

/*  マルチスレッド実装の種類             動かせる？
    1. win/single-thread/g++                 OK
    2. win/multi-thread/native-thread/g++    OK
    3. win/multi-thread/posix-thread/g++     OK
    4. win/single-thread/VC                  OK
    5. win/multi-thread/native-thread/VC     OK
    6. win/multi-thread/posix-thread/VC      OK
    7. mac/single-thread                     OK
    8. mac/multi-thread/posix-thread         OK
    9. linux/single-thread                   OK
   10. linux/multi-thread/posix-thread       OK
*/

#ifdef STND_MULTI_THREAD
  #ifdef STND_OS_WIN
    //pthreadを使わない場合コメントアウト
    //#define USE_PTHREADS
  #else
    //問答無用でpthreads
    #define USE_PTHREADS
  #endif
#endif

// ヘッダーなど
#ifdef STND_MULTI_THREAD
  #ifdef USE_PTHREADS
    #include <pthread.h>
    #ifdef STND_OS_WIN
      #ifndef __GNUC__
        #pragma comment(lib, "pthreadVC2.lib")
      #endif
    #endif
    typedef pthread_mutex_t *mutex_t;
    typedef pthread_t *thread_t;
    #ifdef STND_OS_WIN
      #define __stnd_declspec __cdecl
    #else
      #define __stnd_declspec
    #endif
    #define __stnd_thread_start_retval void*
  #else
    #include <windows.h>
    #include <process.h>
    typedef HANDLE mutex_t;
    typedef HANDLE thread_t;
    #define __stnd_declspec __stdcall
    #define __stnd_thread_start_retval unsigned int
  #endif
#else
  #define __stnd_declspec
  #define __stnd_thread_start_retval int
#endif

// マルチスレッド関係のインライン関数
#ifdef STND_MULTI_THREAD

/**
 * スレッドが終了するのを待ちます
 */
inline void stnd_thread_join( thread_t tid ){
#ifdef USE_PTHREADS
    pthread_join( *tid, NULL );
#else
    WaitForSingleObject( tid, INFINITE );
#endif
};

/**
 * スレッドの作成と開始を行います
 */
inline thread_t stnd_thread_create( __stnd_thread_start_retval (__stnd_declspec *start)( void * ), void* args ){
#ifdef USE_PTHREADS
    pthread_t *ret = (pthread_t *)malloc( sizeof( pthread_t ) );
    pthread_create( ret, NULL, start, args );
    return ret;
#else
    return (HANDLE)_beginthreadex( NULL, 0, start, args, 0, NULL );
#endif
};

/**
 * スレッドオブジェクトを破棄します
 */
inline void stnd_thread_destroy( thread_t tid ){
#ifdef USE_PTHREADS
    free( tid );
#else
    CloseHandle( tid );
#endif
};

/**
 * 新しいmutexオブジェクトを作成します
 */
inline mutex_t stnd_mutex_create(){
#ifdef USE_PTHREADS
    pthread_mutex_t *ret = (pthread_mutex_t *)malloc( sizeof( pthread_mutex_t ) );
    pthread_mutex_init( ret, NULL );
    return ret;
#else
    return CreateMutex( NULL, FALSE, NULL );
#endif
};

/**
 * stnd_mutex_createで作成したmutexオブジェクトを破棄します
 */
inline void stnd_mutex_destroy( mutex_t mutex ){
#ifdef USE_PTHREADS
    pthread_mutex_destroy( mutex );
    free( mutex );
#else
    CloseHandle( mutex );
#endif
};

/**
 * mutexオブジェクトによりロックを取得します
 */
inline void stnd_mutex_lock( mutex_t mutex ){
#ifdef USE_PTHREADS
    pthread_mutex_lock( mutex );
#else
    WaitForSingleObject( mutex, INFINITE );
#endif
};

/**
 * stnd_mutex_lockで取得したロックを解除します
 */
inline void stnd_mutex_unlock( mutex_t mutex ){
#ifdef USE_PTHREADS
    pthread_mutex_unlock( mutex );
#else
    ReleaseMutex( mutex );
#endif
};
#endif // STND_MULTI_THREAD

#ifdef STND_MULTI_THREAD
extern mutex_t hMutex;
extern mutex_t hFFTWMutex;
#endif

//#define DEBUG
#define ST_PI (3.141592653589793238462643383279)
#define SAFE_DELETE_ARRAY(x) if(x){ delete[] (x); x = NULL; }
#define SAFE_DELETE(x) if(x){ delete(x); x = NULL; }

const int    fs = 44100;
const double framePeriod = 2.0;

struct standBP{
    long frameTime;
    int  value;
};

struct standComplex{
    float re;
    float im;
};

#endif

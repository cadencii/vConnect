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

#define LINEBUFF_LEN 4096

#ifdef STND_MULTI_THREAD
extern vconnect::Mutex *hMutex;
extern vconnect::Mutex *hFFTWMutex;
#endif

//#define DEBUG
#define ST_PI (3.141592653589793238462643383279)
#define SAFE_DELETE_ARRAY(x) if(x){ delete[] (x); x = NULL; }
#define SAFE_DELETE(x) if(x){ delete(x); x = NULL; }

#endif

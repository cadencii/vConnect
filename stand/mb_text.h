/**
 * mb_text.h
 * Copyright (C) 2010 kbinani
 *
 * This file is part of mb_text.
 *
 * mb_text is free software; you can redistribute it and/or
 * modify it under the terms of the BSD License.
 *
 * mb_text is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef __mb_text_h__
#define __mb_text_h__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <clocale>
#include <locale>
#include <iostream>

using namespace std;

#ifndef LINEBUFF_LEN
#define LINEBUFF_LEN 4096
#endif

#if defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#  define MB_OS_MAC
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define MB_OS_WIN
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  define MB_OS_WIN
#elif defined(__MWERKS__) && defined(__INTEL__)
#  define MB_OS_WIN
#else
#  define MB_OS_OTHER
#endif

#endif // __mb_text_h__

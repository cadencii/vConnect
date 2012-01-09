/**
 * mb_text.cpp
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
#include "mb_text.h"
#include <cctype>
#ifdef MB_OS_MAC
#include <stdlib.h>
#else
#include <malloc.h>
#endif

#ifdef MB_OS_WIN
//#define MB_USE_STDLIB
#elif defined( MB_OS_MAC )
//#define MB_USE_STDLIB
#else // MB_OS_WIN
//#define MB_USE_STDLIB
#endif // MB_OS_WIN


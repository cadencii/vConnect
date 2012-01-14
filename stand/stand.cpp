/*
 * stand.cpp
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
#include "stand.h"

#ifdef STND_MULTI_THREAD
vconnect::Mutex *hMutex = NULL;
vconnect::Mutex *hFFTWMutex = NULL;
#endif

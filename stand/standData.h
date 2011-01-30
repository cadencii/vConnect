/**
 * standData.cpp
 * Copyright (C) 2010 HAL, kbinani
 *
 * This file is part of v.Connect.
 *
 * v.Connect is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL Lisence.
 *
 * v.Connect is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef __standData_h__
#define __standData_h__

#include "stand.h"

class standData{
public:
    standData(){
        specgram = NULL;
        /*texture = NULL;*/
        isProcessing = false;
        isValid = false;
#ifdef STND_MULTI_THREAD
        waitHandle = NULL;
#endif
    }
#ifdef STND_MULTI_THREAD
    ~standData(){
        if( waitHandle ){
            stnd_mutex_destroy( waitHandle );
        }
    }
#endif
    standSpecgram *specgram;
    standMelCepstrum melCepstrum;
    bool        isProcessing;
    bool        isValid;
#ifdef STND_MULTI_THREAD
    mutex_t     waitHandle;
#endif
};

#endif // __standData_h__

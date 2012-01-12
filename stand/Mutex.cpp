/*
 * Mutex.cpp
 * Copyright © 2012 kbinani
 *
 * This file is part of vConnect-STAND.
 *
 * vConnect-STAND is free software; you can redistribute it and/or
 * modify it under the terms of the GPL License.
 *
 * vConnect-STAND is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "Mutex.h"

namespace vconnect
{
#ifdef _WIN32
    Mutex::Mutex()
    {
        this->mutex = CreateMutex( NULL, FALSE, NULL );
    }

    Mutex::~Mutex()
    {
        if( this->mutex ){
            CloseHandle( this->mutex );
        }
        this->mutex = NULL;
    }

    void Mutex::lock()
    {
        WaitForSingleObject( this->mutex, INFINITE );
    }

    void Mutex::unlock()
    {
        ReleaseMutex( this->mutex );
    }
#else
    Mutex::Mutex()
    {
        this->mutex = (pthread_mutex_t *)malloc( sizeof( pthread_mutex_t ) );
        pthread_mutex_init( this->mutex, NULL );
    }

    Mutex::~Mutex()
    {
        if( this->mutex ){
            pthread_mutex_destroy( this->mutex );
            free( this->mutex );
        }
        this->mutex = NULL;
    }

    void Mutex::lock()
    {
        pthread_mutex_lock( this->mutex );
    }

    void Mutex::unlock()
    {
        pthread_mutex_unlock( mutex );
    }
#endif
}

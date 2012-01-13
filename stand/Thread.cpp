/*
 * Thread.cpp
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
#include "Thread.h"

#include <stdlib.h>

namespace vconnect
{
#ifdef _WIN32
    Thread::Thread( ThreadWorker start, void *argument )
    {
        this->thread = (HANDLE)_beginthreadex( NULL, 0, start, argument, 0, NULL );
    }

    void Thread::join()
    {
        WaitForSingleObject( this->thread, INFINITE );
    }

    Thread::~Thread()
    {
        CloseHandle( this->thread );
    }

    void Thread::tellThreadEnd()
    {
        _endthreadex( 0 );
    }

    void Thread::sleep( unsigned int milliseconds )
    {
        Sleep( milliseconds );
    }
#else
    Thread::Thread( ThreadWorker worker, void *argument )
    {
        this->thread = (pthread_t *)malloc( sizeof( pthread_t ) );
        pthread_create( this->thread, NULL, worker, argument );
    }

    void Thread::join()
    {
        pthread_join( *this->thread, NULL );
    }

    Thread::~Thread()
    {
        free( this->thread );
    }

    void Thread::tellThreadEnd()
    {
    }

    void Thread::sleep( unsigned int milliseconds )
    {
        struct timespec spec;
        time_t seconds = (time_t)(milliseconds / 1000.0);
        long nanoSeconds = (long)((milliseconds - seconds * 1000) * 1000);
        spec.tv_sec = seconds;
        spec.tv_nsec = nanoSeconds;
        nanosleep( &spec, NULL );
    }
#endif
}

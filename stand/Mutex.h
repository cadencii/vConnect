/*
 * Mutex.h
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
#ifndef __Mutex_h__
#define __Mutex_h__

#ifdef _WIN32
    #include <windows.h>
    #include <process.h>
#else
    #include <pthread.h>
    #include <stdlib.h>
#endif

namespace vconnect
{
    class Mutex
    {
    private:
#ifdef _WIN32
        HANDLE mutex;
#else
        pthread_mutex_t *mutex;
#endif

    public:
        /**
         * コンストラクタ
         */
        Mutex();

        /**
         * デストラクタ
         */
        ~Mutex();

        /**
         * ミューテックスをロックする
         */
        void lock();

        /**
         * ミューテックスのロックを解除する
         */
        void unlock();
    };
}

#endif

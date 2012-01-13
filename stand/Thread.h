/*
 * Thread.h
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
#ifndef __Thread_h__
#define __Thread_h__

#ifdef _WIN32
    #include <windows.h>
    #include <process.h>
#else
    #include <pthread.h>
    #include <unistd.h>
#endif

namespace vconnect
{

#ifdef _WIN32
    #define ThreadWorkerDeclspec __stdcall
    typedef unsigned int ThreadWorkerReturnType;
#else
    #define ThreadWorkerDeclspec
    typedef void* ThreadWorkerReturnType;
#endif
    typedef ThreadWorkerReturnType (ThreadWorkerDeclspec *ThreadWorker)( void *argument );

    class Thread
    {
    private:
#ifdef _WIN32
        HANDLE thread;
#else
        pthread_t *thread;
#endif

    public:
        /**
         * スレッドの作成と開始を行います
         * @param start スレッドで実行するワーカーメソッド
         * @param argument ワーカーメソッドに渡す引数
         */
        Thread( ThreadWorker start, void* argument );

        /**
         * スレッドが停止するのを待つ
         */
        void join();

        /**
         * デストラクタ
         */
        ~Thread();

        /**
         * スレッドのワーカーメソッド内で、ワーカーが終了するときに呼ばなくてはならない
         */
        static void tellThreadEnd();

        /**
         * 指定されたミリ秒数だけスレッドを休止する
         * @param milliseconds 休止するミリ秒数
         */
        static void sleep( unsigned int milliseconds );

    private:
        Thread()
        {
        }
    };
}

#endif

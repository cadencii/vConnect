#ifndef __Thread_h__
#define __Thread_h__

#ifdef _WIN32
    #include <windows.h>
    #include <process.h>
#else
    #include <pthread.h>
#endif

namespace vconnect
{

#ifdef _WIN32
    #define ThreadCallbackDeclspec __stdcall
    typedef unsigned int ThreadCallbackReturn;
#else
    #define ThreadCallbackDeclspec
    typedef void* ThreadCallbackReturn;
#endif
    typedef ThreadCallbackReturn (ThreadCallbackDeclspec *ThreadCallback)( void *argument );

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
        Thread( ThreadCallback start, void* argument );

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

    private:
        Thread()
        {
        }
    };
}

#endif

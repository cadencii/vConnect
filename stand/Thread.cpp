#include "Thread.h"

#include <stdlib.h>

namespace vconnect
{
#ifdef _WIN32
    Thread::Thread( ThreadCallback start, void *argument )
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
#else
    Thread::Thread( ThreadCallback start, void *argument )
    {
        this->thread = (pthread_t *)malloc( sizeof( pthread_t ) );
        pthread_create( this->thread, NULL, start, argument );
    }

    void Thread::join()
    {
        pthread_join( *this->thread, NULL );
    }

    Thread::~Thread()
    {
        free( this->thread );
    }
#endif
}

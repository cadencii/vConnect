#include <boost/test/unit_test.hpp>
#include "../Thread.h"
#include "../Mutex.h"

using namespace vconnect;
using namespace std;

BOOST_AUTO_TEST_SUITE(ThreadTest)

class ByRef
{
public:
    int value;
};

static string result = "";
static Mutex *mutex = NULL;

static ThreadWorkerReturnType ThreadWorkerDeclspec taskA( void *argument )
{
    int *count = (int *)argument;
    mutex->lock();
    for( int i = 0; i < *count; i++ ){
        Thread::sleep( 1 );
        result += "a";
    }
    mutex->unlock();
    Thread::tellThreadEnd();
    return NULL;
}

static ThreadWorkerReturnType ThreadWorkerDeclspec taskB( void *argument )
{
    int *count = (int *)argument;
    mutex->lock();
    for( int i = 0; i < *count; i++ ){
        Thread::sleep( 1 );
        result += "b";
    }
    mutex->unlock();
    Thread::tellThreadEnd();
    return NULL;
}

BOOST_AUTO_TEST_CASE(test)
{
    result = "";
    int count = 5;
    mutex = new Mutex();
    Thread a( &taskA, &count );
    Thread b( &taskB, &count );
    a.join();
    b.join();

    string expected = "";
    for( int i = 0; i < count; i++ ){
        expected += "a";
    }
    for( int i = 0; i < count; i++ ){
        expected += "b";
    }
    BOOST_CHECK_EQUAL( expected, result );
}

BOOST_AUTO_TEST_SUITE_END()

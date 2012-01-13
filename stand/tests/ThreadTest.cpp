#include "AllTests.h"
#include "../Thread.h"
#include "../Mutex.h"

using namespace vconnect;
using namespace std;

class ThreadTest : public CppUnit::TestFixture
{
    class ByRef
    {
    public:
        int value;
    };

private:
    static string result;
    static Mutex *mutex;

public:
    void test()
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
        CPPUNIT_ASSERT_EQUAL( expected, result );
    }

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

    CPPUNIT_TEST_SUITE( ThreadTest );
    CPPUNIT_TEST( test );
    CPPUNIT_TEST_SUITE_END();
};

REGISTER_TEST_SUITE( ThreadTest );

string ThreadTest::result = "";
Mutex *ThreadTest::mutex = NULL;


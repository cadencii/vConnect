#ifndef TEST_ThreadTest
#define TEST_ThreadTest
#include "AllTests.h"
#include "../Thread.h"

using namespace vconnect;
using namespace std;

class ThreadTest : public CppUnit::TestFixture
{
private:
    static string result;

public:
    void test()
    {
        Thread a( &taskA, (void *)"a" );
        a.join();
        Thread b( &taskB, (void *)"b" );
        b.join();
        CPPUNIT_ASSERT_EQUAL( string( "ab" ), result );
    }

    static ThreadCallbackReturn ThreadCallbackDeclspec taskA( void *argument )
    {
        string text = (char *)argument;
        result += text;
        return NULL;
    }

    static ThreadCallbackReturn ThreadCallbackDeclspec taskB( void *argument )
    {
        string text = (char *)argument;
        result += text;
        return NULL;
    }

    CPPUNIT_TEST_SUITE( ThreadTest );
    CPPUNIT_TEST( test );
    CPPUNIT_TEST_SUITE_END();
};

REGISTER_TEST_SUITE( ThreadTest );
#endif

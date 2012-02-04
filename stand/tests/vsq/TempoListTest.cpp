#ifndef TEST_TempoListTest
#define TEST_TempoListTest
#include "../AllTests.h"
#include "../../vsq/TempoList.h"

using namespace std;
using namespace vconnect;

class TempoListTest : public CppUnit::TestFixture {
public:
    void testTickToSecond()
    {
        TempoList list;
        CPPUNIT_ASSERT_EQUAL( 0.5, list.tickToSecond( 480 ) );
    }

    void testPush()
    {
        TempoList list;
        list.push( 0, 100.0 ); // 0.6 s / beat
        list.push( 480, 50.0 ); // 1.2 s / beat

        CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, list.tickToSecond( 0 ), DBL_EPSILON );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.6, list.tickToSecond( 480 ), DBL_EPSILON );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( (double)1.8, list.tickToSecond( 960 ), DBL_EPSILON );
    }

    CPPUNIT_TEST_SUITE( TempoListTest );
    CPPUNIT_TEST( testTickToSecond );
    CPPUNIT_TEST( testPush );
    CPPUNIT_TEST_SUITE_END();
};
REGISTER_TEST_SUITE( TempoListTest );
#endif

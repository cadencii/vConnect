#ifndef TEST_TempoListTest
#define TEST_TempoListTest
#include "../AllTests.h"
#include "../../vsq/TempoList.h"

using namespace std;
using namespace vconnect;

class TempoListTest : public CppUnit::TestFixture {
public:
    void testConstruct()
    {
        TempoList list;
        CPPUNIT_ASSERT_EQUAL( 120.0, list.getTempo() );
    }

    void testSetParameter()
    {
        TempoList list;
        list.setParameter( "60", "" );
        CPPUNIT_ASSERT_EQUAL( 60.0, list.getTempo() );
    }

    void testSecondToTick()
    {
        TempoList list;
        CPPUNIT_ASSERT_EQUAL( (long)480, list.secondToTick( 0.5 ) );
    }

    void testTickToSecond()
    {
        TempoList list;
        CPPUNIT_ASSERT_EQUAL( 0.5, list.tickToSecond( 480 ) );
    }

    CPPUNIT_TEST_SUITE( TempoListTest );
    CPPUNIT_TEST( testConstruct );
    CPPUNIT_TEST( testSetParameter );
    CPPUNIT_TEST( testSecondToTick );
    CPPUNIT_TEST( testTickToSecond );
    CPPUNIT_TEST_SUITE_END();
};
REGISTER_TEST_SUITE( TempoListTest );
#endif

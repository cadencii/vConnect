#ifndef TEST_MapTest
#define TEST_MapTest
#include "AllTests.h"
#include "../Map.h"

using namespace std;
using namespace vconnect;

class MapTest : public CppUnit::TestFixture {
public:
    void test()
    {
        Map<int, string> map;
        map.insert( make_pair( 1, "a" ) );
        CPPUNIT_ASSERT_EQUAL( string( "a" ), map[1] );
    }

    CPPUNIT_TEST_SUITE( MapTest );
    CPPUNIT_TEST( test );
    CPPUNIT_TEST_SUITE_END();
};
REGISTER_TEST_SUITE( MapTest );
#endif

#include <boost/test/unit_test.hpp>
#include "../Map.h"

using namespace std;
using namespace vconnect;

BOOST_AUTO_TEST_SUITE(MapTest)

BOOST_AUTO_TEST_CASE(test)
{
    Map<int, string> map;
    map.insert( make_pair( 1, "a" ) );
    BOOST_CHECK_EQUAL( string( "a" ), map[1] );
}

BOOST_AUTO_TEST_SUITE_END()

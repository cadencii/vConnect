#include <boost/test/unit_test.hpp>
#include "../../vsq/TempoList.h"

using namespace std;
using namespace vconnect;

BOOST_AUTO_TEST_SUITE(TempoListTest)

BOOST_AUTO_TEST_CASE(testTickToSecond)
{
    TempoList list;
    BOOST_CHECK_EQUAL( 0.5, list.tickToSecond( 480 ) );
}

BOOST_AUTO_TEST_CASE(testPush)
{
    TempoList list;
    list.push( 0, 100.0 ); // 0.6 s / beat
    list.push( 480, 50.0 ); // 1.2 s / beat

    BOOST_CHECK_CLOSE(0.0, list.tickToSecond( 0 ), FLT_EPSILON);
    BOOST_CHECK_CLOSE(0.6, list.tickToSecond( 480 ), FLT_EPSILON);
    BOOST_CHECK_CLOSE(1.8, list.tickToSecond( 960 ), FLT_EPSILON);
}

BOOST_AUTO_TEST_SUITE_END()

#include <boost/test/unit_test.hpp>
#include "../utau/UtauParameter.h"

using namespace vconnect;
using namespace std;

BOOST_AUTO_TEST_SUITE(UtauParameterTest)

BOOST_AUTO_TEST_CASE(testConstructor)
{
    UtauParameter p( "あ.wav=,0,135,172,14,12" );
    BOOST_CHECK_EQUAL( string( "あ" ), p.fileName );
    BOOST_CHECK_EQUAL( true, p.isWave );
    BOOST_CHECK_EQUAL( string( "あ" ), p.lyric );
    BOOST_CHECK_EQUAL( 0.0f, p.msLeftBlank );
    BOOST_CHECK_EQUAL( 135.0f, p.msFixedLength );
    BOOST_CHECK_EQUAL( 172.0f, p.msRightBlank );
    BOOST_CHECK_EQUAL( 14.0f, p.msPreUtterance );
    BOOST_CHECK_EQUAL( 12.0f, p.msVoiceOverlap );

    UtauParameter p1( "あ.wav=わ,0,135,172,14,12" );
    BOOST_CHECK_EQUAL( string( "わ" ), p1.lyric );

    UtauParameter p2( "あ.stf=,0,135,172,14,12" );
    BOOST_CHECK_EQUAL( false, p2.isWave );
}

BOOST_AUTO_TEST_SUITE_END()

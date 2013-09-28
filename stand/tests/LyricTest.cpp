#include <boost/test/unit_test.hpp>
#include "../vsq/Lyric.h"

using namespace std;
using namespace vconnect;

BOOST_AUTO_TEST_SUITE(LyricTest)

BOOST_AUTO_TEST_CASE(testConstruct)
{
    Lyric lyricA( "あ,a,0.5,0,0" );
    BOOST_CHECK_EQUAL( string( "あ" ), lyricA.getLyric() );

    Lyric lyricB( "\"わ\",\"w a\",1.0,64,0,0" );
    BOOST_CHECK_EQUAL( string( "わ" ), lyricB.getLyric() );
}

BOOST_AUTO_TEST_SUITE_END()

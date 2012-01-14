#ifndef TEST_LyricTest
#define TEST_LyricTest
#include "AllTests.h"
#include "../vsqMetaText/Lyric.h"

using namespace std;
using namespace vconnect;

class LyricTest : public CppUnit::TestFixture {
public:
    void testConstruct()
    {
        Lyric lyricA( "あ,a,0.5,0,0" );
        CPPUNIT_ASSERT_EQUAL( string( "あ" ), lyricA.getLyric() );

        Lyric lyricB( "\"わ\",\"w a\",1.0,64,0,0" );
        CPPUNIT_ASSERT_EQUAL( string( "わ" ), lyricB.getLyric() );
    }

    CPPUNIT_TEST_SUITE( LyricTest );
    CPPUNIT_TEST( testConstruct );
    CPPUNIT_TEST_SUITE_END();
};

REGISTER_TEST_SUITE( LyricTest );
#endif

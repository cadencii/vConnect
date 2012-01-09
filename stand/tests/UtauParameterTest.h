#ifndef TEST_UtauParameterTest
#define TEST_UtauParameterTest
#include "AllTests.h"
#include "../utauVoiceDB/UtauParameter.h"

using namespace vconnect;
using namespace std;

class UtauParameterTest : public CppUnit::TestFixture
{
public:
    void testConstructor()
    {
        UtauParameter p( "あ.wav=,0,135,172,14,12" );
        CPPUNIT_ASSERT_EQUAL( string( "あ" ), p.fileName );
        CPPUNIT_ASSERT_EQUAL( true, p.isWave );
        CPPUNIT_ASSERT_EQUAL( string( "あ" ), p.lyric );
        CPPUNIT_ASSERT_EQUAL( 0.0f, p.msLeftBlank );
        CPPUNIT_ASSERT_EQUAL( 135.0f, p.msFixedLength );
        CPPUNIT_ASSERT_EQUAL( 172.0f, p.msRightBlank );
        CPPUNIT_ASSERT_EQUAL( 14.0f, p.msPreUtterance );
        CPPUNIT_ASSERT_EQUAL( 12.0f, p.msVoiceOverlap );

        UtauParameter p1( "あ.wav=わ,0,135,172,14,12" );
        CPPUNIT_ASSERT_EQUAL( string( "わ" ), p1.lyric );

        UtauParameter p2( "あ.stf=,0,135,172,14,12" );
        CPPUNIT_ASSERT_EQUAL( false, p2.isWave );
    }

    CPPUNIT_TEST_SUITE( UtauParameterTest );
    CPPUNIT_TEST( testConstructor );
    CPPUNIT_TEST_SUITE_END();
};

REGISTER_TEST_SUITE( UtauParameterTest );
#endif

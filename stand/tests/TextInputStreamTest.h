#ifndef TEST_TextInputStreamTest
#define TEST_TextInputStreamTest
#include <cppunit/extensions/HelperMacros.h>
#include "../TextInputStream.h"

using namespace vconnect;

class TextInputStreamTest : public CppUnit::TestFixture
{
public:
    void testTextInputStreamShiftJIS()
    {
        TextInputStream reader( "fixture/TextInputStream/shift_jis_crlf.txt", "Shift_JIS" );

        CPPUNIT_ASSERT( false == reader.isEOF() );

        string actual;
        string expected;
        actual = reader.readLine();
        expected = "だ・い・じ・け・ん";
        CPPUNIT_ASSERT_EQUAL( expected, actual );

        CPPUNIT_ASSERT( false == reader.isEOF() );

        actual = reader.readLine();
        expected = "社会復帰できなくなっちゃうよ";
        CPPUNIT_ASSERT_EQUAL( expected, actual );

        CPPUNIT_ASSERT( true == reader.isEOF() );
    }

    void testTextInputStreamUTF8()
    {
        TextInputStream reader( "fixture/TextInputStream/utf8_lf.txt", "UTF-8" );

        CPPUNIT_ASSERT( false == reader.isEOF() );

        string actual;
        string expected;

        CPPUNIT_ASSERT( false == reader.isEOF() );
        actual = reader.readLine();
        expected = "吾輩は猫である。名前はまだ無い。";
        CPPUNIT_ASSERT_EQUAL( expected, actual );

        CPPUNIT_ASSERT( false == reader.isEOF() );
        actual = reader.readLine();
        expected = "どこで生れたかとんと見当がつかぬ。";
        CPPUNIT_ASSERT_EQUAL( expected, actual );

        CPPUNIT_ASSERT( true == reader.isEOF() );
    }

    CPPUNIT_TEST_SUITE( TextInputStreamTest );
    CPPUNIT_TEST( testTextInputStreamShiftJIS );
    CPPUNIT_TEST( testTextInputStreamUTF8 );
    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION( TextInputStreamTest );
#endif

#ifndef TEST_TextReaderTest
#define TEST_TextReaderTest
#include <cppunit/extensions/HelperMacros.h>
#include "../TextReader.h"

using namespace vconnect;

class TextReaderTest : public CppUnit::TestFixture
{
public:
    void testTextReaderShiftJIS()
    {
        TextReader reader( "fixture/TextReader/shift_jis_crlf.txt", "Shift_JIS" );

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

    void testTextReaderUTF8()
    {
        TextReader reader( "fixture/TextReader/utf8_lf.txt", "UTF-8" );

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

    CPPUNIT_TEST_SUITE( TextReaderTest );
    CPPUNIT_TEST( testTextReaderShiftJIS );
    CPPUNIT_TEST( testTextReaderUTF8 );
    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION( TextReaderTest );
#endif

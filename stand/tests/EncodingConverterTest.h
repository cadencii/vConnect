#ifndef TEST_EncodingConverterTest
#define TEST_EncodingConverterTest
#include "AllTests.h"
#include "../EncodingConverter.h"

using namespace vconnect;
using namespace std;

class EncodingConverterTest : public CppUnit::TestFixture, public vconnect::EncodingConverter
{
public:
    void testConvert()
    {
        EncodingConverter shiftJISConverter( "Shift_JIS", "UTF-8" );
        string actual = shiftJISConverter.convert( getFixture( "fixture/EncodingConverter/shift_jis.txt" ) );
        string expected = "だ・い・じ・け・ん";
        CPPUNIT_ASSERT_EQUAL( expected, actual );

        EncodingConverter utf16leConverter( "UTF-16LE", "UTF-8" );
        actual = utf16leConverter.convert( getFixture( "fixture/EncodingConverter/utf16le.txt" ) );
        expected = "社会復帰できなくなっちゃうよ";
        CPPUNIT_ASSERT_EQUAL( expected, actual );

        EncodingConverter utf32leConverter( "UTF-32LE", "UTF-8" );
        actual = utf32leConverter.convert( getFixture( "fixture/EncodingConverter/utf32le.txt" ) );
        expected = "尾骶骨";
        CPPUNIT_ASSERT_EQUAL( expected, actual );
    }

    void testGetCodeset()
    {
        string expected = "codeset";
        string actual = getCodeset( "language_territory.codeset@modifier" );
        CPPUNIT_ASSERT_EQUAL( expected, actual );

        actual = getCodeset( "language_territory.codeset" );
        CPPUNIT_ASSERT_EQUAL( expected, actual );

        expected = "";
        actual = getCodeset( "language_territory" );
        CPPUNIT_ASSERT_EQUAL( expected, actual );
    }

    /**
     * 指定されたパスに置かれたファイルを文字列に読み込む
     * @param path 読み込むファイルのパス
     * @return 読み込んだデータ
     */
    string getFixture( string path )
    {
        ifstream stream( path.c_str() );
        string result;
        stream >> result;
        stream.close();
        return result;
    }

    CPPUNIT_TEST_SUITE( EncodingConverterTest );
    CPPUNIT_TEST( testConvert );
    CPPUNIT_TEST( testGetCodeset );
    CPPUNIT_TEST_SUITE_END();
};

REGISTER_TEST_SUITE( EncodingConverterTest );
#endif

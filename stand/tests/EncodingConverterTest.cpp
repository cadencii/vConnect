#include <fstream>
#include <boost/test/unit_test.hpp>
#include "../EncodingConverter.h"

using namespace vconnect;
using namespace std;

BOOST_AUTO_TEST_SUITE(EncodingConverterTest)

/**
 * 指定されたパスに置かれたファイルを文字列に読み込む
 * @param path 読み込むファイルのパス
 * @return 読み込んだデータ
 */
static string getFixture( string path )
{
    ifstream stream( path.c_str() );
    string result;
    stream >> result;
    stream.close();
    return result;
}

struct ExtendedEncodingConverter : public EncodingConverter
{
    static string getCodeset(string locale)
    {
        return EncodingConverter::getCodeset(locale);
    }
};

BOOST_AUTO_TEST_CASE(testConvert)
{
    EncodingConverter shiftJISConverter( "Shift_JIS", "UTF-8" );
    string actual = shiftJISConverter.convert( getFixture( "fixture/EncodingConverter/shift_jis.txt" ) );
    string expected = "だ・い・じ・け・ん";
    BOOST_CHECK_EQUAL( expected, actual );

    EncodingConverter utf16leConverter( "UTF-16LE", "UTF-8" );
    actual = utf16leConverter.convert( getFixture( "fixture/EncodingConverter/utf16le.txt" ) );
    expected = "社会復帰できなくなっちゃうよ";
    BOOST_CHECK_EQUAL( expected, actual );

    EncodingConverter utf32leConverter( "UTF-32LE", "UTF-8" );
    actual = utf32leConverter.convert( getFixture( "fixture/EncodingConverter/utf32le.txt" ) );
    expected = "尾骶骨";
    BOOST_CHECK_EQUAL( expected, actual );
}

BOOST_AUTO_TEST_CASE(testGetCodeset)
{
    string expected = "codeset";
    string actual = ExtendedEncodingConverter::getCodeset( "language_territory.codeset@modifier" );
    BOOST_CHECK_EQUAL( expected, actual );

    actual = ExtendedEncodingConverter::getCodeset( "language_territory.codeset" );
    BOOST_CHECK_EQUAL( expected, actual );

    expected = "";
    actual = ExtendedEncodingConverter::getCodeset( "language_territory" );
    BOOST_CHECK_EQUAL( expected, actual );
}

BOOST_AUTO_TEST_SUITE_END()

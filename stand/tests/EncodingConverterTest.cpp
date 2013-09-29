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
    ifstream stream(path.c_str(), std::ios::binary);
    string result;
    while (!stream.eof()) {
        int c = stream.get();
        if (c == EOF) {
            break;
        }
        result.append(1, (char)(0xFF & c));
    }
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
    string const encoding = EncodingConverter::getInternalEncoding();
    {
        EncodingConverter shiftJISConverter( "Shift_JIS", encoding );
        string fixture = getFixture("fixture/EncodingConverter/shift_jis.txt");
        string actual = shiftJISConverter.convert(fixture);
        string expected = "だ・い・じ・け・ん";
        BOOST_CHECK_EQUAL( expected, actual );
    }

    {
        EncodingConverter utf16leConverter( "UTF-16LE", encoding );
        string fixture = getFixture( "fixture/EncodingConverter/utf16le.txt" );
        string actual = utf16leConverter.convert(fixture);
        string expected = "社会復帰できなくなっちゃうよ";
        BOOST_CHECK_EQUAL( expected, actual );
    }

    // Skip this test in windows, because "骶" can't be encoded in Shift_JIS.
    if (encoding != "CP932") {
        EncodingConverter utf32leConverter( "UTF-32LE", encoding );
        string fixture = getFixture("fixture/EncodingConverter/utf32le.txt");
        string actual = utf32leConverter.convert(fixture);
        string expected = "尾骶骨";
        BOOST_CHECK_EQUAL( expected, actual );
    }
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

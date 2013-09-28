#include <fstream>
#include <string>
#include <boost/test/unit_test.hpp>
#include "../TextOutputStream.h"

using namespace vconnect;
using namespace std;

BOOST_AUTO_TEST_SUITE(TextOutputStreamTest)

/**
 * 指定されたパスに置かれたファイルを文字列に読み込む
 * @param path 読み込むファイルのパス
 * @return 読み込んだデータ
 */
static string getContents( string path )
{
    ifstream stream( path.c_str() );
    string result;
    stream >> result;
    stream.close();
    return result;
}

BOOST_AUTO_TEST_CASE(testTextOutputStream)
{
    string outputPath = "textWriterTest.txt";
    char crlf[3] = { 0x0D, 0x0A };
    string newLine = crlf;
    TextOutputStream writer( outputPath, "Shift_JIS", newLine );
    writer.writeLine( "吾輩は猫である。名前はまだ無い。" );
    writer.writeLine( "どこで生れたかとんと見当がつかぬ。" );
    writer.close();

    string expected, actual;
    expected = getContents( "expected/TextOutputStream/shift_jis_crlf.txt" );
    actual = getContents( outputPath );
    BOOST_CHECK_EQUAL( expected, actual );
}

BOOST_AUTO_TEST_SUITE_END()

#ifndef TEST_TextOutputStreamTest
#define TEST_TextOutputStreamTest
#include "AllTests.h"
#include "../TextOutputStream.h"

using namespace vconnect;
using namespace std;

class TextOutputStreamTest : public CppUnit::TestFixture
{
public:
    void testTextOutputStream()
    {
        string outputPath = "textWriterTest.txt";
        char crlf[3] = { 0x0D, 0x0A };
        string newLine = crlf;
        TextOutputStream writer( outputPath, "Shift_JIS", newLine );
        writer.writeLine( "吾輩は猫である。名前はまだ無い。" );
        writer.writeLine( "どこで生れたかとんと見当がつかぬ。" );
        writer.close();

        string expected, actual;
        expected = this->getContents( "expected/TextOutputStream/shift_jis_crlf.txt" );
        actual = this->getContents( outputPath );
        CPPUNIT_ASSERT_EQUAL( expected, actual );
    }

    /**
     * 指定されたパスに置かれたファイルを文字列に読み込む
     * @param path 読み込むファイルのパス
     * @return 読み込んだデータ
     */
    string getContents( string path )
    {
        ifstream stream( path.c_str() );
        string result;
        stream >> result;
        stream.close();
        return result;
    }

    CPPUNIT_TEST_SUITE( TextOutputStreamTest );
    CPPUNIT_TEST( testTextOutputStream );
    CPPUNIT_TEST_SUITE_END();
};

REGISTER_TEST_SUITE( TextOutputStreamTest );
#endif

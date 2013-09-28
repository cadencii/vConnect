#include <boost/test/unit_test.hpp>
#include "../TextInputStream.h"

using namespace vconnect;

BOOST_AUTO_TEST_SUITE(TextInputStreamTest)

BOOST_AUTO_TEST_CASE(testTextInputStreamShiftJIS)
{
    TextInputStream reader( "fixture/TextInputStream/shift_jis_crlf.txt", "Shift_JIS" );

    BOOST_CHECK( true == reader.ready() );

    string actual;
    string expected;
    actual = reader.readLine();
    expected = "だ・い・じ・け・ん";
    BOOST_CHECK_EQUAL( expected, actual );

    BOOST_CHECK( true == reader.ready() );

    actual = reader.readLine();
    expected = "社会復帰できなくなっちゃうよ";
    BOOST_CHECK_EQUAL( expected, actual );

    BOOST_CHECK( false == reader.ready() );
}

BOOST_AUTO_TEST_CASE(testTextInputStreamUTF8)
{
    TextInputStream *textInputStream = new TextInputStream( "fixture/TextInputStream/utf8_lf.txt", "UTF-8" );
    InputStream *reader = (InputStream *)textInputStream;

    BOOST_CHECK( true == reader->ready() );

    string actual;
    string expected;

    BOOST_CHECK( true == reader->ready() );
    actual = reader->readLine();
    expected = "吾輩は猫である。名前はまだ無い。";
    BOOST_CHECK_EQUAL( expected, actual );

    BOOST_CHECK( true == reader->ready() );
    actual = reader->readLine();
    expected = "どこで生れたかとんと見当がつかぬ。";
    BOOST_CHECK_EQUAL( expected, actual );

    BOOST_CHECK( false == reader->ready() );

    delete textInputStream;
}
BOOST_AUTO_TEST_SUITE_END()

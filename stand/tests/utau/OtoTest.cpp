#include <boost/test/unit_test.hpp>
#include "../../Path.h"
#include "../../utau/Oto.h"
#include "../../utau/UtauParameter.h"

using namespace std;
using namespace vconnect;

BOOST_AUTO_TEST_SUITE(OtoTest)

BOOST_AUTO_TEST_CASE(test)
{
    string path = Path::getFullPath("fixture/utau/UtauDB/oto.ini");
    Oto oto(path, "Shift_JIS");
    {
        UtauParameter * parameter = oto.find("a あ");
        BOOST_CHECK(parameter);
        if (parameter) {
            BOOST_CHECK_EQUAL(string("_ああいあうえあ"), parameter->fileName);
        }
    }
    {
        UtauParameter * parameter = oto.find("_ああいあうえあ");
        BOOST_CHECK(parameter);
        if (parameter) {
            BOOST_CHECK_EQUAL(string("_ああいあうえあ"), parameter->fileName);
        }
    }
    {
        UtauParameter * parameter = oto.find("音源に存在しない歌詞を指定");
        BOOST_CHECK(!parameter);
    }
}

BOOST_AUTO_TEST_SUITE_END()

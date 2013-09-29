#include <boost/test/unit_test.hpp>
#include "../../Path.h"
#include "../../utau/PrefixMap.h"

using namespace std;
using namespace vconnect;

BOOST_AUTO_TEST_SUITE(PrefixMapTest)

static string fixture_path_ = "fixture/utau/UtauDB/prefix.map";
static string fixture_encoding_ = "Shift_JIS";

BOOST_AUTO_TEST_CASE(test)
{
    string path = Path::getFullPath(fixture_path_);
    PrefixMap map(path, fixture_encoding_);
    BOOST_CHECK_EQUAL(string("あ"), map.getMappedLyric("あ", 60));
    BOOST_CHECK_EQUAL(string("A\\い↑"), map.getMappedLyric("い", 61));
}

BOOST_AUTO_TEST_SUITE_END()

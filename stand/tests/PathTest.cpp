#include <boost/test/unit_test.hpp>
#include "../Path.h"

using namespace std;
using namespace vconnect;

BOOST_AUTO_TEST_SUITE(PathTest)

BOOST_AUTO_TEST_CASE(getFullPath)
{
#ifdef _MSC_VER
    string expected = "C:\\Windows";
    string actual = Path::getFullPath("C:\\Windows\\..\\Windows\\..\\Windows");
#else
    string expected = "/bin/sh";
    string actual = Path::getFullPath( "/bin/../bin/../bin/sh" );
#endif
    BOOST_CHECK_EQUAL( expected, actual );

    expected = "";
    actual = Path::getFullPath( "/bin/ajsfsajfkjfiuehafuakhfakejshfjkafnjk" );
    BOOST_CHECK_EQUAL( expected, actual );
};

BOOST_AUTO_TEST_CASE(combine)
{
    string separator = Path::getDirectorySeparator();
    string actual = Path::combine( "abc" + separator, separator + "def" );
    string expected = "abc" + separator + "def";
    BOOST_CHECK_EQUAL( expected, actual );
}

BOOST_AUTO_TEST_CASE(getDirectoryName)
{
    string separator = Path::getDirectorySeparator();
    string actual = Path::getDirectoryName( "aaa" + separator + "bbb" + separator );
    string expected = "aaa" + separator + "bbb";
    BOOST_CHECK_EQUAL( expected, actual );

    actual = Path::getDirectoryName( "aaa" + separator + "bbb" );
    expected = "aaa";
    BOOST_CHECK_EQUAL( expected, actual );
}

BOOST_AUTO_TEST_CASE(exists)
{
    BOOST_CHECK_EQUAL( false, Path::exists( "/bin/sladfjskajfsajfaeiuwfhajkrsfds" ) );
#ifdef _MSC_VER
    BOOST_CHECK_EQUAL(true, Path::exists("C:\\Windows"));
#else
    BOOST_CHECK_EQUAL( true, Path::exists( "/bin/sh" ) );
#endif
}

BOOST_AUTO_TEST_CASE(testNormalize)
{
#ifdef _MSC_VER
    string fixture = "abc/def\\ghi\\jkl/mno";
#else
    string fixture = "abc/def\\ghi¥jkl₩mno";
#endif
    string separator = Path::getDirectorySeparator();
    string expected = "abc" + separator + "def" + separator + "ghi" + separator + "jkl" + separator + "mno";
    string actual = Path::normalize( fixture );
    BOOST_CHECK_EQUAL( expected, actual );
}

BOOST_AUTO_TEST_SUITE_END()

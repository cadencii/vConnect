#ifndef TEST_PathTest
#define TEST_PathTest
#include "cppunit/extensions/HelperMacros.h"
#include "../Path.h"

using namespace std;
using namespace vconnect;

class PathTest : public CppUnit::TestFixture
{
public:
    void getFullPath()
    {
        string expected = "/bin/sh";
        string actual = Path::getFullPath( "/bin/../bin/../bin/sh" );
        CPPUNIT_ASSERT_EQUAL( expected, actual );

        expected = "";
        actual = Path::getFullPath( "/bin/ajsfsajfkjfiuehafuakhfakejshfjkafnjk" );
        CPPUNIT_ASSERT_EQUAL( expected, actual );
    };

    void combine()
    {
        string separator = Path::getDirectorySeparator();
        string actual = Path::combine( "abc" + separator, separator + "def" );
        string expected = "abc" + separator + "def";
        CPPUNIT_ASSERT_EQUAL( expected, actual );
    }

    void getDirectoryName()
    {
        string separator = Path::getDirectorySeparator();
        string actual = Path::getDirectoryName( "aaa" + separator + "bbb" + separator );
        string expected = "aaa" + separator + "bbb";
        CPPUNIT_ASSERT_EQUAL( expected, actual );

        actual = Path::getDirectoryName( "aaa" + separator + "bbb" );
        expected = "aaa";
        CPPUNIT_ASSERT_EQUAL( expected, actual );
    }

    void exists()
    {
        CPPUNIT_ASSERT_EQUAL( false, Path::exists( "/bin/sladfjskajfsajfaeiuwfhajkrsfds" ) );
        CPPUNIT_ASSERT_EQUAL( true, Path::exists( "/bin/sh" ) );
    }

    CPPUNIT_TEST_SUITE( PathTest );
    CPPUNIT_TEST( getFullPath );
    CPPUNIT_TEST( combine );
    CPPUNIT_TEST( getDirectoryName );
    CPPUNIT_TEST( exists );
    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION( PathTest );
#endif

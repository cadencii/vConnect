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
    };

    void combine()
    {
        string separator = Path::getDirectorySeparator();
        string actual = Path::combine( "abc" + separator, separator + "def" );
        string expected = "abc" + separator + "def";
        CPPUNIT_ASSERT_EQUAL( expected, actual );
    }

    CPPUNIT_TEST_SUITE( PathTest );
    CPPUNIT_TEST( getFullPath );
    CPPUNIT_TEST( combine );
    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION( PathTest );
#endif

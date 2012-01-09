#ifndef TEST_StringUtilTest
#define TEST_StringUtilTest
#include "AllTests.h"
#include "../StringUtil.h"

using namespace std;
using namespace vconnect;

class StringUtilTest : public CppUnit::TestFixture
{
public:
    void explode()
    {
        list<string> splitted = StringUtil::explode( ",", "aaa,bbb,ccc", 2 );
        CPPUNIT_ASSERT_EQUAL( (string::size_type)2, splitted.size() );
        list<string>::iterator i = splitted.begin();
        string expected = "aaa";
        string actual = *i;
        CPPUNIT_ASSERT_EQUAL( expected, actual );
        i++;
        expected = "bbb,ccc";
        actual = *i;
        CPPUNIT_ASSERT_EQUAL( expected, actual );
    };

    void explodeNonLimit()
    {
        list<string> splitted = StringUtil::explode( "_|_", "aaa_|__|_bb_|_" );
        CPPUNIT_ASSERT_EQUAL( (string::size_type)4, splitted.size() );
        list<string>::iterator i = splitted.begin();
        string expected;
        string actual;

        expected = "aaa";
        actual = *i;
        CPPUNIT_ASSERT_EQUAL( expected, actual );

        i++;
        expected = "";
        actual = *i;
        CPPUNIT_ASSERT_EQUAL( expected, actual );

        i++;
        expected = "bb";
        actual = *i;
        CPPUNIT_ASSERT_EQUAL( expected, actual );

        i++;
        expected = "";
        actual = *i;
        CPPUNIT_ASSERT_EQUAL( expected, actual );
    }

    void explodeDelimiterNotFound()
    {
        list<string> splitted = StringUtil::explode( "|", "aaa,bbb,ccc" );
        CPPUNIT_ASSERT_EQUAL( (string::size_type)1, splitted.size() );

        list<string>::iterator i = splitted.begin();
        string expected = "aaa,bbb,ccc";
        string actual = *i;
        CPPUNIT_ASSERT_EQUAL( expected, actual );
    }

    void testReplace()
    {
        string actual = StringUtil::replace( "abc", "a", "A" );
        string expected = "Abc";
        CPPUNIT_ASSERT_EQUAL( expected, actual );

        actual = StringUtil::replace( "abc", "a", "a" );
        expected = "abc";
        CPPUNIT_ASSERT_EQUAL( expected, actual );
    }

    CPPUNIT_TEST_SUITE( StringUtilTest );
    CPPUNIT_TEST( explode );
    CPPUNIT_TEST( explodeNonLimit );
    CPPUNIT_TEST( explodeDelimiterNotFound );
    CPPUNIT_TEST( testReplace );
    CPPUNIT_TEST_SUITE_END();
};

REGISTER_TEST_SUITE( StringUtilTest );
#endif

#include <boost/test/unit_test.hpp>
#include "../StringUtil.h"

using namespace std;
using namespace vconnect;

BOOST_AUTO_TEST_SUITE(StringUtilTest)

BOOST_AUTO_TEST_CASE(explode)
{
    vector<string> splitted = StringUtil::explode( ",", "aaa,bbb,ccc", 2 );
    BOOST_CHECK_EQUAL( (vector<string>::size_type)2, splitted.size() );
    BOOST_CHECK_EQUAL( string( "aaa" ), splitted[0] );
    BOOST_CHECK_EQUAL( string( "bbb,ccc" ), splitted[1] );
};

BOOST_AUTO_TEST_CASE(explodeNonLimit)
{
    vector<string> splitted = StringUtil::explode( "_|_", "aaa_|__|_bb_|_" );
    BOOST_CHECK_EQUAL( (vector<string>::size_type)4, splitted.size() );
    BOOST_CHECK_EQUAL( string( "aaa" ), splitted[0] );
    BOOST_CHECK_EQUAL( string( "" ), splitted[1] );
    BOOST_CHECK_EQUAL( string( "bb" ), splitted[2] );
    BOOST_CHECK_EQUAL( string( "" ), splitted[3] );
}

BOOST_AUTO_TEST_CASE(explodeDelimiterNotFound)
{
    vector<string> splitted = StringUtil::explode( "|", "aaa,bbb,ccc" );
    BOOST_CHECK_EQUAL( (string::size_type)1, splitted.size() );
    BOOST_CHECK_EQUAL( string( "aaa,bbb,ccc" ), splitted[0] );
}

BOOST_AUTO_TEST_CASE(testReplace)
{
    string actual = StringUtil::replace( "abc", "a", "A" );
    string expected = "Abc";
    BOOST_CHECK_EQUAL( expected, actual );

    actual = StringUtil::replace( "abc", "a", "a" );
    expected = "abc";
    BOOST_CHECK_EQUAL( expected, actual );
}

BOOST_AUTO_TEST_SUITE_END()

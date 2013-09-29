#include <boost/test/unit_test.hpp>
//#include "../AllTests.h"
#include "../../utau/UtauDB.h"
#include "../../Path.h"

using namespace std;
using namespace vconnect;

BOOST_AUTO_TEST_SUITE(UtauDBTest)

BOOST_AUTO_TEST_CASE(testConstruct)
{
    string path = Path::getFullPath( "fixture/utau/UtauDB/oto.ini" );
    UtauDB db( path, "Shift_JIS" );
    string expectedPath = Path::getDirectoryName( path ) + Path::getDirectorySeparator();
    BOOST_CHECK_EQUAL( expectedPath, db.getOtoIniPath() );

    UtauParameter resultByLyric;
    int ret = db.getParams(resultByLyric, "a あ", 60);
    BOOST_CHECK_EQUAL( 1, ret );
    BOOST_CHECK_EQUAL( string( "_ああいあうえあ" ), resultByLyric.fileName );

    UtauParameter resultByFileName;
    ret = db.getParams(resultByFileName, "_ああいあうえあ", 60);
    BOOST_CHECK_EQUAL( 1, ret );
    BOOST_CHECK_EQUAL( string( "_ああいあうえあ" ), resultByFileName.fileName );

    ret = db.getParams(resultByFileName, "音源に存在しない歌詞を指定", 60);
    BOOST_CHECK_EQUAL( 0, ret );

    {
        // search via prefix.map
        UtauParameter parameter;
        int const result = db.getParams(parameter, "あ", 62);
        BOOST_CHECK_EQUAL(1, result);
        BOOST_CHECK_EQUAL(string("あ↑"), parameter.fileName);
    }

    {
        // search phoneme in subdirectory.
        UtauParameter parameter;
        int const result = db.getParams(parameter, "わ", 60);
        BOOST_CHECK_EQUAL(1, result);
        BOOST_CHECK_EQUAL(string("A\\わ"), parameter.fileName);
    }

    // インデックスによるアクセス
    // インデックスによるアクセスでは、エイリアスで登録したものについてはアクセスされない
    BOOST_CHECK_EQUAL( 3, db.size() );
    UtauParameter result;
    ret = db.getParams( result, 0 );
    BOOST_CHECK_EQUAL( 1, ret );
    BOOST_CHECK_EQUAL( string( "_ああいあうえあ" ), result.fileName );
    ret = db.getParams( result, 1 );
    BOOST_CHECK_EQUAL( string( "あ" ), result.fileName );

    ret = db.getParams( result, -1 );
    BOOST_CHECK_EQUAL( 0, ret );
}

BOOST_AUTO_TEST_SUITE_END()

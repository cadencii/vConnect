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
    int ret = db.getParams( resultByLyric, "a あ" );
    BOOST_CHECK_EQUAL( 1, ret );
    BOOST_CHECK_EQUAL( string( "_ああいあうえあ" ), resultByLyric.fileName );

    UtauParameter resultByFileName;
    ret = db.getParams( resultByFileName, "_ああいあうえあ" );
    BOOST_CHECK_EQUAL( 1, ret );
    BOOST_CHECK_EQUAL( string( "_ああいあうえあ" ), resultByFileName.fileName );

    ret = db.getParams( resultByFileName, "音源に存在しない歌詞を指定" );
    BOOST_CHECK_EQUAL( 0, ret );

    // イテレータによるアクセス
    // イテレータによるアクセスでは、エイリアスで登録したものについてもアクセスされる
    Map<string, UtauParameter *>::iterator i;
    int count = 0;
    for( i = db.begin(); i != db.end(); i++ ){
        count++;
    }
    BOOST_CHECK_EQUAL( 3, count );

    Map<string, UtauParameter *>::iterator j = db.begin();
    BOOST_CHECK_EQUAL( string( "_ああいあうえあ" ), j->first );
    BOOST_CHECK_EQUAL( string( "_ああいあうえあ" ), j->second->fileName );
    j++;
    BOOST_CHECK_EQUAL( string( "a あ" ), j->first );
    BOOST_CHECK_EQUAL( string( "_ああいあうえあ" ), j->second->fileName );
    j++;
    BOOST_CHECK_EQUAL( string( "あ" ), j->first );
    BOOST_CHECK_EQUAL( string( "あ" ), j->second->fileName );
    j++;
    BOOST_CHECK( j == db.end() );

    // インデックスによるアクセス
    // インデックスによるアクセスでは、エイリアスで登録したものについてはアクセスされない
    BOOST_CHECK_EQUAL( 2, db.size() );
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

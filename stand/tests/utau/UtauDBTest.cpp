#ifndef TEST_UtauDBTest
#define TEST_UtauDBTest
#include "../AllTests.h"
#include "../../utau/UtauDB.h"
#include "../../Path.h"

using namespace std;
using namespace vconnect;

class UtauDBTest : public CppUnit::TestFixture
{
public:
    void testConstruct()
    {
        string path = Path::getFullPath( "fixture/utau/UtauDB/oto.ini" );
        UtauDB db( path, "Shift_JIS" );
        string expectedPath = Path::getDirectoryName( path ) + Path::getDirectorySeparator();
        CPPUNIT_ASSERT_EQUAL( expectedPath, db.getOtoIniPath() );

        UtauParameter resultByLyric;
        int ret = db.getParams( resultByLyric, "a あ" );
        CPPUNIT_ASSERT_EQUAL( 1, ret );
        CPPUNIT_ASSERT_EQUAL( string( "_ああいあうえあ" ), resultByLyric.fileName );

        UtauParameter resultByFileName;
        ret = db.getParams( resultByFileName, "_ああいあうえあ" );
        CPPUNIT_ASSERT_EQUAL( 1, ret );
        CPPUNIT_ASSERT_EQUAL( string( "_ああいあうえあ" ), resultByFileName.fileName );

        ret = db.getParams( resultByFileName, "音源に存在しない歌詞を指定" );
        CPPUNIT_ASSERT_EQUAL( 0, ret );

        // イテレータによるアクセス
        // イテレータによるアクセスでは、エイリアスで登録したものについてもアクセスされる
        Map<string, UtauParameter *>::iterator i;
        int count = 0;
        for( i = db.begin(); i != db.end(); i++ ){
            count++;
        }
        CPPUNIT_ASSERT_EQUAL( 3, count );

        Map<string, UtauParameter *>::iterator j = db.begin();
        CPPUNIT_ASSERT_EQUAL( string( "_ああいあうえあ" ), j->first );
        CPPUNIT_ASSERT_EQUAL( string( "_ああいあうえあ" ), j->second->fileName );
        j++;
        CPPUNIT_ASSERT_EQUAL( string( "a あ" ), j->first );
        CPPUNIT_ASSERT_EQUAL( string( "_ああいあうえあ" ), j->second->fileName );
        j++;
        CPPUNIT_ASSERT_EQUAL( string( "あ" ), j->first );
        CPPUNIT_ASSERT_EQUAL( string( "あ" ), j->second->fileName );
        j++;
        CPPUNIT_ASSERT( j == db.end() );

        // インデックスによるアクセス
        // インデックスによるアクセスでは、エイリアスで登録したものについてはアクセスされない
        CPPUNIT_ASSERT_EQUAL( 2, db.size() );
        UtauParameter result;
        ret = db.getParams( result, 0 );
        CPPUNIT_ASSERT_EQUAL( 1, ret );
        CPPUNIT_ASSERT_EQUAL( string( "_ああいあうえあ" ), result.fileName );
        ret = db.getParams( result, 1 );
        CPPUNIT_ASSERT_EQUAL( string( "あ" ), result.fileName );

        ret = db.getParams( result, -1 );
        CPPUNIT_ASSERT_EQUAL( 0, ret );
    }

    CPPUNIT_TEST_SUITE( UtauDBTest );
    CPPUNIT_TEST( testConstruct );
    CPPUNIT_TEST_SUITE_END();
};
REGISTER_TEST_SUITE( UtauDBTest );
#endif

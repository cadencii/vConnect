#include <boost/test/unit_test.hpp>
#include "../../Path.h"
#include "../../utau/Oto.h"
#include "../../utau/UtauParameter.h"

using namespace std;
using namespace vconnect;

BOOST_AUTO_TEST_SUITE(OtoTest)

static string fixture_path_ = "fixture/utau/UtauDB/oto.ini";
static string fixture_encoding_ = "Shift_JIS";

BOOST_AUTO_TEST_CASE(find)
{
    string path = Path::getFullPath(fixture_path_);
    Oto oto(path, fixture_encoding_);
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

BOOST_AUTO_TEST_CASE(indexAccess)
{
    string path = Path::getFullPath(fixture_path_);
    Oto oto(path, fixture_encoding_);
    BOOST_CHECK_EQUAL((size_t)3, oto.count());
    {
        UtauParameter * actual = oto[0];
        BOOST_CHECK(actual != nullptr);
        BOOST_CHECK_EQUAL(string("a あ"), actual->lyric);
        BOOST_CHECK_EQUAL(string("_ああいあうえあ"), actual->fileName);
        BOOST_CHECK_CLOSE(500.0f, actual->msLeftBlank, FLT_EPSILON);
        BOOST_CHECK_CLOSE(400.0f, actual->msFixedLength, FLT_EPSILON);
        BOOST_CHECK_CLOSE(-500.0f, actual->msRightBlank, FLT_EPSILON);
        BOOST_CHECK_CLOSE(250.0f, actual->msPreUtterance, FLT_EPSILON);
        BOOST_CHECK_CLOSE(83.333f, actual->msVoiceOverlap, FLT_EPSILON);
        BOOST_CHECK(actual->isWave);
    }
    {
        UtauParameter * actual = oto[1];
        BOOST_CHECK(actual != nullptr);
        BOOST_CHECK_EQUAL(string("あ"), actual->lyric);
        BOOST_CHECK_EQUAL(string("あ"), actual->fileName);
        BOOST_CHECK_CLOSE(500.0f, actual->msLeftBlank, FLT_EPSILON);
        BOOST_CHECK_CLOSE(400.0f, actual->msFixedLength, FLT_EPSILON);
        BOOST_CHECK_CLOSE(-500.0f, actual->msRightBlank, FLT_EPSILON);
        BOOST_CHECK_CLOSE(250.0f, actual->msPreUtterance, FLT_EPSILON);
        BOOST_CHECK_CLOSE(83.333f, actual->msVoiceOverlap, FLT_EPSILON);
        BOOST_CHECK(actual->isWave);
    }
    {
        UtauParameter * actual = oto[2];
        BOOST_CHECK(actual != nullptr);
        BOOST_CHECK_EQUAL(string("あ↑"), actual->lyric);
        BOOST_CHECK_EQUAL(string("あ↑"), actual->fileName);
        BOOST_CHECK_CLOSE(500.0f, actual->msLeftBlank, FLT_EPSILON);
        BOOST_CHECK_CLOSE(400.0f, actual->msFixedLength, FLT_EPSILON);
        BOOST_CHECK_CLOSE(-500.0f, actual->msRightBlank, FLT_EPSILON);
        BOOST_CHECK_CLOSE(250.0f, actual->msPreUtterance, FLT_EPSILON);
        BOOST_CHECK_CLOSE(83.333f, actual->msVoiceOverlap, FLT_EPSILON);
        BOOST_CHECK(actual->isWave);
    }
    {
        UtauParameter * actual = oto[3];
        BOOST_CHECK(actual == nullptr);
    }
}

BOOST_AUTO_TEST_SUITE_END()

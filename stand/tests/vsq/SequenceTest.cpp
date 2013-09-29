#include <boost/test/unit_test.hpp>
#include "../../vsq/Sequence.h"

using namespace std;
using namespace vconnect;

BOOST_AUTO_TEST_SUITE(SequenceTest)

BOOST_AUTO_TEST_CASE(testConstruct)
{
    Sequence sequence;
    // PITCH_BEND
    BP point = sequence.controlCurves[0].get( 0 );
    BOOST_CHECK_EQUAL( (long int)-10000, point.tick );
    BOOST_CHECK_EQUAL( 0, point.value );
    BOOST_CHECK(
        sequence.mMapCurves[CurveTypeEnum::getName( CurveTypeEnum::PITCH_BEND )]
        == &sequence.controlCurves[0]
    );

    // PITCH_BEND_SENS
    point = sequence.controlCurves[1].get( 0 );
    BOOST_CHECK_EQUAL( (long int)-10000, point.tick );
    BOOST_CHECK_EQUAL( 2, point.value );
    BOOST_CHECK(
        sequence.mMapCurves[CurveTypeEnum::getName( CurveTypeEnum::PITCH_BEND_SENS )]
        == &sequence.controlCurves[1]
    );

    // DYNAMICS
    point = sequence.controlCurves[2].get( 0 );
    BOOST_CHECK_EQUAL( (long int)-10000, point.tick );
    BOOST_CHECK_EQUAL( 64, point.value );
    BOOST_CHECK(
        sequence.mMapCurves[CurveTypeEnum::getName( CurveTypeEnum::DYNAMICS )]
        == &sequence.controlCurves[2]
    );

    // BRETHINESS
    point = sequence.controlCurves[3].get( 0 );
    BOOST_CHECK_EQUAL( (long int)-10000, point.tick );
    BOOST_CHECK_EQUAL( 0, point.value );
    BOOST_CHECK(
        sequence.mMapCurves[CurveTypeEnum::getName( CurveTypeEnum::BRETHINESS )]
        == &sequence.controlCurves[3]
    );

    // BRIGHTNESS
    point = sequence.controlCurves[4].get( 0 );
    BOOST_CHECK_EQUAL( (long int)-10000, point.tick );
    BOOST_CHECK_EQUAL( 64, point.value );
    BOOST_CHECK(
        sequence.mMapCurves[CurveTypeEnum::getName( CurveTypeEnum::BRIGHTNESS )]
        == &sequence.controlCurves[4]
    );

    // CLEARNESS
    point = sequence.controlCurves[5].get( 0 );
    BOOST_CHECK_EQUAL( (long int)-10000, point.tick );
    BOOST_CHECK_EQUAL( 0, point.value );
    BOOST_CHECK(
        sequence.mMapCurves[CurveTypeEnum::getName( CurveTypeEnum::CLEARNESS )]
        == &sequence.controlCurves[5]
    );

    // GENDER
    point = sequence.controlCurves[6].get( 0 );
    BOOST_CHECK_EQUAL( (long int)-10000, point.tick );
    BOOST_CHECK_EQUAL( 64, point.value );
    BOOST_CHECK(
        sequence.mMapCurves[CurveTypeEnum::getName( CurveTypeEnum::GENDER )]
        == &sequence.controlCurves[6]
    );
}

BOOST_AUTO_TEST_SUITE_END()

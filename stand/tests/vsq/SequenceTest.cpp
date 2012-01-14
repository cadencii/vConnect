#ifndef TEST_SequenceTest
#define TEST_SequenceTest
#include "../AllTests.h"
#include "../../vsqMetaText/Sequence.h"

using namespace std;
using namespace vconnect;

class SequenceTest : public CppUnit::TestFixture
{
public:
    void testConstruct()
    {
        Sequence sequence;
        // PITCH_BEND
        BP point = sequence.controlCurves[0].get( 0 );
        CPPUNIT_ASSERT_EQUAL( (long int)-10000, point.tick );
        CPPUNIT_ASSERT_EQUAL( 0, point.value );
        CPPUNIT_ASSERT(
            sequence.mMapCurves[CurveTypeEnum::getName( CurveTypeEnum::PITCH_BEND )]
            == &sequence.controlCurves[0]
        );

        // PITCH_BEND_SENS
        point = sequence.controlCurves[1].get( 0 );
        CPPUNIT_ASSERT_EQUAL( (long int)-10000, point.tick );
        CPPUNIT_ASSERT_EQUAL( 2, point.value );
        CPPUNIT_ASSERT(
            sequence.mMapCurves[CurveTypeEnum::getName( CurveTypeEnum::PITCH_BEND_SENS )]
            == &sequence.controlCurves[1]
        );

        // DYNAMICS
        point = sequence.controlCurves[2].get( 0 );
        CPPUNIT_ASSERT_EQUAL( (long int)-10000, point.tick );
        CPPUNIT_ASSERT_EQUAL( 64, point.value );
        CPPUNIT_ASSERT(
            sequence.mMapCurves[CurveTypeEnum::getName( CurveTypeEnum::DYNAMICS )]
            == &sequence.controlCurves[2]
        );

        // BRETHINESS
        point = sequence.controlCurves[3].get( 0 );
        CPPUNIT_ASSERT_EQUAL( (long int)-10000, point.tick );
        CPPUNIT_ASSERT_EQUAL( 0, point.value );
        CPPUNIT_ASSERT(
            sequence.mMapCurves[CurveTypeEnum::getName( CurveTypeEnum::BRETHINESS )]
            == &sequence.controlCurves[3]
        );

        // BRIGHTNESS
        point = sequence.controlCurves[4].get( 0 );
        CPPUNIT_ASSERT_EQUAL( (long int)-10000, point.tick );
        CPPUNIT_ASSERT_EQUAL( 64, point.value );
        CPPUNIT_ASSERT(
            sequence.mMapCurves[CurveTypeEnum::getName( CurveTypeEnum::BRIGHTNESS )]
            == &sequence.controlCurves[4]
        );

        // CLEARNESS
        point = sequence.controlCurves[5].get( 0 );
        CPPUNIT_ASSERT_EQUAL( (long int)-10000, point.tick );
        CPPUNIT_ASSERT_EQUAL( 0, point.value );
        CPPUNIT_ASSERT(
            sequence.mMapCurves[CurveTypeEnum::getName( CurveTypeEnum::CLEARNESS )]
            == &sequence.controlCurves[5]
        );

        // GENDER
        point = sequence.controlCurves[6].get( 0 );
        CPPUNIT_ASSERT_EQUAL( (long int)-10000, point.tick );
        CPPUNIT_ASSERT_EQUAL( 64, point.value );
        CPPUNIT_ASSERT(
            sequence.mMapCurves[CurveTypeEnum::getName( CurveTypeEnum::GENDER )]
            == &sequence.controlCurves[6]
        );
    }

    CPPUNIT_TEST_SUITE( SequenceTest );
    CPPUNIT_TEST( testConstruct );
    CPPUNIT_TEST_SUITE_END();
};
REGISTER_TEST_SUITE( SequenceTest );
#endif

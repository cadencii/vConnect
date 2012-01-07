#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "EncodingConverterTest.h"

int main( int argc, char* argv[] )
{
    CppUnit::TestResult controller;
    CppUnit::TestResultCollector results;
    controller.addListener( &results );

    CppUnit::BriefTestProgressListener progress;
    controller.addListener( &progress );

    CppUnit::TestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );
    runner.run( controller );

    return results.wasSuccessful() ? 0 : 1;
}

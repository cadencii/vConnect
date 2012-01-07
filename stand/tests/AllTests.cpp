#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "EncodingConverterTest.h"
#include "TextReaderTest.h"
#include "TextWriterTest.h"
#include "StringUtilTest.h"

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

    CppUnit::CompilerOutputter outputter( &results, CppUnit::stdCOut() );
    outputter.write();

    return results.wasSuccessful() ? 0 : 1;
}

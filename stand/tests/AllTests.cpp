#include "AllTests.h"

#include "EncodingConverterTest.h"
#include "TextInputStreamTest.h"
#include "TextOutputStreamTest.h"
#include "StringUtilTest.h"
#include "PathTest.h"
#include "UtauParameterTest.h"

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

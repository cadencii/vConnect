#define BOOST_TEST_NO_LIB
#include <boost/test/unit_test.hpp>

boost::unit_test_framework::test_suite* init_unit_test_suite(int argc, char* argv[])
{
	return BOOST_TEST_SUITE("replace output stream");
}

#define BOOST_TEST_MODULE boost_unit_test
#include <boost/test/included/unit_test.hpp>

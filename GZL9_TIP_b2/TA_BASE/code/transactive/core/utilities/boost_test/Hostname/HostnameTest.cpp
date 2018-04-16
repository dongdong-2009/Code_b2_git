// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;
using namespace boost::asio;

BOOST_AUTO_TEST_SUITE(HostnameTest)

BOOST_AUTO_TEST_CASE(test_hostname)
{
    std::cout << boost::asio::ip::host_name() << std::endl;
}

BOOST_AUTO_TEST_CASE(test_test)
{

}

BOOST_AUTO_TEST_SUITE_END()

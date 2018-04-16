// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include "CorbaUtil.h"
#include "core/naming/IDL/src/BindingRequestOriginatorCorbaDef.h"

using namespace TA_Base_Core;

BOOST_AUTO_TEST_SUITE(CorbaUtilTest)

BOOST_AUTO_TEST_CASE(initialize)
{
    CorbaUtil::getInstance().initialise();
}

BOOST_AUTO_TEST_SUITE_END()

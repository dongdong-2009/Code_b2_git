// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include "SingletonTimerUtil.h"
#include "TimerUtilTestFixture.h"
#include "core/utilities/src/DebugUtil.h"
#include "ITimeoutCallback.h"

BOOST_FIXTURE_TEST_SUITE(SingletonTimerUtilTest, TimerUtilTestFixture)

using namespace TA_Base_Core;

struct TimerUser : ITimeoutCallback
{
    virtual void timerExpired(long timerId, void* userData)
    {
        std::cout << "hello, world!" << std::endl;
    }
};

BOOST_AUTO_TEST_CASE(single_hot)
{
    TimerUser user;
    SingletonTimerUtil::getInstance().startPeriodicTimeOutClock(&user, 100);
    boost::this_thread::sleep_for(boost::chrono::milliseconds(200));

    SingletonTimerUtil::getInstance().startPeriodicTimeOutClock(&user, 100);
    boost::this_thread::sleep_for(boost::chrono::milliseconds(200));
}

BOOST_AUTO_TEST_SUITE_END()

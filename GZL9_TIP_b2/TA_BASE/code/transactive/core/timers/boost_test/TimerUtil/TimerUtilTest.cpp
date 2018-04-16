// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include "TimerUtil.h"
#include "ITimeoutCallback.h"
#include "TimerUtilTestFixture.h"
#include "core/utilities/src/DebugUtil.h"

BOOST_FIXTURE_TEST_SUITE(TimerUtilTest, TimerUtilTestFixture)

using namespace TA_Base_Core;

struct TimerUser : ITimeoutCallback
{
    TimerUser(size_t timers = 1, long interval = 100, bool singleShot = false, const char* userData = "hello, world\n", size_t workTime = 10, bool printThreadId = true)
        : m_times(timers),
          m_interval(interval),
          m_singleShot(singleShot),
          m_workTime(workTime),
          m_printThreadId(printThreadId)
    {
        for (size_t i = 0; i < m_times; ++i)
        {
            m_timer.scheduleTimer(this, m_interval + i, m_singleShot, (void*)userData);
        }
    }

    virtual void timerExpired(long timerId, void* userData)
    {
        if (m_printThreadId)
        {
            std::cout << boost::format("[Thrd: %4d] ") %  boost::this_thread::get_id();
        }

        std::cout << (const char*)userData;
        boost::this_thread::sleep_for(boost::chrono::milliseconds(m_workTime));
    }

    void cancelTimer()
    {
        if (!m_singleShot)
        {
            m_timer.cancelTimer(this);
        }
    }

    ~TimerUser()
    {
        cancelTimer();
    }

    TimerUtil m_timer;
    size_t m_interval;
    size_t m_times;
    bool m_singleShot;
    size_t m_workTime;
    bool m_printThreadId;
};

BOOST_AUTO_TEST_CASE(single_hot)
{
    TimerUser user(1, 100, true);
    boost::this_thread::sleep_for(boost::chrono::milliseconds(200));
}

BOOST_AUTO_TEST_CASE(single_hot_many_times)
{
    size_t COUNT = 5;

    for (size_t i = 0; i < COUNT; ++i)
    {
        TimerUser* user = new TimerUser(1, 100, true);
    }

    boost::this_thread::sleep_for(boost::chrono::milliseconds(110 * COUNT));
}

BOOST_AUTO_TEST_CASE(many_timers)
{
    TimerUser user(10, 100, false);
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
}

BOOST_AUTO_TEST_CASE(invalidate)
{
    TimerUser user(10, 100, false);
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
    user.m_timer.invalidate();
}

BOOST_AUTO_TEST_CASE(stress_test)
{    
    for (size_t i = 0; i < 30; ++i)
    {
        std::cout << "\rTimer Stree Test, Round " << i << std::endl;
        TimerUser user(100, 100, false, ".", 10, false);
        boost::this_thread::sleep_for(boost::chrono::milliseconds(2000));
    }
}

BOOST_AUTO_TEST_SUITE_END()

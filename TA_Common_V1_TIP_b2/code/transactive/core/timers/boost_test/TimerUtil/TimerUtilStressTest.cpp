// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include "TimerUtil.h"
#include "ITimeoutCallback.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;

BOOST_AUTO_TEST_SUITE(TimerUtilStressTest)

BOOST_AUTO_TEST_CASE(test_scheduleTimer)
{
    struct TimerUser : ITimeoutCallback
    {
        TimerUser(size_t interval = 500, size_t schedulNumber = 100)
            : m_interval(interval),
              m_schedulNumber(schedulNumber)
        {
        }

        virtual void timerExpired(long timerId, void* userData)
        {
            std::cout << ".";
            boost::this_thread::sleep_for(boost::chrono::milliseconds(randomWorkTime()));
        }

        void operator()()
        {
            static size_t round = 0;

            for (;;)
            {
                std::cout << "\rTimerUtil Test, Round: " << ++round << std::endl;
                schedule();
                boost::this_thread::sleep_for(boost::chrono::seconds(3));
                cancel();
                boost::this_thread::sleep_for(boost::chrono::seconds(1));
            }
        }

        void schedule()
        {
            for (size_t i = 0; i < m_schedulNumber; ++i)
            {
                m_timer.scheduleTimer(this, m_interval + i, false);
            }
        }

        void cancel()
        {
            m_timer.cancelTimer(this);
        }

        size_t randomWorkTime()
        {
            static boost::random::mt19937 gen;
            static boost::random::uniform_int_distribution<> distribution(0,3);
            return distribution(gen);
        }

        TimerUtil m_timer;
        size_t m_interval;
        size_t m_schedulNumber;
    };

    ACE::init();

    DebugUtil::getInstance().setFile("TimerTest.log");
    DebugUtil::getInstance().setMaxSize(30 * 1024 * 1024);
    DebugUtil::getInstance().setLevel(DebugUtil::DebugDebug);

    TimerUser user(500, 100);
    boost::thread t(boost::ref(user));
    t.join();
}

BOOST_AUTO_TEST_SUITE_END()

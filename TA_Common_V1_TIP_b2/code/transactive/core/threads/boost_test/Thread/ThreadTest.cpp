// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include "Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
using namespace TA_Base_Core;

struct MyThread : Thread
{
    MyThread()
        : m_running(false)
    {
    }

    void start()
    {
        m_running = true;
        Thread::start();
    }

    virtual void run()
    {
        while (m_running)
        {
            try
            {
                std::cout << ".";
                ThreadGuard g(m_lock);
                sleep(100);
                throw "hello, wrold";
            }
            catch (...)
            {
            }
        }
    }

    virtual void terminate()
    {
        m_running = false;
    }

    void test()
    {
        terminateAndWait();
        start();
    }

    volatile bool m_running;
    ReEntrantThreadLockable  m_lock;
};

BOOST_AUTO_TEST_SUITE(ThreadTest)

BOOST_AUTO_TEST_CASE(test_handle_leak)
{
    MyThread t;
    bool running = true;

    while (running)
    {
        t.test();      
        Thread::sleep(100);
    }
}

BOOST_AUTO_TEST_SUITE_END()

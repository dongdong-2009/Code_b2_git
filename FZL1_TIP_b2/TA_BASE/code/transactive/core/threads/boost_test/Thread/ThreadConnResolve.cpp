// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include "Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/utilities/src/DebugUtil.h"
using namespace TA_Base_Core;

class ThreadConnResolve : public TA_Base_Core::Thread
{
public:

    ThreadConnResolve()
        : m_bIsTerminate(false)
    {
    }

    ~ThreadConnResolve(void)
    {
        terminateAndWait();
    }

    virtual void run(void)
    {
        while (!m_bIsTerminate)
        {
            try
            {
                throw "hello, world!";
            }
            catch (...)
            {
                std::cout << ".";
            }

            m_bIsTerminate = true;
        }
    }

    virtual void terminate(void)
    {
        m_bIsTerminate = true;
    }

    void startResolve()
    {
        terminateAndWait();
        TA_Base_Core::ThreadGuard guard(m_threadLock);
        m_bIsTerminate = false;
        start();
    }

private:

    volatile bool m_bIsTerminate;
    TA_Base_Core::ReEntrantThreadLockable m_threadLock;
};

BOOST_AUTO_TEST_SUITE(ThreadConnResolveTest)

BOOST_AUTO_TEST_CASE(test_ThreadConnResolve)
{
    ThreadConnResolve* a = new ThreadConnResolve;

    while (true)
    {
        a->startResolve();
        TA_Base_Core::Thread::sleep(100);
    }
}

BOOST_AUTO_TEST_SUITE_END()

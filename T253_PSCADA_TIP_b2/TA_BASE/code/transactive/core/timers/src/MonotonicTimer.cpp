/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/timers/src/MonotonicTimer.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/08/26 16:15:14 $
  * Last modified by:  $Author: limin.zhu $
  *
  * Class for peforming time-based operations with a
  * millisecond scale.
  */

#include "core/timers/src/MonotonicTimer.h"

namespace TA_Base_Core
{
    typedef boost::chrono::steady_clock steady_clock;
    typedef boost::chrono::time_point<steady_clock> time_point;
    typedef time_point::duration nanoseconds;

    MonotonicTimer::MonotonicTimer(bool start_)
        : m_stoped(true)
    {
        if (start_)
        {
            start();
        }
    }

    void MonotonicTimer::start()
    {
        m_stoped = false;
        m_elapsed = nanoseconds::zero();
        m_start = steady_clock::now();
    }

    void MonotonicTimer::stop()
    {
        if (!m_stoped)
        {
            m_stoped = true;
            m_elapsed += caculate();
        }
    }

    bool MonotonicTimer::stoped()
    {
        return m_stoped;
    }

    void MonotonicTimer::resume()
    {
        if (m_stoped)
        {
            m_stoped = false;
            m_start = steady_clock::now();
        }
    }

    boost::uint64_t MonotonicTimer::elapsed()
    {
        return elapsedImpl().count() / 1000000;
    }

    boost::uint64_t MonotonicTimer::elapsed_ns()
    {
        return elapsedImpl().count();
    }

    boost::uint64_t MonotonicTimer::elapsed_ms()
    {
        return elapsedImpl().count() / 1000000;
    }

    unsigned long MonotonicTimer::elapsed_s()
    {
        return static_cast<unsigned long>(elapsedImpl().count() / 1000000000);
    }

    nanoseconds MonotonicTimer::elapsedImpl()
    {
        if (m_stoped)
        {
            return m_elapsed;
        }
        else
        {
            return m_elapsed + caculate();
        }
    }

    nanoseconds MonotonicTimer::caculate()
    {
        if (m_start.time_since_epoch() == nanoseconds::zero())
        {
            return nanoseconds::zero();
        }

        return steady_clock::now() - m_start;
    }

    void MonotonicTimer::clear()
    {
        m_elapsed = nanoseconds::zero();
        m_stoped = true;
        m_start = time_point::min();
    }
}

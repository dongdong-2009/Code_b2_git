/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/timers/src/MonotonicTimer.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: limin.zhu $
  *
  * Class for peforming time-based operations with a
  * millisecond scale.
  *
  */

#ifndef MONOTONICTIMER_H
#define MONOTONICTIMER_H
#include <boost/chrono.hpp>

namespace TA_Base_Core
{
    class MonotonicTimer
    {
    public:

        typedef boost::chrono::steady_clock steady_clock;
        typedef boost::chrono::time_point<steady_clock> time_point;
        typedef time_point::duration nanoseconds;

        MonotonicTimer(bool start_ = true);
        void start();
        void stop();
        void resume();
        void clear();
        bool stoped();
        boost::uint64_t elapsed();      // milliseconds
        boost::uint64_t elapsed_ms();   // milliseconds
        boost::uint64_t elapsed_ns();   // nanoseconds
        unsigned long elapsed_s();      // seconds

    private:

        nanoseconds elapsedImpl();
        nanoseconds caculate();

    private:

        time_point m_start;
        bool m_stoped;
        nanoseconds m_elapsed;
    };
}

#endif

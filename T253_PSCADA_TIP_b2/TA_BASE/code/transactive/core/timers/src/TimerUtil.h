/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/timers/src/TimerUtil.h $
  * @author:	HoaVu
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2018/02/08 10:40:31 $
  * Last modified by:	$Author: Ouyang $
  *
  * Description:
  *
  *
  */

#ifndef TIMER_UTIL_H
#define TIMER_UTIL_H

#ifndef WIN32
	#include "ace/ACE.h"
	#include "ace/Thread_Manager.h"
	#include "ace/Timer_Heap.h"
	#include "ace/Timer_Queue.h"
    #include "ace/Timer_Queue_Adapters.h"
#else
    #include "core/timers/src/WindowsTimerQueue.h"
#endif

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <vector>

namespace TA_Base_Core
{
    class ITimeoutCallback;
}

namespace TA_Base_Core
{
    class TimerUtil : public ACE_Event_Handler
    {
    public:

        TimerUtil();
        virtual ~TimerUtil();

        /**
          *
          * scheduleTimer
          *
          * Schedules an ACE timer
          *
          * @param    timeoutUser                   The registered user
          * @param    timeoutPeriodMSeconds     The timeout period in msec
          * @param  singleShot                  True if a single shot timer
          * @param    userData                  Optional pointer to user defined data, owned by user, never deleted here.
          *
          * @return   the timer ID
          */
        long scheduleTimer(ITimeoutCallback* user, unsigned long timeoutPeriodMSeconds, bool singleShot, void* userData = NULL);

        /**
          *
          * cancelTimer
          *
          * Cancels all timers for a user
          *
          * @param    timeoutUser                   The registered user
          *
          */
        std::vector<void*> cancelTimer(ITimeoutCallback* user);

        /**
          *
          * cancelTimer
          *
          * Cancels an ACE timer
          *
          * @param    timeoutUser                   The registered user
          * @param    timerId                       The timer id returned by scheduleTimer
          *
          */
        void* cancelTimer(ITimeoutCallback* user, long timerId);

        /**
          *
          * handle_timeout
          *
          * Handles a timeout from ACE
          *
          */
        int handle_timeout(const ACE_Time_Value& tv, const void* constArg);

        void invalidate();

    private:

        void* cancelTimerImpl(long timerId);
        bool isExist(ITimeoutCallback* user, long timerId);
        std::vector<long> getTimerIds(ITimeoutCallback* user);
        void removeTimerId(ITimeoutCallback* user, long timerId = -1);

        struct TimerItem
        {
            TimerItem(ITimeoutCallback* user, unsigned long timeoutMs, bool singleShot = true, void* userData = NULL)
                : m_user(user),
                  m_timeoutMs(timeoutMs),
                  m_singleShot(singleShot),
                  m_userData(userData),
                  m_timerId(-1)
            {
            }

            ITimeoutCallback* m_user;
            unsigned long m_timeoutMs;
            bool m_singleShot;
            void* m_userData;
            long m_timerId;
            std::string str();
        };

        typedef std::multimap<ITimeoutCallback*, long>          TimerMap;

#ifndef WIN32
        typedef ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap>  ActiveQueue;
#else
        typedef WindowsTimerQueue                               ActiveQueue;
#endif

        ActiveQueue                                             m_activeQueue;
        ReEntrantThreadLockable                                 m_mapLock;
        TimerMap                                                m_timerMap;
        volatile bool                                           m_invalidated;
    };
}

#endif

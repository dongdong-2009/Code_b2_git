/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/PRIV_3001/TA_BASE/transactive/core/timers/src/TimerUtil.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #3 $
  *
  * Last modification:	$DateTime: 2018/03/23 17:48:46 $
  * Last modified by:	$Author: Ouyang $
  *
  * Description:
  *
  *
  */

#if defined(_MSC_VER)
    #pragma warning(disable:4786 4819)
#endif // defined _MSC_VER

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/threads/src/Thread.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/TimerUtil.h"
#include <boost/format.hpp>

namespace
{
    const unsigned long DEACTIVATE_DELAY(100);
    const unsigned long MAX_DEACTIVATE_ATTEMPT(10);
}

namespace TA_Base_Core
{
    //
    // TimerUtil
    //
    TimerUtil::TimerUtil()
    {
        // Activate the active queue.
        m_activeQueue.activate();
        m_invalidated = false;
    };

    //
    // ~TimerUtil
    //
    TimerUtil::~TimerUtil()
    {
        unsigned long attemptCount = 0;

        // Deactivate the active queue and wait for its thread to terminate.
        // It is done manually to overcome an observed problem where using the
        // queues wait method would block indefinitely as the condition did not
        // seem to be signalled to allow the thread to terminate.
        do
        {
            m_activeQueue.deactivate();
            Thread::sleep(DEACTIVATE_DELAY);
        }
        while ((0 < m_activeQueue.thr_count()) && (MAX_DEACTIVATE_ATTEMPT >= ++attemptCount));
    }

    //
    // scheduleTimer
    //
    long TimerUtil::scheduleTimer(ITimeoutCallback* user, unsigned long timeoutPeriodMSeconds, bool singleShot, void* userData)
    {
        if (m_invalidated || 0 == timeoutPeriodMSeconds || NULL == user)
        {
            return -1;
        }

        // schedule a new timer
        TimerItem* userDefinedData = new TimerItem(user, timeoutPeriodMSeconds, singleShot, userData);
        long timeoutSecs = (long)(timeoutPeriodMSeconds / 1000);
        long timeoutUsecs = (long)(timeoutPeriodMSeconds % 1000) * 1000;

        ACE_Time_Value interval = ACE_Time_Value(timeoutSecs, timeoutUsecs);
        ACE_Time_Value firstRun = ACE_OS::gettimeofday() + interval;

        if (singleShot)
        {
            interval = ACE_Time_Value::zero;
        }

        long timerId =  // <- This is the ID you use to cancel the timer
            m_activeQueue.schedule(this,
                                   reinterpret_cast<void*>(userDefinedData),
                                   firstRun,
                                   interval);

        if (timerId != -1)
        {
            userDefinedData->m_timerId = timerId;
            TA_THREADGUARD(m_mapLock);
            m_timerMap.insert(std::make_pair(user, timerId));
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[temp] TimerUtil::scheduleTimer - %s", userDefinedData->str().c_str());
        }
        else
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Can not schedule timer - %s", userDefinedData->str().c_str());
            delete userDefinedData;
        }

        return timerId;
    }

    //
    // cancelTimer
    //
    std::vector<void*> TimerUtil::cancelTimer(ITimeoutCallback* user)
    {
        std::vector<void*> userDatas;

        if (m_invalidated || 0 == user)
        {
            return userDatas;
        }

        std::vector<long> timerIds = getTimerIds(user);

        for (size_t i = 0; i < timerIds.size(); ++i)
        {
            void* userData = cancelTimerImpl(timerIds[i]);

            if (userData)
            {
                userDatas.push_back(userData);
            }
        }

        removeTimerId(user);
        return userDatas;
    }

    void* TimerUtil::cancelTimer(ITimeoutCallback* user, long timerId)
    {
        if (m_invalidated || timerId < 0)
        {
            return NULL;
        }

        void* userData = cancelTimerImpl(timerId);
        removeTimerId(user, timerId);
        return userData;
    }

    //
    // handle_timeout
    //
    int TimerUtil::handle_timeout(const ACE_Time_Value& tv, const void* constArg)
    {
        if (m_invalidated || NULL == constArg)
        {
            return 0;
        }

        ITimeoutCallback* user = NULL;
        void* userData = NULL;
        long timerId = -1;

        {
            TA_THREADGUARD(m_mapLock);

            TimerItem* userDefinedData = reinterpret_cast<TimerItem*>(const_cast<void*>(constArg));
            user = userDefinedData->m_user;
            timerId = userDefinedData->m_timerId;
            userData = userDefinedData->m_userData;
            bool singleShot = userDefinedData->m_singleShot;

            if (!isExist(user, timerId))
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "[temp] TimerUtil::handle_timeout - cannot find handler for user=%p, timerId=%d, ignored", user, timerId);
                return 0;
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[temp] TimerUtil::handle_timeout - %s", userDefinedData->str().c_str());

            // only delete the data if the timer is single shot
            if (singleShot)
            {
                // remove the expired timer from the map
                // clean up the structure
                delete userDefinedData;
                removeTimerId(user, timerId);
            }
        }

        try
        {
            user->timerExpired(timerId, userData);
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "...", "Leaked out of timerExpired");
        }

        return 0;
    }

    void* TimerUtil::cancelTimerImpl(long timerId)
    {
        void* userData = NULL;
        const void* constArg = NULL;

        m_activeQueue.cancel(timerId, &constArg);

        if (constArg)
        {
            // const arg now contains the original pointer
            TimerItem* userDefinedData = reinterpret_cast<TimerItem*>(const_cast<void*>(constArg));

            if (userDefinedData)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[temp] TimerUtil::cancelTimerImpl - %s", userDefinedData->str().c_str());
                userData = userDefinedData->m_userData;
                delete userDefinedData;
            }
        }

        return userData;
    }

    void TimerUtil::invalidate()
    {
        if (false == m_invalidated)
        {
            TA_THREADGUARD(m_mapLock);
            m_invalidated = true;

            for (TimerMap::iterator it = m_timerMap.begin(); it != m_timerMap.end(); ++it)
            {
                cancelTimerImpl(it->second);
            }

            m_timerMap.clear();
        }
    }

    bool TimerUtil::isExist(ITimeoutCallback* user, long timerId)
    {
        std::pair<TimerMap::iterator, TimerMap::iterator> range = m_timerMap.equal_range(user);

        for (TimerMap::const_iterator it = range.first; it != range.second; ++it)
        {
            if (it->second == timerId)
            {
                return true;
            }
        }

        return false;
    }

    std::vector<long> TimerUtil::getTimerIds(ITimeoutCallback* user)
    {
        TA_THREADGUARD(m_mapLock);
        std::vector<long> timerIds;
        std::pair<TimerMap::iterator, TimerMap::iterator> range = m_timerMap.equal_range(user);

        for (TimerMap::iterator it = range.first; it != range.second; ++it)
        {
            timerIds.push_back(it->second);
        }

        return timerIds;
    }

    void TimerUtil::removeTimerId(ITimeoutCallback* user, long timerId)
    {
        TA_THREADGUARD(m_mapLock);

        if (-1 == timerId)
        {
            m_timerMap.erase(user);
        }
        else
        {
            std::pair<TimerMap::iterator, TimerMap::iterator> range = m_timerMap.equal_range(user);

            for (TimerMap::iterator it = range.first; it != range.second; ++it)
            {
                if (it->second == timerId)
                {
                    m_timerMap.erase(it);
                    break;
                }
            }
        }
    }

    std::string TimerUtil::TimerItem::str()
    {
        return boost::str(boost::format("userDefinedData=%p, timerId=%d, user=%p, timeout=%d, singleShot=%d, userData=%p") % this % m_timerId % m_user % m_timeoutMs % m_singleShot % m_userData);
    }
}

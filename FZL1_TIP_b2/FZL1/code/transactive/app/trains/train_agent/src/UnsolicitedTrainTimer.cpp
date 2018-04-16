/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/trains/train_agent/src/UnsolicitedTrainTimer.cpp $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * This is used to start timers that notify when they expire by a callback.
  * TrainTimer must be waited on to catch timeout events.
  */

#include "app/trains/train_agent/src/UnsolicitedTrainTimer.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

namespace
{
    const unsigned long DEACTIVATE_DELAY( 100 );
    const unsigned long MAX_DEACTIVATE_ATTEMPT( 10 );
}

namespace TA_IRS_App
{
    UnsolicitedTrainTimer::UnsolicitedTrainTimer(IUnsolicitedTimerCallback& callback)
        : m_callback(callback)
    {
        // Activate the active queue.
        m_activeQueue.activate();
    }

	UnsolicitedTrainTimer::~UnsolicitedTrainTimer()
    {
        unsigned long attemptCount = 0;

        // Deactivate the active queue and wait for its thread to terminate.
        // It is done manually to overcome an observed problem where using the
        // queues wait method would block indefinitely as the condition did not
        // seem to be signalled to allow the thread to terminate.
        do
        {
            m_activeQueue.deactivate();
            TA_Base_Core::Thread::sleep( DEACTIVATE_DELAY );
        } while ( ( 0 < m_activeQueue.thr_count() ) && ( MAX_DEACTIVATE_ATTEMPT >= ++attemptCount ) );
    }

	void UnsolicitedTrainTimer::startTimer(unsigned int trainId, unsigned int timeoutMillis, bool logTimerOn, std::string description)
	{
        TA_Base_Core::ThreadGuard guard(m_mapLock);

        // find any existing timer
        // and end it if necessary
        endTimer(trainId);

        // schedule a new timer
        TimerItem* userDefinedData = new TimerItem();
        userDefinedData->trainId = trainId;
        userDefinedData->description = description;
		userDefinedData->logTimerOn = logTimerOn;


        ACE_Time_Value interval;
        interval.msec(timeoutMillis);
        const ACE_Time_Value firstRun = ACE_OS::gettimeofday() + interval;

        long scheduleId =  // <- This is the ID you use to cancel the timer
            m_activeQueue.schedule( this, 
            reinterpret_cast<void*>(userDefinedData),  
            firstRun);

        userDefinedData->timerId = scheduleId;

        // add it to the timer map as well
        m_timerIdentifierMap[trainId] = scheduleId;

		// Log the timer if required
		if(userDefinedData->logTimerOn == true)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"UnsolicitedTrainTimer %s started.", userDefinedData->description.c_str());
		}
	}

    void UnsolicitedTrainTimer::endTimer(unsigned int trainId)
    {
        TA_Base_Core::ThreadGuard guard(m_mapLock);

        // get the entry in the map
        // and remove it
        TrainTimerMap::iterator iter = m_timerIdentifierMap.find(trainId);
        if (iter != m_timerIdentifierMap.end())
        {
            long timerId = iter->second;
            m_timerIdentifierMap.erase(iter);

            // cancel the timer
            const void* constArg=NULL;

            m_activeQueue.timer_queue().cancel( timerId, &constArg );

            // const arg now contains the original pointer
            void* arg = const_cast<void*>( constArg );
            TimerItem* userDefinedData = reinterpret_cast<TimerItem*>(arg);

			// Log the timer if required
			if(userDefinedData->logTimerOn == true)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"UnsolicitedTrainTimer %s ended.", userDefinedData->description.c_str());
			}

            delete userDefinedData;
            userDefinedData = NULL;
        }
    }


	int UnsolicitedTrainTimer::handle_timeout( const ACE_Time_Value &tv, const void *constArg )
    {
        if (constArg == NULL)
		{
			return 0;
		}

        unsigned char trainId = 0;

        // remove the expired timer from the map
        // clean up the structure
        {
            TA_Base_Core::ThreadGuard guard(m_mapLock);

            // extract the data
            void* arg = const_cast<void*>( constArg );
            TimerItem* userDefinedData = reinterpret_cast<TimerItem*>(arg);

            trainId = userDefinedData->trainId;

			// Log the timer if required
			if(userDefinedData->logTimerOn == true)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"UnsolicitedTrainTimer %s expired.", userDefinedData->description.c_str());
			}

            delete userDefinedData;
            userDefinedData = NULL;

            // remove the entry in the map
            TrainTimerMap::iterator iter = m_timerIdentifierMap.find(trainId);
            if (iter != m_timerIdentifierMap.end())
            {
                m_timerIdentifierMap.erase(iter);
            }
        }

        // call the handler
        m_callback.handleTimeout(trainId);

        return 0;
    }

};

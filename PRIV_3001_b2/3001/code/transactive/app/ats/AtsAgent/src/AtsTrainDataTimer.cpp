/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/AtsTrainDataTimer.cpp $
  * @author:   zhou yuan
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * this time is used for AtsTrainData to update the state of m_oa1Valid and m_atcTrainStatusValid 
  * of AtsTrainData
  *
  */

#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "app/ats/AtsAgent/src/AtsTrainDataTimer.h"

namespace
{
    const long DEACTIVATE_DELAY( 100 );
    const long MAX_DEACTIVATE_ATTEMPT( 10 );
	const long INTERVAL_TIME(27);
	const long INVALID_TINER_INTERVAL(-1);
	const long INVALID_TIMER_ID(-1);
}

namespace TA_IRS_App
{
	AtsTrainDataTimer::AtsTrainDataTimer(long oa1Timeout, long atcTimeout)
	{
		FUNCTION_ENTRY("AtsTrainDataTimer");

		// Activate the active queue.
        m_activeQueue.activate();
		
		if(INVALID_TINER_INTERVAL != oa1Timeout && INVALID_TINER_INTERVAL != atcTimeout)
		{
			m_oa1Timeout = oa1Timeout;
			m_atcTimeout = atcTimeout;			
		}
		else
		{
			m_oa1Timeout = INTERVAL_TIME;
			m_atcTimeout = INTERVAL_TIME;	
		}

		FUNCTION_EXIT;
	}

	AtsTrainDataTimer::~AtsTrainDataTimer()
	{
		FUNCTION_ENTRY("~AtsTrainDataTimer");

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
		
		FUNCTION_EXIT
	}

	long AtsTrainDataTimer::startTimer(IAtsTrainDataTimerCallback& callback, 
									   ETimerType timerType, 
									   long timerId)
	{
		FUNCTION_ENTRY("startTimer");
		
		//if we start the timer before , stop it first.
		endTimer(timerId);
		
		//get the interval time from the DB
		
		ACE_Time_Value firstRun;
		if ( AtcTimer == timerType )
		{
			firstRun = ACE_OS::gettimeofday() + ACE_Time_Value(m_atcTimeout);
		}
		else if(Oa1Timer == timerType)
		{
			firstRun = ACE_OS::gettimeofday() + ACE_Time_Value(m_oa1Timeout);
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"the AtsTrainDataTimer type is invalid!");
		}
		
		// schedule a new timer
        TimerItem* userDefinedData = new TimerItem(callback,timerType);

		long retTimerId = m_activeQueue.schedule( this, 
					 reinterpret_cast<void*>(userDefinedData),  
					 firstRun);
		
		FUNCTION_EXIT;
		return retTimerId;
	}

	void AtsTrainDataTimer::endTimer(long timerId)
	{
		FUNCTION_ENTRY("startTimer");
		
		if(INVALID_TIMER_ID != timerId)
		{
			// cancel the timer
			const void* constArg=NULL;
			
	        (m_activeQueue.timer_queue())->cancel( timerId, &constArg );	
		}

		FUNCTION_EXIT;
	}
	
	int AtsTrainDataTimer::handle_timeout(const ACE_Time_Value &tv, const void *constArg )
	{
		FUNCTION_ENTRY("startTimer");
		
		if (constArg == NULL)
		{
			return 0;
		}
		
		// extract the data
		void* arg = const_cast<void*>( constArg );
		TimerItem* userDefinedData = reinterpret_cast<TimerItem*>(arg);

		userDefinedData->callback.handleTimeout(userDefinedData->timerType);

		FUNCTION_EXIT;
		return 0;
	}

};

/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/4669_T01271350/TA_BASE/transactive/core/timers/src/SingletonTimerUtil.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2008/11/28 16:26:01 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *
  */


#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER


#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#include "core/threads/src/ThreadPoolManager.h"

#include "core/timers/src/SingletonTimerUtil.h"

namespace
{
    const unsigned long DEACTIVATE_DELAY( 100 );
    const unsigned long MAX_DEACTIVATE_ATTEMPT( 10 );
}

namespace TA_Base_Core
{

	//
	// Initialisation of statics
	//
	SingletonTimerUtil *					        SingletonTimerUtil::m_singletonTimerUtil = 0;
	unsigned int							        SingletonTimerUtil::m_singletonRefCounter = 0;
	ReEntrantThreadLockable							SingletonTimerUtil::m_singletonLock;

	//
	// SingletonTimerUtil
	//
	SingletonTimerUtil::SingletonTimerUtil()
	{
		// create ThreadedWorker objects, based on the runtime parameter
		// "ThreadedWorkers". Use the default number if not defined
		int maxNumber = 30;
		if ( true == RunParams::getInstance().isSet ( "TimerThreadedWorkers" ) )
		{
			maxNumber = atoi ( RunParams::getInstance().get( "TimerThreadedWorkers" ).c_str() );
		}

		m_threadPoolManager = new ThreadPoolManager(maxNumber);

		TA_ASSERT (m_threadPoolManager != NULL, "Thread Pool Manager not created");
	};

	//
	// ~SingletonTimerUtil
	//
	SingletonTimerUtil::~SingletonTimerUtil()
	{
		// clean up internal objects and the background task running
		m_threadPoolManager->cleanUp();
		delete m_threadPoolManager;
		m_threadPoolManager = NULL;
	}

	//
	// getInstance
	//
	SingletonTimerUtil & SingletonTimerUtil::getInstance()
	{
		// prevent multiple threads creating multiple instances
		// simultaneously
		ThreadGuard guard( m_singletonLock );

		// if SingletonTimerUtil has not been created yet
		if ( 0 == m_singletonTimerUtil )
		{
			// create one and only one instance
			m_singletonTimerUtil = new SingletonTimerUtil();
		}

		// increment reference counter and return the single instance of
		// SingletonTimerUtil
		m_singletonRefCounter++;
		return *m_singletonTimerUtil;
	}

	//
	// removeInstance
	//
	void SingletonTimerUtil::removeInstance()
	{
		// prevent multiple threads creating multiple instances
		// simultaneously
		ThreadGuard guard( m_singletonLock );

		m_singletonRefCounter--;

		// if not referenced anywhere
		if ( 0 == m_singletonRefCounter)
		{
			delete m_singletonTimerUtil;
			m_singletonTimerUtil = 0;
		}
	}

	//
	// startPeriodicTimeOutClock
	//
	long SingletonTimerUtil::startPeriodicTimeOutClock ( ITimeoutCallback * user,
														 unsigned long timeoutPeriodMSeconds,
														 bool singleShot,
                                                         void* userData)
	{
        return m_timerUtil.scheduleTimer(user, timeoutPeriodMSeconds, singleShot, userData);
	}

	//
	// stopPeriodicTimeOutClock
	//
	std::vector<void*> SingletonTimerUtil::stopPeriodicTimeOutClock ( ITimeoutCallback * user )
	{
        std::vector<void*> userData = m_timerUtil.cancelTimer(user);
		return userData;
	}

    //
	// stopPeriodicTimeOutClock
	//
	void* SingletonTimerUtil::stopPeriodicTimeOutClock ( ITimeoutCallback * user, long timerId )
	{
        void* userData = m_timerUtil.cancelTimer(user, timerId);
		return userData;
	}


	void SingletonTimerUtil::queueWorkItem ( IWorkItemPtr workItem )
	{
		m_threadPoolManager->queueWorkItem(workItem);
	}

	void SingletonTimerUtil::removeWorkItem ( IWorkItemPtr workItem )
	{
		m_threadPoolManager->removeWorkItem(workItem);
	}
}

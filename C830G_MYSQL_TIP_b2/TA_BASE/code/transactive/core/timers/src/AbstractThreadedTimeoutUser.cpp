/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/4669_T01271350/TA_BASE/transactive/core/timers/src/AbstractThreadedTimeoutUser.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2008/11/28 16:26:01 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	The class AbstractThreadedTimeoutUser provides the implementation of threading for a
  * user of the SingletonTimerUtil class
  *
  */

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/AbstractThreadedTimeoutUser.h"


namespace TA_Base_Core
{

	AbstractThreadedTimeoutUser::AbstractThreadedTimeoutUser ()
	:
    m_singletonTimerUtil(NULL)
	{
		// get handle to the ThreadPoolSingletonManager
		m_singletonTimerUtil = &SingletonTimerUtil::getInstance();
	}


	AbstractThreadedTimeoutUser::~AbstractThreadedTimeoutUser()
	{
		if ( 0 != m_singletonTimerUtil )
		{
			// stop multi threads accessing this variable
			ThreadGuard guard( m_lock );

			// remove handle to the ThreadPoolSingletonManager
			SingletonTimerUtil::removeInstance();
			m_singletonTimerUtil = 0;
		}
	}

	void AbstractThreadedTimeoutUser::timerExpired(long timerId, void* userData)
	{
		// stop multi threads accessing this variable
		ThreadGuard guard( m_lock );

        IWorkItemPtr workItem( new ThreadedTimeoutWorkItem(*this, timerId, userData) );
		// schedule a work item in ThreadPool for this 
		// timeout
		if ( 0 != m_singletonTimerUtil )
		{
			m_singletonTimerUtil->queueWorkItem ( workItem );
		}	
	}

	ThreadedTimeoutWorkItem::ThreadedTimeoutWorkItem (AbstractThreadedTimeoutUser& parent, long timerId, void* userData)
		: m_parent (parent),
          m_timerId(timerId),
          m_userData(userData)
	{
	}

	ThreadedTimeoutWorkItem::~ThreadedTimeoutWorkItem ()
	{
	}

	void ThreadedTimeoutWorkItem::executeWorkItem()
	{
		//
		// callback from ThreadPool for scheduled work
		//

		// notify the user
		m_parent.threadedTimerExpired(m_timerId, m_userData);
	}
}

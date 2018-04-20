/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/timers/src/TimedWaitSemaphore.cpp $
  * @author:  Darren Sampson
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This class provides a timed wait semaphore implementation with a list of Conditions
  * 
  * NOTE: Only works with pthreads
  */
#include "core/utilities/src/DebugUtil.h"
#include "core/timers/src/TimedWaitSemaphore.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/Thread.h"

namespace TA_Base_Core
{
	TimedWaitSemaphore::TimedWaitSemaphore( unsigned int initial )
		: m_semCount( initial ),
		  m_waitCount(0),
		  m_bDestroyed(false)
	{}

	TimedWaitSemaphore::~TimedWaitSemaphore()
	{
		FUNCTION_ENTRY( "~TimedWaitSemaphore" );
		
		m_bDestroyed = true;
		
		/**
		 * JIRA TAIP-19383 Train agent crashes on timer use
		 * Waiting for all timedWaited threads to finish, then sleep enough 
		 * time let the caller functions of timedWait() safely complete.
		 */
		{
			ThreadGuard guard(m_semLock);
			std::list<Condition*>::iterator it(m_condList.begin()), ie(m_condList.end());
			while(it != ie)
			{
				(*it)->signal();
				++it;
			}
		}
		
		while(m_waitCount > 0)
			Thread::sleep(200); 
						
		FUNCTION_EXIT;
	}

	void TimedWaitSemaphore::wait(void)
	{
		FUNCTION_ENTRY( "wait" );
		
		if(m_bDestroyed)
			return;
		
		Condition *pCond = NULL;
		{
			ThreadGuard guard(m_semLock);		
		
			if(m_semCount > 0)
			{
				-- m_semCount;
			
				FUNCTION_EXIT;			
				return;
			}
		
			pCond = new Condition();
			m_condList.push_back(pCond);
		}
		
		if(pCond)
		{
			pCond->wait();
			delete pCond;
		}		

		FUNCTION_EXIT;		
	}

	bool TimedWaitSemaphore::tryWait(void)
	{
		FUNCTION_ENTRY( "tryWait" );
		
		if(m_bDestroyed)
			return false;
	
		ThreadGuard guard(m_semLock);
		if(m_semCount > 0)
		{
			-- m_semCount;
			
			FUNCTION_EXIT;			
			return true;
		}

		FUNCTION_EXIT;		
		return false;
	}

	void TimedWaitSemaphore::post(void)
	{
		FUNCTION_ENTRY( "post" );
		
		if(m_bDestroyed)
			return;
			
		ThreadGuard guard(m_semLock);
		if(!m_condList.empty())
		{
			m_condList.front()->signal();
			m_condList.pop_front();
			
			FUNCTION_EXIT;
			return;
		}
		
		++ m_semCount;
		FUNCTION_EXIT;		
	}
	
    bool TimedWaitSemaphore::timedWait(unsigned long timeoutMsec)
    {
		FUNCTION_ENTRY( "timedWait" );
		
		if(m_bDestroyed)
			return false;
		
		{
			ThreadGuard guard(m_countLock);
			++m_waitCount;
		}
		
		bool bSucceeded = false;
		
        if ( false == tryWait() )
        {
			if(m_bDestroyed)
			{
				ThreadGuard guard(m_countLock);
				--m_waitCount;			
				return false;
			}
			
			Condition cond;

			{
				ThreadGuard guard(m_semLock);
				m_condList.push_back(&cond);
			}

			if(!cond.timedWait(timeoutMsec))
			{
				ThreadGuard guard(m_semLock);
				m_condList.remove(&cond);
			}
			else
			{
				bSucceeded = true;
			}
        }
		else
		{
			bSucceeded = true;
		}

		{
			ThreadGuard guard(m_countLock);
			--m_waitCount;
		}
		
		FUNCTION_EXIT;		
		return bSucceeded;
    }
}

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/trains/train_agent/src/TrainTimer.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  *
  */

///////////////////////////////////////////////////////////
//  TrainTimer.cpp
//  Implementation of the Class TrainTimer
//  Created on:      28-Nov-2003 02:27:57 PM
///////////////////////////////////////////////////////////

#include "TrainTimer.h"
#include "app/trains/train_agent/src/TARethrow.h"
#include "app/trains/train_agent/src/TrainException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include <algorithm>

using namespace TA_IRS_App;

const unsigned int TrainTimer::TIMEOUT_POLL_PERIOD          =  100;  // millisecs
const unsigned int TrainTimer::NEW_TRANSACTION_RETRY_PERIOD =  100;  // millisecs

TrainTimer::TrainTimer(TrainTransactionManager & trainTransactionManager)
: m_trainTransactionManager(trainTransactionManager),
m_doTerminate(false)
{
	FUNCTION_ENTRY("Constructor");
	FUNCTION_EXIT;
}


TrainTimer::~TrainTimer()
{
	FUNCTION_ENTRY("Destructor");
	terminateAndWait();
	FUNCTION_EXIT;
}

TrainTimer::TrainTimerEntry::~TrainTimerEntry()
{
}


/**
 * This method will continually check the current transactions and decrement their
 * timeTicks every second.  Any transactions with timeTicks reaching zero will
 * cause a timeout to be performed on them 
 */
void TrainTimer::run()
{
	FUNCTION_ENTRY("run");
	while (!m_doTerminate)
	{
		// Cycle round every 100ms
		sleep(TIMEOUT_POLL_PERIOD);

		// Update all the waiting transactions -
		// Note extra scope for thread lock release between polls
		{
			TA_THREADGUARD(m_waitListLock);

			for (unsigned int index = 0; index < m_waitList.size(); ++index)
            {
				if ( 0 == m_waitList[index].timeTick ) 
				{
					// do nothing - timer disabled
				}
				else if ( 1 == m_waitList[index].timeTick ) // timer expired
				{
					std::stringstream s;
					s << "Timer expired for train (" << static_cast<int>(m_waitList[index].trainID) << "), releasing semaphore (" << (unsigned long)m_waitList[index].waitSemaphore << ")";
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
						TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());

					m_waitList[index].timeTick = 0; // disable timer
					m_waitList[index].waitSemaphore->post();
                }
				else
                {
					// this should never happen, just debugging
					if (m_waitList[index].timeTick == 0)
					{
						std::stringstream s;
						s << "Timer (" << index << ") for train (" << static_cast<int>(m_waitList[index].trainID) << "), has unexpected timeTick=" << (long)m_waitList[index].timeTick;
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
							TA_Base_Core::DebugUtil::DebugError, s.str().c_str());

						m_waitList[index].timeTick = 1; // set to sane value
					}

					--m_waitList[index].timeTick;
					// every 3 seconds dump log message to show timer countdown progress...
					if ( (m_waitList[index].timeTick ) % static_cast<int>( 3 * 1000 / TIMEOUT_POLL_PERIOD ) == 0) 
					{
						std::stringstream s;
						s << "Timer (" << index << ") for train (" << static_cast<int>(m_waitList[index].trainID) << "), has (" << (long)m_waitList[index].timeTick * TIMEOUT_POLL_PERIOD / 1000 << ") s to go";
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
							TA_Base_Core::DebugUtil::DebugDebug, s.str().c_str());
					}
                }
			}

        }
		
	}
	FUNCTION_EXIT;
}


/**
 * The terminate() method should cause run() to return. eg: CorbaUtil::
 * GetInstance().Shutdown() or EndDialog(m_dialog)  NOTE: probably not a good idea
 * to call this method directly - call terminateAndWait() instead
 */
void TrainTimer::terminate()
{
	FUNCTION_ENTRY("terminate");
	m_doTerminate = true;
	FUNCTION_EXIT;
}


/**
 * Create a new timer.
 * @param timeOutMillis - if 0 use default timeout, otherwise setup timer for specified time
 */
void TrainTimer::startTimer(unsigned int trainID, unsigned int timeOutMilli, std::string description)
{
	FUNCTION_ENTRY("startTimer");
	while (!m_doTerminate)  // Stop any waiting clients at shutdown.
	{
		{ // extra scope to ensure lock is released between wait retries
			TA_THREADGUARD(m_waitListLock);

			bool  transactionExists = false;
			for (unsigned int index = 0; index < m_waitList.size(); ++index)
			{
				if (m_waitList[index].trainID==trainID)
				{
					transactionExists = true;
					break;
				}
			}

			if (!transactionExists)
			{
                // first check if this timer has been signalled already
                // this is done when TIMS has sent an unsolicited 'ready' message
                // before we have finished processing a response
                std::vector<unsigned int>::iterator findIter = std::find(m_signalledList.begin(), m_signalledList.end(), trainID);
                if (findIter != m_signalledList.end())
                {
                    // the timer was signalled before it started, dont start it
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
				        TA_Base_Core::DebugUtil::DebugWarn, "Not starting timer %s for train %d. It was signalled already", 
						description.c_str(), trainID);
                    FUNCTION_EXIT;
				    return;
                }

				std::stringstream s;
				s << "Timer " << description << " (" << m_waitList.size() << ") for train (" << static_cast<int>(trainID) << "), has started (" << timeOutMilli / 1000 << ") s to go";
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
					TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
				
				TrainTimerEntry   entry;

				entry.trainID         = trainID;
				entry.waitSemaphore   = new TA_Base_Core::Semaphore(0);
                entry.timeTick        = calcTicks(timeOutMilli);
				entry.description	  = description;

				m_waitList.push_back(entry);
				FUNCTION_EXIT;
				return;
			}
		}

		if (static_cast<long>(timeOutMilli) - NEW_TRANSACTION_RETRY_PERIOD > 0)
		{
			timeOutMilli = timeOutMilli - NEW_TRANSACTION_RETRY_PERIOD;
		}
		else 
		{
			timeOutMilli = 1; // to avoid unlikely senario of enabling timer to default value.
			std::stringstream s;
			s << "New transaction " << description << " timed out before it started, for train (" << static_cast<int>(trainID) << ")";
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
				TA_Base_Core::DebugUtil::DebugWarn, s.str().c_str());
			TA_THROW(TrainTimerTimeoutException());
		}

		sleep(NEW_TRANSACTION_RETRY_PERIOD);
	}
	FUNCTION_EXIT;
}



bool TrainTimer::waitTimer(unsigned int trainID)
{
	FUNCTION_ENTRY("waitTimer");
	TA_Base_Core::Semaphore*   signal = NULL;
	std::string trainTimerDescription = "";

	// Get the signal - extra scope for lock release
	{  
		TA_THREADGUARD(m_waitListLock);
		for (unsigned int index = 0;index<m_waitList.size(); index++)
		{
			if (m_waitList[index].trainID==trainID)
			{
				signal = m_waitList[index].waitSemaphore;
				trainTimerDescription = m_waitList[index].description;
				break;
			}
		}
	}

    if (signal == NULL)
    {
        // the timer was signalled before it started, dont wait on it
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
			TA_Base_Core::DebugUtil::DebugWarn, "Not waiting on timer for train %d. It was signalled already", trainID);

        FUNCTION_EXIT;
		return true;
    }

	signal->wait(); // block here for a while

    {
		TA_THREADGUARD(m_waitListLock);
		for (unsigned int index = 0;index<m_waitList.size(); index++)
		{
			if (m_waitList[index].trainID==trainID)
			{
				/// @todo is checking against remaining ticks the best way to do this, or is another flag more appropriate ???
				if (0 == m_waitList[index].timeTick)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
						"TrainTimer %s Train %d expired.", trainTimerDescription.c_str(), trainID);
					// timer expired
					TA_THROW(TrainTimerTimeoutException());
				}
				FUNCTION_EXIT;
				return true; // timer interupted prematurely
			}
		}
	}

	TA_ASSERT(false,"Timer not found after signal.");
    FUNCTION_EXIT;
	return false; // will only get here get here in release mode - should exit/throw exception in debug
}


/**
 * This method will signal a waiting transaction.
 */
bool TrainTimer::signalTimer(unsigned int trainID)
{
	FUNCTION_ENTRY("signalTimer");
	TA_THREADGUARD(m_waitListLock);
	bool status = false;
	std::string trainTimerDescription = "";

	for (unsigned int index = 0;index<m_waitList.size(); index++)
	{
		if (m_waitList[index].trainID==trainID)
		{
			trainTimerDescription = m_waitList[index].description;
			
			std::stringstream s;
			s << "Timer " << trainTimerDescription << " (" << index << ") for train (" << static_cast<int>(m_waitList[index].trainID) << "), has been signalled";
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
				TA_Base_Core::DebugUtil::DebugDebug, s.str().c_str());

			m_waitList[index].waitSemaphore->post();
			FUNCTION_EXIT;
			status = true;
			break;
		}
	}
	if (true == status)
	{
		TA_Base_Core::Thread::sleep(10); 
	}
    else
    {
        // this means a non-existent timer was signalled
        // store this because in some cases a timer can be signalled
        // before it is started
        std::vector<unsigned int>::iterator findIter = std::find(m_signalledList.begin(), m_signalledList.end(), trainID);
        if (findIter == m_signalledList.end())
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
				TA_Base_Core::DebugUtil::DebugWarn, "Timer for train %d was signalled before it started", trainID);

            m_signalledList.push_back(trainID);
        }
    }
	FUNCTION_EXIT;
	return status;  // Function could have timeout out or decided not to wait. 
}


/**
 * This method will remove a transaction 
 */
void TrainTimer::endTimer(unsigned int trainID)
{
	FUNCTION_ENTRY("endTimer");
	TA_THREADGUARD(m_waitListLock);

	std::vector<TrainTimerEntry>::iterator  it;

	for (it=m_waitList.begin();it!=m_waitList.end();++it)
	{
		if (it->trainID==trainID)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"TrainTimer %s for train %d ended.", it->description.c_str(), trainID);
			it->timeTick = 0;
			delete it->waitSemaphore; it->waitSemaphore = 0;
			m_waitList.erase(it);
			FUNCTION_EXIT;
			return;
		}
	}

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
		"Train timer for train %d not found to be removed.", trainID);
    std::vector<unsigned int>::iterator findIter = std::find(m_signalledList.begin(), m_signalledList.end(), trainID);
    if (findIter != m_signalledList.end())
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
			TA_Base_Core::DebugUtil::DebugWarn, "Signalled flag for train %d is being cleared", trainID);

        m_signalledList.erase(findIter);
    }

	FUNCTION_EXIT;
}

inline unsigned int TrainTimer::calcTicks(unsigned int millisec) const
{
    return static_cast<unsigned int>( 1 + (millisec * 1.0) / (TIMEOUT_POLL_PERIOD * 1.0));
}



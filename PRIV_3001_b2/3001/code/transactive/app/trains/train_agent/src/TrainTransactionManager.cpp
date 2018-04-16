/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/trains/train_agent/src/TrainTransactionManager.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class manages a list of clients that are awaiting a response from a train.
  * This manager will ensure that only one message is sent to the train at a time
  * and will wait for a reply. 
  *
  */

///////////////////////////////////////////////////////////
//  TrainTransactionManager.cpp
//  Implementation of the Class TrainTransactionManager
//  Created on:      28-Nov-2003 02:27:57 PM
///////////////////////////////////////////////////////////

#include <algorithm>

#include "TrainTransactionManager.h"
#include "app/trains/train_agent/src/TARethrow.h"
#include "app/trains/train_agent/src/TrainException.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainProtocol.h" // only for Bad Command message number
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#define TIMEOUT_POLL_PERIOD           100  // millisecs
#define NEW_TRANSACTION_RETRY_PERIOD  100  // millisecs

using namespace TA_IRS_App;

TrainTransactionManager::TrainTransactionManager()
                       : m_doTerminate(false),
					     m_waitTimeout(0)
{
	FUNCTION_ENTRY("Constructor");
	FUNCTION_EXIT;
}



TrainTransactionManager::~TrainTransactionManager()
{
	FUNCTION_ENTRY("Destructor");
	terminateAndWait();
	FUNCTION_EXIT;
}


void TrainTransactionManager::setWaitTimeout(unsigned int timeMillisecs)
{
	FUNCTION_ENTRY("setWaitTimeout");
	unsigned int newTimeout = calcTicks(timeMillisecs); 

	if (m_waitTimeout!=newTimeout)
	{
		if (newTimeout==0)
		{
			terminateAndWait();
			m_doTerminate = false;
		}
		else
		{
			start();
		}
		m_waitTimeout = newTimeout;
	}
	FUNCTION_EXIT;
}

/**
 * This method is called to pass a train event to any waiting threads for
 * processing.  Returns true if message processed.
 */
bool TrainTransactionManager::onTrainEvent(TrainEvent& trainEvent)
{
	FUNCTION_ENTRY("onTrainEvent");
	FUNCTION_EXIT;
	return signalTransaction(trainEvent.getTrainID(),trainEvent);
}


/**
 * This method will continually check the current transactions and decrement their
 * timeTicks every second.  Any transactions with timeTicks reaching zero will
 * cause a timeout to be performed on them 
 */
void TrainTransactionManager::run()
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
//				if (m_waitList[index].timeTick>m_waitTimeout)

				/// @note stopping 1 tick early to allow control over post()'ing
				if ( 0 == m_waitList[index]->timeTick ) 
				{
					// do nothing - timer disabled
				}
				else if ( 1 == m_waitList[index]->timeTick ) 
				{
					std::stringstream s;
					s << "Timer expired for train (" << static_cast<int>(m_waitList[index]->trainID) << "), releasing semaphore (" << (long)m_waitList[index]->waitSemaphore << ")" << std::ends;
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
						TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
					
					m_waitList[index]->timeTick = 0; // disable timer
					m_waitList[index]->waitSemaphore->post();
				}
				else
                {
					--m_waitList[index]->timeTick;
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
void TrainTransactionManager::terminate()
{
	FUNCTION_ENTRY("terminate");
	m_doTerminate = true;
	FUNCTION_EXIT;
}


/**
 * Create a new transaction.
 * @param timeOutMillis - if 0 use default timeout, otherwise setup timer for specified time
 */
void TrainTransactionManager::newTransaction(unsigned int trainID, const std::vector<int> & replyEvent, unsigned int timeOutMilli)
{
	FUNCTION_ENTRY("newTransaction");

	unsigned int timeoutTicks = (0==timeOutMilli ? m_waitTimeout: calcTicks(timeOutMilli));

	while (!m_doTerminate)  // Stop any waiting clients at shutdown.
	{
		{ // extra scope to ensure lock is released between wait retries
			TA_THREADGUARD(m_waitListLock);

			bool  transactionExists = false;
			for (unsigned int index = 0; index < m_waitList.size(); ++index)
			{
				if (m_waitList[index]->trainID==trainID)
				{
					transactionExists = true;
					break;
				}
			}

			if (!transactionExists)
			{
				TrainWaitEntry*   entry = new TrainWaitEntry();

				entry->trainID         = trainID;
				entry->waitSemaphore   = new TA_Base_Core::Semaphore(0);
// auto_ptr initializes to zero				entry.trainEvent      = NULL;
				entry->replyEvent      = replyEvent; 
                entry->timeTick        = timeoutTicks;

				m_waitList.push_back(entry);
				FUNCTION_EXIT;
				return;
			}
		}

		if (timeoutTicks > 0)
		{
			timeoutTicks--;
		}
		else 
		{
			std::stringstream s;
			s << "New transaction timed out before it started, for train (" << static_cast<int>(trainID) << ")" << std::ends;
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
				TA_Base_Core::DebugUtil::DebugError, s.str().c_str());
			TA_THROW(TrainMessageTimeoutException());
		}

		sleep(NEW_TRANSACTION_RETRY_PERIOD);
	}
	FUNCTION_EXIT;
}


/**
 * Will wait for the specified train to respond or a timeout to occur.  It will return the signalled event (or NULL
 * if timeout).  The caller is responsible for deleting the returned pointer.
 */
TrainEventPtr TrainTransactionManager::waitForTransaction(unsigned int trainID)
{
	FUNCTION_ENTRY("waitForTransaction");
	TA_Base_Core::Semaphore*   signal = NULL;

	// Get the signal - extra scope for lock release
	{  
		TA_THREADGUARD(m_waitListLock);
		for (unsigned int index = 0;index<m_waitList.size(); index++)
		{
			if (m_waitList[index]->trainID==trainID)
			{
				signal = m_waitList[index]->waitSemaphore;
				break;
			}
		}
	}

	TA_ASSERT(signal!=NULL,"Train semaphore not found.");

	{
		std::stringstream s;
		s << "Waiting for event for train (" << static_cast<int>(trainID) << "), on semaphore (" << (long)signal << ")" << std::ends;
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
			TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
	}

	signal->wait();
	
	{
		std::stringstream s;
		s << "Waiting for event complete, for train (" << static_cast<int>(trainID) << "), on semaphore (" << (long)signal << ")" << std::ends;
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
			TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
	}

	{
		// Get the reply event
		TA_THREADGUARD(m_waitListLock);
		for (unsigned int index = 0;index<m_waitList.size(); index++)
		{
			if (m_waitList[index]->trainID==trainID)
			{
				TrainEventPtr event = m_waitList[index]->trainEvent;
				if (event.get()==NULL)
				{
					// Timeout must have occurred
					TA_THROW(TrainMessageTimeoutException());
				}
// auto_ptr already did this				m_waitList[index].trainEvent = NULL;

				{
					std::stringstream s;
					s << "Retrieved event for train (" << static_cast<int>(m_waitList[index]->trainID) << ")" << std::ends;
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
						TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
				}

				FUNCTION_EXIT;
				return event;
			}
		}
	}
	TA_ASSERT(false,"Transaction not found after signal.");
	FUNCTION_EXIT;
	return TrainEventPtr();
}


bool TrainTransactionManager::abortTransaction(TrainID trainID)
{
	FUNCTION_ENTRY("abortTransaction");
	bool status = false;

	{
		TA_THREADGUARD(m_waitListLock);
		
		for (unsigned int index = 0;index<m_waitList.size(); index++)
		{
			if (m_waitList[index]->trainID == trainID)
			{
				int theFirstReplyEvent = *(m_waitList[index]->replyEvent.begin());
				if (theFirstReplyEvent == COMMAND_NO_REPLY_EVENT)
				{
					std::stringstream s;
					s << "Event not expected for pending transaction on train (" << static_cast<int>(m_waitList[index]->trainID) 
						<< "), expected (" << theFirstReplyEvent
						<< "), not signalling..." << std::ends;
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, s.str().c_str());
					break;
				}
				else // Now, we have a valid transaction, but received an invalid event, so terminate any transaction 
				{
#if 0 // TIP5
					TA_ASSERT(m_waitList[index]->trainEvent.get()==NULL,"Duplicate signal received for train.");
#endif
					if (! (m_waitList[index]->trainEvent.get()==NULL) ) 
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Duplicate signal received for train.");
						// TIP5 - relaxing assert, so make sure we exit cleanly
						status = false;
						break;
					}

					/// @todo examine current status of semaphore, to see if anyone already signalled.
					
					{
						std::stringstream s;
						s << "ABORT transaction - Invalid Event received for train (" << static_cast<int>(m_waitList[index]->trainID) << ")"
							<< ", releasing semaphore (" << (long)m_waitList[index]->waitSemaphore << ")" 
							<< std::ends;
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, s.str().c_str());
					}
					
					m_waitList[index]->trainEvent = TrainEventPtr(0);
					m_waitList[index]->waitSemaphore->post();
					FUNCTION_EXIT;
					status = true;
					break;
				}
			}
		}
		
	}

	if (true == status)
	{
		TA_Base_Core::Thread::sleep(10); 
	}
	FUNCTION_EXIT;
	return status;  // Function could have timeout out or decided not to wait. 
}

/**
 * This method will signal a waiting transaction.
 */
bool TrainTransactionManager::signalTransaction(unsigned int trainID, TrainEvent& event)
{
	FUNCTION_ENTRY("signalTransaction");
	bool status = false;

	{
		TA_THREADGUARD(m_waitListLock);
		
		for (unsigned int index = 0;index<m_waitList.size(); index++)
		{
			if (m_waitList[index]->trainID == trainID)
			{
				int theFirstReplyEvent = *(m_waitList[index]->replyEvent.begin());
				std::vector<int>::iterator matchingEvent = 
					std::find(m_waitList[index]->replyEvent.begin(), 
					          m_waitList[index]->replyEvent.end(),
							  event.getMessageType() );
				if (theFirstReplyEvent == COMMAND_NO_REPLY_EVENT)
				{
					std::stringstream s;
					s << "Event not expected for pending transaction on train (" << static_cast<int>(m_waitList[index]->trainID) 
						<< "), expected (" << theFirstReplyEvent 
						<< "), got message type (" << event.getMessageType() 
						<< "), not signalling..." << std::ends;
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, s.str().c_str());
					break;
				}
//				else if (m_waitList[index]->replyEvent == event.getMessageType() ||
				else if (matchingEvent != m_waitList[index]->replyEvent.end() ||
					COMMAND_BAD_COMMAND == event.getMessageType() )
				{
#if 0 // TIP5
					TA_ASSERT(m_waitList[index]->trainEvent.get()==NULL,"Duplicate signal received for train.");
#endif
					if (! (m_waitList[index]->trainEvent.get()==NULL) ) 
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Duplicate signal received for train.");
						// TIP5 - relaxing assert, so make sure we exit cleanly
						status = false;
						break;
					}

					/// @todo examine current status of semaphore, to see if anyone already signalled.
					
					{
						std::stringstream s;
						s << "Event received for train (" << static_cast<int>(m_waitList[index]->trainID) << "), releasing semaphore (" << (long)m_waitList[index]->waitSemaphore << ")" << std::ends;
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
							TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
					}
					
					m_waitList[index]->trainEvent = TrainEventPtr(new TrainEvent(event));
					m_waitList[index]->waitSemaphore->post();
					status = true;
					break;
				}
				else if ( (61 == event.getMessageType() ) || (62 == event.getMessageType() ) )
				{
					std::stringstream s;
					s << "OA1 or ATC Alarm Event receieved for train (" << static_cast<int>(m_waitList[index]->trainID) << "), message type (" << event.getMessageType() << ")" << std::ends;
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
					break;
				}
				else
				{
					std::stringstream s;
					s << "Unknown message type received for train (" << static_cast<int>(m_waitList[index]->trainID) << "), message type (" << event.getMessageType() << ")" << std::ends;
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, s.str().c_str());
					break;
				}
			}
		}
		
	}
	if (true == status)
	{
		TA_Base_Core::Thread::sleep(10); 
	}

	FUNCTION_EXIT;
	return status;  // Function could have timeout out or decided not to wait. 
}


/**
 * This method will remove a transaction 
 */
void TrainTransactionManager::removeTransaction(unsigned int trainID)
{
	FUNCTION_ENTRY("removeTransaction");
	TA_THREADGUARD(m_waitListLock);

	std::vector<TrainWaitEntry*>::iterator  it;

	for (it=m_waitList.begin();it!=m_waitList.end();++it)
	{
		if ((*it)->trainID==trainID)
		{
			{
				std::stringstream s;
				s << "Removing event for train (" << static_cast<int>(trainID) << "), deleting semaphore (" << (long)((*it)->waitSemaphore) << ")" << std::ends;
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
					TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
			}
			
			//delete it->trainEvent;  /// @todo Check this - is it deleting something we reference later (see TrainManager)
			delete (*it)->waitSemaphore; 
			delete (*it);
			/// @todo test this doesn't break dodgy code keeping refs to these...
			//it->trainEvent = 0;
			//it->waitSemaphore = 0;
			m_waitList.erase(it);
			FUNCTION_EXIT;
			return;
		}
	}

/*
	for (unsigned int index = 0;index<m_waitList.size(); index++)
	{
		if (m_waitList[index].trainID==trainID)
		{
			delete m_waitList[index].trainEvent;
			delete m_waitList[index].waitSemaphore;
			//m_waitList.erase(&m_waitList[index]);
			m_waitList.erase(m_waitList.at(index));
			FUNCTION_EXIT;
			return;
		}
	}
*/
	TA_ASSERT(false,"Transaction not found to removed.");
	FUNCTION_EXIT;
}

inline unsigned int TrainTransactionManager::calcTicks(unsigned int millisec) const
{
	// always at least one tick
    return static_cast<unsigned int>( (millisec * 1.0) / (TIMEOUT_POLL_PERIOD * 1.0) + 1);
}

bool TrainTransactionManager::hasTrainsactions(TrainID trainID)
{
	FUNCTION_ENTRY("hasTrainsactions");

	bool has = false;

	{
		TA_THREADGUARD(m_waitListLock);
		
		unsigned int index = 0;
		while((has == false) && (index < m_waitList.size()))
		{
			if (m_waitList[index]->trainID == trainID)
			{
				has = true;
			}
			index++;
		}
	}

	FUNCTION_EXIT;
	return has;
}



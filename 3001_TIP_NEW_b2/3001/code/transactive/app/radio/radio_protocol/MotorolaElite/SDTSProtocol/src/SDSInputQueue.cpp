/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSInputQueue.cpp $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Manages the receiption of SDS events. The rate of incoming events may affect 
  * the rate of sending events
  */

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSInputQueue.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSEventObserver.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSQueueManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/Thread.h"

using namespace TA_Base_Core;

namespace TA_IRS_App {

    SDSInputQueue::SDSInputQueue(SDSQueueManager& queueManager):
                    m_queueManager(queueManager), m_active(false)
	{
		FUNCTION_ENTRY( "Constructor" );
		FUNCTION_EXIT;
	}

	SDSInputQueue::~SDSInputQueue()
	{
		FUNCTION_ENTRY( "Destructor" );
		FUNCTION_EXIT;
	}


	void SDSInputQueue::start()
	{
		FUNCTION_ENTRY("start");
		
		m_active = true;

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"call to start");
		unsigned short result = activate();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"start result=%d",result);
//		TA_ASSERT(result == 0, "Error, Fail to activate the message queue!");

		FUNCTION_EXIT;
	}

	void SDSInputQueue::stop()
	{
		FUNCTION_ENTRY("stop");
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"call to stop");
		m_active = false;
		wait();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"stop");
//        m_queue.enqueue(NULL); // wake up thread and exit

		FUNCTION_EXIT;

	}


	void SDSInputQueue::enqueue(SDSEvent* rspMsg)
	{
		FUNCTION_ENTRY("enqueue");

		m_queue.enqueue(rspMsg);

		FUNCTION_EXIT;
	}


	int SDSInputQueue::svc()
	{
		FUNCTION_ENTRY( "svc" );

		while( m_active )//the queue still active
		{
            try
            {
				if (m_queue.is_empty())
				{
					if (m_active) TA_Base_Core::Thread::sleep(100);
					continue;
				}

			    // method will block until a request is available
			    ACE_Method_Request *request = m_queue.dequeue(); //ACE_Time_Value = 0

			    // update latest response timestamp
			    m_queueManager.updateResponseTimestamp();

			    if (request != NULL )
			    {
				    request->call();
			    }

			    SDSEvent* event = dynamic_cast< SDSEvent* >(request);
			    if ( event != NULL )
			    {
				    notifyObservers(event);
			    }

                delete request; // free memory
            }
            catch (TransactiveException& e)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
            }
            catch (...)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Unknown exception");
            }
		}
		
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"call to close");
		//close the thread by explictedly calling the close method, may not be a good way
		unsigned short result = close();
		TA_ASSERT(result == 0, "Error, Fail to deactivate the message queue!");	

		FUNCTION_EXIT;
		return 0;
	}


	void SDSInputQueue::registerObserver(SDSEventObserver* evtObj)
	{
		FUNCTION_ENTRY("registerObserver");

        ThreadGuard lvLock(m_lock);

        // do not add if already registered
		std::vector<SDSEventObserver*>::iterator pos;
		for (pos = m_observers.begin();pos != m_observers.end();++pos)
		{
			if (evtObj == *pos)
			{
                return;
			}
		}    
		m_observers.push_back(evtObj);

		FUNCTION_EXIT;
	}


	void SDSInputQueue::deregisterObserver(SDSEventObserver* evtObj)
	{
		FUNCTION_ENTRY("deregisterObserver");

        ThreadGuard lvLock(m_lock);

		std::vector<SDSEventObserver*>::iterator pos;
		for (pos = m_observers.begin();pos != m_observers.end();++pos)
		{
			if (evtObj == *pos)
			{
				pos=m_observers.erase(pos);
			}
		}

		FUNCTION_EXIT;
	}


	void SDSInputQueue::notifyObservers(SDSEvent* event)
	{
		FUNCTION_ENTRY("notifyObservers");

        ThreadGuard lvLock(m_lock);

		std::vector<SDSEventObserver*>::iterator pos;

		for (pos = m_observers.begin();pos != m_observers.end();++pos)
		{
			(*pos)->notify(event);
		}

		FUNCTION_EXIT;
	}

	void SDSInputQueue::clearQueue()
	{
		FUNCTION_ENTRY("clearQueue");
		 
		int r = m_queue.queue()->flush();
		if (-1 != r) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Message clear of SDS Input Queue: %u", r);
		} else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error removing message from SDS Input Queue");
		}

		FUNCTION_EXIT;
	}
}

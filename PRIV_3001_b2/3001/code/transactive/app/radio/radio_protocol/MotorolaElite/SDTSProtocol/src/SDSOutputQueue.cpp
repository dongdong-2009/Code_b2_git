/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSOutputQueue.cpp $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Manages the sending of SDDS requests. The internal scheduler is subject to
  * rate limiting. Rate limiting is based upon rate of sending and receiving
  * messages between the SDS Client and SDS Host.
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSFunction.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSOutputQueue.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSQueueManager.h"
#include "core/threads/src/Thread.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{
	SDSOutputQueue::SDSOutputQueue(SDSQueueManager& queueManager) : 
            m_queueManager(queueManager), m_active(false)
	{
		FUNCTION_ENTRY( "Constructor" );
		FUNCTION_EXIT;
	}

	SDSOutputQueue::~SDSOutputQueue()
	{
		FUNCTION_ENTRY( "Destructor" );
		FUNCTION_EXIT;
	}


	void SDSOutputQueue::start()
	{
		FUNCTION_ENTRY("start");
		
		m_active = true;
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"call to start");
		unsigned short result = activate();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"start result=%d",result);
//		TA_ASSERT(result == 0, "Error, Fail to activate the message queue!");
		

		FUNCTION_EXIT;
	}

	void SDSOutputQueue::stop()
	{
		FUNCTION_ENTRY("stop");
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"call to stop"); 
		m_active = false;
		wait();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"stop");
 //       m_queue.enqueue(NULL); // wake up thread and exit

		FUNCTION_EXIT;

	}

	void SDSOutputQueue::enqueue(SDSFunction* requestMsg)
	{
		FUNCTION_ENTRY("enqueue");

		m_queue.enqueue(requestMsg);

		FUNCTION_EXIT;
	}


	int SDSOutputQueue::svc()
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

			    //m_queueManager.rateLimit();  //Removed::don't have any user to set the setPeriod

                if (request != NULL)
                {
    			    request->call();
                }

			    // update latest request timestamp
			    m_queueManager.updateRequestTimestamp();

				delete request;
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
    
		//close the thread by explictedly calling the close method, may not be a good way
		unsigned short result = close();
		TA_ASSERT(result == 0, "Error, Fail to deactivate the message queue!");	

		FUNCTION_EXIT;
		return 0;
	}

	void SDSOutputQueue::clearQueue()
	{
		FUNCTION_ENTRY("clearQueue");

		int r = m_queue.queue()->flush();
		if (-1 != r) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Message clear of SDS Output Queue: %u", r);
		} else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error removing message from SDS Output Queue");
		}

		FUNCTION_EXIT;
	}
}

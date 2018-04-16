/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/src/STISStatusRequestQueue.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class maintains a queue of STIS status request messages.
  * It processes each of them in its own thread, passing them on
  * to the STISManager for handling.
  *
  */

#include "STISStatusRequestQueue.h"
#include "STISManager.h"

namespace TA_IRS_App
{

    STISStatusRequestQueue::STISStatusRequestQueue()
        : m_queueSemaphore(0), m_isRunning(false)
    {
    }


    STISStatusRequestQueue::~STISStatusRequestQueue()
    {
    }


    void STISStatusRequestQueue::addMessageToQueue(STISStatusRequest* message)
    {
        if (m_isRunning)
        {
            TA_Base_Core::ThreadGuard guard(m_messageLock);
            
            m_messages.push(message);
            m_queueSemaphore.post();
        }
    }


    void STISStatusRequestQueue::run()
    {
        m_isRunning = true;

        while (m_isRunning)
        {
			STISStatusRequest* request = NULL;
			while(m_isRunning && (request = take()) != NULL)
			{
				handleMessage(request);
            }
            m_queueSemaphore.wait();
        }
		
		clearQueue();
    }


    void STISStatusRequestQueue::terminate()
    {
        m_isRunning = false;
        m_queueSemaphore.post();
    }


    void STISStatusRequestQueue::handleMessage(STISStatusRequest* message)
    {
        STISManager::getInstance()->handleStatusRequestMessage(message);
    }

	STISStatusRequest* STISStatusRequestQueue::take()
	{
        TA_Base_Core::ThreadGuard guard(m_messageLock);
		STISStatusRequest* r = NULL;
		if(!m_messages.empty())
		{
			r = m_messages.front();
			m_messages.pop();
		}
		return r;
	}

	void STISStatusRequestQueue::clearQueue()
	{
        TA_Base_Core::ThreadGuard guard(m_messageLock);

		while(!m_messages.empty())
		{
			delete m_messages.front();
			m_messages.pop();
		}
	}

} // namespace TA_IRS_App

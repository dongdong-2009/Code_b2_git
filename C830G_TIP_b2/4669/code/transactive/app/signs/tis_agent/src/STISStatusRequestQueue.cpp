/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISStatusRequestQueue.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
        if (m_isRunning == true)
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
            { // Acquire threadlock
                TA_Base_Core::ThreadGuard guard(m_messageLock);

                while (!m_messages.empty())
                {
                    handleMessage(m_messages.front());
                    m_messages.pop();
                }
            } // Release threadlock

            m_queueSemaphore.wait();
        }
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

} // namespace TA_IRS_App
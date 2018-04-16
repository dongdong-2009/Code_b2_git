#include "STISStatusProcessQueue.h"
#include "STISManager.h"
#include "STISStatusRequest.h"

namespace TA_IRS_App
{

    STISStatusProcessQueue::STISStatusProcessQueue()
        : m_queueSemaphore(0), m_isRunning(false)
    {
    }


    STISStatusProcessQueue::~STISStatusProcessQueue()
    {
    }


    void STISStatusProcessQueue::addMessageToQueue(STISStatusRequest* message)
    {
        if (m_isRunning)
        {
            TA_Base_Core::ThreadGuard guard(m_messageLock);
            
            m_messages.push(message);
            m_queueSemaphore.post();
        }
    }


    void STISStatusProcessQueue::run()
    {
        m_isRunning = true;

        while (m_isRunning)
        {
			STISStatusRequest* request = NULL;
			while(m_isRunning && (request = take()) != NULL)
			{
				request->processResponse();
				delete request;
            }
            m_queueSemaphore.wait();
        }

		clearQueue();
    }


    void STISStatusProcessQueue::terminate()
    {
        m_isRunning = false;
        m_queueSemaphore.post();
    }


	STISStatusRequest* STISStatusProcessQueue::take()
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

	void STISStatusProcessQueue::clearQueue()
	{
        TA_Base_Core::ThreadGuard guard(m_messageLock);

		while(!m_messages.empty())
		{
			delete m_messages.front();
			m_messages.pop();
		}
	}

} // namespace TA_IRS_App

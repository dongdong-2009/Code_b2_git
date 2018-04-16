/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/RATISMessenger.cpp $
  * @author:   San Teo
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class does nothing other than keeping a list of vetting response that is to be sent to
  * the STIS server, regularly check if it has something in the list and getting the STISManager to 
  * send the vetting response.  The intention of this class it to allow the vetting response to be run on 
  * a separate thread so that the client's call to STISManager's submitRATISVettingResponse could return 
  * before it timed out in a lengthy operation like clearing the train messages.  
  *
  */

#include "app/signs/tis_agent/src/RATISMessenger.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/AgentModeMonitor.h"


namespace TA_IRS_App
{
    using TA_Base_Core::DebugUtil;

	//
    // Initialise statics.
    //

	TA_Base_Core::ReEntrantThreadLockable RATISMessenger::m_queueLock;
	TA_Base_Core::ReEntrantThreadLockable RATISMessenger::m_terminateLock;
    const int RATISMessenger::POLL_INTERVAL = 1000;  // check every 1 second?

	RATISMessenger::RATISMessenger() : m_isTerminating(false)
	{
	}


	RATISMessenger::~RATISMessenger()
	{
	}


    void RATISMessenger::addRATISVettingResponse(const long key, const bool approved, const short priority, const char* content, const char* sessionID )
    {
        VettingRATISResponse resp;
        resp.key = key;
        resp.approved = approved;
        resp.priority = priority;
        resp.content = (content != NULL) ? content : "";
        resp.sessionID = (sessionID != NULL) ? sessionID : "";

        TA_Base_Core::ThreadGuard guard(m_queueLock);
        m_queue.push(resp);
    }


	void RATISMessenger::run()
	{
		FUNCTION_ENTRY("run()");

		while(!isTerminating())
		{
			if( AgentModeMonitor::getInstance().isInControlMode())
			{
                do
                {
                    VettingRATISResponse resp;
                    bool gotSomethingToSend;

                    {// put guard here so that we can add more ratis vetting response while we're submitting the response
                        TA_Base_Core::ThreadGuard guard(m_queueLock);
                        if (m_queue.empty())
                        {
                            break; // break out from the while loop
                        }
                        // if there is something in the queue, pop the first item in the queue, submit a request
                        resp = m_queue.front();
                        m_queue.pop();
                        gotSomethingToSend = true;
                    }

                    if (gotSomethingToSend)
                    {
        				STISManager::getInstance()->submitRATISVettingResponse(resp.key, resp.approved, resp.priority, resp.content, resp.sessionID);
                    }
                } while (true);
			}
			this->sleep(POLL_INTERVAL);
		}

		FUNCTION_EXIT;
	}


	void RATISMessenger::terminate()
	{
		FUNCTION_ENTRY("terminate");
        setIsTerminating();
		FUNCTION_EXIT;
	}


    void RATISMessenger::setIsTerminating()
    {
        TA_Base_Core::ThreadGuard guard(m_terminateLock);
        m_isTerminating = true;
    }


    bool RATISMessenger::isTerminating()
    {
        TA_Base_Core::ThreadGuard guard(m_terminateLock);
        return m_isTerminating;
    }

}  // namespace TA_IRS_App
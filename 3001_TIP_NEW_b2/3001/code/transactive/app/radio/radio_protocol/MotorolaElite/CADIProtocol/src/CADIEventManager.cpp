
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventManager.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */

#include "ace/Time_Value.h"
#include "ace/OS.h"
#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventManager.h"
#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADIProtocol.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace
{
    // class to start a different thread to receive event
    class EventTester :  public virtual ACE_Task_Base
    {
	    public:
	    EventTester()
	    {
	    }

	    virtual ~EventTester()
	    {
	    }

	    virtual int  svc (void)
	    {
            registerCadiEventCallback(TA_IRS_App::CADIEventManager::eventCallback);
            return 0;
	    }
    };
}

namespace TA_IRS_App {

    static EventTester eventListener;

	CADIEventManager::CADIEventManager(): 
	m_shutdown(false)
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	CADIEventManager::~CADIEventManager()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

    void CADIEventManager::startEventListener()
    {
        eventListener.activate();
    }

    void CADIEventManager::stopEventListener()
    {
        eventListener.close();
    }

	int CADIEventManager::perform(ACE_Method_Request *req)
	{
		FUNCTION_ENTRY("perform");
		FUNCTION_EXIT;
    
		// Queue requests
		return m_queue.enqueue(req);
	}

	int CADIEventManager::svc()
	{
		FUNCTION_ENTRY("svc");

		// Dispatch request
		while (!m_shutdown)
		{
            try
            {
			    ACE_Time_Value tv(5);
			    tv += ACE_OS::time(0);

			    ACE_Method_Request *request = this->m_queue.dequeue(&tv); 

			    if(request == NULL) continue;

			    int result = request->call();
				
				CADIEvent *event = dynamic_cast<CADIEvent*>(request);

				if (event != NULL)
				{
					notifyObservers(*event);
				}

                delete request;
            }
            catch (...)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception thrown");
            }

		}

		FUNCTION_EXIT;
		return 0;
	}

	int CADIEventManager::shutdown()
	{
		FUNCTION_ENTRY("shutdown");
		m_shutdown = true;

        // wake up thread
        cadiEventManager::instance()->m_queue.enqueue(NULL);

		FUNCTION_EXIT;
		return 0;
	}

	void CADIEventManager::eventCallback(CADIEventType et, void* arg)
	{
		FUNCTION_ENTRY("eventCallback");
        /*
		switch(et)
		{
		case CADEV_Affiliation:
			{
				cadiEventManager::instance()->m_queue.enqueue(
                    new AffiliationEvent(*((CADI_RADIO_EVENT_INFO*)arg)));
			}
			break;

		case CADEV_CallAlert:
//			TA_ASSERT(false, "CADEV_CallAlert event");
			break;

		case CADEV_ChangeMe:
//			TA_ASSERT(false, "CADEV_ChangeMe event");
			break;
    
		case CADEV_CmdStatus:
			{
				cadiEventManager::instance()->m_queue.enqueue(
                    new CmdStatusEvent(*((CADI_CMD_STATUS_DATA*)arg)));
			}
			break;
    
		case CADEV_Deaffiliation:
			{
                cadiEventManager::instance()->m_queue.enqueue(
                    new DeaffiliationEvent(*((CADI_RADIO_EVENT_INFO*)arg)));
			}
        
			break;
    
		case CADEV_EmergencyAlarm:
			{
                cadiEventManager::instance()->m_queue.enqueue(
                    new EmergencyAlarmEvent(*((CADI_EA_DATA*)arg)));
			}
			break;
    
		case CADEV_GroupCall:
			{
                cadiEventManager::instance()->m_queue.enqueue(
                    new GroupCallEvent(*((CADI_GROUP_CALL_DATA*)arg)));
			}
			break;
    
		case CADEV_InterconCall:
//			TA_ASSERT(false, "CADEV_InterconCall event");
			break;
    
		case CADEV_Message:
//			TA_ASSERT(false, "CADEV_Message event");
			break;
    
		case CADEV_PrivateCall:
			{
                cadiEventManager::instance()->m_queue.enqueue(
                    new PrivateCallEvent(*((CADI_PRIVATE_CALL_DATA*)arg)));
			}
			break;
    
		case CADEV_RadioAck:
			{
                cadiEventManager::instance()->m_queue.enqueue(
                    new RadioAckEvent(*((CADI_RADIO_ACK_DATA*)arg)));
			}
			break;
    
		case CADEV_Session:
			{
                cadiEventManager::instance()->m_queue.enqueue(
                    new SessionEvent(*((CADI_SESSION_STATUS*)arg)));
			}
			break;
    
		case CADEV_Status:
			{
                cadiEventManager::instance()->m_queue.enqueue(
                    new StatusEvent(*((CADI_STATUS_DATA*)arg)));
			}
			break;
    
		case CADEV_ZCStatus:
			{
                cadiEventManager::instance()->m_queue.enqueue(
                    new ZCStatusEvent(*((CADI_ZC_STATUS_DATA*)arg)));
			}
			break;

        default:
//			TA_ASSERT(false, "Unknown event");

            ;
		}
        */
		FUNCTION_EXIT;
	}

}

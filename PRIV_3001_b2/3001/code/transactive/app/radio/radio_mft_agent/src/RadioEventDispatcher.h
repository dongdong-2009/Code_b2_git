/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_mft_agent/src/RadioEventDispatcher.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#ifndef RADIO_EVENT_DISPATCHER_H_INCLUDED
#define RADIO_EVENT_DISPATCHER_H_INCLUDED

#include "ace/Task.h"
#include "ace/Activation_Queue.h"

//#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/CDIEventObserver.h"
//#include "app/radio/radio_mft_agent/src/RadioMFTAgentEventImpl.h"
#include "app/radio/radio_mft_agent/src/IRadioMFTAgentCommand.h"
#include "app/radio/radio_mft_agent/src/EventToResultFactory.h"
#include "app/radio/radio_mft_agent/src/FutureManager.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentAliveHelper.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"

#include"app/radio/radio_mft_agent/src/RemoteAPIRadioEvent.h"
#include"app/radio/radio_mft_agent/src/RadioMFTAgentRemoteApiEventImpl.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <queue>

class RadioEventDispatcher : public virtual ACE_Task_Base//, public virtual TA_IRS_App::CDIEventObserver 
{
	public:

		RadioEventDispatcher(FutureManager* pManager,MCC7500SDK::IAPIRequest_ptr &ptr, TA_Base_Core::ReEntrantThreadLockable &apiLock);
		RadioEventDispatcher();

		virtual ~RadioEventDispatcher();


		void start();
        void stop();
		
		virtual int  svc (void);	

		bool addCallback(TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr pCallback);

		void setMFTCommand(IRadioMFTAgentCommand* pCommand);

		void onRAPIReady()
		{
			//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"sending dispatcher Ready");
			m_RadioMFTAgentRemoteApiEventImpl.onMcdiDispatchReady();

			// Check MCC status

		}
		void processEvent(TA_IRS_App::RemoteAPIRadioEvent* event);

		void reconnect()
		{
			m_RadioMFTAgentRemoteApiEventImpl.reconnect();
		}

	private:

		void checkResult(ACE_Future<FutureResultT>& future);


	private:

		bool m_deactivate;

		//RadioMFTAgentEventImpl m_RadioMFTAgentEventImpl;
		RadioMFTAgentRemoteApiEventImpl m_RadioMFTAgentRemoteApiEventImpl;
		FutureManager* m_pFutureManager;
		
        /**
          * synchronisation object
          */
        TA_Base_Core::ReEntrantThreadLockable m_queueLock;
        /**
	      * Used to signal clients of the queue.
	      */
        TA_Base_Core::Semaphore m_eventSem;

		// To handle the connection status of radio (mcc/gateway) and iscs
		RadioMFTAgentAliveHelper m_statusHelper;
};

#endif 

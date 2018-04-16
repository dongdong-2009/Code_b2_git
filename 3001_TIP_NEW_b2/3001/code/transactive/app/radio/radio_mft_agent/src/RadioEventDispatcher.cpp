/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_mft_agent/src/RadioEventDispatcher.cpp $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#include "app/radio/radio_mft_agent/src/RadioEventDispatcher.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/CDI_Event.h"
//#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/CDI_IncomingMessageQueue.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "Mca/mca.h"
#include "Mcdi/callback.h"
#include "Mcdi/registration.h"
#include "Mcdi/applications_reset.h"
#include "Mcdi/cdi_status.h"
#include "Mcdi/dispatch_status.h"
#include "Mcdi/pers_db_sync.h"
#include "Mcdi/rc_db_sync.h"

#include "Mca/al_call.h"
#include "Mca/emergency_alarm.h"
#include "Mca/emergency_call.h"
#include "Mca/general_transmit.h"
#include "Mca/inbound_call.h"  	
#include "Mca/inbound_ptt_id.h"	
#include "Mca/transmit_mode_select.h"  
#include "Mca/multi_select.h"
#include "Mca/patch.h"
#include "Mca/private_call.h"
#include "Mca/radio_status.h"
#include "Mca/refresh.h"
#include "Mca/rsc_assignment.h"
#include "Mca/volume.h"
#include "Mca/resource_reject_status.h"
#include "Mca/resource_xmit_status.h"
#include "Mca/rsc_select.h"
#include "Mca/phone_control.h"
#include "Mca/rtdial.h"
#include "Mca/audible_alarm.h"
#include "Mca/tone_gen.h"

#include"app/radio/radio_mft_agent/src/RemoteAPIRadioEvent.h"
// !for debugging only !!!!!!
#include "app/radio/radio_manager_gui/src/DebugMsg.h"

#include "app/radio/radio_mft_agent/src/RemoteAPICallBackMessage.h"


RadioEventDispatcher::RadioEventDispatcher(FutureManager* pManager,MCC7500SDK::IAPIRequest_ptr &ptr, TA_Base_Core::ReEntrantThreadLockable &apiLock) : 
	m_pFutureManager(pManager), m_eventSem(0),m_RadioMFTAgentRemoteApiEventImpl(ptr, apiLock), m_statusHelper(&m_RadioMFTAgentRemoteApiEventImpl)
{
	
}

RadioEventDispatcher::~RadioEventDispatcher()
{
	stop();

}


void RadioEventDispatcher::start()
{
    FUNCTION_ENTRY( "start" );
	WORD result = activate();
    
    if (result != 0)
    {
        throw TA_Base_Core::TransactiveException("Error, Fail to activate the message queue!");
    }

	m_deactivate = false;

    FUNCTION_EXIT;
}


void RadioEventDispatcher::stop()
{
    FUNCTION_ENTRY( "stop" );

    m_deactivate = true;

	wait();
    
    FUNCTION_EXIT;
}

int RadioEventDispatcher::svc()
{
    FUNCTION_ENTRY( "svc" );

	while ( !m_deactivate )
    {
		TA_Base_Core::Thread::sleep(100);
	}

    FUNCTION_EXIT;
	return 0;
}
void RadioEventDispatcher::processEvent(TA_IRS_App::RemoteAPIRadioEvent* event)
{
FUNCTION_ENTRY("processEvent");

	ACE_Future<FutureResultT> future;

	TA_ASSERT(event != NULL, "RemoteAPIRadioEvent is NULL");

	//Make sure that the RadioMFTState is in NORMAL State before processing the event.
	//If it is not in Normal state then ignore the message
	if (RadioMFTAgentStatusManager::NORMAL_STATE != The_StatusManager::instance()->getRadioMFTState())
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"RadioMFTState is still not yet in Normal State, message will be ignored.");
		return;
	}

    switch (event->getMessageType())
    {

	case eMsgMcdiNewDatabaseGrantedID: 
			m_RadioMFTAgentRemoteApiEventImpl.onMcdiDispatchReady();
			break;
		case eMsgMcdiNewDatabaseNotGrantedID:
            m_RadioMFTAgentRemoteApiEventImpl.onMcdiDispatchNotReady();
			break;

		case eMsgMcdiDatabaseStatusID:
		{
			McdiDatabaseStatus* statusEvent = dynamic_cast<McdiDatabaseStatus*>(event);
			
			switch( statusEvent->get_wStatus() )
			{
			case TA_IRS_App::Mcdi::DispatchStatusEvent::Dispatch_Ready:
				m_RadioMFTAgentRemoteApiEventImpl.onMccStatus(RadioMFTAgentRemoteApiEventImpl::DB_STATUS);
				break;
			case TA_IRS_App::Mcdi::DispatchStatusEvent::Dispatch_Not_Ready:
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Database is not ready yet");
				m_RadioMFTAgentRemoteApiEventImpl.onMcdiDispatchNotReady();
			}
		}
		break;
		/*case MCDI_CLIENT_REGISTRATION_STATUS:
 			break;

        case MCDI_DISPATCH_READY_MSG: 
			m_RadioMFTAgentRemoteApiEventImpl.onMcdiDispatchReady();
			break;
        case MCDI_DISPATCH_NOTREADY_MSG:
            m_RadioMFTAgentRemoteApiEventImpl.onMcdiDispatchNotReady();
			break;

        case MCDI_DISPATCH_STATUS:
		{
			TA_IRS_App::Mcdi::DispatchStatusEvent* statusEvent = dynamic_cast<TA_IRS_App::Mcdi::DispatchStatusEvent*>(event);
			
			switch( statusEvent->getStatus() )
			{
			case TA_IRS_App::Mcdi::DispatchStatusEvent::Dispatch_Ready:
				m_RadioMFTAgentEventImpl.onMcdiDispatchReady();
				break;
			case TA_IRS_App::Mcdi::DispatchStatusEvent::Dispatch_Not_Ready:
				m_RadioMFTAgentEventImpl.onMcdiDispatchNotReady();
			}
		}
		break;
*/
        case MCDI_RC_RETRIEVE_REQ_MSG:
//			future = m_pFutureManager->getFutureResult(RETRIEVE_RCDB_GRANT);
//			m_pFutureManager->detachFuture(future);
			m_RadioMFTAgentRemoteApiEventImpl.onMcdiRetrieveRCDatabaseRequest();
			break;
		case MCDI_RC_RETRIEVE_RESP_ACK_MSG:
			m_RadioMFTAgentRemoteApiEventImpl.onMcdiRetrieveRCDatabaseResponseAck();
			break;

        case MCDI_PERS_SYNC_START_MSG:
			m_RadioMFTAgentRemoteApiEventImpl.onMcdiSynchronisationStart();
			break;
        case MCDI_PERS_SYNC_COMPLETE_STS:
			m_RadioMFTAgentRemoteApiEventImpl.onMcdiSyncCompletionStatus();
			break;
        case MCDI_PERS_SYNC_COMPLETE_MSG:
			m_RadioMFTAgentRemoteApiEventImpl.onMcdiSynchronisationComplete();
			break;

		case eMsgMcdiAudioProcessingStatusID: 
			{
			McdiAudioProcessingStatus* statusEvent = dynamic_cast<McdiAudioProcessingStatus*>(event);
			if (statusEvent->get_wStatus() == TA_IRS_App::Mcdi::DispatchStatusEvent::Dispatch_Ready) {
				m_RadioMFTAgentRemoteApiEventImpl.onMccStatus(RadioMFTAgentRemoteApiEventImpl::AUDIO_STATUS);				
			} else {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Audio is not ready yet");
			}
			break;
			}
		case eMsgMcaPeripheralsStatusID:
			{
			McaPeripheralsStatus* statusEvent = dynamic_cast<McaPeripheralsStatus*>(event);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Peripheral Status : %s", 
				RadioMFTAgentHelper::convertGPIOMToString(statusEvent->get_wStatus() ).c_str() );

			//Monitor the Peripheral Status if it is GPIOM_NOT_AVAILABLE
			//if it's true, then do reconnection to MCC Server.
			if (statusEvent->get_wStatus() == TA_IRS_App::Mcdi::DispatchStatusEvent::GPIOM_NOT_VAILABLE ) 
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Peripheral Status is GPIOM_NOT_AVAILABLE, calling reconnection to MCC Server...");
				The_StatusManager::instance()->failConnectToRadio();
			}

			break;
			}
		// Resource Assignment
		case eMsgMcaAssignmentStatusID:
			if ( m_RadioMFTAgentRemoteApiEventImpl.onAssignmentStatus(event) )
			{

				future = EventToResultFactory::getResult(m_pFutureManager,event);

			    //TA_IRS_App::Mca::AssignmentStatus* pEvent = 
				   // dynamic_cast<TA_IRS_App::Mca::AssignmentStatus*>(event);

				if (future.ready())
				{
					FutureResultT result;
					future.get(result);
					if (result.m_state != 0)
					{
						m_RadioMFTAgentRemoteApiEventImpl.updateGroupResource(event, future);
					}
    				m_pFutureManager->updateFuture(future);
				}
			}
			break;		
		case eMsgMcaAssignResourceErrorID:
			m_RadioMFTAgentRemoteApiEventImpl.onAssignResourceErr(event, true);
			future = EventToResultFactory::getResult(m_pFutureManager,event);
			m_pFutureManager->detachFuture(future);
			m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("AssignResource Error");
			break;
			
		case eMsgMcaDeassignResourceErrorID:
			m_RadioMFTAgentRemoteApiEventImpl.onAssignResourceErr(event, false);
			future = EventToResultFactory::getResult(m_pFutureManager,event);
			m_pFutureManager->detachFuture(future);
			m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("DeassignResource Error");
			break;				
		
		// Select resource
		case eMsgMcaResourceSelectStatusID:
			future = EventToResultFactory::getResult(m_pFutureManager,event);

			if ((m_RadioMFTAgentRemoteApiEventImpl.onResourceSelectStatus(event)) && (future.ready()))
			{
				m_RadioMFTAgentRemoteApiEventImpl.processSelectResourceFunc(future);
			}
			
			m_pFutureManager->updateFuture(future);
			
			break;       

		case eMsgMcaSelectResourceErrorID:
			future = m_pFutureManager->getFutureResult(SELECT_RESOURCE);
			
			if (future.ready()) 
			{
				m_RadioMFTAgentRemoteApiEventImpl.onSelectResourceError(event, future);
			}
			m_pFutureManager->detachFuture(future);

			m_RadioMFTAgentRemoteApiEventImpl.onSetupCallFailure();
			m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("SelectResourceError");
			break;
		case eMsgMcaDeselectResourceErrorID:
			future = m_pFutureManager->getFutureResult(DESELECT_RESOURCE);
			if (future.ready()) {
				m_RadioMFTAgentRemoteApiEventImpl.onDeselectResourceError(event, future);
			}
			m_pFutureManager->detachFuture(future);
			m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("DeselectResourceError");
			
			break;

		case eMsgMcaFreeMselGroupStatusID:
		{
			future = m_pFutureManager->getFutureResult(GET_FREE_MSEL);
			if (future.ready())
			{
				FutureResultT result;
				future.get(result);
				m_RadioMFTAgentRemoteApiEventImpl.onGetFreeMselGroup(event, result.m_param);
				m_pFutureManager->updateFuture(future);
			}
            //The_StatusManager::instance()->clearPendingConnectResource(RR_MSELECT);
		}
		break;

		case eMsgMcaGetFreeMselGroupErrorID:
			future = m_pFutureManager->getFutureResult(GET_FREE_MSEL);
            if (future.ready())
            {
			    m_pFutureManager->detachFuture(future);
                RadioResource res = The_StatusManager::instance()->getPendingConnectResource(RR_MSELECT);
                The_StatusManager::instance()->clearPendingConnectResource(RR_MSELECT);
                The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(res, NAG);
                m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("Unable to get free msel group");
            }
			break;

		case eMsgMcaMselGroupStatusID:
			future = EventToResultFactory::getResult(m_pFutureManager,event);
			m_pFutureManager->updateFuture(future);
			m_RadioMFTAgentRemoteApiEventImpl.onMselGroupStatus(event);
			break;
		
		case eMsgMcaOpenMselErrorID:
			future = m_pFutureManager->getFutureResult(OPEN_MSEL);
			m_pFutureManager->detachFuture(future);
            m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("Unable to open msel");
			m_RadioMFTAgentRemoteApiEventImpl.onOpenMselError(event);
			break;
		case eMsgMcaCloseMselErrorID:
			future = m_pFutureManager->getFutureResult(CLOSE_MSEL);
			m_pFutureManager->detachFuture(future);
            m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("Unable to close msel");
			m_RadioMFTAgentRemoteApiEventImpl.onCloseMselError(event);
			break;

		case eMsgMcaMselMemberStatusID:
			future = EventToResultFactory::getResult(m_pFutureManager,event);
			m_pFutureManager->updateFuture(future);
            m_RadioMFTAgentRemoteApiEventImpl.onMselMemberStatus(event);
			break;
			
		case eMsgMcaAddMselMemberWarningID:
			future = EventToResultFactory::getResult(m_pFutureManager,event);
			if (future.ready())
			{
				FutureResultT result;
				future.get(result);
                m_RadioMFTAgentRemoteApiEventImpl.onAddMselMemberWrn(event, result.m_resourceId);
        		m_pFutureManager->detachFuture(future);
            }
			break;
		case eMsgMcaAddMselMemberErrorID:
			future = EventToResultFactory::getResult(m_pFutureManager,event);
			if (future.ready())
			{
				FutureResultT result;
				future.get(result);
                m_RadioMFTAgentRemoteApiEventImpl.onAddMselMemberErr(event, result.m_resourceId);
			    m_pFutureManager->detachFuture(future);
            }
			m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("AddMselMemberError");
			break;

		case eMsgMcaDeleteMselMemberErrorID:
			future = EventToResultFactory::getResult(m_pFutureManager,event);
			if (future.ready())
			{
				FutureResultT result;
				future.get(result);
                m_RadioMFTAgentRemoteApiEventImpl.onDeleteMselMemberErr(event, result.m_resourceId);
			    m_pFutureManager->detachFuture(future);
            }
			m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("DeleteMselMemberError");
			break;

		case eMsgMcaMselMemberRefreshID:
			m_RadioMFTAgentRemoteApiEventImpl.onMselMemberRefresh(event);
			break;

        // MCA_PATCH_MSG
		case eMsgMcaAddPatchMemberWarningID:
			future = EventToResultFactory::getResult(m_pFutureManager,event);
			if (future.ready())
			{
				FutureResultT result;
				future.get(result);
                m_RadioMFTAgentRemoteApiEventImpl.onAddPatchMemberWrn(event, result.m_resourceId);
			    m_pFutureManager->detachFuture(future);
            }
			break;
		case eMsgMcaAddPatchMemberErrorID:
			future = EventToResultFactory::getResult(m_pFutureManager,event);
			if (future.ready())
			{
				FutureResultT result;
				future.get(result);				
                m_RadioMFTAgentRemoteApiEventImpl.onAddPatchMemberErr(event, result.m_resourceId);
			    m_pFutureManager->detachFuture(future);
			} else {
				m_RadioMFTAgentRemoteApiEventImpl.onAddPatchMemberErr(event, 0);
			}
			break;
		case eMsgMcaDeletePatchMemberErrorID:
			future = EventToResultFactory::getResult(m_pFutureManager,event);
			if (future.ready())
			{
				FutureResultT result;
				future.get(result);
                m_RadioMFTAgentRemoteApiEventImpl.onDeletePatchMemberErr(event, result.m_resourceId);
			    m_pFutureManager->detachFuture(future);
            }
			m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("DeletePatchMemberError");
			break;

		case eMsgMcaFreePatchGroupStatusID:
		{
			future = m_pFutureManager->getFutureResult(GET_FREE_PATCHGRP);
            if (future.ready())
            {
			    m_pFutureManager->updateFuture(future);
			    FutureResultT result;
			    future.get(result);
			    m_RadioMFTAgentRemoteApiEventImpl.onGetFreePatchGroup(event, result.m_param);
            }
            //The_StatusManager::instance()->clearPendingConnectResource(RR_PATCHGROUP);
		}
		break;
		case eMsgMcaGetFreePatchGroupErrorID:
			future = m_pFutureManager->getFutureResult(GET_FREE_PATCHGRP);
            if (future.ready())
            {
				m_RadioMFTAgentRemoteApiEventImpl.onGetFreePatchGroupError();
			    m_pFutureManager->detachFuture(future);
            }
			break;
			
		case eMsgMcaPatchMemberStatusID:
			future = EventToResultFactory::getResult(m_pFutureManager,event);
			m_pFutureManager->updateFuture(future);
			m_RadioMFTAgentRemoteApiEventImpl.onPatchMemberStatus(event);
			break;
		
		case eMsgMcaPatchGroupStatusID:
            m_RadioMFTAgentRemoteApiEventImpl.onPatchGroupStatus(event);
			break;
		case eMsgMcaPatchMemberRefreshID:
            m_RadioMFTAgentRemoteApiEventImpl.onPatchMemberRefresh(event);
			break;

		// Make private call 
		case eMsgMcaPrivateCallStatusID:
			future = EventToResultFactory::getResult(m_pFutureManager,event);
			if ((m_RadioMFTAgentRemoteApiEventImpl.onPrivateCallStatus(event)) && (future.ready()))
			{
				m_RadioMFTAgentRemoteApiEventImpl.processSelectResourceFunc(future);
			}

			m_pFutureManager->updateFuture(future);
			
			break;
		case eMsgMcaResourceRejectStatusID:
			m_RadioMFTAgentRemoteApiEventImpl.onResourceRejectStatus(event);
			break;

		case eMsgMcaBeginPrivateCallErrorID:
			future = m_pFutureManager->getFutureResult(BEGIN_PRIVATE_CALL);
			m_pFutureManager->detachFuture(future);
			m_RadioMFTAgentRemoteApiEventImpl.onSetupCallFailure();
            m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("Unable to begin private call");
			break;		

		case eMsgMcaAnswerPrivateCallErrorID:
			future = m_pFutureManager->getFutureResult(ANSWER_PRIVATE_CALL);
			m_pFutureManager->detachFuture(future);
            m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("Unable to answer private call");
			break;		

		case eMsgMcaEndPrivateCallErrorID:
			future = m_pFutureManager->getFutureResult(END_PRIVATE_CALL);

			//This will check if the error type is equal MCA_RSC_TRANSMITTING.
			//If so, EndPrivateCall command will resend to MCC again.
			if (m_RadioMFTAgentRemoteApiEventImpl.onEndPrivCallErrHandler(event))
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Resending end private call to MCC");
				m_RadioMFTAgentRemoteApiEventImpl.onResendEndPrvCall();		
			}
			else
			{
				m_pFutureManager->detachFuture(future);
				m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("Unable to end private call");
			}
			break;
		
		// Ambience Listening
		case eMsgMcaAmbienceListeningCallStatusID:
			future = EventToResultFactory::getResult(m_pFutureManager,event);
			m_pFutureManager->updateFuture(future);
			m_RadioMFTAgentRemoteApiEventImpl.onAmbienceListeningCallStatus(event);			
	        break;

		case eMsgMcaBeginAmbienceListeningCallErrorID:
			future = m_pFutureManager->getFutureResult(BEGIN_AMBIENCE_LISTENING);
			m_pFutureManager->detachFuture(future);
            m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("Unable to begin ambience listening");
			break;
		case eMsgMcaEndAmbienceListeningCallErrorID:
			future = m_pFutureManager->getFutureResult(END_AMBIENCE_LISTENING);
			m_pFutureManager->detachFuture(future);
            m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("Unable to end ambience listening");
			break;

			// Emergency
		case eMsgMcaEmergencyAlarmStatusID:
			m_RadioMFTAgentRemoteApiEventImpl.onEmergencyAlarmStatus(event);
			break;

		case eMsgMcaEmergencyCallStatusID:
			future = EventToResultFactory::getResult(m_pFutureManager,event);
			m_RadioMFTAgentRemoteApiEventImpl.onEmergencyCallStatus(event);
			break;
		case eMsgMcaBeginConsoleEmergencyCallErrorID:
			future = m_pFutureManager->getFutureResult(BEGIN_EMERGENCY_CALL);
			m_pFutureManager->detachFuture(future);
            m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("Unable to begin emergency call");
			break;
		case eMsgMcaEndEmergencyErrorID:
			future = m_pFutureManager->getFutureResult(END_EMERGENCY_CALL);
			m_pFutureManager->detachFuture(future);
            m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("Unable to end private call");
			break;

		case eMsgMcaGeneralTransmitStatusID:
			future = m_pFutureManager->getFutureResult(BEGIN_GENERAL_XMIT);
            if (m_RadioMFTAgentRemoteApiEventImpl.onGeneralTransmitStatus(event, future))
			{
				m_pFutureManager->updateFuture(future);
			}
			break;
		case eMsgMcaBeginGeneralTransmitErrorID:
			future = m_pFutureManager->getFutureResult(BEGIN_GENERAL_XMIT);
            m_RadioMFTAgentRemoteApiEventImpl.onBeginGeneralXmitErr(event);
			m_pFutureManager->detachFuture(future);
			break;
		case eMsgMcaBeginGeneralTransmitWarningID:
			future = m_pFutureManager->getFutureResult(BEGIN_GENERAL_XMIT);
            m_RadioMFTAgentRemoteApiEventImpl.onBeginGeneralXmitWrn(event);
			m_pFutureManager->detachFuture(future);
			break;
		case eMsgMcaResourceTransmitStatusID:
			m_RadioMFTAgentRemoteApiEventImpl.onResourceTransmitStatus(event);
			break;

		case eMsgMcaInboundCallStatusID:
			m_RadioMFTAgentRemoteApiEventImpl.onInboundCallStatus(event);
			break;

		case eMsgMcaInboundPttIdStatusID:
			m_RadioMFTAgentRemoteApiEventImpl.onInboundPttIdStatus(event);
			break;


		case eMsgMcaInboundPrivateCallStatusID:
			m_RadioMFTAgentRemoteApiEventImpl.onInboundPrivateCallStatus(event);
			break;

		/*case eMsgMcaInboundRadioStatusID:
            m_RadioMFTAgentRemoteApiEventImpl.onInboundRadioMessageStatus(event);
			break;*/

		case eMsgMcaRefreshStatusID:
            m_RadioMFTAgentRemoteApiEventImpl.onRefreshStatus(event);
			break;

		case eMsgMcaResourceVolumeStatusID:
            m_RadioMFTAgentRemoteApiEventImpl.onResourceVolumeStatus(event);
			break;

		case eMsgMcdiNewDatabaseAvailableID:
			m_RadioMFTAgentRemoteApiEventImpl.onNewDatabaseAvailable(event);
			break;

		case eMsgMcdiDatabaseUpdateNotificationID:
			m_RadioMFTAgentRemoteApiEventImpl.onDatabaseUpdateNotification(event);
			break;
		case MCA_PHONE_ON_HOOK_ERR:
			future = m_pFutureManager->getFutureResult(END_PHONE_CALL);
			m_pFutureManager->detachFuture(future);
            m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("Unable to on-hook phone ");
			break;

		case MCA_DIAL_STRING_ERR:
			future = m_pFutureManager->getFutureResult(DIAL_PHONE);
			m_pFutureManager->detachFuture(future);
            m_RadioMFTAgentRemoteApiEventImpl.sendErrorMessage("Unable to dial phone number");
			break;
		case MCA_TONE_STATUS:
			m_RadioMFTAgentRemoteApiEventImpl.onEmergencyAlarmTone(event);
			break;
		default:

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Event discarded..." );
	}

	checkResult(future);

    FUNCTION_EXIT;	
}

void RadioEventDispatcher::checkResult(ACE_Future<FutureResultT>& future)
{

	if (!future.ready())
	{
		return;
	}
	else
	{
		FutureResultT result;
		future.get(result);


		if ( result.m_exceptionIndicator == FutureResultT::NO_STATE)
		{
			// no command initiated by this client
			return;
		}
		
		if (result.m_exceptionIndicator == FutureResultT::ABORTED)
		{
			std::ostringstream logStr;
			logStr << "Command [" << result.get() <<"] is aborted.";

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, logStr.str().c_str() );
		}

		m_pFutureManager->removeFuture(future, false);
	}
}

                                               
bool RadioEventDispatcher::addCallback(TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr pCallback)
{
	return m_RadioMFTAgentRemoteApiEventImpl.addCallback(pCallback);
}


void RadioEventDispatcher::setMFTCommand(IRadioMFTAgentCommand* pCommand)
{
	//m_RadioMFTAgentEventImpl.setMFTCommand(pCommand);
	m_RadioMFTAgentRemoteApiEventImpl.setMFTCommand(pCommand);
}
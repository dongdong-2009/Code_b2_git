/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentEventImpl.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 11:47:25 $
 * Last modified by:  $Author: builder $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "app/radio/radio_mft_agent_icdv4/src/RadioResourceConfigFacade.h"

#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentEventImpl.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentHelper.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentCallbackAdapter.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioApplicationStateManager.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentStatusManager.h"

#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/CDI_Function.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleDispatchException.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleStateManager.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioErrorMsg.h"

#include "Mca/mca.h"
#include "Mca/private_call.h"
#include "Mca/multi_select.h"
#include "Mca/patch.h"
#include "Mcdi/callback.h"
#include "Mcdi/registration.h"
#include "Mca/beep.h"
#include "Mca/tone_gen.h"

#include "bus/radio/radio_bus/src/RadioTypes.h"

#include "core/message/types/RadioAlarms_MessageTypes.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

RadioMFTAgentEventImpl::RadioMFTAgentEventImpl() : 
	m_pCommand(NULL),
	m_bOperationMode(false)
{
    FUNCTION_ENTRY("RadioMFTAgentEventImpl");

	m_serverAgent.setEntityName(RadioMFTAgentHelper::getRadioServerAgentEntityName(),true);

	m_pCallback = new RadioMFTAgentCallbackAdapter();

    The_ResourceConfig::instance()->setEventImpl(this);
    The_StatusManager::instance()->setMFTAgentCallback(m_pCallback);

    FUNCTION_EXIT;
}


RadioMFTAgentEventImpl::~RadioMFTAgentEventImpl()
{
    FUNCTION_ENTRY("~RadioMFTAgentEventImpl");

	if ( m_pCallback != NULL )
	{
		delete m_pCallback;
		m_pCallback = NULL;
	}
	
    FUNCTION_EXIT;
}


void RadioMFTAgentEventImpl::onMcdiDispatchReady()
{
    FUNCTION_ENTRY("onMcdiDispatchReady");

	m_pApStateMgr = m_pCommand->getApStateMgr();

	if (m_bOperationMode == true)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Set application state to RA_OPERATION_MODE");
		
		m_pApStateMgr->setApState(RA_OPERATION_MODE);
		m_pCallback->callback_applicationStateMessage(RA_OPERATION_MODE,0);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Set application state to RA_INITIALISATION_MODE");
		// show progress
		m_pApStateMgr->setApState(RA_INITIALISATION_MODE);
		//m_pCallback->callback_applicationStateMessage(RA_INITIALISATION_MODE,0);
		//The_ResourceConfig::instance()->startRetrieveDB();
		
	m_bOperationMode=true;
onMcdiDispatchReady();
	}

	//fixed TD14269
	//closeAlarm(TA_Base_Core::RadioAlarms::RadioConsoleAlarm);
	//fixed TD14269

    FUNCTION_EXIT;
}


void RadioMFTAgentEventImpl::onMcdiDispatchNotReady()
{
    //Report error to GUI but allow
    //initialization state to complete.

    FUNCTION_ENTRY("onMcdiDispatchNotReady");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to fully initialise the Mca API." );

	// NOTE: percent reserved for future use. At present always 0.
	m_pCallback->callback_applicationStateMessage(RA_UNKNOWN, 0); //Haipeng changed the RA_CONFIGURATION_MODE to RA_UNKNOWN
	m_pApStateMgr->setApState(RA_UNKNOWN);
	
	//fixed TD14269
	submitAlarm(TA_Base_Core::RadioAlarms::RadioConsoleAlarm);
	//fixed TD14269

    FUNCTION_EXIT;
}


void RadioMFTAgentEventImpl::onMcdiRetrieveRCDatabaseRequest()
{
    FUNCTION_ENTRY("onMcdiRetrieveRCDatabaseRequest");

	TA_IRS_App::Mcdi::RetrieveRCDatabaseGrant pFunction;
	pFunction.execute();

    FUNCTION_EXIT;
}

void RadioMFTAgentEventImpl::onMcdiRetrieveRCDatabaseResponseAck()
{
    FUNCTION_ENTRY("onMcdiRetrieveRCDatabaseResponseAck");

    // TODO:

    FUNCTION_EXIT;
}


void RadioMFTAgentEventImpl::onMcdiSynchronisationStart()
{
    //Force application state to initializing
    //Set PersROM complete percent to zero
    FUNCTION_ENTRY("onMcdiSynchronisationStart");

	m_pCallback->callback_applicationStateMessage(RA_INITIALISATION_MODE,0);

    FUNCTION_EXIT;
}

void RadioMFTAgentEventImpl::onMcdiSyncCompletionStatus()
{
	//Unused, but reserved for initialisation
    //progress indicator.

    FUNCTION_ENTRY("onMcdiSyncCompletionStatus");
    FUNCTION_EXIT;
}


void RadioMFTAgentEventImpl::onMcdiSynchronisationComplete()
{
	/*  PersROM update finished, but cannot
		change application state yet, as not
		yet finished DUI refresh.
		Unused, but reserved for initialisation
		progress indicator. */
	
    FUNCTION_ENTRY("onMcdiSynchronisationComplete");
    FUNCTION_EXIT;
}


void RadioMFTAgentEventImpl::onResourceTransmitStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
    FUNCTION_ENTRY("onResourceTransmitStatus");

	TA_IRS_App::Mca::ResourceTransmitStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::ResourceTransmitStatus*>(pConEvent);

	try
	{
		RadioResource resource = 
			The_StatusManager::instance()->getResourcebyChannel(pEvent->getChannel());
		
		IRadioMFTAgentCallback::ResourceActivityState state = convertResourceTXStatus(pEvent->getStatus());

		if ( state == IRadioMFTAgentCallback::No_Resource_Activity ||
			 state == IRadioMFTAgentCallback::Resource_Tx_Ended )
		{
			The_StatusManager::instance()->setFeatureAttributeIsActive(resource, false);
		}
		else
		{
			The_StatusManager::instance()->setFeatureAttributeIsActive(resource, true);
		}
		
		if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
		{
			m_pCallback->callback_resourceActivityMessage(resource,state);
		}
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Exception occurred onResourceTransmitStatus()");
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentEventImpl::onEmergencyAlarmStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
    FUNCTION_ENTRY("onEmergencyAlarmStatus");

	TA_IRS_App::Mca::EmergencyAlarmStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::EmergencyAlarmStatus*>(pConEvent);

    RadioMFTAgentStatusManager* lManager = The_StatusManager::instance();

	/** 
	 *	get the resource of the operator position that 
	 *	has taken responsibility for handling the emergency alarm
	 */	
	RadioResource resource = lManager->getResourcebyChannel(pEvent->getChannel());

    if (resource.type == RR_NIL)
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
			"Resource not found for channel[%d].",pEvent->getChannel());
        return;
    }

	RadioResource changedby;
	RadioResource radioUnit;
	try
	{
		// TD17224
		changedby.id = pEvent->getResourceID(); // indicates the resource id of the operator position that has taken 
												// responsibility for handling the emergency.

		// indicates the radio ID corresponding to the emergency alarm
		radioUnit = lManager->getResourceBySSI(pEvent->getRadioSSI());
		changedby.ssi = pEvent->getRadioSSI(); 
		changedby.alias = radioUnit.alias;
		changedby.type = RR_RADIO;
		// TD17224
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Undefined operation position rid[%d] for handling emergency alarm.",pEvent->getResourceID());
	}

	RadioEmergencyModeType type;

	switch(pEvent->getStatus())
	{
		case TA_IRS_App::Mca::EmergencyAlarmStatus::Alarm_Recognized:
		case TA_IRS_App::Mca::EmergencyAlarmStatus::Alarm_Refresh:
			type = RE_EMERGENCY;
			break;
		case TA_IRS_App::Mca::EmergencyAlarmStatus::Alarm_Received:
		case TA_IRS_App::Mca::EmergencyAlarmStatus::Emergency_Alarm_Refresh:
			type = RE_UNACK_EMERGENCY;
			break;
		
		default:
			type = RE_NO_EMERGENCY;
	}

	RadioEmergencyModeType prevType = lManager->getFeatureAttributeEmergencyMode(resource);

	lManager->setFeatureAttributeEmergencyMode(resource,type);

	if ((m_pApStateMgr->getApState() == RA_OPERATION_MODE ) && (prevType != type)) // TD18118
	{
		// On Emergency_Call_Ended event, the radioSSI returned by Motorola interface 
		// is 0 or some unrecognised number. We may as well get the stored EmergencyRadioUnit attribute
		if (type == RE_NO_EMERGENCY)
		{
			changedby.ssi = lManager->getFeatureAttributeEmergencyRadioUnit(resource);
			radioUnit = lManager->getResourceBySSI(changedby.ssi);
			changedby.alias = radioUnit.alias;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"[onEmergencyAlarmStatus] type=%d, radioUnit=%d",type,radioUnit.ssi);				

		lManager->setFeatureAttributeEmergencyMode(radioUnit,type);

		lManager->setFeatureAttributeEmergencyRadioUnit(resource,changedby.ssi);
		
		m_pCallback->callback_resourceEmergencyModeMessage(resource,type,changedby);
		if ((type == RE_UNACK_EMERGENCY) && (!lManager->isAudibleAlarmOn()))
		{
			try
			{
				TA_IRS_App::Mca::BeginTone pBeginTone(MCA_EMERGENCY_ALARM_TONE,TA_IRS_App::TheConsoleStateManager::instance()->getClientID());			
				pBeginTone.execute();
			}
			catch(TA_IRS_App::ConsoleDispatchException& e)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
			}
		}
		else if (lManager->isAudibleAlarmOn())
		{
			try
			{
				TA_IRS_App::Mca::EndTone pEndTone;			
				pEndTone.execute();
			}
			catch(TA_IRS_App::ConsoleDispatchException& e)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
			}			
		}	
	}
	
	
    FUNCTION_EXIT;
}

void RadioMFTAgentEventImpl::onEmergencyCallStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{

    FUNCTION_ENTRY("onEmergencyCallStatus");

	TA_IRS_App::Mca::EmergencyCallStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::EmergencyCallStatus*>(pConEvent);

    RadioMFTAgentStatusManager* lManager = The_StatusManager::instance();

	RadioResource resource = 
		lManager->getResourcebyChannel(pEvent->getChannel());

    if (resource.type == RR_NIL)
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
			"Resource not found for channel[%d].",pEvent->getChannel());
        return;
    }

	RadioResource changedBy;
	RadioResource radioUnit;
	try
	{
		// TD17224
		changedBy.id = pEvent->getOperatorRID(); // indicates the resource id of the operator position that has taken 
												// responsibility for handling the emergency.

		// indicates the radio ID corresponding to the emergency alarm
		RadioResource radioUnit = lManager->getResourceBySSI(pEvent->getRadioSSI());
		changedBy.ssi = pEvent->getRadioSSI(); 
		changedBy.alias = radioUnit.alias;
		changedBy.type = RR_RADIO;
		// TD17224
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Undefined operation position rid[%d] for handling emergency call.",pEvent->getOperatorRID());
	}
	
	RadioEmergencyModeType type = RE_NO_EMERGENCY;
	
	switch(pEvent->getStatus())
	{
		case TA_IRS_App::Mca::EmergencyCallStatus::Emergency_Call_State:
		case TA_IRS_App::Mca::EmergencyCallStatus::Unreg_Emergency_Call_Refresh:		
			type = RE_UNACK_EMERGENCY;
			break;
		case TA_IRS_App::Mca::EmergencyCallStatus::Reg_Emergency_Call_Refresh:		
			type = RE_EMERGENCY;
			break;
		case TA_IRS_App::Mca::EmergencyCallStatus::Emergency_Call_Ended:		
			type = RE_NO_EMERGENCY;
	}

	RadioEmergencyModeType prevType = lManager->getFeatureAttributeEmergencyMode(resource);
	
	lManager->setFeatureAttributeEmergencyMode(resource,type);

	if ((m_pApStateMgr->getApState() == RA_OPERATION_MODE ) && (prevType != type)) // TD18118
	{
		// On Emergency_Call_Ended event, the radioSSI returned by Motorola interface 
		// is 0 or some unrecognised number. We may as well get the stored EmergencyRadioUnit attribute
		if (type == RE_NO_EMERGENCY)
		{
			changedBy.ssi = lManager->getFeatureAttributeEmergencyRadioUnit(resource);
			radioUnit = lManager->getResourceBySSI(changedBy.ssi);
			changedBy.alias = radioUnit.alias;
		}
		
		lManager->setFeatureAttributeEmergencyMode(radioUnit,type);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"[onEmergencyCallStatus] type=%d, radioUnit=%d",type,radioUnit.ssi);				
		
		lManager->setFeatureAttributeEmergencyRadioUnit(resource,changedBy.ssi);

		m_pCallback->callback_resourceEmergencyModeMessage(resource,type,changedBy);

		if ((type == RE_UNACK_EMERGENCY) && (!lManager->isAudibleAlarmOn()))
		{
			try
			{
				TA_IRS_App::Mca::BeginTone pBeginTone(MCA_EMERGENCY_ALARM_TONE,TA_IRS_App::TheConsoleStateManager::instance()->getClientID());			
				pBeginTone.execute();
			}
			catch(TA_IRS_App::ConsoleDispatchException& e)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
			}
		}
		else if (lManager->isAudibleAlarmOn())
		{
			try
			{
				TA_IRS_App::Mca::EndTone pEndTone;			
				pEndTone.execute();
			}
			catch(TA_IRS_App::ConsoleDispatchException& e)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
			}			
		}
	}
	
    FUNCTION_EXIT;
}


bool RadioMFTAgentEventImpl::onGeneralTransmitStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent, ACE_Future<FutureResultT>& future)
{
    FUNCTION_ENTRY("onGeneralTransmitStatus");

	TA_IRS_App::Mca::GeneralTransmitStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::GeneralTransmitStatus*>(pConEvent);
	
	if (pEvent->getStatus()==TA_IRS_App::Mca::GeneralTransmitStatus::General_Transmit_Begun)
	{
		if (future.ready())
		{
			m_pCommand->generalTransmit(IRadioMFTAgentCommand::END);
			return true;
		}
	}
	else if (pEvent->getStatus() == TA_IRS_App::Mca::GeneralTransmitStatus::General_Transmit_Ended)
	{

        RadioMFTAgentStatusManager* lManager = The_StatusManager::instance();

		// release MSel group if open
		ResourceGroupCollection mselGrp = lManager->getMselGroupCollection();

		ResourceGroupCollection::iterator iter = mselGrp.begin();
		
		for ( ; iter != mselGrp.end(); ++iter)
		{
			RadioResource resource = lManager->getTransactiveResourceByID((*iter).first);
            if (resource.type != RR_NIL)
            {
			    try
			    {

				    if (resource.type==RR_MSELECT)
				    {
					    if (lManager->getFeatureAttributeIsGrpConnected(resource) == CONNECTED)
					    {
						    m_pCommand->releaseMselGroup(resource, false);

                            // AZ: temporary fix so it doesn't call this multiple times
    					    lManager->setFeatureAttributeIsGrpConnected(resource, RELEASED);
					    }
				    }
				    else if (resource.type==RR_MSITE)
				    {
					    if (lManager->getFeatureAttributeIsGrpConnected(resource) == CONNECTED)
					    {
						    m_pCommand->releaseMselSite(resource);

                            // AZ: temporary fix so it doesn't call this multiple times
    					    lManager->setFeatureAttributeIsGrpConnected(resource, RELEASED);
					    }
				    }
			    }
			    catch(...)
			    {
				    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					    "Excecption caught onGeneralTransmitStatus()" );				
			    }
            }
		}

		std::vector<RadioResource> sites = 
			lManager->getMFTResourceCollection(RR_SITE);

		std::vector<RadioResource>::iterator it = sites.begin();
		for(; it != sites.end(); ++it)
		{
			if ( lManager->getFeatureAttributeIsSelected(*it) )
				m_pCommand->releaseSite(*it);
		}
	}

    FUNCTION_EXIT;
	return false;
}

void RadioMFTAgentEventImpl::onInboundCallStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
    FUNCTION_ENTRY("onInboundCallStatus");

	TA_IRS_App::Mca::InboundCallStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::InboundCallStatus*>(pConEvent);

	try
	{
		RadioResource resource = 
			The_StatusManager::instance()->getResourcebyChannel(pEvent->getChannel());


		PrivateCallState state = PC_NOT_INVOLVED;

		if ( pEvent->getStatus() != TA_IRS_App::Mca::InboundCallStatus::Inbound_Call_Ended)
		{
			state = PC_INCOMING;
		}
		else
		{
			The_StatusManager::instance()->setFeatureAttributeIsActive(resource,false);
		}

		
		The_StatusManager::instance()->setFeatureAttributePtCallState(resource,state);

		if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
		{
			m_pCallback->callback_inboundCallMessage(resource,state);
		}
	}
	catch(...)
	{
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentEventImpl::onInboundPttIdStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
    FUNCTION_ENTRY("onInboundPttIdStatus");

	TA_IRS_App::Mca::InboundPttIdStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::InboundPttIdStatus*>(pConEvent);

	try
	{
		RadioResource src =
			The_StatusManager::instance()->getRadioResourceBySSI(pEvent->getUnitID());
		
		if (src.type == RR_NIL)
			src.ssi = pEvent->getUnitID();

		RadioResource listener = 
			The_StatusManager::instance()->getResourcebyChannel(pEvent->getChannel());

		The_StatusManager::instance()->setFeatureAttributeIsActive(listener,true);

		if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
		{
			m_pCallback->callback_resourcePTTMessage(listener,src,true); 
		}
	}
	catch(...)
	{
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentEventImpl::onInboundPrivateCallStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
    FUNCTION_ENTRY("onInboundPrivateCallStatus");

	TA_IRS_App::Mca::InboundPrivateCallStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::InboundPrivateCallStatus*>(pConEvent);

	RadioResource caller = 
		The_StatusManager::instance()->getRadioResourceBySSI(pEvent->getRadioSSI());

	if (MCA_INBOUND_PVT_CALL == pEvent->getStatus())
	{
		The_StatusManager::instance()->setFeatureAttributePtCallState(caller,PC_INCOMING);

		if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
		{
			try
			{
				TA_IRS_App::Mca::SoundBeeps pSoundBeep(MAX_BEEPS,3);			
				pSoundBeep.execute();
			}
			catch(TA_IRS_App::ConsoleDispatchException& e)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
			}
			m_pCallback->callback_privateCallMessage(caller,PC_INCOMING);
		}
	}
	
    FUNCTION_EXIT;
}



void RadioMFTAgentEventImpl::onInboundRadioMessageStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
    FUNCTION_ENTRY("onInboundRadioMessageStatus");

	TA_IRS_App::Mca::InboundRadioMessageStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::InboundRadioMessageStatus*>(pConEvent);

	try
	{
		bool status;
		CORBA_CALL_RETURN(status,m_serverAgent,_non_existent,());
	    if( status )
	    {
		    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			    "RadioServerAgent object reference is non-existent." );

		    // Should reconnect to serverAgent??
		    return;
	    }
			CORBA_CALL(m_serverAgent,radioStatusMsg,(pEvent->getUnitId(),pEvent->getMessage()));
			
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,  SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );

	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Exception occurred in radioStatusMsg().");
	}


    FUNCTION_EXIT;
}


void RadioMFTAgentEventImpl::onRefreshStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
    FUNCTION_ENTRY("onRefreshStatus");

	TA_IRS_App::Mca::RefreshStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::RefreshStatus*>(pConEvent);

	if (pEvent->getStatus()==TA_IRS_App::Mca::RefreshStatus::Refresh_Ended)
	{
		// get data from MCDB, and wait for the callback
		//The_ResourceConfig::instance()->startRetrieveDB();
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentEventImpl::onResourceVolumeStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{

    FUNCTION_ENTRY("onResourceVolumeStatus");
	
	TA_IRS_App::Mca::ResourceVolumeStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::ResourceVolumeStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "ResourceVolumeStatus is NULL");

	try
	{
		RadioResource resource = 
			The_StatusManager::instance()->getResourcebyChannel(pEvent->getChannel());

		AssociatedSpeaker speaker = NIL;

		The_StatusManager::instance()->setFeatureAttributeSpeakerVolume(resource,pEvent->getVolume());

		// Reason for volume change - not in volume.h
	/*	switch(pEvent->getStatus())
		{
			case MCA_RSC_SELECT_VOLUME_CHANGE:
				speaker = SELECTED_SPEAKER;
				
				break;
			case MCA_RSC_DESELECT_VOLUME_CHANGE:
				speaker = UNSELECTED_SPEAKER;
				The_StatusManager::instance()->setFeatureAttributeUnSpeakerVolume(resource,pEvent->getVolume());
				break;
			case MCA_VOLUME_REFRESH:
			case MCA_USER_VOLUME_CHANGE
		}*/

		if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
		{
			m_pCallback->callback_resourceVolumeMessage(resource,speaker,pEvent->getVolume());
		}	
	}
	catch(...)
	{
	}
	
    FUNCTION_EXIT;
}


bool RadioMFTAgentEventImpl::onResourceSelectStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
    FUNCTION_ENTRY("onResourceSelectStatus");

	TA_IRS_App::Mca::ResourceSelectStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::ResourceSelectStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "ResourceSelectStatus is NULL");


	RadioResource resource;
	try
	{
		resource = The_StatusManager::instance()->getResourcebyChannel(pEvent->getChannel());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Getting resource id %u from channel %u",resource.id,pEvent->getChannel());
	}
	catch(...)
	{
	}

	if (resource.id==0)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
			"No resource found for ResourceSelectStatus with channel[%d]",pEvent->getChannel());
		return false;
	}
	else if (resource.id == The_StatusManager::instance()->getPrivateCallResource().id) // Is a Private Call resource
	{
		return true;
	}

	AssociatedSpeaker state = NIL;

	switch(pEvent->getStatus())
	{
		case TA_IRS_App::Mca::ResourceSelectStatus::Resource_Selected:
			state = SELECTED_SPEAKER;
			break;
		case TA_IRS_App::Mca::ResourceSelectStatus::Resource_Deselected:
			state = UNSELECTED_SPEAKER;
	}

	The_StatusManager::instance()->setFeatureAttributeIsSelected(resource,state);

	if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
	{
		m_pCallback->callback_resourceSelectMessage(resource,state);
	}	

    FUNCTION_EXIT;
	return false;
}


bool RadioMFTAgentEventImpl::onAssignmentStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
    FUNCTION_ENTRY("onAssignmentStatus");

	bool bRequestByMe = true;

	TA_IRS_App::Mca::AssignmentStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::AssignmentStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "AssignmentStatus is NULL");

	if ( (pEvent->getClientId() == MCDI_NULL_CLIENT_ID) || (pEvent->getClientId()!=TA_IRS_App::TheConsoleStateManager::instance()->getClientID()))
	{
		// Assignment or Deassignment part of refresh or requested by other console
		bRequestByMe = false;
	}

    RadioMFTAgentStatusManager* lManager = The_StatusManager::instance();

	bool bIsAssigned = false;
	bool bIsProfile = false;
	unsigned int offset = 0;
    bool bIsMonitored = false;

	RadioResource resource;
	if (pEvent->getStatus()==TA_IRS_App::Mca::AssignmentStatus::Resource_Assigned)
	{
		bIsAssigned = true;
		
		if ((pEvent->getClientId() == MCDI_NULL_CLIENT_ID) &&
			(m_pApStateMgr->getApState() == RA_INITIALISATION_MODE))// on refresh
		{
			resource.id = pEvent->getResourceId();
			resource.type = RR_RADIO;  // Temporary type assigned, will be reassigned onMCDBRefreshComplete()
			lManager->addChannel(pEvent->getResourceId(),pEvent->getChannel(),true);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Adding Channel %u for radio %u",pEvent->getChannel(),pEvent->getResourceId());
		}
		else
		{
			try
			{
				resource = lManager->getConsoleResourceByID(pEvent->getResourceId());

                if (resource.type != RR_NIL)
                {
				    bIsMonitored = lManager->getFeatureAttributeButton(resource, bIsProfile, offset);

                }
                // always add the channel
				lManager->addChannel(pEvent->getResourceId(),pEvent->getChannel());
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Exception occurred onAssignmentStatus.");
			}
		}

        if (resource.type!=RR_PRIVATECALL)
        {
    		lManager->setFeatureAttributeIsAssigned(resource,bIsAssigned);
        }
	}
	else if (pEvent->getStatus()==TA_IRS_App::Mca::AssignmentStatus::Resource_Deassigned)
	{
//		if (bRequestByMe)
		{
			resource = lManager->getResourcebyChannel(pEvent->getChannel());

			lManager->deleteChannel(resource.id); //lichao ++

            if (resource.type != RR_NIL)
            {

			    lManager->setFeatureAttributeIsAssigned(resource,false);
			    lManager->setFeatureAttributeButton(resource,0, 0);
            }
		}
	}
	else if (pEvent->getStatus()==TA_IRS_App::Mca::AssignmentStatus::All_Resource_Deassigned)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"No valid resources are presently assigned to the console");
	}

	if ((m_pApStateMgr->getApState() == RA_OPERATION_MODE) && (resource.type!=RR_PRIVATECALL))
	{
		m_pCallback->callback_resourceAssignmentMessage(resource,bIsProfile,offset,bIsAssigned);
	}	

    FUNCTION_EXIT;
	return true; //bRequestByMe;
}


void RadioMFTAgentEventImpl::onAssignResourceErr(TA_IRS_App::ConsoleDispatchEvent* pConEvent, bool bAssign)
{
    FUNCTION_ENTRY("onAssignmentStatusErr");
    
	if ( bAssign )
	{
		TA_IRS_App::Mca::AssignResourceError* pEvent = 
			dynamic_cast<TA_IRS_App::Mca::AssignResourceError*>(pConEvent);

		TA_ASSERT( pEvent != NULL, "AssignResourceError is NULL");


		std::ostringstream logErr;
		logErr << "Error [" << pEvent->getError() <<"] occurred on RID=" << pEvent->getResourceId() << 
			" on Channel="<< pEvent->getChannel()<<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, logErr.str().c_str() );
	}
	else
	{
		TA_IRS_App::Mca::DeassignResourceError* pEvent = 
			dynamic_cast<TA_IRS_App::Mca::DeassignResourceError*>(pConEvent);

		TA_ASSERT( pEvent != NULL, "DeassignResourceError is NULL");

		std::ostringstream logErr;
		logErr << "Error [" << pEvent->getError() <<"] occurred on Channel="<< pEvent->getChannel()<<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, logErr.str().c_str() );		
	}
	
	FUNCTION_EXIT;
}


bool RadioMFTAgentEventImpl::onPrivateCallStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
    FUNCTION_ENTRY("onPrivateCallStatus");

	TA_IRS_App::Mca::PrivateCallStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::PrivateCallStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "PrivateCallStatus is NULL");

	bool isEstablished = false;

	RadioResource resource = 
		The_StatusManager::instance()->getRadioResourceBySSI(pEvent->getRadioSSI());

	PrivateCallState status;

	switch(pEvent->getStatus())
	{
		case TA_IRS_App::Mca::PrivateCallStatus::Private_Call_Target_Ringing:
			case TA_IRS_App::Mca::PrivateCallStatus::Private_Call_Attempt_To_Send:
			status = PC_OUTGOING;
			break;
		case TA_IRS_App::Mca::PrivateCallStatus::Private_Call_Begun:
			status = PC_ESTABLISHED;
			isEstablished = true;
			break;
		case TA_IRS_App::Mca::PrivateCallStatus::Private_Call_Ended:
			status = PC_NOT_INVOLVED;

			//	release Patch Group if this resource is involved in DLTS/EPAX patching
			if (The_StatusManager::instance()->isTempPatchInvolved(resource))
			{
				RadioResource pg = The_StatusManager::instance()->getTempPatchGroup();
				m_pCommand->releasePatchGroup(pg);

				RadioResource dummy;
				The_StatusManager::instance()->setTempPatchGroup(dummy);

				The_StatusManager::instance()->delResource(pg);
			}
			break;
			
		default:
			status = PC_NOT_INVOLVED;
	}

	The_StatusManager::instance()->setFeatureAttributePtCallState(resource,status);
	
	if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
	{
		m_pCallback->callback_privateCallMessage(resource,status);
	}	


    FUNCTION_EXIT;
	return isEstablished;
}

void RadioMFTAgentEventImpl::onAmbienceListeningCallStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
    FUNCTION_ENTRY("onAmbienceListeningCallStatus");


	TA_IRS_App::Mca::AmbienceListeningCallStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::AmbienceListeningCallStatus*>(pConEvent);
	TA_ASSERT( pEvent != NULL, "AmbienceListeningCallStatus is NULL");

	RadioResource resource = 
		The_StatusManager::instance()->getRadioResourceBySSI(pEvent->getRadioSSI());

	if (resource.type == RR_NIL)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
			"No resource found for AmbienceListeningCallStatus on SSI[%d]",pEvent->getRadioSSI());
		return;
	}

	AmbienceListeningCallState status = ALC_NOT_INVOLVED;

	if (pEvent->getStatus()==TA_IRS_App::Mca::AmbienceListeningCallStatus::Ambience_Listening_Call_Begun)
	{
		status = ALC_ESTABLISHED;
	}
	else if (pEvent->getStatus()==TA_IRS_App::Mca::AmbienceListeningCallStatus::Ambience_Listening_Call_Ended)
	{
		status = ALC_NOT_INVOLVED;
	}
	else
	{
		return; // for AttemptingToSend & Target_Ringing state,
				// no need to update GUI Manager & StatusManager
	}

	The_StatusManager::instance()->setFeatureAttributeALCallState(resource,status);

	if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
	{
		m_pCallback->callback_ambienceListeningMessage(resource,status);
	}
	
    FUNCTION_EXIT;
}

void RadioMFTAgentEventImpl::onGetFreePatchGroup(TA_IRS_App::ConsoleDispatchEvent* pConEvent, ResourceIdentifier id)
{
    FUNCTION_ENTRY("onGetFreePatchGroup");
    
	TA_IRS_App::Mca::FreePatchGroupStatus * pEvent = 
				dynamic_cast<TA_IRS_App::Mca::FreePatchGroupStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "FreePatchGroupStatus is NULL");

	if ( pEvent->getPatchGroup() == NULL_PATCH_GROUP)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"No free patch groups are available.");
	}
	else
	{
		RadioResource resource = The_StatusManager::instance()->getTransactiveResourceByID(id);
        if (resource.type != RR_NIL)
        {
		    try
		    {

			    The_StatusManager::instance()->addPatchGroup(pEvent->getPatchGroup(),id);
			    The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,CONNECTED);

			    m_pCommand->connectPatchGroup(resource,IRadioMFTAgentCommand::ACTION,pEvent->getPatchGroup());
		    }
		    catch(...)
		    {
			    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
				    "Cannot connect Patch group with resource [%d]",id);
		    }
        }
	}

	FUNCTION_EXIT;
}


void RadioMFTAgentEventImpl::onPatchGroupStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
	//Reconstruct state for existing Patch
	//Group

    FUNCTION_ENTRY("onPatchGroupStatus");

	TA_IRS_App::Mca::PatchGroupStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::PatchGroupStatus*>(pConEvent);

	if ( m_pApStateMgr->getApState() == RA_INITIALISATION_MODE )
	{
		/* AZ: Don't know why we need this??
		RadioResource resource = The_StatusManager::instance()->getResourceByGroupNo(pEvent->getPatchGroup(),RR_PATCHGROUP);

		switch(pEvent->getStatus())	
		{
		case TA_IRS_App::Mca::PatchGroupStatus::Patch_Group_Empty:
			The_StatusManager::instance()->delPatchGroup(resource.id);
			The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,RELEASED);
			break;
		case TA_IRS_App::Mca::PatchGroupStatus::Patch_Group_Active:
			The_StatusManager::instance()->setFeatureAttributeIsActive(resource,true);
			break;
		case TA_IRS_App::Mca::PatchGroupStatus::Patch_Group_Inactive:
			The_StatusManager::instance()->setFeatureAttributeIsActive(resource,false);
			break;
		}
		*/
	}
	else
	{

	}

    FUNCTION_EXIT;
}

void RadioMFTAgentEventImpl::onPatchMemberStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
    FUNCTION_ENTRY("onPatchMemberStatus");

	TA_IRS_App::Mca::PatchMemberStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::PatchMemberStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "PatchMemberStatus is NULL");

	RadioResource member = The_StatusManager::instance()->getResourcebyChannel(pEvent->getChannel());

	if ( m_pApStateMgr->getApState() == RA_INITIALISATION_MODE )
	{
		switch (pEvent->getStatus())
		{
		case TA_IRS_App::Mca::PatchMemberStatus::Patch_Member_Added:
			The_StatusManager::instance()->addPatchMember(pEvent->getPatchGroup(),member);
			break;
		case TA_IRS_App::Mca::PatchMemberStatus::Patch_Member_Deleted:
			The_StatusManager::instance()->delPatchMember(pEvent->getPatchGroup(),member);
			break;
		case TA_IRS_App::Mca::PatchMemberStatus::Patch_Member_In_Used:
			The_StatusManager::instance()->delPatchMember(pEvent->getPatchGroup(),member);
			break;
			
		}
	}
	else 
	{
		if (pEvent->getStatus()==TA_IRS_App::Mca::PatchMemberStatus::Patch_Member_Deleted)
		{
			if (member.type == RR_BIM)
			{
				m_pCommand->deassignResource(member, pEvent->getChannel());
			}
			else if ((member.type == RR_TALKGROUP) && (!The_StatusManager::instance()->getFeatureAttributeIsMonitored(member)))
			{
				m_pCommand->deassignResource(member, pEvent->getChannel());	
			}
		}
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentEventImpl::onPatchMemberRefresh(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
    FUNCTION_ENTRY("onPatchMemberRefresh");

	TA_IRS_App::Mca::PatchMemberRefresh* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::PatchMemberRefresh*>(pConEvent);	

	TA_ASSERT( pEvent != NULL, "PatchMemberRefresh is NULL");


	std::vector<BYTE> lcidStatus = pEvent->getChannelList();

	LogicalChannel channel = pEvent->getStartChannel();
	
	if ( pEvent->getPatchGroup() == MCA_OTHER_OP_PATCH_GROUP)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"PatchGroup is a is a member of a patch group in another console.");
		return;
	}
	else if (pEvent->getStatus() == MCA_GROUP_IS_EMPTY)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"PatchGroup is empty.");

		return;
	}

	std::vector<BYTE>::iterator iter = lcidStatus.begin();

	for ( ; iter!=lcidStatus.end(); ++iter)
	{
		for (int i =7; i>=0; i--)
		{
			RadioResource member = The_StatusManager::instance()->getResourcebyChannel(channel);

			bool state = (*iter >> i) & 1;

			if (state)
			{
				try
				{
					TA_IRS_App::Mca::DeletePatchMember pDelete(channel);			
					pDelete.execute();
				}
				catch(TA_IRS_App::ConsoleDispatchException& e)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
				}
			}
			channel++;
		}
	}
	
    FUNCTION_EXIT;
}


void RadioMFTAgentEventImpl::onGetFreeMselGroup(TA_IRS_App::ConsoleDispatchEvent* pConEvent, ResourceIdentifier id)
{
    FUNCTION_ENTRY("onGetFreeMselGroup");
    
	TA_IRS_App::Mca::FreeMselGroupStatus * pEvent = 
		dynamic_cast<TA_IRS_App::Mca::FreeMselGroupStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "FreeMselGroupStatus is NULL");

	if ( pEvent->getMselGroup() == NULL_MSEL_GROUP )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No free MSel groups are available");
		return;
	}
	else
	{
		RadioResource resource = The_StatusManager::instance()->getTransactiveResourceByID(id);
        if (resource.type != RR_NIL)
        {
		    try
		    {
			    The_StatusManager::instance()->addMselGroup(pEvent->getMselGroup(),id);
			    The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,CONNECTED);
				  
				// open the multiselect group, then add the member
			    //m_pCommand->connectMselGroup(resource,IRadioMFTAgentCommand::ACTION,pEvent->getMselGroup());		
				m_pCommand->connectMselGroup(resource, IRadioMFTAgentCommand::OPEN, pEvent->getMselGroup());
		    }
		    catch(...)
		    {
			    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
				    "Cannot connect Msel group with resource [%d]",id);
		    }
        }
	}

	FUNCTION_EXIT;
}

void RadioMFTAgentEventImpl::onMselGroupStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
    FUNCTION_ENTRY("onMselGroupStatus");

	TA_IRS_App::Mca::MselGroupStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::MselGroupStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "MselGroupStatus is NULL");

	RadioResource resource = The_StatusManager::instance()->getResourceByGroupNo(pEvent->getMselGroup(), RR_MSELECT);

	if (resource.type != RR_NIL)
	{
		if ( pEvent->getStatus() == TA_IRS_App::Mca::MselGroupStatus::Multi_Select_Group_Opened)
		{
			// after multiselect group is opened, now add the member
			The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,CONNECTED);
		}    
		else if (pEvent->getStatus() == TA_IRS_App::Mca::MselGroupStatus::Multi_Select_Group_Closed)
		{
			The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,RELEASED);
	
			// send Unselect notification to RadioManager for MultiSelect/MultiSite resource 
			// to depress SelectSpeaker button
			m_pCallback->callback_resourceSelectMessage(resource,UNSELECTED_SPEAKER);
		}
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentEventImpl::onMselMemberStatus(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
    FUNCTION_ENTRY("onMselMemberStatus");

	/*
	 * Not directly used, IDL method manages it
	 */
    FUNCTION_EXIT;
}


void RadioMFTAgentEventImpl::onMselMemberRefresh(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
    FUNCTION_ENTRY("onMselMemberRefresh");

	TA_IRS_App::Mca::MselMemberRefresh* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::MselMemberRefresh*>(pConEvent);	

	TA_ASSERT( pEvent != NULL, "MselMemberRefresh is NULL");


	std::vector<BYTE> lcidStatus = pEvent->getChannelList();

	LogicalChannel channel = pEvent->getStartChannel();
	
	if ( pEvent->getStatus() == MCA_GROUP_IS_EMPTY)
	{
		std::ostringstream logStr;
		logStr << "MselGroup[" << pEvent->getMselGroup() << 
			"] is a member of an msel group in another console." <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logStr.str().c_str());
		return;
	}


	std::vector<BYTE>::iterator iter = lcidStatus.begin();

	for ( ; iter!=lcidStatus.end(); ++iter)
	{
		for (int i =7; i>=0; i--)
		{
			RadioResource member = The_StatusManager::instance()->getResourcebyChannel(channel);

			bool state = (*iter >> i) & 1;

			if (state)
			{
				try
				{
					TA_IRS_App::Mca::DeleteMultiselectMember pCommand(channel,pEvent->getMselGroup());			
					pCommand.execute();
				}
				catch(TA_IRS_App::ConsoleDispatchException& e)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
				}
			}
			channel++;
		}
	}	

	// close MselGroup
	try
	{
		TA_IRS_App::Mca::CloseMultiselect pFunction(pEvent->getMselGroup());
		pFunction.execute();
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentEventImpl::onMcdbRefreshComplete()
{
    // continue with reading transactive data, and synching with the MCDB

    The_StatusManager::instance()->initDataFromDB();

	m_pApStateMgr->setApState(RA_OPERATION_MODE);
	
	try
	{
        RadioResource res = The_StatusManager::instance()->getPrivateCallResource();
        if (res.type != RR_NIL)
        {
    		m_pCommand->assignResource(res);
        }

/*        res = The_StatusManager::instance()->getBIMResource();
        if (res.type != RR_NIL)
        {
    		m_pCommand->assignResource(res);
        }*/

		/*res = The_StatusManager::instance()->getPhoneResource();
        if (res.type != RR_NIL)
        {
    		m_pCommand->assignResource(res);
        }*/
	}
	catch(const TA_IRS_App::ConsoleDispatchException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
	}
	catch(...)
	{
	}

	m_bOperationMode = true;
	
    // finished, we are now in operation mode
	m_pCallback->callback_applicationStateMessage(RA_OPERATION_MODE,0);

}



// ++TD14322
void RadioMFTAgentEventImpl::onAddMselMemberWrn(TA_IRS_App::ConsoleDispatchEvent* pConEvent, ResourceIdentifier id)
{
	TA_IRS_App::Mca::AddMselMemberWarning* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::AddMselMemberWarning*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to add msel member " << res.alias;

    sendErrorMessage(ostr.str());
}

void RadioMFTAgentEventImpl::onAddMselMemberErr(TA_IRS_App::ConsoleDispatchEvent* pConEvent, ResourceIdentifier id)
{
	TA_IRS_App::Mca::AddMselMemberError* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::AddMselMemberError*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to add msel member " << res.alias;

    sendErrorMessage(ostr.str());
}

void RadioMFTAgentEventImpl::onDeleteMselMemberErr(TA_IRS_App::ConsoleDispatchEvent* pConEvent, ResourceIdentifier id)
{
	TA_IRS_App::Mca::DeleteMselMemberError* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::DeleteMselMemberError*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to delete msel member " << res.alias;

    sendErrorMessage(ostr.str());
}

void RadioMFTAgentEventImpl::onAddPatchMemberWrn(TA_IRS_App::ConsoleDispatchEvent* pConEvent, ResourceIdentifier id)
{
	TA_IRS_App::Mca::AddPatchMemberWarning* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::AddPatchMemberWarning*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to add patch member " << res.alias;

    sendErrorMessage(ostr.str());
}

void RadioMFTAgentEventImpl::onAddPatchMemberErr(TA_IRS_App::ConsoleDispatchEvent* pConEvent, ResourceIdentifier id)
{
	TA_IRS_App::Mca::AddPatchMemberError* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::AddPatchMemberError*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to add patch member " << res.alias;

    sendErrorMessage(ostr.str());
}

void RadioMFTAgentEventImpl::onDeletePatchMemberErr(TA_IRS_App::ConsoleDispatchEvent* pConEvent, ResourceIdentifier id)
{
	TA_IRS_App::Mca::DeletePatchMemberError* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::DeletePatchMemberError*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to delete patch member " << res.alias;

    sendErrorMessage(ostr.str());
}

void RadioMFTAgentEventImpl::onBeginGeneralXmitWrn(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
	TA_IRS_App::Mca::BeginGeneralTransmitWarning* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::BeginGeneralTransmitWarning*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    sendErrorMessage("Warning beginning general transmit");
}

void RadioMFTAgentEventImpl::onBeginGeneralXmitErr(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
	TA_IRS_App::Mca::BeginGeneralTransmitError* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::BeginGeneralTransmitError*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    sendErrorMessage("Error beginning general transmit");
}

void RadioMFTAgentEventImpl::sendErrorMessage(const std::string msg)
{
    m_pCallback->callback_applicationErrorMessage(msg);
}

// TD14322++

IRadioMFTAgentCallback::ResourceActivityState RadioMFTAgentEventImpl::convertResourceTXStatus(
	TA_IRS_App::Mca::ResourceTransmitStatus::RegT status)
{
    FUNCTION_ENTRY("convertResourceTXStatus");

	IRadioMFTAgentCallback::ResourceActivityState state = 
			IRadioMFTAgentCallback::No_Resource_Activity;

	
	switch( status )
	{
		case TA_IRS_App::Mca::ResourceTransmitStatus::Resource_Transmit_Begun:
			state = IRadioMFTAgentCallback::Resource_Tx_Begun;
			break;
		case TA_IRS_App::Mca::ResourceTransmitStatus::Resource_Transmit_Busy:		
		case TA_IRS_App::Mca::ResourceTransmitStatus::Resource_Transmit_Callback:		
		case TA_IRS_App::Mca::ResourceTransmitStatus::Resource_Transmit_Without_Radio:		
			state = IRadioMFTAgentCallback::Resource_Tx_Busy;
			break;
		case TA_IRS_App::Mca::ResourceTransmitStatus::Resource_Transmit_Idle:		
			state = IRadioMFTAgentCallback::Resource_Tx_Ended;
			break;
	}

    FUNCTION_EXIT;

	return state;
}



bool RadioMFTAgentEventImpl::addCallback(TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr pCallback)
{
    FUNCTION_ENTRY("addCallback");
    
	RadioMFTAgentCallbackAdapter* pCallbackAdapter = 
			dynamic_cast<RadioMFTAgentCallbackAdapter*>(m_pCallback);

	TA_ASSERT(pCallbackAdapter!= NULL, "pCallbackAdapter is NULL.");

	FUNCTION_EXIT;
	return pCallbackAdapter->addCallbackCollection(pCallback);
}


void RadioMFTAgentEventImpl::setMFTCommand(IRadioMFTAgentCommand* pCommand)
{
	m_pCommand = pCommand;

	m_pApStateMgr = m_pCommand->getApStateMgr();
}

void RadioMFTAgentEventImpl::updateGroupResource(TA_IRS_App::ConsoleDispatchEvent* pConEvent, ACE_Future<FutureResultT>& future)
{
    FUNCTION_ENTRY("updateGroupResource");

	TA_IRS_App::Mca::AssignmentStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::AssignmentStatus*>(pConEvent);

	RadioResource resource;
	

	FutureResultT result;
	future.get(result);

	if (result.m_state == ADD_PATCH_MEMBER)
	{
		resource.id = pEvent->getResourceId(); // only rid is needed to add member
		m_pCommand->connectPatchGroup(resource,IRadioMFTAgentCommand::END,result.m_param);
	}
	else if (result.m_state == ADD_MSEL_MEMBER)
	{
		resource.id = pEvent->getResourceId(); // only rid is needed to add member
		m_pCommand->connectMselGroup(resource,IRadioMFTAgentCommand::END,result.m_param);
	}
	else if (result.m_state == SELECT_RESOURCE)
	{
		resource = The_StatusManager::instance()->getTransactiveResourceByID(result.m_param);
/*
		if (resource.type == RR_SITE)
		{
			m_pCommand->connectSite(resource,IRadioMFTAgentCommand::ACTION);
		}
		else if (resource.type == RR_ANNGROUP)
		{
			m_pCommand->selectAG(resource,IRadioMFTAgentCommand::ACTION);
		}		
        */
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentEventImpl::processPrivateCallFunc(ACE_Future<FutureResultT>& future)
{
    FUNCTION_ENTRY("processPrivateCallFunc");

	FutureResultT result;
	future.get(result);

	RadioResource resource;
	
	if (result.m_state == BEGIN_PRIVATE_CALL)
	{
		resource.type = RR_RADIO;
		resource.ssi  = result.m_param;
		m_pCommand->makePrivateCall(resource,IRadioMFTAgentCommand::ACTION);
	}
	else if (result.m_state == ANSWER_PRIVATE_CALL)
	{
		resource.type = RR_RADIO;
		resource.ssi  = result.m_param;
		m_pCommand->answerPrivateCall(resource,IRadioMFTAgentCommand::ACTION);
	}
	else if (result.m_state == BEGIN_GENERAL_XMIT)
	{
		m_pCommand->generalTransmit(IRadioMFTAgentCommand::START); //lichao++
	}
	
    FUNCTION_EXIT;
	
}

//fixed TD14269
void RadioMFTAgentEventImpl::submitAlarm(const TA_Base_Core::MessageType& pAlarm)
{	
	TA_Base_Core::ThreadGuard guard(m_lock);
	FUNCTION_ENTRY("submitAlarm");
	
    TA_Base_Core::IEntityData* entity = NULL;
    try
    {   
        entity = RadioMFTAgentHelper::getRadioMFTAgentEntity();

        // construct the DescriptionParameters list
        TA_Base_Core::DescriptionParameters parameters;
		
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarm ( 
            pAlarm, 
            entity->getKey(),	     // entity key
            entity->getTypeKey(),
            parameters,                                              // parameters
			entity->getName(),
            entity->getLocation(),
            entity->getSubsystem());	     // subystem key
    }
    catch(...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to raise alarm");
    }

    delete entity;
    entity = NULL;
	
    FUNCTION_EXIT;	
}

void RadioMFTAgentEventImpl::closeAlarm(const TA_Base_Core::MessageType& pAlarm)
{
    // Threadsafe 
    TA_Base_Core::ThreadGuard guard(m_lock);
	FUNCTION_ENTRY("closeAlarm");
	
    TA_Base_Core::IEntityData* entity = NULL;
    try
    {   
        entity = RadioMFTAgentHelper::getRadioMFTAgentEntity();
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(
			pAlarm, 
			entity->getKey(), 
			entity->getLocation());
    }
    catch(...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to close the alarm");
    }

    delete entity;
    entity = NULL;

    FUNCTION_EXIT;	
}

/*
void RadioMFTAgentEventImpl::processPhoneCallFunc(ACE_Future<FutureResultT>& future)
{
	FutureResultT result;
	future.get(result);

	if (result.m_state == DIAL_PHONE)
	{
		std::ostringstream data;
		data << result.m_param;

		m_pCommand->makePhoneCall(data.str().c_str(),IRadioMFTAgentCommand::ACTION);
	}

}
*/
/*bool RadioMFTAgentEventImpl::isPhoneSelected(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{

	bool result = false;
	
	TA_IRS_App::Mca::DialingStatus* pEvent1 = 
		dynamic_cast<TA_IRS_App::Mca::DialingStatus*>(pConEvent);

	if (pEvent1 != 0)
	{
		return true;
	}
	TA_IRS_App::Mca::PhoneStatus* pEvent = 
		dynamic_cast<TA_IRS_App::Mca::PhoneStatus*>(pConEvent);

	if (pEvent->getStatus()==TA_IRS_App::Mca::PhoneStatus::Phone_Selected)
	{
		result = true;
	}

	return result;
}*/
void RadioMFTAgentEventImpl::onEmergencyAlarmTone(TA_IRS_App::ConsoleDispatchEvent* pConEvent)
{
	TA_IRS_App::Mca::ToneStatus* pEvent1 = 
		dynamic_cast<TA_IRS_App::Mca::ToneStatus*>(pConEvent);

	if (pEvent1 != 0)
	{
		if ( (pEvent1->getClientId() == TA_IRS_App::TheConsoleStateManager::instance()->getClientID()) &&
			 (pEvent1->getTone() == 4) )// Emergency Alarm Tone "tone_gen.h"
		{
			The_StatusManager::instance()->setAudibleAlarm(true);
		}
		else
		{
			The_StatusManager::instance()->setAudibleAlarm(false);
		}
		
	}
}

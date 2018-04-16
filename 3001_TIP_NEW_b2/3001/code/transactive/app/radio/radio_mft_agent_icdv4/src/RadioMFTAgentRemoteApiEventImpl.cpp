#include"app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentRemoteApiEventImpl.h"
#include"app/radio/radio_mft_agent_icdv4/src/IRadioMFTAgentCommand.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioResourceConfigFacade.h"

//#include "app/radio/radio_mft_agent_icdv4/src/private_call.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentRemoteApiEventImpl.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentHelper.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentCallbackAdapter.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioApplicationStateManager.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentStatusManager.h"

#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/CDI_Function.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/CDI_Event.h"
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

using namespace ICDV4;

RadioMFTAgentRemoteApiEventImpl::RadioMFTAgentRemoteApiEventImpl(MCC7500SDK::IAPIRequest_ptr &ptr) //: 
	/*m_pCommand(NULL),
	m_bOperationMode(false)*/
{
    FUNCTION_ENTRY("RadioMFTAgentRemoteApiEventImpl");

	//m_serverAgent.setEntityName(RadioMFTAgentHelper::getRadioServerAgentEntityName());

	m_pCallback = new RadioMFTAgentCallbackAdapter();

    The_ResourceConfig::instance()->setEventImpl(this);
    The_StatusManager::instance()->setMFTAgentCallback(m_pCallback);
	The_ResourceConfig::instance()->setIAPIRequest_ptr(ptr);
    FUNCTION_EXIT;
}


RadioMFTAgentRemoteApiEventImpl::~RadioMFTAgentRemoteApiEventImpl()
{
    FUNCTION_ENTRY("~RadioMFTAgentRemoteApiEventImpl");
/*
	if ( m_pCallback != NULL )
	{
		delete m_pCallback;
		m_pCallback = NULL;
	}
.
*/
    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteApiEventImpl::onMcdiDispatchReady()
{
    FUNCTION_ENTRY("onMcdiDispatchReady");

	m_pApStateMgr = m_pCommand->getApStateMgr();

	if (m_bOperationMode == true)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Callback to application" );
		m_pApStateMgr->setApState(RA_OPERATION_MODE);
		m_pCallback->callback_applicationStateMessage(RA_OPERATION_MODE,0);
	}
	else
	{
		// show progress
		m_pApStateMgr->setApState(RA_INITIALISATION_MODE);
		//m_pCallback->callback_applicationStateMessage(RA_INITIALISATION_MODE,0);
		The_ResourceConfig::instance()->startRetrieveDB(); 

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Call to start retrieve from DB" );

	}
	
	//fixed TD14269
	closeAlarm(TA_Base_Core::RadioAlarms::RadioConsoleAlarm);
	//fixed TD14269

    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteApiEventImpl::onMcdiDispatchNotReady()
{
    //Report error to GUI but allow
    //initialization state to complete.

    FUNCTION_ENTRY("onMcdiDispatchNotReady");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to fully initialise the Mca API." );

	// NOTE: percent reserved for future use. At present always 0.
	if (m_bOperationMode == true) {
		m_pCallback->callback_applicationStateMessage(RA_UNKNOWN, 0); //Haipeng changed the RA_CONFIGURATION_MODE to RA_UNKNOWN
		m_pCallback->callback_applicationErrorMessage("Unable to connect to database");
		m_pApStateMgr->setApState(RA_UNKNOWN);
	
	//fixed TD14269
		submitAlarm(TA_Base_Core::RadioAlarms::RadioConsoleAlarm);
	//fixed TD14269
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteApiEventImpl::onMcdiRetrieveRCDatabaseRequest()
{
    FUNCTION_ENTRY("onMcdiRetrieveRCDatabaseRequest");
/*
	TA_IRS_App::Mcdi::RetrieveRCDatabaseGrant pFunction;
	pFunction.execute();
*/
    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::onMcdiRetrieveRCDatabaseResponseAck()
{
    FUNCTION_ENTRY("onMcdiRetrieveRCDatabaseResponseAck");

    // TODO:

    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteApiEventImpl::onMcdiSynchronisationStart()
{
    //Force application state to initializing
    //Set PersROM complete percent to zero
    FUNCTION_ENTRY("onMcdiSynchronisationStart");
/*
	m_pCallback->callback_applicationStateMessage(RA_INITIALISATION_MODE,0);
*/
    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::onMcdiSyncCompletionStatus()
{
	//Unused, but reserved for initialisation
    //progress indicator.

    FUNCTION_ENTRY("onMcdiSyncCompletionStatus");
    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteApiEventImpl::onMcdiSynchronisationComplete()
{
	/*  PersROM update finished, but cannot
		change application state yet, as not
		yet finished DUI refresh.
		Unused, but reserved for initialisation
		progress indicator. */
	
    FUNCTION_ENTRY("onMcdiSynchronisationComplete");
    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteApiEventImpl::onResourceTransmitStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onResourceTransmitStatus");

	ICDV4::McaResourceTransmitStatus* pEvent = 
		dynamic_cast<ICDV4::McaResourceTransmitStatus*>(pConEvent);

	try
	{
		RadioResource resource = 
			The_StatusManager::instance()->getResourcebyChannel(pEvent->get_channel());
		
		IRadioMFTAgentCallback::ResourceActivityState state = convertResourceTXStatus((TA_IRS_App::Mca::ResourceTransmitStatus::RegT)pEvent->get_wStatus());

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


void RadioMFTAgentRemoteApiEventImpl::onEmergencyAlarmStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onEmergencyAlarmStatus");

	ICDV4::McaEmergencyAlarmStatus* pEvent = 
		dynamic_cast<ICDV4::McaEmergencyAlarmStatus*>(pConEvent);

    RadioMFTAgentStatusManager* lManager = The_StatusManager::instance();

	/** 
	 *	get the resource of the operator position that 
	 *	has taken responsibility for handling the emergency alarm
	 */	
	RadioResource resource = lManager->getResourcebyChannel(pEvent->get_channel());

    if (resource.type == RR_NIL)
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
			"Resource not found for channel[%d].",pEvent->get_channel());
        return;
    }

	RadioResource changedby;
	RadioResource radioUnit;
	try
	{
		// TD17224
		changedby.id = pEvent->get_resource(); // indicates the resource id of the operator position that has taken 
												// responsibility for handling the emergency.

		// indicates the radio ID corresponding to the emergency alarm
		radioUnit = lManager->getResourceBySSI(pEvent->get_unit_id());
		changedby.ssi = pEvent->get_unit_id(); 
		changedby.alias = radioUnit.alias;
		changedby.type = RR_RADIO;
		// TD17224
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Undefined operation position rid[%d] for handling emergency alarm.",pEvent->get_resource());
	}

	RadioEmergencyModeType type = RE_NO_EMERGENCY;
	bool refresh = false;
	switch(pEvent->get_wStatus())
	{
		//case TA_IRS_App::Mca::EmergencyAlarmStatus::Alarm_Recognized:
		//case TA_IRS_App::Mca::EmergencyAlarmStatus::Alarm_Refresh:
		case MCA_EMERG_ACKNOWLEDGED:
			type = RE_EMERGENCY;
			break;
		case MCA_ACKNOWLEDGED_EMERG_ALARM_REFRESH:
			type = RE_EMERGENCY;
			refresh = true;
			break;
		//case TA_IRS_App::Mca::EmergencyAlarmStatus::Alarm_Received:
		//case TA_IRS_App::Mca::EmergencyAlarmStatus::Emergency_Alarm_Refresh:
		case MCA_FEATURE_STATUS_CODE: // MCA_RADIO_EMERG_ALARM_RECEIVED
			type = RE_UNACK_EMERGENCY;
			break;
		case MCA_UNACKNOWLEDGED_EMERG_ALARM_REFRESH:
			type = RE_UNACK_EMERGENCY;
			refresh = true;
			break;

		case MCA_EMERG_ALARM_ENDED:
			type = RE_NO_EMERGENCY;
			break;
		case MCA_EMERG_ALARM_ENDED_REFRESH:
			type = RE_NO_EMERGENCY;
			refresh = true;
			break;
		
		default:
			type = RE_NO_EMERGENCY;
			refresh = true;
	}

	RadioEmergencyModeType prevType = lManager->getFeatureAttributeEmergencyMode(resource);

	lManager->setFeatureAttributeEmergencyMode(resource,type);

	if (type == RE_NO_EMERGENCY && lManager->isAudibleAlarmOn()) {
		try
		{
			//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Supposed to Call API EndTone?");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling API EndTone");
			m_pCommand->endTone();
		}
		catch(TA_IRS_App::ConsoleDispatchException& e)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
		}	
	}

	if ((m_pApStateMgr->getApState() == RA_OPERATION_MODE ) && (!refresh)) // TD18118
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
		if ((type != RE_NO_EMERGENCY) && (!lManager->isAudibleAlarmOn()))
		{
			try
			{
				//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Supposed to Call API BeginTone?");
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling API BeginTone");
				m_pCommand->beginTone(MCA_EMERGENCY_ALARM_TONE);
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
				//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Supposed to Call API EndTone?");
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling API EndTone");
				m_pCommand->endTone();
			}
			catch(TA_IRS_App::ConsoleDispatchException& e)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
			}			
		}	
	}
	
	
    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::onEmergencyCallStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{

    FUNCTION_ENTRY("onEmergencyCallStatus");

	ICDV4::McaEmergencyCallStatus* pEvent = 
		dynamic_cast<ICDV4::McaEmergencyCallStatus*>(pConEvent);

    RadioMFTAgentStatusManager* lManager = The_StatusManager::instance();

	RadioResource resource = 
		lManager->getResourcebyChannel(pEvent->get_channel());

    if (resource.type == RR_NIL)
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
			"Resource not found for channel[%d].",pEvent->get_channel());
        return;
    }

	RadioResource changedBy;
	RadioResource radioUnit;
	try
	{
		// TD17224
		//changedBy.id = pEvent->get_wInitiator(); // indicates the resource id of the operator position that has taken 
												// responsibility for handling the emergency.
		// TODO: RAGHU cannot find the OperatorRID

		// indicates the radio ID corresponding to the emergency alarm
		RadioResource radioUnit = lManager->getResourceBySSI(pEvent->get_unit_id());
		changedBy.ssi = pEvent->get_unit_id(); 
		changedBy.id = pEvent->get_unit_id();
		changedBy.alias = radioUnit.alias;
		changedBy.type = RR_RADIO;
		// TD17224
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Undefined operation position rid[%d] for handling emergency call.",pEvent->get_resource());
	}
	
	RadioEmergencyModeType type = RE_NO_EMERGENCY;
	bool refresh = false;
	switch(pEvent->get_wStatus())
	{
	//case MCA_EMERG_CALL_STATE://TA_IRS_App::Mca::EmergencyCallStatus::Emergency_Call_State:
		case MCA_UNACKNOWLEDGED_EMERG_CALL_REFRESH://TA_IRS_App::Mca::EmergencyCallStatus::Unreg_Emergency_Call_Refresh:		
			type = RE_UNACK_EMERGENCY;
			refresh = true;
			break;
		//case TA_IRS_App::Mca::EmergencyCallStatus::Reg_Emergency_Call_Refresh:		
		case MCA_EMERG_CALL_STATE:
			type = RE_UNACK_EMERGENCY;
			break;
		case MCA_ACKNOWLEDGED_EMERG_CALL_REFRESH:
			type = RE_EMERGENCY;
			refresh = true;
			break;
		//case TA_IRS_App::Mca::EmergencyCallStatus::Emergency_Call_Ended:	
		case MCA_EMERG_ENDED:
			type = RE_NO_EMERGENCY;
			break;
		case MCA_EMERG_CALL_ENDED_REFRESH:
			type = RE_NO_EMERGENCY;
			refresh = true;
			break;
		case MCA_PATCHED_EMERG_CALL_STATE:
			type = RE_NO_EMERGENCY;
			break;
		case MCA_PATCHED_EMERG_CALL_REFRESH:
			type = RE_NO_EMERGENCY;
			refresh = true;
			break;
		//case TA_IRS_App::Mca::EmergencyCallStatus::Emergency_Call_Ended_Refresh:
		//	m_pCommand->endEmergencyCall(resource);
		//	type = RE_NO_EMERGENCY;
	}

	RadioEmergencyModeType prevType = lManager->getFeatureAttributeEmergencyMode(resource);
	
	lManager->setFeatureAttributeEmergencyMode(resource,type);

	if (type == RE_NO_EMERGENCY&& lManager->isAudibleAlarmOn()) {
		try
		{
			//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Supposed to Call API EndTone?");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling API EndTone");
			m_pCommand->endTone();
		}
		catch(TA_IRS_App::ConsoleDispatchException& e)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
		}	
	}

	if ((m_pApStateMgr->getApState() == RA_OPERATION_MODE ) && (!refresh)) // TD18118
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

		//if (pEvent->get_wStatus() != MCA_EMERG_CALL_STATE)
			m_pCallback->callback_resourceEmergencyModeMessage(resource,type,changedBy);
		if ((type != RE_NO_EMERGENCY) && (!lManager->isAudibleAlarmOn()))
		{
			try
			{
				//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Supposed to Call API BeginTone?");
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling API BeginTone");
				m_pCommand->beginTone(MCA_EMERGENCY_ALARM_TONE);
			}
			catch(TA_IRS_App::ConsoleDispatchException& e)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
			}
		}
		else if ((type == RE_NO_EMERGENCY) && lManager->isAudibleAlarmOn())
		{
			try
			{
				//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Supposed to Call API EndTone?");
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling API EndTone");
				m_pCommand->endTone();
			}
			catch(TA_IRS_App::ConsoleDispatchException& e)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
			}			
		}
	}
	
    FUNCTION_EXIT;
}


bool RadioMFTAgentRemoteApiEventImpl::onGeneralTransmitStatus(ICDV4::RemoteAPIRadioEvent* pConEvent, ACE_Future<FutureResultT>& future)
{
    FUNCTION_ENTRY("onGeneralTransmitStatus");

	ICDV4::McaGeneralTransmitStatus* pEvent = 
		dynamic_cast<ICDV4::McaGeneralTransmitStatus*>(pConEvent);
	
	if (pEvent->get_wStatus()==TA_IRS_App::Mca::GeneralTransmitStatus::General_Transmit_Begun)
	{
		if (future.ready())
		{
			m_pCommand->generalTransmit(IRadioMFTAgentCommand::END);
			return true;
		}
	}
	else if (pEvent->get_wStatus() == TA_IRS_App::Mca::GeneralTransmitStatus::General_Transmit_Ended)
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

void RadioMFTAgentRemoteApiEventImpl::onInboundCallStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onInboundCallStatus");

	ICDV4::McaInboundCallStatus* pEvent = 
		dynamic_cast<ICDV4::McaInboundCallStatus*>(pConEvent);

	try
	{
		RadioResource resource = 
			The_StatusManager::instance()->getResourcebyChannel(pEvent->get_channel());


		PrivateCallState state = PC_NOT_INVOLVED;

		if ( pEvent->get_wStatus() != TA_IRS_App::Mca::InboundCallStatus::Inbound_Call_Ended)
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


void RadioMFTAgentRemoteApiEventImpl::onInboundPttIdStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onInboundPttIdStatus");

	ICDV4::McaInboundPttIdStatus* pEvent = 
		dynamic_cast<ICDV4::McaInboundPttIdStatus*>(pConEvent);

	try
	{
		RadioResource src =
			The_StatusManager::instance()->getRadioResourceBySSI(pEvent->get_unit_id());
		
		if (src.type == RR_NIL)
			src.ssi = pEvent->get_unit_id();

		RadioResource listener = 
			The_StatusManager::instance()->getResourcebyChannel(pEvent->get_channel());

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


void RadioMFTAgentRemoteApiEventImpl::onInboundPrivateCallStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onInboundPrivateCallStatus");

	ICDV4::McaInboundPrivateCallStatus* pEvent = 
		dynamic_cast<ICDV4::McaInboundPrivateCallStatus*>(pConEvent);

	RadioResource caller = 
		The_StatusManager::instance()->getRadioResourceBySSI(pEvent->get_unit_id());

	if (MCA_INBOUND_RADIO_PVT_CALL == pEvent->get_wStatus() || MCA_INBOUND_CONSOLE_PVT_CALL == pEvent->get_wStatus())
	{
		// check whether we have private call resource ID
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Finding channel for private call");		
		RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
		LogicalChannel channel = The_StatusManager::instance()->getChannel(pc.id);
		if (0 == channel) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "No channel found");
			The_StatusManager::instance()->addChannel(pc.id, pEvent->get_channel());
		}


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



void RadioMFTAgentRemoteApiEventImpl::onInboundRadioMessageStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onInboundRadioMessageStatus");

	ICDV4::McaInboundRadioStatus* pEvent = 
		dynamic_cast<ICDV4::McaInboundRadioStatus*>(pConEvent);

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
		CORBA_CALL(m_serverAgent,radioStatusMsg,(pEvent->get_unit_id(),pEvent->get_wRadioStatus()));
			
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


void RadioMFTAgentRemoteApiEventImpl::onRefreshStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onRefreshStatus");

	ICDV4::McaRefreshStatus* pEvent = 
		dynamic_cast<ICDV4::McaRefreshStatus*>(pConEvent);

	if (pEvent->get_wStatus()==TA_IRS_App::Mca::RefreshStatus::Refresh_Ended)
	{
		// get data from MCDB, and wait for the callback
		//The_ResourceConfig::instance()->startRetrieveDB();
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteApiEventImpl::onResourceVolumeStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{

    FUNCTION_ENTRY("onResourceVolumeStatus");
	
	ICDV4::McaResourceVolumeStatus* pEvent = 
		dynamic_cast<ICDV4::McaResourceVolumeStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "ResourceVolumeStatus is NULL");

	try
	{
		RadioResource resource = 
			The_StatusManager::instance()->getResourcebyChannel(pEvent->get_Channel());

		AssociatedSpeaker speaker = NIL;

		The_StatusManager::instance()->setFeatureAttributeSpeakerVolume(resource,pEvent->get_byVolumeLevel());

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
			m_pCallback->callback_resourceVolumeMessage(resource,speaker,pEvent->get_byVolumeLevel());
		}	
	}
	catch(...)
	{
	}
	
    FUNCTION_EXIT;
}


bool RadioMFTAgentRemoteApiEventImpl::onResourceSelectStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onResourceSelectStatus");

	ICDV4::McaResourceSelectStatus* pEvent = 
		dynamic_cast<ICDV4::McaResourceSelectStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "ResourceSelectStatus is NULL");


	RadioResource resource;
	try
	{
		resource = The_StatusManager::instance()->getResourcebyChannel(pEvent->get_channel());
	}
	catch(...)
	{
	}

	if (resource.id==0)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
			"No resource found for ResourceSelectStatus with channel[%d]",pEvent->get_channel());
		return false;
	}
	else if (resource.id == The_StatusManager::instance()->getPrivateCallResource().id)//33554433)/*The_StatusManager::instance()->getPrivateCallResource().id) // Is a Private Call resource*/
	{
		return true;
	}

	AssociatedSpeaker state = NIL;
LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"Status for speaker: %u",pEvent->get_wStatus());
	switch(pEvent->get_wStatus())
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

void RadioMFTAgentRemoteApiEventImpl::onSelectResourceError(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
	FUNCTION_ENTRY("onSelectResourceError");

	ICDV4::McaSelectResourceError* pEvent = 
		dynamic_cast<ICDV4::McaSelectResourceError*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "ResourceSelectStatus is NULL");


	RadioResource resource;
	try
	{
		resource = The_StatusManager::instance()->getResourcebyChannel(pEvent->get_channel());
	}
	catch(...)
	{
	}

	AssociatedSpeaker state = UNSELECTED_SPEAKER;

	The_StatusManager::instance()->setFeatureAttributeIsSelected(resource,state);

	if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
	{
		m_pCallback->callback_resourceSelectMessage(resource,state);
	}	
	FUNCTION_EXIT;
}


bool RadioMFTAgentRemoteApiEventImpl::onAssignmentStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onAssignmentStatus");

	bool bRequestByMe = true;

	ICDV4::McaAssignmentStatus* pEvent = 
		dynamic_cast<ICDV4::McaAssignmentStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "AssignmentStatus is NULL");

	if ( (pEvent->get_byClientId() == MCDI_NULL_CLIENT_ID) || (pEvent->get_byClientId()!=TA_IRS_App::TheConsoleStateManager::instance()->getClientID()))
	{
		// Assignment or Deassignment part of refresh or requested by other console
		bRequestByMe = false;
	}

    RadioMFTAgentStatusManager* lManager = The_StatusManager::instance();

	bool bIsAssigned = false;
	bool bIsProfile = false;
	unsigned int offset = 0;
    bool bIsMonitored = false;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"AssignmentStatusMessage Channel: %u  Resource: %u  Status: %u",pEvent->get_channel(),pEvent->get_resource(),pEvent->get_wStatus());
	RadioResource resource;
	if (pEvent->get_wStatus()==1024)
	{
		bIsAssigned = true;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"AssignmentStatusMessage Adding to channel");
		if ((pEvent->get_byClientId() == MCDI_NULL_CLIENT_ID) &&
			(m_pApStateMgr->getApState() == RA_INITIALISATION_MODE))// on refresh
		{
			resource.id = pEvent->get_resource();
			resource.type = RR_RADIO;  // Temporary type assigned, will be reassigned onMCDBRefreshComplete()
			lManager->addChannel(pEvent->get_resource(),pEvent->get_channel(),true);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Refresh, Adding Channel %u for radio %u",pEvent->get_channel(),pEvent->get_resource());
		}
		else
		{
			try
			{
				resource = lManager->getConsoleResourceByID(pEvent->get_resource());

                if (resource.type != RR_NIL)
                {
				    bIsMonitored = lManager->getFeatureAttributeButton(resource, bIsProfile, offset);

                }
                // always add the channel
				lManager->addChannel(pEvent->get_resource(),pEvent->get_channel());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Initialization, Adding Channel %u for radio %u",pEvent->get_channel(),pEvent->get_resource());
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
	else if (pEvent->get_wStatus()==TA_IRS_App::Mca::AssignmentStatus::Resource_Deassigned)
	{
//		if (bRequestByMe)
		{
			resource = lManager->getResourcebyChannel(pEvent->get_channel());

			lManager->deleteChannel(resource.id); //lichao ++

            if (resource.type != RR_NIL)
            {

			    lManager->setFeatureAttributeIsAssigned(resource,false);
			    lManager->setFeatureAttributeButton(resource,0, 0);
            }
		}
	}
	else if (pEvent->get_wStatus()==TA_IRS_App::Mca::AssignmentStatus::All_Resource_Deassigned)
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


void RadioMFTAgentRemoteApiEventImpl::onAssignResourceErr(ICDV4::RemoteAPIRadioEvent* pConEvent, bool bAssign)
{
    FUNCTION_ENTRY("onAssignmentStatusErr");
    
	if ( bAssign )
	{
		ICDV4::McaAssignResourceError* pEvent = 
			dynamic_cast<ICDV4::McaAssignResourceError*>(pConEvent);

		TA_ASSERT( pEvent != NULL, "AssignResourceError is NULL");


		std::ostringstream logErr;
		logErr << "Error [" << pEvent->get_wErrorNum() <<"] occurred on RID=" << pEvent->get_resource() << 
			" on Channel="<< pEvent->get_channel()<<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, logErr.str().c_str() );
	}
	else
	{
		ICDV4::McaDeassignResourceError* pEvent = 
			dynamic_cast<ICDV4::McaDeassignResourceError*>(pConEvent);

		TA_ASSERT( pEvent != NULL, "DeassignResourceError is NULL");

		std::ostringstream logErr;
		logErr << "Error [" << pEvent->get_wErrorNum() <<"] occurred on Channel="<< pEvent->get_channel()<<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, logErr.str().c_str() );		
	}
	
	FUNCTION_EXIT;
}


bool RadioMFTAgentRemoteApiEventImpl::onPrivateCallStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onPrivateCallStatus");

	ICDV4::McaPrivateCallStatus* pEvent = 
		dynamic_cast<ICDV4::McaPrivateCallStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "PrivateCallStatus is NULL");

	bool isEstablished = false;

	RadioResource resource = 
		The_StatusManager::instance()->getRadioResourceBySSI(pEvent->get_unit_id());

	PrivateCallState status;

	switch(pEvent->get_wStatus())
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

void RadioMFTAgentRemoteApiEventImpl::onAmbienceListeningCallStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onAmbienceListeningCallStatus");


	ICDV4::McaAmbienceListeningCallStatus* pEvent = 
		dynamic_cast<ICDV4::McaAmbienceListeningCallStatus*>(pConEvent);
	TA_ASSERT( pEvent != NULL, "AmbienceListeningCallStatus is NULL");

	RadioResource resource = 
		The_StatusManager::instance()->getRadioResourceBySSI(pEvent->get_unit_id());

	if (resource.type == RR_NIL)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
			"No resource found for AmbienceListeningCallStatus on SSI[%d]",pEvent->get_unit_id());
		return;
	}

	AmbienceListeningCallState status = ALC_NOT_INVOLVED;

	if (pEvent->get_wStatus()==TA_IRS_App::Mca::AmbienceListeningCallStatus::Ambience_Listening_Call_Begun)
	{
		status = ALC_ESTABLISHED;
	}
	else if (pEvent->get_wStatus()==TA_IRS_App::Mca::AmbienceListeningCallStatus::Ambience_Listening_Call_Ended)
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

void RadioMFTAgentRemoteApiEventImpl::onGetFreePatchGroup(ICDV4::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id)
{
    FUNCTION_ENTRY("onGetFreePatchGroup");
    
	ICDV4::McaFreePatchGroupStatus * pEvent = 
				dynamic_cast<ICDV4::McaFreePatchGroupStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "FreePatchGroupStatus is NULL");

	if ( pEvent->get_wPatchGroup() == NULL_PATCH_GROUP)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"No free patch groups are available.");
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Free patch group number %d", pEvent->get_wPatchGroup());
		RadioResource resource = The_StatusManager::instance()->getTransactiveResourceByID(id);
        if (resource.type != RR_NIL)
        {
		    try
		    {

			    The_StatusManager::instance()->addPatchGroup(pEvent->get_wPatchGroup(),id);
			    The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,CONNECTED);

			    m_pCommand->connectPatchGroup(resource,IRadioMFTAgentCommand::ACTION,pEvent->get_wPatchGroup());
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


void RadioMFTAgentRemoteApiEventImpl::onPatchGroupStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
	//Reconstruct state for existing Patch
	//Group

    FUNCTION_ENTRY("onPatchGroupStatus");

	ICDV4::McaPatchGroupStatus* pEvent = 
		dynamic_cast<ICDV4::McaPatchGroupStatus*>(pConEvent);

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

void RadioMFTAgentRemoteApiEventImpl::onPatchMemberStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onPatchMemberStatus");

	ICDV4::McaPatchMemberStatus* pEvent = 
		dynamic_cast<ICDV4::McaPatchMemberStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "PatchMemberStatus is NULL");

	RadioResource member = The_StatusManager::instance()->getResourcebyChannel(pEvent->get_channel());

	if (TA_IRS_App::Mca::PatchMemberStatus::Patch_Member_Added == pEvent->get_wStatus()) {		
		AssociatedSpeaker state = SELECTED_SPEAKER;

		The_StatusManager::instance()->setFeatureAttributeIsSelected(member,state);

		if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
		{
			m_pCallback->callback_resourceSelectMessage(member,state);
		}	
	}

	if ( m_pApStateMgr->getApState() == RA_INITIALISATION_MODE )
	{
		switch (pEvent->get_wStatus())
		{
		case TA_IRS_App::Mca::PatchMemberStatus::Patch_Member_Added:
			The_StatusManager::instance()->addPatchMember(pEvent->get_wPatchGroup(),member);
			break;
		case TA_IRS_App::Mca::PatchMemberStatus::Patch_Member_Deleted:
			The_StatusManager::instance()->delPatchMember(pEvent->get_wPatchGroup(),member);
			break;
		case TA_IRS_App::Mca::PatchMemberStatus::Patch_Member_In_Used:
			The_StatusManager::instance()->delPatchMember(pEvent->get_wPatchGroup(),member);
			break;
			
		}
	}
	else 
	{
		if (pEvent->get_wStatus()==TA_IRS_App::Mca::PatchMemberStatus::Patch_Member_Deleted)
		{
			if (member.type == RR_BIM)
			{
				m_pCommand->deassignResource(member, pEvent->get_channel());
			}
			else if ((member.type == RR_TALKGROUP) && (!The_StatusManager::instance()->getFeatureAttributeIsMonitored(member)))
			{
				m_pCommand->deassignResource(member, pEvent->get_channel());	
			}
		}
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteApiEventImpl::onPatchMemberRefresh(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onPatchMemberRefresh");

	ICDV4::McaPatchMemberRefresh* pEvent = 
		dynamic_cast<ICDV4::McaPatchMemberRefresh*>(pConEvent);	

	TA_ASSERT( pEvent != NULL, "PatchMemberRefresh is NULL");


	std::vector<BYTE> lcidStatus ;// RAGHU -- TODO= pEvent->get_abyPatchBitmap();

	LogicalChannel channel = pEvent->get_startChannel();
	
	if ( pEvent->get_wPatchGroup() == MCA_OTHER_OP_PATCH_GROUP)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"PatchGroup is a is a member of a patch group in another console.");
		return;
	}
	else if (pEvent->get_byPatchGroupStatus() == MCA_GROUP_IS_EMPTY)
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


void RadioMFTAgentRemoteApiEventImpl::onGetFreeMselGroup(ICDV4::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id)
{
    FUNCTION_ENTRY("onGetFreeMselGroup");
    
	ICDV4::McaFreeMselGroupStatus * pEvent = 
		dynamic_cast<ICDV4::McaFreeMselGroupStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "FreeMselGroupStatus is NULL");

	if ( pEvent->get_wMselGroup() == NULL_MSEL_GROUP )
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
			    The_StatusManager::instance()->addMselGroup(pEvent->get_wMselGroup(),id);
			    The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,CONNECTED);
				    
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "onGetFreeMselGroup, group Number: %u", pEvent->get_wMselGroup());
			    m_pCommand->connectMselGroup(resource,IRadioMFTAgentCommand::OPEN,pEvent->get_wMselGroup());		
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

void RadioMFTAgentRemoteApiEventImpl::onMselGroupStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onMselGroupStatus");

	ICDV4::McaMselGroupStatus* pEvent = 
		dynamic_cast<ICDV4::McaMselGroupStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "MselGroupStatus is NULL");

	RadioResource resource = The_StatusManager::instance()->getResourceByGroupNo(pEvent->get_wMselGroup(), RR_MSELECT);

	if (resource.type != RR_NIL)
	{
		if ( pEvent->get_wStatus() == TA_IRS_App::Mca::MselGroupStatus::Multi_Select_Group_Opened)
		{
			The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,CONNECTED);
			m_pCommand->connectMselGroup(resource,IRadioMFTAgentCommand::ACTION,pEvent->get_wMselGroup());		
		}    
		else
		{
			//m_pCommand->releaseMselGroup(resource,pEvent->get_wMselGroup(),IRadioMFTAgentCommand::ACTION);
			
			if (The_StatusManager::instance()->getFeatureAttributeIsGrpConnected(resource) != RELEASED) {
				The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,RELEASED);
				m_pCommand->releaseMselGroup(resource, true);	
			}

			// send Unselect notification to RadioManager for MultiSelect/MultiSite resource 
			// to depress SelectSpeaker button
			m_pCallback->callback_resourceSelectMessage(resource,UNSELECTED_SPEAKER);
		}
	}

    FUNCTION_EXIT;
}



void RadioMFTAgentRemoteApiEventImpl::onOpenMselError(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onMselGroupStatus");

	ICDV4::McaOpenMselError* pEvent = 
		dynamic_cast<ICDV4::McaOpenMselError*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "MselGroupStatus is NULL");

	RadioResource resource = The_StatusManager::instance()->getResourceByGroupNo(pEvent->get_wMselGroup(), RR_MSELECT);

	The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,RELEASED);

	// send Unselect notification to RadioManager for MultiSelect/MultiSite resource 
	// to depress SelectSpeaker button
	m_pCallback->callback_resourceSelectMessage(resource,UNSELECTED_SPEAKER);

    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteApiEventImpl::onCloseMselError(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onMselGroupStatus");

	ICDV4::McaCloseMselError* pEvent = 
		dynamic_cast<ICDV4::McaCloseMselError*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "MselGroupStatus is NULL");

	RadioResource resource = The_StatusManager::instance()->getResourceByGroupNo(pEvent->get_wMselGroup(), RR_MSELECT);

	The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,CONNECTED);

	// send Unselect notification to RadioManager for MultiSelect/MultiSite resource 
	// to depress SelectSpeaker button
	m_pCallback->callback_resourceSelectMessage(resource,SELECTED_SPEAKER);

    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::onMselMemberStatus(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onMselMemberStatus");

	/*
	 * Not directly used, IDL method manages it
	 */
    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteApiEventImpl::onMselMemberRefresh(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onMselMemberRefresh");

	ICDV4::McaMselMemberRefresh* pEvent = 
		dynamic_cast<ICDV4::McaMselMemberRefresh*>(pConEvent);	

	TA_ASSERT( pEvent != NULL, "MselMemberRefresh is NULL");


	std::vector<BYTE> lcidStatus;  // RAGHU __ TODO = pEvent->get_abyMselBitmap();

	LogicalChannel channel = pEvent->get_startChannel();
	
	if ( pEvent->get_byMselGroupStatus() == MCA_GROUP_IS_EMPTY)
	{
		std::ostringstream logStr;
		logStr << "MselGroup[" << pEvent->get_wMselGroup() << 
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
					TA_IRS_App::Mca::DeleteMultiselectMember pCommand(channel,pEvent->get_wMselGroup());			
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
		TA_IRS_App::Mca::CloseMultiselect pFunction(pEvent->get_wMselGroup());
		pFunction.execute();
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::onMcdbRefreshComplete()
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

		//res = The_StatusManager::instance()->getPhoneResource();
  //      if (res.type != RR_NIL)
  //      {
  //  		m_pCommand->assignResource(res);
  //      }
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
void RadioMFTAgentRemoteApiEventImpl::onAddMselMemberWrn(ICDV4::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id)
{
	ICDV4::McaAddMselMemberWarning* pEvent = 
		dynamic_cast<ICDV4::McaAddMselMemberWarning*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to add msel member " << res.alias;

    sendErrorMessage(ostr.str());
}

void RadioMFTAgentRemoteApiEventImpl::onAddMselMemberErr(ICDV4::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id)
{
	ICDV4::McaAddMselMemberError* pEvent = 
		dynamic_cast<ICDV4::McaAddMselMemberError*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to add msel member " << res.alias;

    sendErrorMessage(ostr.str());
}

void RadioMFTAgentRemoteApiEventImpl::onDeleteMselMemberErr(ICDV4::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id)
{
	ICDV4::McaDeleteMselMemberError* pEvent = 
		dynamic_cast<ICDV4::McaDeleteMselMemberError*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to delete msel member " << res.alias;

    sendErrorMessage(ostr.str());
}

void RadioMFTAgentRemoteApiEventImpl::onAddPatchMemberWrn(ICDV4::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id)
{
	ICDV4::McaAddPatchMemberWarning* pEvent = 
		dynamic_cast<ICDV4::McaAddPatchMemberWarning*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to add patch member " << res.alias;

    sendErrorMessage(ostr.str());
}

void RadioMFTAgentRemoteApiEventImpl::onAddPatchMemberErr(ICDV4::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id)
{
	ICDV4::McaAddPatchMemberError* pEvent = 
		dynamic_cast<ICDV4::McaAddPatchMemberError*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "onAddPatchMemberErr: %u (0x%x)", pEvent->get_wErrorNum(), pEvent->get_wErrorNum());

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to add patch member " << res.alias;

	RadioResource group;
	group.type = RR_PATCHGROUP;
	group.id = pEvent->get_wPatchGroup();
	m_pCallback->callback_resourceSelectMessage(group, UNSELECTED_SPEAKER);

    sendErrorMessage(ostr.str());
}

void RadioMFTAgentRemoteApiEventImpl::onDeletePatchMemberErr(ICDV4::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id)
{
	ICDV4::McaDeletePatchMemberError* pEvent = 
		dynamic_cast<ICDV4::McaDeletePatchMemberError*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to delete patch member " << res.alias;

    sendErrorMessage(ostr.str());
}

void RadioMFTAgentRemoteApiEventImpl::onBeginGeneralXmitWrn(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
	ICDV4::McaBeginGeneralTransmitWarning* pEvent = 
		dynamic_cast<ICDV4::McaBeginGeneralTransmitWarning*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    sendErrorMessage("Warning beginning general transmit");
}

void RadioMFTAgentRemoteApiEventImpl::onBeginGeneralXmitErr(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
	ICDV4::McaBeginGeneralTransmitError* pEvent = 
		dynamic_cast<ICDV4::McaBeginGeneralTransmitError*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    sendErrorMessage("Error beginning general transmit");
}

void RadioMFTAgentRemoteApiEventImpl::sendErrorMessage(const std::string msg)
{
    m_pCallback->callback_applicationErrorMessage(msg);
}

// TD14322++

IRadioMFTAgentCallback::ResourceActivityState RadioMFTAgentRemoteApiEventImpl::convertResourceTXStatus(
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



bool RadioMFTAgentRemoteApiEventImpl::addCallback(TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr pCallback)
{
    FUNCTION_ENTRY("addCallback");
    
	RadioMFTAgentCallbackAdapter* pCallbackAdapter = 
			dynamic_cast<RadioMFTAgentCallbackAdapter*>(m_pCallback);

	TA_ASSERT(pCallbackAdapter!= NULL, "pCallbackAdapter is NULL.");

	FUNCTION_EXIT;
	return pCallbackAdapter->addCallbackCollection(pCallback);
}


void RadioMFTAgentRemoteApiEventImpl::setMFTCommand(IRadioMFTAgentCommand* pCommand)
{
	m_pCommand = pCommand;

	m_pApStateMgr = m_pCommand->getApStateMgr();
}

void RadioMFTAgentRemoteApiEventImpl::updateGroupResource(ICDV4::RemoteAPIRadioEvent* pConEvent, ACE_Future<FutureResultT>& future)
{
    FUNCTION_ENTRY("updateGroupResource");

	ICDV4::McaAssignmentStatus* pEvent = 
		dynamic_cast<ICDV4::McaAssignmentStatus*>(pConEvent);

	RadioResource resource;
	

	FutureResultT result;
	future.get(result);

	if (result.m_state == ADD_PATCH_MEMBER)
	{
		resource.id = pEvent->get_resource(); // only rid is needed to add member
		m_pCommand->connectPatchGroup(resource,IRadioMFTAgentCommand::END,result.m_param);
	}
	else if (result.m_state == ADD_MSEL_MEMBER)
	{
		resource.id = pEvent->get_resource(); // only rid is needed to add member
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

void RadioMFTAgentRemoteApiEventImpl::processPrivateCallFunc(ACE_Future<FutureResultT>& future)
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
void RadioMFTAgentRemoteApiEventImpl::submitAlarm(const TA_Base_Core::MessageType& pAlarm)
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

void RadioMFTAgentRemoteApiEventImpl::closeAlarm(const TA_Base_Core::MessageType& pAlarm)
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


/*void RadioMFTAgentRemoteApiEventImpl::processPhoneCallFunc(ACE_Future<FutureResultT>& future)
{
	FutureResultT result;
	future.get(result);

	if (result.m_state == DIAL_PHONE)
	{
		std::ostringstream data;
		data << result.m_param;

		//m_pCommand->makePhoneCall(data.str().c_str(),IRadioMFTAgentCommand::ACTION);
	}

}*/
void RadioMFTAgentRemoteApiEventImpl::onEmergencyAlarmTone(ICDV4::RemoteAPIRadioEvent* pConEvent)
{
	ICDV4::McaToneStatus* pEvent1 = 
		dynamic_cast<ICDV4::McaToneStatus*>(pConEvent);

	if (pEvent1 != 0)
	{
		if ( (pEvent1->get_byClientId() == TA_IRS_App::TheConsoleStateManager::instance()->getClientID()) &&
			 (pEvent1->get_tone() == 4) )// Emergency Alarm Tone "tone_gen.h"
		{
			The_StatusManager::instance()->setAudibleAlarm(true);
		}
		else
		{
			The_StatusManager::instance()->setAudibleAlarm(false);
		}
		
	}
}
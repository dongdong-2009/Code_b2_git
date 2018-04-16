#include"app/radio/radio_mft_agent/src/RadioMFTAgentRemoteApiEventImpl.h"
#include"app/radio/radio_mft_agent/src/IRadioMFTAgentCommand.h"
#include "app/radio/radio_mft_agent/src/RadioResourceConfigFacade.h"

//#include "app/radio/radio_mft_agent/src/private_call.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentRemoteApiEventImpl.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentHelper.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentCallbackAdapter.h"
#include "app/radio/radio_mft_agent/src/RadioApplicationStateManager.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentStatusManager.h"

//#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/CDI_Function.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/CDI_Event.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleDispatchException.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleStateManager.h"

#include "app/radio/radio_manager_gui/src/RadioErrorMsg.h"

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
#include"app/radio/radio_mft_agent/src/RadioMFTAgentRemoteApiEventImpl.h"

#include "bus\radio\radio_agent\idl\src\commonradioagentidl.h"
#include "bus/TA_MessageResource/resource.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

using namespace TA_IRS_App;


//RadioMFTAgentRemoteApiEventImpl::UNASSIGNED_GROUP_STATUS = -1;

RadioMFTAgentRemoteApiEventImpl::RadioMFTAgentRemoteApiEventImpl(MCC7500SDK::IAPIRequest_ptr& ptr, TA_Base_Core::ReEntrantThreadLockable & apiLock) //: 
	/*m_pCommand(NULL),
	m_bOperationMode(false)*/
{
    FUNCTION_ENTRY("RadioMFTAgentRemoteApiEventImpl");

	//m_serverAgent.setEntityName(RadioMFTAgentHelper::getRadioServerAgentEntityName());

	m_pCallback = new RadioMFTAgentCallbackAdapter();

    The_ResourceConfig::instance()->setEventImpl(this);
    The_StatusManager::instance()->setMFTAgentCallback(m_pCallback);
	The_ResourceConfig::instance()->setIAPIRequest_ptr(ptr,apiLock);

	//m_statusHelper = new RadioMFTAgentAliveHelper(this);

	mccReady[0] = mccReady[1] = mccReady[2] = false;
	initialized = false;
	m_curUpdateType = RR_NIL;
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

	The_StatusManager::instance()->initialized();
	if (m_bOperationMode == true)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Callback to application" );
		m_pApStateMgr->setApState(RA_OPERATION_MODE);
		m_pCallback->callback_applicationStateMessage(RA_OPERATION_MODE,0);
		if (!The_StatusManager::instance()->getRadioStatus() && !The_StatusManager::instance()->getIscsStatus()) {
			// normal mode, no need to retrieve DB from radio, straight from DB
			The_ResourceConfig::instance()->startRetrieveDB(RadioResourceConfigFacade::DB_INIT_ISCS_MODE);
		} else {
			// degraded, either ISCS or radio. get the DB from radio
			The_ResourceConfig::instance()->startRetrieveDB(RadioResourceConfigFacade::DB_INIT_RADIO_MODE);
		}
	}
	else
	{
		// show progress
		m_pApStateMgr->setApState(RA_INITIALISATION_MODE);
		//m_pCallback->callback_applicationStateMessage(RA_INITIALISATION_MODE,0);
		
		if (!The_StatusManager::instance()->getRadioStatus() && !The_StatusManager::instance()->getIscsStatus()) {
			// normal mode, no need to retrieve DB from radio, straight from DB
			The_ResourceConfig::instance()->startRetrieveDB(RadioResourceConfigFacade::DB_INIT_ISCS_MODE);
		} else {
			// degraded, either ISCS or radio. get the DB from radio
			The_ResourceConfig::instance()->startRetrieveDB(RadioResourceConfigFacade::DB_INIT_RADIO_MODE);
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Call to start retrieve from DB" );

	}
	
	//fixed TD14269
	closeAlarm(TA_Base_Core::RadioAlarms::RadioConsoleAlarm);
	//fixed TD14269

    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::onMcdiLocalTrunking()
{
    FUNCTION_ENTRY("onMcdiLocalTrunking");

	The_ResourceConfig::instance()->startRetrieveDB(RadioResourceConfigFacade::DB_SYNC_RADIO_MODE);

    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::onMcdiNormalMode()
{
	FUNCTION_ENTRY("onMcdiNormalMode");

	// go back to normal mode. resync with ISCS database
	The_StatusManager::instance()->syncFromDb(RR_NIL, m_pCallback);

	FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::onMcdiDispatchNotReady()
{
    //Report error to GUI but allow
    //initialization state to complete.

    FUNCTION_ENTRY("onMcdiDispatchNotReady");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to fully initialise the Mca API." );

	// NOTE: percent reserved for future use. At present always 0.
	if (m_bOperationMode == true) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Operation Mode, calling callbacks" );
		
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


void RadioMFTAgentRemoteApiEventImpl::onResourceTransmitStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onResourceTransmitStatus");

	McaResourceTransmitStatus* pEvent = 
		dynamic_cast<McaResourceTransmitStatus*>(pConEvent);

	try
	{
		RadioResource resource = 
			The_StatusManager::instance()->getResourcebyChannel(pEvent->get_channel());
		
		IRadioMFTAgentCallback::ResourceActivityState state = convertResourceTXStatus(
			pEvent->get_wStatus());

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


void RadioMFTAgentRemoteApiEventImpl::onEmergencyAlarmStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onEmergencyAlarmStatus");

	McaEmergencyAlarmStatus* pEvent = 
		dynamic_cast<McaEmergencyAlarmStatus*>(pConEvent);

	if (pEvent->get_IndividualAlias() == "") {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Not processing EmergencyAlarmStatus. Empty Individual Alias");
		FUNCTION_EXIT;
		return;
	}

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

void RadioMFTAgentRemoteApiEventImpl::onEmergencyCallStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{

    FUNCTION_ENTRY("onEmergencyCallStatus");

	McaEmergencyCallStatus* pEvent = 
		dynamic_cast<McaEmergencyCallStatus*>(pConEvent);

	if (pEvent->get_IndividualAlias() == "") {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Not processing EmergencyCallStatus. Empty Individual Alias");
		FUNCTION_EXIT;
		return;
	}

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

	RadioEmergencyModeType prevType = lManager->getFeatureAttributeEmergencyMode(resource);

	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				 "Event Status For Emergency Call: %d",pEvent->get_wStatus());

	switch(pEvent->get_wStatus())
	{
	//case MCA_EMERG_CALL_STATE://TA_IRS_App::Mca::EmergencyCallStatus::Emergency_Call_State:
		case MCA_UNACKNOWLEDGED_EMERG_CALL_REFRESH://TA_IRS_App::Mca::EmergencyCallStatus::Unreg_Emergency_Call_Refresh:		
			type = RE_UNACK_EMERGENCY;
			refresh = true;
			break;
		//case TA_IRS_App::Mca::EmergencyCallStatus::Reg_Emergency_Call_Refresh:		
		case MCA_EMERG_CALL_STATE:
			if (prevType == RE_EMERGENCY) {
				type = RE_EMERGENCY;
			} else {
				type = RE_UNACK_EMERGENCY;
			}
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

	if ((m_pApStateMgr->getApState() == RA_OPERATION_MODE )) // TD18118
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


bool RadioMFTAgentRemoteApiEventImpl::onGeneralTransmitStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ACE_Future<FutureResultT>& future)
{
    FUNCTION_ENTRY("onGeneralTransmitStatus");

	// for private call
	FutureResultT result;
	if (future.ready()) {
		future.get(result);
		if (result.m_state == ANSWER_PRIVATE_CALL) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "General Transmit to trigger AnswerPrivateCall, end it now");
			
			// DCR item, before ending, give the radio system establish the channel
			TA_Base_Core::Thread::sleep(1000);
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "General Transmit : Ending after 1 sec");
			m_pCommand->generalTransmit(IRadioMFTAgentCommand::CommandState::END);
		} 
	}else {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "General Transmit Status for other events");
	}

	McaGeneralTransmitStatus* pEvent = 
		dynamic_cast<McaGeneralTransmitStatus*>(pConEvent);
	RadioResource curRes = The_StatusManager::instance()->getActiveResource();
	if (curRes.type == RR_NIL)
	{
		FUNCTION_EXIT;
		return true;
	}
	PrivateCallState state = PC_NOT_INVOLVED;

	if (pEvent->get_wStatus()==TA_IRS_App::Mca::GeneralTransmitStatus::General_Transmit_Begun)
	{
		state = PC_INCOMING;
	}

	if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
	{
		m_pCallback->callback_inboundCallMessage(curRes,state);
	}
	FUNCTION_EXIT;
	return true;
}

void RadioMFTAgentRemoteApiEventImpl::onInboundCallStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onInboundCallStatus");

	McaInboundCallStatus* pEvent = 
		dynamic_cast<McaInboundCallStatus*>(pConEvent);

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


void RadioMFTAgentRemoteApiEventImpl::onInboundPttIdStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onInboundPttIdStatus");

	McaInboundPttIdStatus* pEvent = 
		dynamic_cast<McaInboundPttIdStatus*>(pConEvent);

	try
	{
		RadioResource src =
			The_StatusManager::instance()->getRadioResourceBySSI(pEvent->get_unit_id());
		
		if (src.type == RR_NIL) {
			src.ssi = pEvent->get_unit_id();
			src.alias = pEvent->get_IndividualAlias();
		}

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


void RadioMFTAgentRemoteApiEventImpl::onInboundPrivateCallStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onInboundPrivateCallStatus");

	McaInboundPrivateCallStatus* pEvent = 
		dynamic_cast<McaInboundPrivateCallStatus*>(pConEvent);

	RadioResource caller = 
		The_StatusManager::instance()->getRadioResourceBySSI(pEvent->get_unit_id());

	if (MCA_INBOUND_RADIO_PVT_CALL == pEvent->get_wStatus() || MCA_INBOUND_CONSOLE_PVT_CALL == pEvent->get_wStatus())
	{
		// check whether we have private call resource ID
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Finding channel for private call");		
		RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
		LogicalChannel channel = The_StatusManager::instance()->getChannel(pc.id);

		if (0 == channel) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "No channel found");
			The_StatusManager::instance()->addChannel(pc.id, pEvent->get_channel());
		}
		
		The_StatusManager::instance()->insertResourceChannelOnMap(pEvent->get_unit_id(),caller.id);
		The_StatusManager::instance()->setFeatureAttributePtCallState(caller,PC_INCOMING);

		if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
		{
			// TODO, peter.wong, call sound beep API
			//try
			//{
			//	TA_IRS_App::Mca::SoundBeeps pSoundBeep(MAX_BEEPS,3);			
			//	pSoundBeep.execute();
			//}
			//catch(TA_IRS_App::ConsoleDispatchException& e)
			//{
			//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
			//}
			m_pCallback->callback_privateCallMessage(caller,PC_INCOMING);
		}
	}
	
    FUNCTION_EXIT;
}



//void RadioMFTAgentRemoteApiEventImpl::onInboundRadioMessageStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
//{
//    FUNCTION_ENTRY("onInboundRadioMessageStatus");
//
//	McaInboundRadioStatus* pEvent = 
//		dynamic_cast<McaInboundRadioStatus*>(pConEvent);
//
//	try
//	{
//		bool status;
//		CORBA_CALL_RETURN(status,m_serverAgent,_non_existent,());
//	    if( status )
//	    {
//		    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
//			    "RadioServerAgent object reference is non-existent." );
//
//		    // Should reconnect to serverAgent??
//		    return;
//	    }
//		CORBA_CALL(m_serverAgent,radioStatusMsg,(pEvent->get_unit_id(),pEvent->get_wRadioStatus()));
//			
//	}
//	catch(const CORBA::Exception& e)
//	{
//        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,  SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
//               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
//
//	}
//	catch(...)
//	{
//		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
//			"Exception occurred in radioStatusMsg().");
//	}
//
//
//    FUNCTION_EXIT;
//}
//

void RadioMFTAgentRemoteApiEventImpl::onRefreshStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onRefreshStatus");

	McaRefreshStatus* pEvent = 
		dynamic_cast<McaRefreshStatus*>(pConEvent);

	if (pEvent->get_wStatus()==TA_IRS_App::Mca::RefreshStatus::Refresh_Begun)
	{
		mccReady[DB_STATUS] = false;
		mccReady[AUDIO_STATUS] = false;
		mccReady[PERIPHERAL_STATUS] = false;
	}
	if (pEvent->get_wStatus()==TA_IRS_App::Mca::RefreshStatus::Refresh_Ended)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Checking MCC ready for API call");
		bool ready = true;
		
		if (!mccReady[DB_STATUS]) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DB Status is not ready");
			ready = false;
		}
		if (!mccReady[AUDIO_STATUS]) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Audio Status is not ready");
			ready = false;
		}
		if (!mccReady[PERIPHERAL_STATUS])
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Peripheral Status is not ready");
			ready = false;
		}		
		
		if (ready) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC ready for API call, now start retrieving DB");
			onMcdiDispatchReady();
		} 
		else 
		{
			m_pCommand->beginRefresh();
		}
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::onMccStatus(short index) {
	mccReady[index] = true;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Setting ready status for index %d and initialized is %d", index, initialized);
	if (!initialized) {
		//Peripheral Message is not included in the initialization at all.
		//RadioManager only needs DB_READY and AUDIO to initialize.
		if (mccReady[DB_STATUS] && /*mccReady[PERIPHERAL_STATUS] &&*/ mccReady[AUDIO_STATUS]) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC ready for API call, now start retrieving DB");
			onMcdiDispatchReady();
			initialized = true;
		}
	}
}


void RadioMFTAgentRemoteApiEventImpl::onResourceVolumeStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{

    FUNCTION_ENTRY("onResourceVolumeStatus");
	
	McaResourceVolumeStatus* pEvent = 
		dynamic_cast<McaResourceVolumeStatus*>(pConEvent);

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


bool RadioMFTAgentRemoteApiEventImpl::onResourceSelectStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onResourceSelectStatus");

	McaResourceSelectStatus* pEvent = 
		dynamic_cast<McaResourceSelectStatus*>(pConEvent);

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
		switch (pEvent->get_wStatus()) 
		{
			case TA_IRS_App::Mca::ResourceSelectStatus::Resource_Selected:
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Private Call (id = %u) SelectResource successful", resource.id);
				The_StatusManager::instance()->setPrivateCallResourceSelected(true);
				break;
			case TA_IRS_App::Mca::ResourceSelectStatus::Resource_Deselected:
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Private Call (id = %u) DeSelectResource successful", resource.id);
				The_StatusManager::instance()->setPrivateCallResourceSelected(false);
			break;
		}
		
		return true;
	} 
	if (The_StatusManager::instance()->hasPendingConnectResource(RR_PATCHGROUP)) {
		int id = The_StatusManager::instance()->getPendingConnectResource(RR_PATCHGROUP).id;
		std::vector<RadioResource> m = The_StatusManager::instance()->getGroupMembers(id);
		std::vector<RadioResource>::iterator iter = m.begin();
		if (resource.id == (*iter).id) {
			return true;
		}
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

void RadioMFTAgentRemoteApiEventImpl::onSelectResourceError(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ACE_Future<FutureResultT>& future)
{
	FUNCTION_ENTRY("onSelectResourceError");

	McaSelectResourceError* pEvent = 
		dynamic_cast<McaSelectResourceError*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "ResourceSelectStatus is NULL");

	FutureResultT result;
	future.get(result);
	if (result.m_state == ADD_PATCH_MEMBER) {
		m_pCommand->releasePatchGroup(The_StatusManager::instance()->getPendingConnectResource(RR_PATCHGROUP));
	}

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

	//if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
	//{
	//	m_pCallback->callback_operationCancelMessage(RA_SELECT_STATE, resource);
	//}	
	FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::onDeselectResourceError(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ACE_Future<FutureResultT>& future)
{
	FUNCTION_ENTRY("onSelectResourceError");

	McaDeselectResourceError* pEvent = 
		dynamic_cast<McaDeselectResourceError*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "ResourceSelectStatus is NULL");

	FutureResultT result;
	future.get(result);
	if (result.m_state == DELETE_PATCH_MEMBER) {
		if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
		{
			m_pCallback->callback_resourceSelectMessage(The_StatusManager::instance()->getPendingConnectResource(RR_PATCHGROUP),UNSELECTED_SPEAKER);
		}			
		RadioResource patch;
		patch.id = result.m_resourceId;
		patch.type = RR_PATCHGROUP;
        The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(patch, NAG);
		The_StatusManager::instance()->clearPendingConnectResource(RR_PATCHGROUP);
	}


	RadioResource resource;
	try
	{
		resource = The_StatusManager::instance()->getResourcebyChannel(pEvent->get_channel());
	}
	catch(...)
	{
	}

	AssociatedSpeaker state = SELECTED_SPEAKER;

	The_StatusManager::instance()->setFeatureAttributeIsSelected(resource,state);

	//if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
	//{
	//	m_pCallback->callback_operationCancelMessage(RA_DESELECT_STATE, resource);
	//}	
	FUNCTION_EXIT;
}

bool RadioMFTAgentRemoteApiEventImpl::onAssignmentStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onAssignmentStatus");

	bool bRequestByMe = true;

	McaAssignmentStatus* pEvent = 
		dynamic_cast<McaAssignmentStatus*>(pConEvent);

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

		if (resource.id == The_StatusManager::instance()->getPrivateCallResource().id) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Initialization, selecting private call resource");
			m_pCommand->selectResource(The_StatusManager::instance()->getPrivateCallResource());
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

			// for DB sync, if received resource deassign, update UI
			if (resource.type == RR_TALKGROUP)
			{
				m_pCallback->callback_resourceAssignmentMessage(resource, bIsProfile, offset, false);
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


void RadioMFTAgentRemoteApiEventImpl::onAssignResourceErr(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, bool bAssign)
{
    FUNCTION_ENTRY("onAssignmentStatusErr");

	RadioResource resource;
    
	if ( bAssign )
	{
		McaAssignResourceError* pEvent = 
			dynamic_cast<McaAssignResourceError*>(pConEvent);

		resource = The_StatusManager::instance()->getResourcebyChannel(pEvent->get_channel());

		TA_ASSERT( pEvent != NULL, "AssignResourceError is NULL");


		std::ostringstream logErr;
		logErr << "Error [" << pEvent->get_wErrorNum() <<"] occurred on RID=" << pEvent->get_resource() << 
			" on Channel="<< pEvent->get_channel()<<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, logErr.str().c_str() );

		// canceling UI operation
		if (0 != resource.id && m_pApStateMgr->getApState() == RA_OPERATION_MODE ) {
			m_pCallback->callback_operationCancelMessage(RA_ASSIGN_STATE, resource);
		}
	}
	else
	{
		McaDeassignResourceError* pEvent = 
			dynamic_cast<McaDeassignResourceError*>(pConEvent);

		resource = The_StatusManager::instance()->getResourcebyChannel(pEvent->get_channel());

		TA_ASSERT( pEvent != NULL, "DeassignResourceError is NULL");

		std::ostringstream logErr;
		logErr << "Error [" << pEvent->get_wErrorNum() <<"] occurred on Channel="<< pEvent->get_channel()<<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, logErr.str().c_str() );	

		// canceling UI operation
		if (0 != resource.id && m_pApStateMgr->getApState() == RA_OPERATION_MODE ) {
			m_pCallback->callback_operationCancelMessage(RA_DEASSIGN_STATE, resource);
		}
	}
	
	FUNCTION_EXIT;
}


bool RadioMFTAgentRemoteApiEventImpl::onPrivateCallStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onPrivateCallStatus");

	McaPrivateCallStatus* pEvent = 
		dynamic_cast<McaPrivateCallStatus*>(pConEvent);

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
			
			The_StatusManager::instance()->insertResourceChannelOnMap(pEvent->get_unit_id(),resource.id);
			
			break;

		case TA_IRS_App::Mca::PrivateCallStatus::Private_Call_Begun:
			status = PC_ESTABLISHED;
			The_StatusManager::instance()->insertResourceChannelOnMap(pEvent->get_unit_id(),resource.id);
			isEstablished = true;

			break;
		case TA_IRS_App::Mca::PrivateCallStatus::Private_Call_Ended:
			status = PC_NOT_INVOLVED;
			
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Private Call Ended");

			//	release Patch Group if this resource is involved in DLTS/EPAX patching
			if (The_StatusManager::instance()->isTempPatchInvolved(resource))
			{
				RadioResource pg = The_StatusManager::instance()->getTempPatchGroup();
				m_pCommand->releasePatchGroup(pg);

				RadioResource dummy;
				The_StatusManager::instance()->setTempPatchGroup(dummy);

				The_StatusManager::instance()->delResource(pg);

			}
			
			if (pEvent->get_unit_id() != 0)
			{
				resource.id = The_StatusManager::instance()->getResourceIdFromChannelId(pEvent->get_unit_id());
				The_StatusManager::instance()->deleteResourceChangeRecOnMap (pEvent->get_unit_id());
			}

			onEndCallTerimination(pConEvent);
			
			

			//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Private call ended, deselecting private call resource");
			//m_pCommand->deselectResource(The_StatusManager::instance()->getPrivateCallResource(),pEvent->get_channel());
			break;
			
		default:
			status = PC_NOT_INVOLVED;
	}

	The_StatusManager::instance()->setFeatureAttributePtCallState(resource,status);
	
	if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
			"Private Call Resource Id : %d, Status :%d, unit id : %d, channel : %d",resource.id, status,pEvent->get_unit_id(),pEvent->get_channel());
		
		m_pCallback->callback_privateCallMessage(resource,status);
	}	


    FUNCTION_EXIT;
	return isEstablished;
}

void RadioMFTAgentRemoteApiEventImpl::onResourceRejectStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
	FUNCTION_ENTRY("onResourceRejectStatus");
    
	McaResourceRejectStatus* pEvent = dynamic_cast<McaResourceRejectStatus*>(pConEvent);
	unsigned short status = pEvent->get_wStatus();
	unsigned short channel = pEvent->get_channel();    
	ResourceIdentifier id=The_StatusManager::instance()->getPrivateCallResource().id;
	unsigned short activeCallChannel =The_StatusManager::instance()->getChannel(id);

	unsigned short ssi=The_StatusManager::instance()->getPrivateCallResource().ssi;

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
		"onResourceRejectStatus Resource Id : %d, Status :%d, msg channel: %d, activeCallChannel : %d, SSI=%d",id,status,channel,activeCallChannel,ssi  );

	// if it is private call related
	if( activeCallChannel == channel) 
	{
		if (((0x0400+0x062) == status) || //MCA_CALL_TIMEOUT
			((0x0400+0x07B) == status )) // MCA_CALL_TIMER_EXPIRED
		{
			RadioResource res = The_StatusManager::instance()->deactivateActivePrivateCall();
			if (0 != res.ssi) 
			{	
				if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
				{
					m_pCallback->callback_privateCallMessage(res,PC_NOT_INVOLVED);
				}	
			}
		}
		else if((0x0400+0x06F) == status )  //#define MCA_USER_REQUESTED_DISCONNECT           (MCA_FEATURE_STATUS_CODE + 0x6F) /* Dimetra   */
		{
			RadioResource resource = The_StatusManager::instance()->getPrivateCallResource();
			//if (0 != resource.ssi) 
			//{	
				if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
				{
					m_pCallback->callback_privateCallMessage(resource,PC_REJECTED);
				}	
			//}

		}
	}

	FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::onAmbienceListeningCallStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onAmbienceListeningCallStatus");


	McaAmbienceListeningCallStatus* pEvent = 
		dynamic_cast<McaAmbienceListeningCallStatus*>(pConEvent);
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

void RadioMFTAgentRemoteApiEventImpl::onGetFreePatchGroup(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id)
{
    FUNCTION_ENTRY("onGetFreePatchGroup");
    
	McaFreePatchGroupStatus * pEvent = 
				dynamic_cast<McaFreePatchGroupStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "FreePatchGroupStatus is NULL");
	RadioResource resource = The_StatusManager::instance()->getTransactiveResourceByID(id);

	if ( pEvent->get_wPatchGroup() == NULL_PATCH_GROUP)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"No free patch groups are available.");
		//m_pCallback->callback_operationCancelMessage(RA_SELECT_STATE, resource);
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Free patch group number %d", pEvent->get_wPatchGroup());
        if (resource.type != RR_NIL)
        {
		    try
		    {
			    The_StatusManager::instance()->addPatchGroup(pEvent->get_wPatchGroup(),id);
			    The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,CONNECTED);

				//// save patch group size 
				//std::vector<RadioResource> members = The_StatusManager::instance()->getGroupMembers(id);	
				//patchGroupSize[pEvent->get_wPatchGroup()] = members.size();
				//members.clear();
				//patchMemberStatus[pEvent->get_wPatchGroup()].clear();
				//for (int i = 0; i < members.size(); i++)
				//	patchMemberStatus[pEvent->get_wPatchGroup()].push_back(UNASSIGNED_GROUP_STATUS);

			    m_pCommand->connectPatchGroup(resource,IRadioMFTAgentCommand::ACTION,pEvent->get_wPatchGroup());
		    }
		    catch(...)
		    {
			    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
				    "Cannot connect Patch group with resource [%d]",id);
		    }
		} else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Resource Type is NIL, cannot process further");
		}
	}

	FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::onGetFreePatchGroupError()
{
	FUNCTION_ENTRY("onGetFreePatchGroupError");

	RadioResource resource = The_StatusManager::instance()->getPendingConnectResource(RR_PATCHGROUP);
	The_StatusManager::instance()->clearPendingConnectResource(RR_PATCHGROUP);
	The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource, NAG);
	sendErrorMessage("Unable to get free patch group");
	//m_pCallback->callback_operationCancelMessage(RA_SELECT_STATE, resource);

	FUNCTION_EXIT;
}


void RadioMFTAgentRemoteApiEventImpl::onPatchGroupStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
	//Reconstruct state for existing Patch
	//Group

    FUNCTION_ENTRY("onPatchGroupStatus");

	McaPatchGroupStatus* pEvent = 
		dynamic_cast<McaPatchGroupStatus*>(pConEvent);

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

void RadioMFTAgentRemoteApiEventImpl::onPatchMemberStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onPatchMemberStatus");

	McaPatchMemberStatus* pEvent = 
		dynamic_cast<McaPatchMemberStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "PatchMemberStatus is NULL");
	
	AssociatedSpeaker state;
	RadioResource member = The_StatusManager::instance()->getResourcebyChannel(pEvent->get_channel());
	unsigned short groupNo = pEvent->get_wPatchGroup();
	std::vector<short>::iterator iter;

	switch(pEvent->get_wStatus()) {
		case MCA_ADDED:		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Patch member %u added", member.id);
			state = SELECTED_SPEAKER;

			The_StatusManager::instance()->setFeatureAttributeIsSelected(member,state);

			if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
			{
				m_pCallback->callback_resourceSelectMessage(member,state);
			}
			m_pCommand->addNextPatchMember(member, The_StatusManager::instance()->getPendingConnectResource(RR_PATCHGROUP).id, groupNo);
			break;
		case MCA_DELETED:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Patch member %u removed", member.id);
			state = UNSELECTED_SPEAKER;

			The_StatusManager::instance()->setFeatureAttributeIsSelected(member,state);

			if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
			{
				m_pCallback->callback_resourceSelectMessage(member,state);
			}	
			break;
		case MCA_IN_USE:

			break;
		case MCA_PENDING:
			break;
		case MCA_PENDING_AND_IN_USE:
			{
				sendErrorMessage("Add patch member error");
				RadioResource res = The_StatusManager::instance()->getPendingConnectResource(RR_PATCHGROUP);
				The_StatusManager::instance()->clearPendingConnectResource(RR_PATCHGROUP);
				m_pCommand->releasePatchGroup(res);
				break;
			}
	}

	//if (TA_IRS_App::Mca::PatchMemberStatus::Patch_Member_Added == pEvent->get_wStatus()) {		

	//} else {
	//	switch(
	//}

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


void RadioMFTAgentRemoteApiEventImpl::onPatchMemberRefresh(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onPatchMemberRefresh");

	McaPatchMemberRefresh* pEvent = 
		dynamic_cast<McaPatchMemberRefresh*>(pConEvent);	

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

	//std::vector<BYTE>::iterator iter = lcidStatus.begin();

	//for ( ; iter!=lcidStatus.end(); ++iter)
	//{
	//	for (int i =7; i>=0; i--)
	//	{
	//		RadioResource member = The_StatusManager::instance()->getResourcebyChannel(channel);

	//		bool state = (*iter >> i) & 1;

	//		if (state)
	//		{
	//			try
	//			{
	//				TA_IRS_App::Mca::DeletePatchMember pDelete(channel);			
	//				pDelete.execute();
	//			}
	//			catch(TA_IRS_App::ConsoleDispatchException& e)
	//			{
	//				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
	//			}
	//		}
	//		channel++;
	//	}
	//}
	
    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteApiEventImpl::onGetFreeMselGroup(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id)
{
    FUNCTION_ENTRY("onGetFreeMselGroup");
    
	McaFreeMselGroupStatus * pEvent = 
		dynamic_cast<McaFreeMselGroupStatus*>(pConEvent);

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
			    m_pCommand->connectMselGroup(resource,IRadioMFTAgentCommand::ACTION,pEvent->get_wMselGroup());		
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

void RadioMFTAgentRemoteApiEventImpl::onMselGroupStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onMselGroupStatus");

	McaMselGroupStatus* pEvent = 
		dynamic_cast<McaMselGroupStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "MselGroupStatus is NULL");


	//if (resource.type != RR_NIL)
	//{
		if ( pEvent->get_wStatus() == TA_IRS_App::Mca::MselGroupStatus::Multi_Select_Group_Opened)
		{
			RadioResource resource = The_StatusManager::instance()->getResourceByGroupNo(pEvent->get_wMselGroup(), RR_MSELECT);
			The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,CONNECTED);
			//m_pCommand->connectMselGroup(resource,IRadioMFTAgentCommand::ACTION,pEvent->get_wMselGroup());	
			m_pCallback->callback_resourceSelectMessage(resource, SELECTED_SPEAKER);
		}    
		else
		{
			RadioResource resource = The_StatusManager::instance()->getPendingConnectResource(RR_MSELECT);
			//m_pCommand->releaseMselGroup(resource,pEvent->get_wMselGroup(),IRadioMFTAgentCommand::ACTION);
			
			if (The_StatusManager::instance()->getFeatureAttributeIsGrpConnected(resource) != RELEASED) {
				The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,RELEASED);
				m_pCommand->releaseMselGroup(resource, false);	
			}
			if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE ) {
				m_pCallback->callback_resourceSelectMessage(resource, UNSELECTED_SPEAKER);
			}
			The_StatusManager::instance()->clearPendingConnectResource(RR_MSELECT);
		}
	//}

    FUNCTION_EXIT;
}



void RadioMFTAgentRemoteApiEventImpl::onOpenMselError(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onMselGroupStatus");

	McaOpenMselError* pEvent = 
		dynamic_cast<McaOpenMselError*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "MselGroupStatus is NULL");

	RadioResource resource = The_StatusManager::instance()->getResourceByGroupNo(pEvent->get_wMselGroup(), RR_MSELECT);

	//The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,RELEASED);

	//m_pCallback->callback_operationCancelMessage(RA_SELECT_STATE, resource);
	m_pCommand->releaseMselGroup(The_StatusManager::instance()->getPendingConnectResource(RR_MSELECT), true);

    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteApiEventImpl::onCloseMselError(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onMselGroupStatus");

	McaCloseMselError* pEvent = 
		dynamic_cast<McaCloseMselError*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "MselGroupStatus is NULL");

	RadioResource resource = The_StatusManager::instance()->getPendingConnectResource(RR_MSELECT);

	m_pCallback->callback_resourceSelectMessage(resource, UNSELECTED_SPEAKER);

	The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,NAG);
	The_StatusManager::instance()->clearPendingConnectResource(RR_MSELECT);

	if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE ) {
		std::vector<RadioResource> members = The_StatusManager::instance()->getGroupMembers(resource.id);
		std::vector<RadioResource>::iterator iter = members.begin();
		for(; iter!=members.end(); iter++) {		
			m_pCallback->callback_resourceSelectMessage(*iter,UNSELECTED_SPEAKER);
		}
		RadioResource mselect;
		mselect.id = resource.id;
		mselect.type = RR_MSELECT;
		m_pCallback->callback_resourceSelectMessage(mselect, UNSELECTED_SPEAKER);
	}

	//m_pCallback->callback_operationCancelMessage(RA_DESELECT_STATE, resource);

    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::onMselMemberStatus(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onMselMemberStatus");

	McaMselMemberStatus* pEvent = dynamic_cast<McaMselMemberStatus*>(pConEvent);

	TA_ASSERT( pEvent != NULL, "MselMemberStatus is NULL");

	RadioResource resource = The_StatusManager::instance()->getResourcebyChannel(pEvent->get_channel());

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "onMselMemberStatus, Msel Radio Resource: id=%u, ssi=%u, alias=%s", resource.id, resource.ssi, resource.alias);


	
	switch(pEvent->get_wStatus()) {
		case MCA_ADDED:		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Multi-select member %u added", resource.id);
			m_pCommand->addNextMselMember(resource, The_StatusManager::instance()->getPendingConnectResource(RR_MSELECT).id, pEvent->get_wMselGroup());
			break;
		case MCA_DELETED:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Multi-select member %u removed", resource.id);
			break;
	}


    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteApiEventImpl::onMselMemberRefresh(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
    FUNCTION_ENTRY("onMselMemberRefresh");

	McaMselMemberRefresh* pEvent = 
		dynamic_cast<McaMselMemberRefresh*>(pConEvent);	

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

    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::onMcdbRefreshComplete(short mode)
{
	if (RadioResourceConfigFacade::DB_INIT_ISCS_MODE == mode) {
		The_StatusManager::instance()->initDataFromDb();
	} else if (RadioResourceConfigFacade::DB_INIT_RADIO_MODE == mode) {
		The_StatusManager::instance()->initDataFromMcc();
	} else if (RadioResourceConfigFacade::DB_SYNC_RADIO_MODE == mode) {
		The_StatusManager::instance()->syncFromMcc(RR_NIL,m_pCallback);
	} else if (RadioResourceConfigFacade::DB_UPDATE_MODE == mode) {
		// TODO, need to have the resource type
		The_StatusManager::instance()->syncFromMcc(m_curUpdateType, m_pCallback);
	}

	m_pApStateMgr->setApState(RA_OPERATION_MODE);
	
	try
	{
        RadioResource res = The_StatusManager::instance()->getPrivateCallResource();
        if (res.type != RR_NIL)
        {
    		m_pCommand->assignResource(res);
        }
	}
	catch(const TA_IRS_App::ConsoleDispatchException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,e.what());
	}
	catch(...)
	{
	}

	if (RadioResourceConfigFacade::DB_INIT_ISCS_MODE == mode || RadioResourceConfigFacade::DB_INIT_RADIO_MODE == mode) {
		m_bOperationMode = true;

		// finished, we are now in operation mode
		m_pCallback->callback_applicationStateMessage(RA_OPERATION_MODE,0);
	}

}



// ++TD14322
void RadioMFTAgentRemoteApiEventImpl::onAddMselMemberWrn(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id)
{
	McaAddMselMemberWarning* pEvent = 
		dynamic_cast<McaAddMselMemberWarning*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to add msel member " << res.alias;

    sendErrorMessage(ostr.str());
}

void RadioMFTAgentRemoteApiEventImpl::onAddMselMemberErr(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id)
{
	McaAddMselMemberError* pEvent = 
		dynamic_cast<McaAddMselMemberError*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to add msel member " << res.alias;

	m_pCommand->releaseMselGroup(The_StatusManager::instance()->getPendingConnectResource(RR_MSELECT), true);

    sendErrorMessage(ostr.str());
}

void RadioMFTAgentRemoteApiEventImpl::onDeleteMselMemberErr(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id)
{
	McaDeleteMselMemberError* pEvent = 
		dynamic_cast<McaDeleteMselMemberError*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to delete msel member " << res.alias;

	RadioResource resource = The_StatusManager::instance()->getResourceByID(id, RR_TALKGROUP);
	if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE ) {
		m_pCallback->callback_resourceSelectMessage(resource, UNSELECTED_SPEAKER);
	}

    sendErrorMessage(ostr.str());
}

void RadioMFTAgentRemoteApiEventImpl::onAddPatchMemberWrn(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id)
{
	McaAddPatchMemberWarning* pEvent = 
		dynamic_cast<McaAddPatchMemberWarning*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to add patch member " << res.alias;

    sendErrorMessage(ostr.str());
}

void RadioMFTAgentRemoteApiEventImpl::onAddPatchMemberErr(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id)
{
	McaAddPatchMemberError* pEvent = 
		dynamic_cast<McaAddPatchMemberError*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "onAddPatchMemberErr: %u (0x%x)", pEvent->get_wErrorNum(), pEvent->get_wErrorNum());

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to add patch member " << res.alias;

    sendErrorMessage(ostr.str());

	m_pCommand->releasePatchGroup(The_StatusManager::instance()->getPendingConnectResource(RR_PATCHGROUP));
}

void RadioMFTAgentRemoteApiEventImpl::onDeletePatchMemberErr(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ResourceIdentifier id)
{
	McaDeletePatchMemberError* pEvent = 
		dynamic_cast<McaDeletePatchMemberError*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    RadioResource res = The_StatusManager::instance()->getConsoleResourceByID(id);
    std::ostringstream ostr;
    ostr << "Unable to delete patch member " << res.alias;

    sendErrorMessage(ostr.str());

	RadioResource resource = The_StatusManager::instance()->getResourceByID(id, RR_TALKGROUP);
	if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE ) {
		m_pCallback->callback_resourceSelectMessage(resource, UNSELECTED_SPEAKER);
	}
}

void RadioMFTAgentRemoteApiEventImpl::onBeginGeneralXmitWrn(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
	McaBeginGeneralTransmitWarning* pEvent = 
		dynamic_cast<McaBeginGeneralTransmitWarning*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    sendErrorMessage("Warning beginning general transmit");
}

void RadioMFTAgentRemoteApiEventImpl::onBeginGeneralXmitErr(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
	McaBeginGeneralTransmitError* pEvent = 
		dynamic_cast<McaBeginGeneralTransmitError*>(pConEvent);	
	TA_ASSERT( pEvent != NULL, "Type mismatch");

    sendErrorMessage("Error beginning general transmit");
}

void RadioMFTAgentRemoteApiEventImpl::sendErrorMessage(const std::string msg)
{
    m_pCallback->callback_applicationErrorMessage(msg);
}

// TD14322++

IRadioMFTAgentCallback::ResourceActivityState RadioMFTAgentRemoteApiEventImpl::convertResourceTXStatus(
	int status)
{
    FUNCTION_ENTRY("convertResourceTXStatus");

	IRadioMFTAgentCallback::ResourceActivityState state = 
			IRadioMFTAgentCallback::No_Resource_Activity;

	
	switch( status )
	{
		case TA_IRS_App::Mca::ResourceTransmitStatus::Resource_Transmit_Begun:
		case TA_IRS_App::Mca::ResourceTransmitStatus::Resource_Transmit_Busy:	
			state = IRadioMFTAgentCallback::Resource_Tx_Begun;
			break;	
		case TA_IRS_App::Mca::ResourceTransmitStatus::Resource_Transmit_Callback:		
		case TA_IRS_App::Mca::ResourceTransmitStatus::Resource_Transmit_Without_Radio:	
		case TA_IRS_App::Mca::ResourceTransmitStatus::Resource_Transmit_Data_Busy:
		case TA_IRS_App::Mca::ResourceTransmitStatus::Resource_Transmit_Pending:
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

void RadioMFTAgentRemoteApiEventImpl::updateGroupResource(TA_IRS_App::RemoteAPIRadioEvent* pConEvent, ACE_Future<FutureResultT>& future)
{
    FUNCTION_ENTRY("updateGroupResource");

	McaAssignmentStatus* pEvent = 
		dynamic_cast<McaAssignmentStatus*>(pConEvent);

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
	else if (result.m_state == BROADCAST_TRAIN) {
		resource.type = (RadioResourceType)result.m_resourceType;
		resource.id = (unsigned long)result.m_resourceId;
		m_pCommand->broadcastTrain(resource, IRadioMFTAgentCommand::ACTION);
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::processSelectResourceFunc(ACE_Future<FutureResultT>& future)
{
    FUNCTION_ENTRY("processSelectResourceFunc");

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
	} else if (result.m_state == ADD_PATCH_MEMBER) 
	{
		// All patch member already added, resource also selected now update ui
		RadioResource resource;
		resource.id = result.m_resourceId;
		resource.type = RR_PATCHGROUP;

		AssociatedSpeaker state = SELECTED_SPEAKER;

		if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
		{
			m_pCallback->callback_resourceSelectMessage(resource,state);
		}		
        The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource, CONNECTED);
	} else if (result.m_state == DELETE_PATCH_MEMBER) 
	{
		// All patch member already deleted, resource also deselected now update ui
		RadioResource resource;
		resource.id = result.m_resourceId;
		resource.type = RR_PATCHGROUP;

		if ( m_pApStateMgr->getApState() == RA_OPERATION_MODE )
		{
			m_pCallback->callback_resourceSelectMessage(resource,UNSELECTED_SPEAKER);
		}

		// update status manager		
		The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource, RELEASED);
		The_StatusManager::instance()->clearPendingConnectResource(RR_PATCHGROUP);
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
void RadioMFTAgentRemoteApiEventImpl::onEmergencyAlarmTone(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
	McaToneStatus* pEvent1 = 
		dynamic_cast<McaToneStatus*>(pConEvent);

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

void RadioMFTAgentRemoteApiEventImpl::reconnect()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Radio event handler re-initializing");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "De-initializing system status (db, audio and peripheral status)");
	initialized = false;
	
	mccReady[DB_STATUS] = false;
	mccReady[AUDIO_STATUS] = false;
	mccReady[PERIPHERAL_STATUS] = false;

	// reconnection is requested, meaning the operation was halted due to some reason
	// need to reset back to false
	m_bOperationMode = false;
}

void RadioMFTAgentRemoteApiEventImpl::onNewDatabaseAvailable(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
	FUNCTION_ENTRY("onNewDatabaseAvailable");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Received New Database Available. Grant the update");
	m_pCommand->updateDbGrant(true);
	FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::onDatabaseUpdateNotification(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
	FUNCTION_ENTRY("onDatabaseUpdateNotification");
	
	bool status = The_StatusManager::instance()->getRadioStatus() || The_StatusManager::instance()->getIscsStatus();

	if (!status) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MFT Agent is in normal mode. Do not process database notification from MCC");
		return;
	}

	McdiDatabaseUpdateNotification* pEvent = dynamic_cast<McdiDatabaseUpdateNotification*>(pConEvent);

	int updateType = pEvent->get_wDataGroup();
	if (McdiDatabaseUpdateNotification::DB_UPDATE_TALKGROUP == updateType) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MFT Agent is in degraded mode. Now synchronizing talkgroup resource with MCC");
		m_curUpdateType = RR_TALKGROUP;
		The_ResourceConfig::instance()->startRetrieveDB(RadioResourceConfigFacade::DB_UPDATE_MODE);
	} else if (McdiDatabaseUpdateNotification::DB_UNIT_ALIASES == updateType) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MFT Agent is in degraded mode. Now synchronizing radio resource with MCC");
		m_curUpdateType = RR_RADIO;
		The_ResourceConfig::instance()->startRetrieveDB(RadioResourceConfigFacade::DB_UPDATE_MODE);
	} else {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Not processing update type %u from MCC", updateType);
	}
	FUNCTION_EXIT;
}

void RadioMFTAgentRemoteApiEventImpl::onEndCallTerimination (TA_IRS_App::RemoteAPIRadioEvent * pConEvent)
{
	//check if event is End of Call
	// if yes process it if not ignore it
	
	McaPrivateCallStatus * pEvent = dynamic_cast<McaPrivateCallStatus*>(pConEvent);
	
	RadioResource resource;

	resource.id = pEvent->get_unit_id();
	resource.alias = pEvent->get_IndividualAlias();
	resource.type = RR_RADIO;

	m_pCallback->callback_callTerminated(resource);

}

void RadioMFTAgentRemoteApiEventImpl::onSetupCallFailure ()
{
	FUNCTION_ENTRY("onSetupCallFailure");
	
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"onSetupCallFailure");

	m_pCallback->callback_callSetupFailure();

	FUNCTION_EXIT;
}

bool RadioMFTAgentRemoteApiEventImpl::onEndPrivCallErrHandler(TA_IRS_App::RemoteAPIRadioEvent* pConEvent)
{
	FUNCTION_ENTRY("onEndCallError");
	bool retVal = false;

	McaEndPrivateCallError* endPrivateErrEvent = dynamic_cast<McaEndPrivateCallError*>(pConEvent);

	if (NULL != endPrivateErrEvent)
	{
		//Check Error code
		switch (endPrivateErrEvent->get_wErrorNum())
		{
		case MCA_RSC_TRANSMITTING:
			retVal = true;
			break;
		}
	}

	FUNCTION_EXIT;
	return retVal;
}

void RadioMFTAgentRemoteApiEventImpl::onResendEndPrvCall()
{
	FUNCTION_ENTRY("onResendEndPrvCall");

	//Send an error message to the operator.
	//If the operator clicks OK, then endPrivateCall command will send again.
	if (IDOK == sendErrorMessage(UE_190013))
	{
		RadioResource resource = The_StatusManager::instance()->getPrivateCallResource();
		m_pCommand->endPrivateCall(resource);

	}

	FUNCTION_EXIT;
}

long RadioMFTAgentRemoteApiEventImpl::sendErrorMessage(long messageID)
{
	FUNCTION_ENTRY("sendErrorMessage");

	FUNCTION_EXIT;
	return m_pCallback->callback_sendErrorMessage(messageID);
}
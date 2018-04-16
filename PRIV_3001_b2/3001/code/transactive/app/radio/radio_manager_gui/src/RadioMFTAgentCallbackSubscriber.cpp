/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioMFTAgentCallbackSubscriber.cpp $
  * @author:  Frik Stuart
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioManager.h"
#include "app/radio/radio_manager_gui/src/RadioMFTAgentCallbackSubscriber.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentHelper.h"
#include "app/radio/radio_manager_gui/src/DebugMsg.h"
#include "app/radio/radio_manager_gui/src/RadioErrorMsg.h"
#include "app/radio/radio_manager_gui/src/RadioCallForwardingPage/CRadioCallForwardingPage.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/RadioEntityData.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/message/types/RadioAudit_MessageTypes.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RadioMFTAgentCallbackSubscriber::RadioMFTAgentCallbackSubscriber() : 
	m_commsSender(NULL),
	m_entityKey(0),
	m_entitySubsystem(0),
	m_entityLocation(0)
{
    m_commsSender = TA_Base_Core::MessagePublicationManager::getInstance().
        getCommsMessageSender(TA_Base_Core::RadioComms::Context);

    TA_Base_Core::IEntityData* entity = NULL;
	try 
	{
		entity = RadioMFTAgentHelper::getRadioMFTAgentEntity();
		m_entityLocation = entity->getLocation();
		m_entityKey = entity->getKey();
		m_entitySubsystem = entity->getSubsystem();
	}
	catch (TA_Base_Core::DatabaseException& e) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Database error while initializing RadioMFTAgentCallbackSubscriber");
	}

	if (NULL != entity) {
		delete entity;
	}
}

RadioMFTAgentCallbackSubscriber::~RadioMFTAgentCallbackSubscriber()
{
	if (m_commsSender != NULL)
	{
		delete m_commsSender;
		m_commsSender = NULL;
	}
}

void RadioMFTAgentCallbackSubscriber::applicationStateMessage( TA_IRS_Bus::Radio::EApplicationState state, CORBA::Short percent )
{
    FUNCTION_ENTRY("applicationStateMessage");

	if ( state == TA_IRS_Bus::Radio::AP_INITIALISATION_MODE)
	{
		//Fixed TD14271
		The_RadioOperations::instance()->initialiseMode();
	}
	else if ( state == TA_IRS_Bus::Radio::AP_OPERATION_MODE)
	{
		The_RadioOperations::instance()->databaseReadyEvent();
		The_RadioOperations::instance()->operationMode();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC Status is OK");
		The_RadioSystemStatus::instance()->updateGX("Type[MCCConnectionStatus] StatusOK");
	}
	else if ( state == TA_IRS_Bus::Radio::AP_CONFIGURATION_MODE )
	{
		RadioResource resource;
		The_RadioOperations::instance()->editMode(resource, true); 
	}
	else if (state == TA_IRS_Bus::Radio::AP_APIFAILURE_MODE) //fixed TD14271
	{
		RadioResource resource;
		The_RadioOperations::instance()->editMode(resource, true); //to disable all the GUI elements
		The_RadioOperations::instance()->apiFailedMode();//to show the error message.
	}
	else if (state == TA_IRS_Bus::Radio::AP_CONNECTIONFAILURE_MODE)
	{
		RadioResource resource;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC Status is NOK");
 		The_RadioSystemStatus::instance()->updateGX("Type[MCCConnectionStatus] StatusFail");
		The_RadioOperations::instance()->editMode(resource, true); //to disable all the GUI elements
		The_RadioOperations::instance()->connectionFail();//to show the error message.
	}
	else if (state == TA_IRS_Bus::Radio::AP_MCC_CONNECTION_ESTABLISH)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC Object is created, mcc is ok");	
		// re-initilise failure flags
		The_RadioOperations::instance()->initialise();
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentCallbackSubscriber::applicationErrorMessage(const char* message )
{
    FUNCTION_ENTRY("applicationErrorMessage");

    //AddErrorMessage(message);
	The_RadioSystemStatus::instance()->showErrorMessageFromRadio();
    
    FUNCTION_EXIT;
}

void RadioMFTAgentCallbackSubscriber::operationCancelMessage(
			TA_IRS_Bus::Radio::EApiState state,
			const TA_IRS_Bus::Radio::RadioResource& resource)
{
	FUNCTION_ENTRY("operationCancelMessage");

	RadioResource data;
	ApiState stateData = RadioMFTAgentHelper::convertApiStateFromIDL(state);
	RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, data);

	The_RadioOperations::instance()->cancelApiOperation(stateData,data);

	FUNCTION_EXIT;
}

void RadioMFTAgentCallbackSubscriber::groupStateMessage(
    const TA_IRS_Bus::Radio::RadioResource& resource, TA_IRS_Bus::Radio::EGroupConnectionState state)
{
    FUNCTION_ENTRY("groupStateMessage");

	RadioResource data;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, data);

	The_RadioOperations::instance()->onGroupConnectionStateEvent(data, (GroupConnectionState) state);

    FUNCTION_EXIT;
}

void RadioMFTAgentCallbackSubscriber::resourceAssignmentMessage( 
	const TA_IRS_Bus::Radio::RadioResource& resource, 
	CORBA::Short buttonSet, CORBA::Short buttonRef, TA_IRS_Bus::Radio::EAssignmentState status )
{
    FUNCTION_ENTRY("resourceAssignmentMessage");

	/*
	 * @note This has different siginificance to the Motorola API depending upon whether the resource
	 * is a Motorola managed resource, or a Transactive managed(i.e configurable) resource   
	 */

	RadioResource data;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, data);

	if ( status == TA_IRS_Bus::Radio::ASSIGNED )
	{
		// ?? include buttonSet
		The_RadioOperations::instance()->onResourceAssignmentEvent(data, buttonRef);
	}
	else
	{
		The_RadioOperations::instance()->onResourceDeAssignmentEvent(data);
	}
	
    FUNCTION_EXIT;
	
}


void RadioMFTAgentCallbackSubscriber::resourceSelectMessage ( 
	const TA_IRS_Bus::Radio::RadioResource& resource, 
	TA_IRS_Bus::Radio::ESpeakerState status)
{
    FUNCTION_ENTRY("resourceSelectMessage ");

	RadioResource data;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, data);

	if ( status == TA_IRS_Bus::Radio::SPEAKER_SELECT )
	{
		The_RadioOperations::instance()->onSelectSpeakerSelectedEvent(data);
	}
	else
	{
		The_RadioOperations::instance()->onSelectSpeakerUnselectedEvent(data);
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentCallbackSubscriber::resourceVolumeMessage(
	const TA_IRS_Bus::Radio::RadioResource& resource, 
	TA_IRS_Bus::Radio::ESpeakerState speaker, 
	CORBA::Short volume)
{
    FUNCTION_ENTRY("resourceVolumeMessage");


	RadioResource data;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, data);
	
	The_RadioOperations::instance()->onVolumeChangedEvent(data,volume);
	 
	FUNCTION_EXIT;
}


void RadioMFTAgentCallbackSubscriber::inboundCallMessage( 
	const TA_IRS_Bus::Radio::RadioResource& caller, 
	TA_IRS_Bus::Radio::EInboundCallState status)
{
    FUNCTION_ENTRY("inboundCallMessage");

	RadioResource data;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(caller, data);
	
	if (status == TA_IRS_Bus::Radio::IC_NEW)
		The_RadioOperations::instance()->onInboundCallEvent(data);
	else
		The_RadioOperations::instance()->onInboundCallCanceledEvent(data);	

	 FUNCTION_EXIT;
}


void RadioMFTAgentCallbackSubscriber::privateCallMessage( 
	const TA_IRS_Bus::Radio::RadioResource& caller, 
	TA_IRS_Bus::Radio::EPrivateCallState status)
{
    FUNCTION_ENTRY("privateCallMessage");

	RadioResource data;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(caller, data);
	TCHAR buff[200];
	
	LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"privateCallMessage : caller Id : %d, alias : %s status : %d",
				caller.id, caller.alias,(int)status);

	short callForwardingState = The_RadioOperations::instance()->getCallForwardingState();

	if (status == TA_IRS_Bus::Radio::PC_INCOMING)
	{
		if (CRadioCallForwardingPage::CALL_FORWARDING_UNCONDITIONAL == callForwardingState) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Radio Call is in Unconditional Call Forwarding State. Ignoring the call update");
			return;
		} 
		else if (CRadioCallForwardingPage::NEVER_FORWARD == callForwardingState) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Radio Call is in normal mode (no call forwarding). Updating the Radio Manager UI");
			The_RadioOperations::instance()->onPrivateCallRinging(data);
		} 
		else 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Radio Call is in Not Reachable Call Forwarding State. Updating the Radio Manager UI");
			The_RadioOperations::instance()->onPrivateCallRinging(data);
		}

		wsprintf(buff, "incoming call : id=%d ssi=%d", data.id, data.ssi);
		AddDebugMessage(buff);

	    if (m_commsSender != NULL)
		{
			TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent callEvent;
			callEvent.caller = caller;
			callEvent.status = status;
			callEvent.isEmergencyMode = false;

			time_t now;
			time(&now);

			callEvent.timeOfCall = now;
			
			CORBA::Any msg;
			msg <<= callEvent;

			wsprintf(buff, "Send comms: entity=%d, subsys=%d, loc=%d", m_entityKey, m_entitySubsystem, m_entityLocation);

			AddDebugMessage(buff);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending Incoming state to call banner %u", caller.id);
			

			m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStatus,
			m_entityKey, msg,
			m_entitySubsystem, m_entityLocation);
		}		
	}
	else if (status == TA_IRS_Bus::Radio::PC_ESTABLISHED)
	{		
		unsigned long trainId = The_RadioOperations::instance()->isDriverCall(data.id);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing driver call, trainId = %u", trainId);
		
		The_RadioOperations::instance()->onPrivateCallEstablished(data);

		TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent callEvent;
		callEvent.caller = caller;

        callEvent.caller.id = data.id;
        callEvent.caller.ssi = data.ssi;
        callEvent.caller.type = RadioMFTAgentHelper::convertResourceType(data.type);
        
		callEvent.status = status;
		callEvent.isEmergencyMode = false;

		time_t now;
		time(&now);

		callEvent.timeOfCall = now;
		
		if (trainId != 0) 
		{
			callEvent.caller.id = trainId;
			callEvent.caller.ssi = trainId;
		}

		CORBA::Any msg;
		msg <<= callEvent;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending ESTABLISHED state to call banner %u", caller.id);
		m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStatus,
		m_entityKey, msg,
		m_entitySubsystem, m_entityLocation);

	}
	else if (status == TA_IRS_Bus::Radio::PC_NOT_INVOLVED)
	{
		unsigned long trainId = The_RadioOperations::instance()->isDriverCall(data.id);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing driver call, trainId = %u", trainId);
		if (trainId != 0) 
		{
			The_RadioOperations::instance()->setNoDriverCall(data.id);
			The_RadioOperations::instance()->onResetDriverCall(trainId);
		} 
		
		The_RadioOperations::instance()->onPrivateCallEnded(data);
		
		wsprintf(buff, "ending call : id=%d ssi=%d", data.id, data.ssi);
		AddDebugMessage(buff);
		
		TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent callEvent;
		callEvent.caller = caller;

        callEvent.caller.id = data.id;
        callEvent.caller.ssi = data.ssi;
        callEvent.caller.type = RadioMFTAgentHelper::convertResourceType(data.type);
        
		callEvent.status = status;
		callEvent.isEmergencyMode = false;

		time_t now;
		time(&now);

		callEvent.timeOfCall = now;

		if (trainId != 0) {
			callEvent.caller.id = trainId;
			callEvent.caller.ssi = trainId;
		}
		
		CORBA::Any msg;
		msg <<= callEvent;

		wsprintf(buff, "not involved: id=%d ssi=%d entity=%d, subsys=%d, loc=%d", data.id, data.ssi, m_entityKey, m_entitySubsystem, m_entityLocation);

		AddDebugMessage(buff);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending END state to call banner %u", caller.id);
		m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStatus,
		m_entityKey, msg,
		m_entitySubsystem, m_entityLocation);

	}
	else if (status == TA_IRS_Bus::Radio::PC_OUTGOING) //Fixed TD#14131
	{
		unsigned long trainId = The_RadioOperations::instance()->isDriverCall(data.id);
		The_RadioOperations::instance()->onPrivateCallProcessed(data);		
		TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent callEvent;
		callEvent.caller = caller;

        callEvent.caller.id = data.id;
        callEvent.caller.ssi = data.ssi;
        callEvent.caller.type = RadioMFTAgentHelper::convertResourceType(data.type);
        
		callEvent.status = status;
		callEvent.isEmergencyMode = false;

		time_t now;
		time(&now);

		callEvent.timeOfCall = now;
		
		if (trainId != 0) 
		{
			callEvent.caller.id = trainId;
			callEvent.caller.ssi = trainId;
		}

		CORBA::Any msg;
		msg <<= callEvent;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending OUTGOING state to call banner %u", caller.id);
		m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStatus,
		m_entityKey, msg,
		m_entitySubsystem, m_entityLocation);
	}

	FUNCTION_EXIT;	
}




void RadioMFTAgentCallbackSubscriber::privateCallMessageRejected( 
	const TA_IRS_Bus::Radio::RadioResource& caller)
{
	FUNCTION_ENTRY("privateCallMessageRejected");

	RadioResource data;
	RadioMFTAgentHelper::convertRadioResourceFromIDL(caller, data);

	LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"privateCallMessageRejected : caller Id : %d, alias : %s ",
		caller.id, caller.alias);

	The_RadioOperations::instance()->onPrivateCallRejected(data);
	
	FUNCTION_EXIT;	
}

void RadioMFTAgentCallbackSubscriber::resourceActivityMessage( 
	const TA_IRS_Bus::Radio::RadioResource& resource, 
	TA_IRS_Bus::Radio::EResourceActivityType status)
{
    FUNCTION_ENTRY("resourceActivityMessage");

	RadioResource data;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, data);
	
	if ((status == TA_IRS_Bus::Radio::NO_ACTIVITY) || (status == TA_IRS_Bus::Radio::RX_ENDED))
	{
		The_RadioOperations::instance()->onResourceActivityEndedEvent(data);
	}
	else
	{
		The_RadioOperations::instance()->onResourceActivityEvent(data);
	}
	
	FUNCTION_EXIT;
}


void RadioMFTAgentCallbackSubscriber::resourcePTTMessage( 
	const TA_IRS_Bus::Radio::RadioResource& listener, 														 
	const char*  srcAlias, 
	TA_IRS_Bus::Radio::EPTTState status)
{
    FUNCTION_ENTRY("resourcePTTMessage");

	RadioResource receiver;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(listener, receiver);

	RadioResource source;
	source.alias = srcAlias;
    //RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, source);

    if ( status == TA_IRS_Bus::Radio::PTT_PUSHED)
	{
		The_RadioOperations::instance()->onInboundPTTEvent(receiver,source);
	}
	else
	{
		The_RadioOperations::instance()->onInboundPTTReleasedEvent(receiver);
	}
	
	FUNCTION_EXIT;	
}


void RadioMFTAgentCallbackSubscriber::resourceEmergencyModeMessage( 
	const TA_IRS_Bus::Radio::RadioResource& resource, 
	TA_IRS_Bus::Radio::EmergencyModeState status, 
	const TA_IRS_Bus::Radio::RadioResource& changedBy)
{
    FUNCTION_ENTRY("resourceEmergencyModeMessage");

	RadioResource data;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, data);

	// TD17224 - pass in the radio unit that triggered the emergency
	RadioResource rUnit;
	RadioMFTAgentHelper::convertRadioResourceFromIDL(changedBy, rUnit);

	switch(status)
	{
		case TA_IRS_Bus::Radio::RE_UNACK_EMERGENCY:
			The_RadioOperations::instance()->onResourceEmergencyEvent(data,rUnit);
			// TD18166
			{
				TA_ASSERT(m_commsSender != NULL, "m_commsSender is NULL!");
				TA_IRS_Bus::Radio::RadioResource radioUnit;
				RadioMFTAgentHelper::convertRadioResourceToIDL(rUnit,radioUnit);
				radioUnit.id = radioUnit.ssi;
				TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent callEvent;
				callEvent.caller = radioUnit;
				callEvent.status = TA_IRS_Bus::Radio::PC_INCOMING;
				callEvent.isEmergencyMode = true;

				time_t now;
				time(&now);

				callEvent.timeOfCall = now;
				
				CORBA::Any msg;
				msg <<= callEvent;

				m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStatus,
				m_entityKey, msg,
				m_entitySubsystem, m_entityLocation);
			}
			break;

		case TA_IRS_Bus::Radio::RE_EMERGENCY:
			The_RadioOperations::instance()->onResourceEmergencyAcknowledgedEvent(data,rUnit);

			// TD18166
			{
				TA_ASSERT(m_commsSender != NULL, "m_commsSender is NULL!");
				TA_IRS_Bus::Radio::RadioResource radioUnit;
				RadioMFTAgentHelper::convertRadioResourceToIDL(rUnit,radioUnit);
				radioUnit.id = rUnit.ssi;
				TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent callEvent;
				callEvent.caller = radioUnit;
				callEvent.status = TA_IRS_Bus::Radio::PC_ESTABLISHED;
				callEvent.isEmergencyMode = true;

				time_t now;
				time(&now);

				callEvent.timeOfCall = now;
				
				CORBA::Any msg;
				msg <<= callEvent;

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Send Ack Emergency to CallBanner for Radio %d", radioUnit.ssi);

				m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStatus,
				m_entityKey, msg,
				m_entitySubsystem, m_entityLocation);
			}			
			// TD18166
			break;

		case TA_IRS_Bus::Radio::RE_NO_EMERGENCY:	
			The_RadioOperations::instance()->onResourceEmergencyEndedEvent(data,rUnit);			
			// TD18166
			{
				TA_ASSERT(m_commsSender != NULL, "m_commsSender is NULL!");
				TA_IRS_Bus::Radio::RadioResource radioUnit;
				RadioMFTAgentHelper::convertRadioResourceToIDL(rUnit,radioUnit);
				radioUnit.id = rUnit.ssi;
				TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent callEvent;
				callEvent.caller = radioUnit;
				callEvent.status = TA_IRS_Bus::Radio::PC_NOT_INVOLVED;
				callEvent.isEmergencyMode = true;

				time_t now;
				time(&now);

				callEvent.timeOfCall = now;
				
				CORBA::Any msg;
				msg <<= callEvent;

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Send Ack Emergency to CallBanner for Radio %d", radioUnit.ssi);

				m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStatus,
				m_entityKey, msg,
				m_entitySubsystem, m_entityLocation);
			}		
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentCallbackSubscriber::resourceAmbListeningMessage(
	const TA_IRS_Bus::Radio::RadioResource& resource,
	TA_IRS_Bus::Radio::EAmbienceListeningState status)
{
    FUNCTION_ENTRY("resourceAmbListeningMessage");

	RadioResource data;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, data);

	AmbienceListeningCallState alcState = ALC_NOT_INVOLVED;

	if (status==TA_IRS_Bus::Radio::ALC_INCOMING) 
		alcState = ALC_INCOMING;
	else if (status==TA_IRS_Bus::Radio::ALC_ESTABLISHED) 
		alcState = ALC_ESTABLISHED;
		
	The_RadioOperations::instance()->onAmbienceListeningEvent(data,alcState);

    FUNCTION_EXIT;
}

void RadioMFTAgentCallbackSubscriber::resourceChangeMessage(
	const TA_IRS_Bus::Radio::RadioResource& oldResourceC,
	const TA_IRS_Bus::Radio::RadioResource& newResourceC)
{
	FUNCTION_ENTRY("resourceChangeMessage");
	RadioResource oldResource;
	RadioResource newResource;

	RadioMFTAgentHelper::convertRadioResourceFromIDL(oldResourceC, oldResource);
	RadioMFTAgentHelper::convertRadioResourceFromIDL(newResourceC, newResource);

	if (RR_NIL != oldResource.type && RR_NIL != newResource.type) {
		// update resource
		// if need
		// but only if the id is different
		if (oldResource.id != newResource.id) {
			The_RadioOperations::instance()->notifyDbUpdate(oldResource, newResource);
		}		
	} else if (RR_NIL == oldResource.type && RR_NIL != newResource.type) {
		// new resource
		The_RadioOperations::instance()->notifyDbNew(newResource);
	} else if (RR_NIL != oldResource.type && RR_NIL == newResource.type) {
		// delete resource
		The_RadioOperations::instance()->notifyDbDelete(oldResource);
	} else if (RR_NIL == oldResource.type && RR_NIL == newResource.type) {
		The_RadioOperations::instance()->notifyDbFinish();
	}
	FUNCTION_EXIT;
}

void RadioMFTAgentCallbackSubscriber::privateCallRemoved(RadioResource& resource)
{
	FUNCTION_ENTRY("privateCallRemoved");	
	
	TA_IRS_Bus::Radio::RadioResource caller;
	RadioMFTAgentHelper::convertRadioResourceToIDL(resource,caller);

	TCHAR buff[200];
	TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent callEvent;
	callEvent.caller = caller;

	callEvent.caller.id = resource.id;
	callEvent.caller.ssi = resource.ssi;
	callEvent.caller.type = RadioMFTAgentHelper::convertResourceType(resource.type);

	callEvent.status = TA_IRS_Bus::Radio::PC_ESTABLISHED;
	callEvent.isEmergencyMode = false;

	time_t now;
	time(&now);

	callEvent.timeOfCall = now;

	CORBA::Any msg;
	msg <<= callEvent;

	wsprintf(buff, "not involved: id=%d ssi=%d entity=%d, subsys=%d, loc=%d", resource.id, resource.ssi, m_entityKey, m_entitySubsystem, m_entityLocation);

	AddDebugMessage(buff);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending END state to call banner %u", caller.id);
	m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStatus,
		m_entityKey, msg,
		m_entitySubsystem, m_entityLocation);

	FUNCTION_EXIT;
}

CORBA::Long RadioMFTAgentCallbackSubscriber::sendErrorMessage(CORBA::Long messageID)
{
	FUNCTION_ENTRY("sendErrorMessage");

	TA_Base_Bus::TransActiveMessage userMsg;
	FUNCTION_EXIT;

	return userMsg.showMsgBox(messageID);
}
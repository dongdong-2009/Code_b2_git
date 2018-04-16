/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioMFTAgentCallbackSubscriber.cpp $
  * @author:  Frik Stuart
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioMFTAgentCallbackSubscriber.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentHelper.h"
#include "app/radio/radio_manager_gui_icdv4/src/DebugMsg.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioErrorMsg.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/RadioEntityData.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

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
	m_entityLocation(0),
	m_bIsPrivateCallIncoming(true)
{
    m_commsSender = TA_Base_Core::MessagePublicationManager::getInstance().
        getCommsMessageSender(TA_Base_Core::RadioComms::Context);

    TA_Base_Core::IEntityData* entity = RadioMFTAgentHelper::getRadioMFTAgentEntity();
    m_entityLocation = entity->getLocation();
    m_entityKey = entity->getKey();
    m_entitySubsystem = entity->getSubsystem();

    delete entity;
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


    FUNCTION_EXIT;
}

void RadioMFTAgentCallbackSubscriber::applicationErrorMessage(const char* message )
{
    FUNCTION_ENTRY("applicationErrorMessage");

    //AddErrorMessage(message);
	The_RadioSystemStatus::instance()->showErrorMessageFromRadio();
    
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
	
	if (status == TA_IRS_Bus::Radio::PC_INCOMING)
	{
		m_bIsPrivateCallIncoming = true;
		The_RadioOperations::instance()->onPrivateCallRinging(data);

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
		
		CORBA::Any msg;
		msg <<= callEvent;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending ESTABLISHED state to call banner %u", caller.id);
		m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStatus,
		m_entityKey, msg,
		m_entitySubsystem, m_entityLocation);

	}
	else if (status == TA_IRS_Bus::Radio::PC_NOT_INVOLVED)
	{

		The_RadioOperations::instance()->onPrivateCallEnded(data);

		wsprintf(buff, "ending call : id=%d ssi=%d", data.id, data.ssi);
		AddDebugMessage(buff);

		if (m_bIsPrivateCallIncoming)
		{

			m_bIsPrivateCallIncoming = false;
			
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
			
			CORBA::Any msg;
			msg <<= callEvent;

			wsprintf(buff, "not involved: id=%d ssi=%d entity=%d, subsys=%d, loc=%d", data.id, data.ssi, m_entityKey, m_entitySubsystem, m_entityLocation);

			AddDebugMessage(buff);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending END state to call banner %u", caller.id);
			m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStatus,
			m_entityKey, msg,
			m_entitySubsystem, m_entityLocation);
		}
	}
	else if (status == TA_IRS_Bus::Radio::PC_OUTGOING) //Fixed TD#14131
	{
		The_RadioOperations::instance()->onPrivateCallProcessed(data);
	}

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
	const TA_IRS_Bus::Radio::RadioResource& resource, 
	TA_IRS_Bus::Radio::EPTTState status)
{
    FUNCTION_ENTRY("resourcePTTMessage");

	RadioResource receiver;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(listener, receiver);

	RadioResource source;
    RadioMFTAgentHelper::convertRadioResourceFromIDL(resource, source);

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

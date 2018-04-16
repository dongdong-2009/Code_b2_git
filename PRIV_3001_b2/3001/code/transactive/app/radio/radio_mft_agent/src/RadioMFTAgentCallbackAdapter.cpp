/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent/src/RadioMFTAgentCallbackAdapter.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif



#include "app/radio/radio_mft_agent/src/RadioMFTAgentCallbackAdapter.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentHelper.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentHelper.h"

RadioMFTAgentCallbackAdapter::RadioMFTAgentCallbackAdapter() :
m_entity(NULL)
{
    FUNCTION_ENTRY("RadioMFTAgentCallbackAdapter");
	
	m_commsSender = TA_Base_Core::MessagePublicationManager::getInstance().
					getCommsMessageSender(TA_Base_Core::RadioComms::Context);

	m_entity = RadioMFTAgentHelper::getRadioMFTAgentEntity();
	m_entityLocation = m_entity->getLocation();
	m_entityKey = m_entity->getKey();
	m_entitySubsystem = m_entity->getSubsystem();
	
	FUNCTION_EXIT;    
}


RadioMFTAgentCallbackAdapter::~RadioMFTAgentCallbackAdapter()
{
    FUNCTION_ENTRY("~RadioMFTAgentCallbackAdapter");
    FUNCTION_EXIT;    
}


void RadioMFTAgentCallbackAdapter::callback_applicationStateMessage(ApplicationState state, unsigned int percent)
{
    FUNCTION_ENTRY("callback_applicationStateMessage");

	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"CallBack ApplicationStateMsg : State : %d", state);

	try
	{
		while(m_callbackCollection.empty())
		{
			Sleep(100);
		}
		std::vector<TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr>::iterator iter = m_callbackCollection.begin();

		for ( ; iter != m_callbackCollection.end(); ++iter )
		{
			if ( *iter != NULL )
			{
				switch(state)
				{
				case RA_INITIALISATION_MODE:
					(*iter)->applicationStateMessage(TA_IRS_Bus::Radio::AP_INITIALISATION_MODE,percent);
					break;
				case RA_OPERATION_MODE:
					(*iter)->applicationStateMessage(TA_IRS_Bus::Radio::AP_OPERATION_MODE,percent);
					break;
				case RA_CONFIGURATION_MODE:
					(*iter)->applicationStateMessage(TA_IRS_Bus::Radio::AP_CONFIGURATION_MODE,percent);
					break;
				case RA_UNKNOWN: //fixed TD14271
					(*iter)->applicationStateMessage(TA_IRS_Bus::Radio::AP_APIFAILURE_MODE,percent);
					break;
				case RA_CONNECTION_FAILURE_MODE:
					(*iter)->applicationStateMessage(TA_IRS_Bus::Radio::AP_CONNECTIONFAILURE_MODE, percent);
					break;
				case RA_MCC_CONNECTION_ESTABLISH:
					(*iter)->applicationStateMessage(TA_IRS_Bus::Radio::AP_MCC_CONNECTION_ESTABLISH, percent);
					break;
				default:
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "ApplicationState is unknown.");
				}
			}
		}
	}
	catch(const CORBA::Exception& e)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Exception occur on Corba: %s",
			e._name() );

	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in callback_applicationStateMessage().");
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentCallbackAdapter::callback_applicationErrorMessage(const std::string& message)
{
    FUNCTION_ENTRY("callback_applicationErrorMessage");

	try
	{
		std::vector<TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr>::iterator iter = m_callbackCollection.begin();

		for ( ; iter != m_callbackCollection.end(); ++iter )
		{
			if ( *iter != NULL )
			{
				(*iter)->applicationErrorMessage(message.c_str());
			}
		}
	}
	catch(const CORBA::Exception& e)
	{
        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );

	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in callback_applicationErrorMessage().");
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentCallbackAdapter::callback_operationCancelMessage(ApiState state, const RadioResource& resource)
{
	FUNCTION_ENTRY("callback_operationCancelMessage");	
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
		TA_IRS_Bus::Radio::EApiState corbaState;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);
		corbaState = RadioMFTAgentHelper::convertApiStateToIDL(state);
		std::vector<TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr>::iterator iter = m_callbackCollection.begin();

		for ( ; iter != m_callbackCollection.end(); ++iter )
		{
			if ( *iter != NULL )
			{
				(*iter)->operationCancelMessage(corbaState,corbaResource);
			}
		}
	}
	catch(const CORBA::Exception& e)
	{
        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );

	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in callback_applicationErrorMessage().");
	}
	FUNCTION_EXIT;
}

void RadioMFTAgentCallbackAdapter::callback_groupStateMessage(const RadioResource& resource, GroupConnectionState state)
{
    FUNCTION_ENTRY("callback_groupStateMessage");

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		
		TA_IRS_Bus::Radio::EGroupConnectionState status = (TA_IRS_Bus::Radio::EGroupConnectionState) state;
		
		std::vector<TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr>::iterator iter = m_callbackCollection.begin();

		for ( ; iter != m_callbackCollection.end(); ++iter )
		{
			if ( *iter != NULL )
			{
				(*iter)->groupStateMessage(corbaResource, status);
			}
		}
	}
	catch(const CORBA::Exception& e)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
			TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in callback_groupStateMessage().");
	}

    FUNCTION_EXIT;    
}


void RadioMFTAgentCallbackAdapter::callback_resourceAssignmentMessage(const RadioResource& resource, unsigned int buttonSet, unsigned int buttonRef, bool bAssigned)
{
    FUNCTION_ENTRY("callback_resourceAssignmentMessage");

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		
		TA_IRS_Bus::Radio::EAssignmentState status = TA_IRS_Bus::Radio::DEASSIGNED;
		
		if ( bAssigned )
		{
			status = TA_IRS_Bus::Radio::ASSIGNED;
		}

		std::vector<TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr>::iterator iter = m_callbackCollection.begin();

		for ( ; iter != m_callbackCollection.end(); ++iter )
		{
			if ( *iter != NULL )
			{
				(*iter)->resourceAssignmentMessage(corbaResource,buttonSet, buttonRef,status);
			}
		}
	}
	catch(const CORBA::Exception& e)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
			TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in callback_resourceAssignmentMessage().");
	}

    FUNCTION_EXIT;    
}


void RadioMFTAgentCallbackAdapter::callback_resourceSelectMessage(
	const RadioResource& resource, 
	AssociatedSpeaker status)
{
    FUNCTION_ENTRY("callback_resourceSelectMessage");

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		TA_IRS_Bus::Radio::ESpeakerState eState;

		switch(status)
		{
			case SELECTED_SPEAKER:
				eState = TA_IRS_Bus::Radio::SPEAKER_SELECT;
				break;
			case UNSELECTED_SPEAKER:
				eState = TA_IRS_Bus::Radio::SPEAKER_UNSELECTED;
				break;

			default:
				eState = TA_IRS_Bus::Radio::SPEAKER_NIL;
		}
	
		
		std::vector<TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr>::iterator iter = m_callbackCollection.begin();

		for ( ; iter != m_callbackCollection.end(); ++iter )
		{
			if ( *iter != NULL )
			{
				(*iter)->resourceSelectMessage(corbaResource,eState);
			}
		}
	}
	catch(const CORBA::Exception& e)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
			TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in callback_resourceSelectMessage().");
	}
	
    FUNCTION_EXIT;
}


void RadioMFTAgentCallbackAdapter::callback_resourceVolumeMessage(const RadioResource& resource, AssociatedSpeaker speaker, unsigned int volume)
{
    FUNCTION_ENTRY("callback_resourceVolumeMessage");

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		TA_IRS_Bus::Radio::ESpeakerState  speaker = TA_IRS_Bus::Radio::SPEAKER_NIL;
		
		std::vector<TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr>::iterator iter = m_callbackCollection.begin();

		for ( ; iter != m_callbackCollection.end(); ++iter )
		{
			if ( *iter != NULL )
			{
				(*iter)->resourceVolumeMessage(corbaResource,speaker, volume);
			}
		}
	}
	catch(const CORBA::Exception& e)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
			TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in callback_resourceVolumeMessage().");
	}
	
    FUNCTION_EXIT;    
}


void RadioMFTAgentCallbackAdapter::callback_inboundCallMessage(
	const RadioResource& caller, 
	PrivateCallState status)
{
    FUNCTION_ENTRY("callback_inboundCallMessage");

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(caller, corbaResource);

		TA_IRS_Bus::Radio::EInboundCallState corbaState = TA_IRS_Bus::Radio::IC_ENDED;
		
		if ( status == PC_INCOMING )
		{
			corbaState = TA_IRS_Bus::Radio::IC_NEW;
		}

		std::vector<TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr>::iterator iter = m_callbackCollection.begin();

		for ( ; iter != m_callbackCollection.end(); ++iter )
		{
			if ( *iter != NULL )
			{
				(*iter)->inboundCallMessage(corbaResource,corbaState);
			}
		}

	}
	catch(const CORBA::Exception& e)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
			TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in callback_inboundCallMessage().");
	}

    FUNCTION_EXIT;    
}

void RadioMFTAgentCallbackAdapter::callback_privateCallMessage(
	const RadioResource& caller,
	PrivateCallState status)
{
    FUNCTION_ENTRY("callback_privateCallMessage");

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(caller, corbaResource);

		TA_IRS_Bus::Radio::EPrivateCallState eState = TA_IRS_Bus::Radio::PC_NOT_INVOLVED;

		switch(status)
		{
			case PC_INCOMING:
				eState = TA_IRS_Bus::Radio::PC_INCOMING;
				break;
			case PC_OUTGOING:
				eState = TA_IRS_Bus::Radio::PC_OUTGOING;
				break;
			case PC_ESTABLISHED:
				eState = TA_IRS_Bus::Radio::PC_ESTABLISHED;
		}
		
		std::vector<TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr>::iterator iter = m_callbackCollection.begin();

		for ( ; iter != m_callbackCollection.end(); ++iter )
		{
			if ( *iter != NULL )
			{
				if (status!=PC_REJECTED)
					(*iter)->privateCallMessage(corbaResource,eState);
				else
					(*iter)->privateCallMessageRejected(corbaResource);
			}
		}
	}
	catch(const CORBA::Exception& e)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
			TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in callback_privateCallMessage().");
	}
	
    FUNCTION_EXIT;    
}


void RadioMFTAgentCallbackAdapter::callback_resourceActivityMessage( 
	const RadioResource& resource, 
	ResourceActivityState status )
{
    FUNCTION_ENTRY("callback_resourceActivityMessage");

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		TA_IRS_Bus::Radio::EResourceActivityType state;
		switch(status)
		{
			case Resource_Tx_Begun: 
				state = TA_IRS_Bus::Radio::RX_BEGUN; break;
			case Resource_Tx_Busy: 
				state = TA_IRS_Bus::Radio::RX_BUSY;  break;
			case Resource_Tx_Ended:
				state = TA_IRS_Bus::Radio::RX_ENDED;  break;

			default:
				state = TA_IRS_Bus::Radio::NO_ACTIVITY;
		}

		std::vector<TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr>::iterator iter = m_callbackCollection.begin();

		for ( ; iter != m_callbackCollection.end(); ++iter )
		{
			if ( *iter != NULL )
			{
				(*iter)->resourceActivityMessage(corbaResource, state);
			}
		}
	}
	catch(const CORBA::Exception& e)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
			TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in callback_resourceActivityMessage().");
	}


    FUNCTION_EXIT;    
}


void RadioMFTAgentCallbackAdapter::callback_resourcePTTMessage(const RadioResource& listener, const RadioResource& src, bool bPushed)
{
    FUNCTION_ENTRY("callback_resourcePTTMessage");

	try
	{
		TA_IRS_Bus::Radio::RadioResource cListener;
        RadioMFTAgentHelper::convertRadioResourceToIDL(listener, cListener);


		TA_IRS_Bus::Radio::RadioResource cResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(src, cResource);


		TA_IRS_Bus::Radio::EPTTState status = TA_IRS_Bus::Radio::PTT_PUSHED;
		
		std::vector<TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr>::iterator iter = m_callbackCollection.begin();

		for ( ; iter != m_callbackCollection.end(); ++iter )
		{
			if ( *iter != NULL )
			{
				(*iter)->resourcePTTMessage(cListener,src.alias.c_str(),status);
			}
		}

	}
	catch(const CORBA::Exception& e)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
			TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in callback_resourcePTTMessage().");
	}
	
    FUNCTION_EXIT;    
}


void RadioMFTAgentCallbackAdapter::callback_resourceEmergencyModeMessage(
	const RadioResource& resource, 
	RadioEmergencyModeType type,
	RadioResource changedBy)
{
    FUNCTION_ENTRY("callback_resourceEmergencyModeMessage");

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);


		TA_IRS_Bus::Radio::RadioResource corbChangedBy;
        RadioMFTAgentHelper::convertRadioResourceToIDL(changedBy, corbChangedBy);


		TA_IRS_Bus::Radio::EmergencyModeState corbaState = TA_IRS_Bus::Radio::RE_NO_EMERGENCY;

		switch( type )
		{
			case RE_EMERGENCY:
				corbaState = TA_IRS_Bus::Radio::RE_EMERGENCY;
				break;
			case RE_UNACK_EMERGENCY:		
				corbaState = TA_IRS_Bus::Radio::RE_UNACK_EMERGENCY;
		}		

		std::vector<TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr>::iterator iter = m_callbackCollection.begin();

		for ( ; iter != m_callbackCollection.end(); ++iter )
		{
			if ( *iter != NULL )
			{
				(*iter)->resourceEmergencyModeMessage(corbaResource,corbaState,corbChangedBy);
			}
		}

	}
	catch(const CORBA::Exception& e)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
			TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in callback_resourceEmergencyModeMessage().");
	}
	
    FUNCTION_EXIT;    
}

void RadioMFTAgentCallbackAdapter::callback_ambienceListeningMessage( const RadioResource& resource, AmbienceListeningCallState state)
{
    FUNCTION_ENTRY("callback_ambienceListeningMessage");

	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaResource);

		TA_IRS_Bus::Radio::EAmbienceListeningState corbaState;

		switch(state)
		{
			case ALC_NOT_INVOLVED:
				corbaState = TA_IRS_Bus::Radio::ALC_NOT_INVOLVED;
				break;
			case ALC_INCOMING:		
				corbaState = TA_IRS_Bus::Radio::ALC_INCOMING;
				break;
			case ALC_ESTABLISHED:		
				corbaState = TA_IRS_Bus::Radio::ALC_ESTABLISHED;
				break;
		}		

		std::vector<TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr>::iterator iter = m_callbackCollection.begin();

		for ( ; iter != m_callbackCollection.end(); ++iter )
		{
			if ( *iter != NULL )
			{
				(*iter)->resourceAmbListeningMessage(corbaResource,corbaState);
			}
		}

	}
	catch(const CORBA::Exception& e)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
			TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in callback_ambienceListeningMessage().");
	}
	FUNCTION_EXIT;
}

void RadioMFTAgentCallbackAdapter::callback_resourceChangeMessage(const RadioResource& oldResource,	const RadioResource& newResource)
{
	FUNCTION_ENTRY("callback_resourceChangeMessage");
	
	try
	{
		TA_IRS_Bus::Radio::RadioResource corbaOldResource;
		TA_IRS_Bus::Radio::RadioResource corbaNewResource;
        RadioMFTAgentHelper::convertRadioResourceToIDL(oldResource, corbaOldResource);
		RadioMFTAgentHelper::convertRadioResourceToIDL(newResource, corbaNewResource);

		std::vector<TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr>::iterator iter = m_callbackCollection.begin();

		for ( ; iter != m_callbackCollection.end(); ++iter )
		{
			if ( *iter != NULL )
			{
				(*iter)->resourceChangeMessage(corbaOldResource,corbaNewResource);
			}
		}

	}
	catch(const CORBA::Exception& e)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
			TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in callback_resourceChangeMessage().");
	}
	FUNCTION_EXIT;
}


bool RadioMFTAgentCallbackAdapter::addCallbackCollection(TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr pCallback)
{
    FUNCTION_ENTRY("addCallbackCollection");
    

	if ( CORBA::is_nil(pCallback) )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr is NULL");

		return false;
	}
	else
		m_callbackCollection.push_back(pCallback);

	
	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				 "CallbackCount : %d",m_callbackCollection.size());

	FUNCTION_EXIT;
	return true;
}


void RadioMFTAgentCallbackAdapter::callback_callTerminated(RadioResource& resource)
{
	 FUNCTION_ENTRY("callback_callTerminated");
	
	 try
	 {
		 
		 TA_IRS_Bus::RadioMFTAgentCallbackIDL::CallTermination callEvent;
		 TA_IRS_Bus::Radio::RadioResource corbaRadioResource;
		 RadioMFTAgentHelper::convertRadioResourceToIDL(resource, corbaRadioResource);

		 
		 callEvent.caller = corbaRadioResource;

		 
		 LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					  "callback_callTermination");

		 CORBA::Any msg;
		 msg <<= callEvent;
		
		 LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
			 "Sending Info : EntityKey : %d SubsystemKey : %d LocationKey : %d, resource ID : %u",
					 m_entityKey,m_entitySubsystem,m_entityLocation,callEvent.caller.id);

 		 m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStatus,
 										 m_entityKey, msg,
 										 m_entitySubsystem, m_entityLocation);
		 
	 }
	 catch(const CORBA::Exception& e)
	 {
		 LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
			 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	 }
	 catch(...)
	 {
		 LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in callback_resourceChangeMessage().");
	 }


	 FUNCTION_EXIT;
}

void RadioMFTAgentCallbackAdapter::callback_callSetupFailure()
{
	FUNCTION_ENTRY("callback_callSetupFailure");
	
	try
	{

		TA_IRS_Bus::RadioMFTAgentCallbackIDL::CallSetupFailed callSetupFailed;
		
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"callback_callSetupFailure");

		callSetupFailed.bFailed = true;
		CORBA::Any msg;
		msg <<= callSetupFailed;

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
			"Sending Info : EntityKey : %d SubsystemKey : %d LocationKey : %d",
			m_entityKey,m_entitySubsystem,m_entityLocation);

		m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStatus,
			m_entityKey, msg,
			m_entitySubsystem, m_entityLocation);

	}
	catch(const CORBA::Exception& e)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
			TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Exception occurred in callback_resourceChangeMessage().");
	}


	FUNCTION_EXIT;
}

long RadioMFTAgentCallbackAdapter::callback_sendErrorMessage(long messageID)
{
	FUNCTION_ENTRY("sendErrorMessage");

	//Note that there is only 1 callback that is registered to RadioMFT library.
	std::vector<TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr>::iterator iter = m_callbackCollection.begin();
	FUNCTION_EXIT;

	return (*iter)->sendErrorMessage(messageID);
}
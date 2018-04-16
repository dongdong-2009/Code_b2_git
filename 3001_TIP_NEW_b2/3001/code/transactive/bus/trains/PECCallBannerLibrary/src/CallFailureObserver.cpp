
#include "CallFailureObserver.h"
#include "core\utilities\src\DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioEntityData.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "bus\radio\radio_agent\idl\src\RadioMFTAgentCallbackIDL.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "bus/banner_pages/call_page_library/src/CallEntryDoesNotExistException.h"

namespace TA_IRS_Bus
{

	CallFailureObserver::CallFailureObserver(CallEntryManager * callEntryMngr)
		: m_callManager(callEntryMngr)
	{
		
		
		std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		TA_Base_Core::IConsole* console = NULL;
		std::string entityName;
		
		try
		{ 
			console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
			entityName = console->getName() + "RadioMFTAgent";
			delete console;
		}
		catch(TA_Base_Core::TransactiveException& e)
		{
			TA_THROW (e);
		}

		TA_Base_Core::IEntityData* entities =
					TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);

		if(entities == NULL)
		{
			const char* msg = "There is no Radio MFTAgent at this location.";
			TA_Base_Core::TransactiveException transEx(msg);

			TA_THROW(transEx);
		}

		std::string locationName   = "";
		unsigned long entityKey    = 0;
		unsigned long subsystemKey = 0;
		unsigned long locationKey = 0;

		try
		{
			entityKey    = entities->getKey();
			subsystemKey = entities->getSubsystem();
			locationName = entities->getLocationName();
			locationKey = entities->getLocation();
		}
		catch(...)
		{
			delete entities;
			entities = NULL;
			
			const char* msg = "Error on getting entryies for the entity";
			TA_Base_Core::TransactiveException transEx(msg);
			
			TA_THROW(transEx);
		}

		delete entities;
		entities = NULL;

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"Subscription Info : EntityKey : %d, SubsystemKey : %d, LocationKey : %d"
					,entityKey,subsystemKey,locationKey);
		
		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
			TA_Base_Core::RadioComms::RadioCallStatus,
			(TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*)(this),
			entityKey,
			subsystemKey,
			locationKey);
	}

	CallFailureObserver::~CallFailureObserver(void)
	{
		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages((TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*)(this));
	}
	
	void CallFailureObserver::notifyCallTermination (unsigned long callId)
	{
		FUNCTION_ENTRY("notifyCallTermination");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
			"PECCallFactory notifying Call Entry ID : %d",
			callId);
		
		try
		{
			std::vector<ICallEntry *> callEntries = m_callManager->getAllCallEntries();
			
			std::vector<ICallEntry *>::iterator callIter = callEntries.begin();
			
			for (; callIter != callEntries.end(); callIter++)
			{
				PECCallEntry * pecCallEntrty = dynamic_cast <PECCallEntry *>(*callIter);
				
				if (pecCallEntrty)
				{
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"trainID : %d, primaryTsi : %u, secondaryTsi : %u",
						static_cast<int>(pecCallEntrty->getTrainId()),pecCallEntrty->getPrimaryTsi(), pecCallEntrty->getSecondaryTsi());  

					if (pecCallEntrty && pecCallEntrty->isCallActive() && 
						(callId == pecCallEntrty->getPrimaryTsi() || callId == pecCallEntrty->getSecondaryTsi()))
					{
						LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo, "Calling onCallTermination");
						pecCallEntrty->onCallTermination();
						break;
					}
				}
				
			}

		}
		catch(CallEntryDoesNotExistException & callEntryException)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Call Entry Does Not Exist, no need for notification");
		}

		FUNCTION_EXIT;
	}
	
	void CallFailureObserver::notifyCallSetupFailed ()
	{
		FUNCTION_ENTRY("notifyCallSetupFailed");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"PECCallFactory notifyCallSetupFailed");

		std::vector<ICallEntry *> callEntries = m_callManager->getAllCallEntries();

		std::vector<ICallEntry *>::iterator callIter= callEntries.begin();

		for (; callIter != callEntries.end(); callIter++)
		{
			PECCallEntry * pecCallEntrty = dynamic_cast <PECCallEntry *>(*callIter);
		
			// protect other call entry by making sure the type is correct 
			// before calling the function of specific class
			if ( pecCallEntrty && pecCallEntrty->isCallActive() )
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo, "Calling onCallSetupFailed");
				pecCallEntrty->onCallSetupFailed();
				break;
			}
		}
		
		FUNCTION_EXIT;
	}

	void CallFailureObserver::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
	{
		FUNCTION_ENTRY("receiveSpecialisedMessage");
		TA_IRS_Bus::RadioMFTAgentCallbackIDL::CallTermination * msgCallTermination;
		TA_IRS_Bus::RadioMFTAgentCallbackIDL::CallSetupFailed * msgCallSetupFailed;

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"CallFailureObserver receiveSpecialisedMessage");		

		if( (message.messageState >>= msgCallTermination) != 0 )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"notifying listener about termination, resource ID : %u",
				        msgCallTermination->caller.id);
			notifyCallTermination(msgCallTermination->caller.id);
		}

		if( (message.messageState >>= msgCallSetupFailed) != 0 )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"notifying listener about termination");
			notifyCallSetupFailed();
		}


	}
}
 
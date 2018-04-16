/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallFactory.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * This class is modified for KRTC to display incoming SDS messages from
  * RadioServer Agent
  *
  */


// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786 4250)
#endif // _MSC_VER

#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallFactory.h"
#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallEntry.h"
#include "bus/banner_pages/call_page_library/src/CallEntryDoesNotExistException.h"

#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioEntityData.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::CommsMessageCorbaDef;
using TA_Base_Core::MessageSubscriptionManager;
using TA_Base_Core::SpecialisedMessageSubscriber;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::RadioEntityData;
using TA_Base_Core::IEntityData;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::DataException;


namespace TA_IRS_Bus
{

    AuthorisationCallFactory::AuthorisationCallFactory(
		TA_IRS_Bus::CallEntryManager& callManager,
        unsigned long locationKey)
        :
        m_callManager(callManager)
    {
        try
        {
            initialise(locationKey);
        }
        catch(...)
        {
            //
            // Unsubscribe to messages before leaving the constructor
            //
            MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
            
            throw;
        }
    }


    void AuthorisationCallFactory::initialise(unsigned long locationKey)
    {
        //
		// Fix TD13617 
        // Get the RadioMFTAgent's entity data instead of Radio Server Agent
        //
		std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
		std::string entityName = console->getName() + "RadioMFTAgent";
		delete console;

        IEntityData* entities =
            EntityAccessFactory::getInstance().getEntity(entityName);

        if(entities == NULL)
        {
            const char* msg = "There is no Radio MFTAgent at this location.";
            const char* fld = "Radio Agent";
            TA_THROW(DataException(msg, DataException::MISSING_MANDATORY, fld));
        }

		// TD#14311 - add subscribing messages to MFT to delete entry when 
		//            message is read

        unsigned long entityKey    = 0;
        unsigned long subsystemKey = 0;
        
        try
        {
            entityKey    = entities->getKey();
            subsystemKey = entities->getSubsystem();

			delete entities;
			entities = NULL;
        }
        catch(...)
        {
            delete entities;
            entities = NULL;
            throw;
        }

        //
        // Subscribe to ReceiveText notifications from the RadioMFTAgent
		// when Incoming Text message is already read
        //
        MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
            TA_Base_Core::RadioComms::ReceiveText,
            (SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*)(this),
            entityKey,
            subsystemKey,
            locationKey);
    }


    AuthorisationCallFactory::~AuthorisationCallFactory()
    {
        //
        // Unsubscribe to messages
        //
        MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
    }



    void AuthorisationCallFactory::receiveSpecialisedMessage(
        const CommsMessageCorbaDef& message)
    {
        FUNCTION_ENTRY("receiveSpecialisedMessage");

        
        //
        // Acquire the notifications lock so that other threads can acquire
        // this lock (when released) to avoid being interrupted by incoming
        // notifications.
        //
        ThreadGuard lock(m_notificationsLock);


        //
        // Make sure the message is an Incoming Text notification
        //
        if( std::string(message.messageTypeKey) ==
            TA_Base_Core::RadioComms::ReceiveText.getTypeKey() )
        {
			//
			// Extract to the ReceiveText notification
			//
			TA_IRS_Bus::RadioServerAgentIDL::recvText* notification;
			if( ( message.messageState >>= notification ) == 0 )
			{
				FUNCTION_EXIT;
				return;
			}

			//
			// Log a message with the details of the notification.
			//
			LOG_GENERIC(
				SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				getNotificationAsString( *notification ) );

			processEvent(*notification);        
		}
		
        FUNCTION_EXIT;
    }


	void AuthorisationCallFactory::processEvent(const TA_IRS_Bus::RadioServerAgentIDL::recvText& message)
	{
		FUNCTION_ENTRY("processEvent");

        std::string callId = getUnsignedLongAsString(message.messageData.key);
        
        //
        // Create a new SDS call entry.
        //
		if (!message.messageData.isRead)
		{
			unsigned long entryId = getUniqueCallEntryId();
			TA_IRS_Bus::ICallEntry* newEntry =
				new AuthorisationCallEntry(entryId, message,*this);

            // add the call Id
			if (!doesCallIdExist(callId))
			{
				//
				// Inform the CallEntryManager that a new entry has been created.
				// The CallEntryManager takes responsibility for the memory of the
				// call entry.
				//
				m_callManager.addCallEntry(newEntry);


				//
				// Save the entry id / call id association so that we can find this
				// call entry when other messages arrive for it.
				//
				saveCallId(callId, entryId);
			}			
		}
		else 
		{
			if (doesCallIdExist(callId))
            {
				unsigned long entryId = retrieveCallId(callId);

				m_callManager.deleteCallEntry(entryId,AuthorisationCallEntry::CALL_ENTRY_TYPE);
                deleteCallId(callId);
            }
		}
		FUNCTION_EXIT;
	}




    void AuthorisationCallFactory::deleteCallEntry(
        unsigned long callEntryId)
    {
        try
        {
            m_callManager.deleteCallEntry(callEntryId, AuthorisationCallEntry::CALL_ENTRY_TYPE);
        }
        catch(const TA_IRS_Bus::CallEntryDoesNotExistException&)
        {
            std::string message = "Attempt to delete Call Entry that does not ";
            message            += "exist. Call entry id: '";
            message            += getUnsignedLongAsString(callEntryId);
            message            += "'";

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, message);
        }
    }
    

    std::string AuthorisationCallFactory::getNotificationAsString( const TA_IRS_Bus::RadioServerAgentIDL::recvText& message)
    {
        std::ostringstream asStream;
        asStream <<" 'Incoming SDS text message from" << message.originator.alias.in()
			     << "(" << message.originator.ssi<< ")'";

        return asStream.str();
    }


    std::string AuthorisationCallFactory::getCallInformation( const TA_IRS_Bus::RadioServerAgentIDL::recvText& message)
    {
        return message.messageData.messageText.in();        
    }


    std::string AuthorisationCallFactory::getCallSourceAsString(const TA_IRS_Bus::RadioServerAgentIDL::recvText& message)
    {
        // TES #457: Source should show name and TSI
        std::ostringstream ostr;
        ostr << message.originator.alias.in() << " (" 
             << message.originator.ssi << ")";

        return ostr.str();
    }


}// TA_IRS_Bus

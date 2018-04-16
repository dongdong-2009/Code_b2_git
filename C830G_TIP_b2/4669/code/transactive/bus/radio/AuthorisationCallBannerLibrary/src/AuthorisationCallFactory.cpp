/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Dirk McCormick
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This factory listens for three Authorisation notifications from the
  * Radio Agent:
  *
  * 1. New Authorisation - A new authorisation request has been received.
  *    An AuthorisationCallEntry is created and added to the CallEntryManager.
  *
  * 2. Accepted Authorisation - An authorisation request has been approved.
  *    If the Authorisation call dialog is open, it is told to close.
  *    The call entry is then removed from the call banner.
  *
  * 3. Rejected Authorisation - An authorisation request has been denied.
  *    If the Authorisation call dialog is open, it is told to close.
  *    The call entry is then removed from the call banner.
  *
  * 4. Cancelled Authorisation - An authorisation request has been cancelled.
  *    If the Authorisation call dialog is open, it is told to close.
  *    The call entry is then removed from the call banner.
  *    If the Authorisation call dialog is not open, the information field
  *    of the call entry in the banner is changed to Cancelled, and the
  *    entry is not deleted until it is activated.
  */


// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786 4250)
#endif // _MSC_VER

#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallFactory.h"
#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallEntry.h"
#include "bus/radio/RadioEntityAccess/src/RadioEntityAccess.h"
#include "bus/banner_pages/call_page_library/src/CallEntryDoesNotExistException.h"

#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioSessionEntityData.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"


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
			m_bRadioSessionOk = false ;
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
        // Get the radio session's entity data from its type and location.
        //
        unsigned long consoleKey = RadioEntityAccess::getConsoleKey();
        TA_Base_Core::IRadioSessionEntityData * entityData = RadioEntityAccess::getRadioSessionEntity(locationKey, consoleKey);
		if ( entityData ==NULL  )
		{
			    m_bRadioSessionOk =false ;
            	LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "No radio session found , will exit the radio session init ");
				return ;
		}

		

        unsigned long entityKey = entityData->getKey();
        unsigned long subsystemKey = entityData->getSubsystem();
        std::string sessionEntityName = entityData->getName();
        m_locationName = entityData->getLocationName();

        //
        // Set the entity name of the Radio Agent CORBA interface and the
        // location name
        //
        m_radioSession.setEntityName(sessionEntityName,false);
		m_bRadioSessionOk = true ;

        
        //
        // Acquire the notifications lock before subscribing to messages
        // so that any notifications are received after the initial batch of
        // authorisation requests are retrieved.
        //
        ThreadGuard guard(m_notificationsLock);

        //
        // Subscribe to Authorisation notifications from the Radio Agent
        //
        MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
            TA_Base_Core::RadioComms::RadioAuthorisationNotification,
            (SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*)(this),
            entityKey,
            subsystemKey,
            locationKey);


        //
        // Get the existing authorisation request from the Radio Agent
        //
        getExistingMessages();
    }


    void AuthorisationCallFactory::getExistingMessages()
    {
		if ( !isRadioSessionOk () )
		{
			return ;
		}
        TA_Base_Bus::IRadioSessionCorbaDef::CallAuthorisationList_var requests;
        CORBA_CALL_RETURN( requests, m_radioSession, getCallAuthorisations, () );//limin

        //
        // For each authorisation request retrieved from the Radio Agent,
        // treat it as though a new notification had arrived.
        //
        for(unsigned long i = 0; i < requests->length(); i ++)
        {
            receiveNewAuthorisationRequest( requests[i] );
        }
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
        // Make sure the message is an Authorisation notification
        //
        if( std::string(message.messageTypeKey) !=
            TA_Base_Core::RadioComms::RadioAuthorisationNotification.getTypeKey() )
        {
            FUNCTION_EXIT;
            return;
        }


        //
        // Extract to the Authorisation notification
        //
        TA_Base_Bus::IRadioSessionCorbaDef::CallAuthorisationNotification* notification;
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
            getNotificationAsString( *notification ).c_str() );


        //
        // Work out what sort of Authorisation notification has arrived and
        // pass it to the appropriate method to be processed.
        //
        switch(notification->action)
        {
            //
            // New Authorisation
            //
            case TA_Base_Bus::IRadioSessionCorbaDef::NEW_AUTHORISATION:
            {
                receiveNewAuthorisationRequest( (*notification).request );
                break;
            }

            //
            // Accepted Authorisation
            //
            case TA_Base_Bus::IRadioSessionCorbaDef::ACCEPTED_AUTHORISATION:
            {
                receiveAuthorisationRejectedRequest( (*notification).request );
                break;
            }

            //
            // Rejected Authorisation
            //
            case TA_Base_Bus::IRadioSessionCorbaDef::REJECTED_AUTHORISATION:
            {
                receiveAuthorisationAcceptedRequest( (*notification).request );
                break;
            }

            //
            // Cancelled Authorisation
            //
            case TA_Base_Bus::IRadioSessionCorbaDef::CANCELLED_AUTHORISATION:
            {
                receiveAuthorisationCancelledRequest( (*notification).request );
                break;
            }


        }// switch(notification->action)


        FUNCTION_EXIT;
    }


    void AuthorisationCallFactory::receiveNewAuthorisationRequest(
        const TA_Base_Bus::IRadioSessionCorbaDef::CallAuthorisationRequest& request)
    {
        FUNCTION_ENTRY("receiveNewAuthorisationRequest");

        if ( !isRadioSessionOk()) 
		{
            FUNCTION_EXIT; 
			return ;
		}
        //
        // The notification for this authorisation request may already have
        // been received if a message slipped through between subscribing to
        // notifications and calling the getExistingMessages method.
        // If it has already been received, just ignore it.
        //
        std::string callId = getUnsignedLongAsString(request.callID);
        if( doesCallIdExist(callId) )
        {
            return;
        }

        
        //
        // Create a new Authorisation call entry.
        //
        unsigned long entryId = getUniqueCallEntryId();
		TA_IRS_Bus::ICallEntry* newEntry =
            new AuthorisationCallEntry(entryId, request, m_radioSession, *this);
        

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

        FUNCTION_EXIT;
    }


    void AuthorisationCallFactory::receiveAuthorisationRejectedRequest(
        const TA_Base_Bus::IRadioSessionCorbaDef::CallAuthorisationRequest& request)
    {
        FUNCTION_ENTRY("receiveAuthorisationRejectedRequest");

        //
        // Retrieve the entry id using the call id.
        //
        std::string callId = getUnsignedLongAsString(request.callID);
        unsigned long entryId = retrieveCallId(callId);

        try
        {
            //
            // Inform the AuthorisationCallEntry that a reject authorisation
            // notification has been received.
            //
            TA_IRS_Bus::ICallEntry& callEntry = m_callManager.getCallEntry(entryId);
            AuthorisationCallEntry& authorisationCallEntry =
                dynamic_cast<AuthorisationCallEntry&>(callEntry);

            authorisationCallEntry.rejectNotificationReceived();

            //
            // Tell the CallEntryManager to delete the call entry.
            //
            m_callManager.deleteCallEntry(entryId, AuthorisationCallEntry::CALL_ENTRY_TYPE);
        }
        catch(const TA_IRS_Bus::CallEntryDoesNotExistException&)
        {
            std::string message = "Reject Notification received for call "
                                  "entry that does not exist. Radio call id: ";
            message            += callId;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, message.c_str());
        }

        //
        // The Authorisation entry has been deleted so delete the
        // entry id / call id association.
        //
        deleteCallId(callId);

        FUNCTION_EXIT;
    }


    void AuthorisationCallFactory::receiveAuthorisationAcceptedRequest(
        const TA_Base_Bus::IRadioSessionCorbaDef::CallAuthorisationRequest& request)
    {
        FUNCTION_ENTRY("receiveAuthorisationAcceptedRequest");

        //
        // Retrieve the entry id using the call id.
        //
        std::string callId = getUnsignedLongAsString(request.callID);
        unsigned long entryId = retrieveCallId(callId);

        try
        {
            //
            // Inform the AuthorisationCallEntry that an accept authorisation
            // notification has been received.
            //
            TA_IRS_Bus::ICallEntry& callEntry = m_callManager.getCallEntry(entryId);
            AuthorisationCallEntry& authorisationCallEntry =
                dynamic_cast<AuthorisationCallEntry&>(callEntry);

            authorisationCallEntry.acceptNotificationReceived();

            //
            // Tell the CallEntryManager to delete the call entry.
            //
            m_callManager.deleteCallEntry(entryId, AuthorisationCallEntry::CALL_ENTRY_TYPE);
        }
        catch(const TA_IRS_Bus::CallEntryDoesNotExistException&)
        {
            std::string message = "Reset Notification received for call ";
                                  "entry that does not exist. Radio call id: ";
            message            += callId;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, message.c_str());
        }

        //
        // The Authorisation entry has been deleted so delete the
        // entry id / call id association.
        //
        deleteCallId(callId);

        FUNCTION_EXIT;
    }


    void AuthorisationCallFactory::receiveAuthorisationCancelledRequest(
        const TA_Base_Bus::IRadioSessionCorbaDef::CallAuthorisationRequest& request)
    {
        FUNCTION_ENTRY("receiveAuthorisationCancelledRequest");

        //
        // Retrieve the entry id using the call id.
        //
        std::string callId = getUnsignedLongAsString(request.callID);
        unsigned long entryId = retrieveCallId(callId);

        try
        {
            //TD18077, commmented by hongzhi
			//
            // Inform the AuthorisationCallEntry that a cancelled authorisation
            // notification has been received.
            //
			
            //TA_IRS_Bus::ICallEntry& callEntry = m_callManager.getCallEntry(entryId);
			//AuthorisationCallEntry& authorisationCallEntry =
            //    dynamic_cast<AuthorisationCallEntry&>(callEntry);

            //bool deleteImmediately =
            //    authorisationCallEntry.cancelNotificationReceived();

            //
            // If the call entry is showing a dialog when the cancelled
            // notification is received, it can be deleted immediately.
            // If it is not showing the dialog, it is only deleted when
            // activated. (see the cancelledEntryActivated method below)
            //
            //if(deleteImmediately)
            //{
			//TD18077, hongzhi
            m_callManager.deleteCallEntry(entryId, AuthorisationCallEntry::CALL_ENTRY_TYPE);

                //
                // The Authorisation entry has been deleted so delete the
                // entry id / call id association.
                //
            deleteCallId(callId);
            //}
            //else
            //{
            //    m_callManager.refreshCallEntry(entryId);
            //}
        }
        catch(const TA_IRS_Bus::CallEntryDoesNotExistException&)
        {
            std::string message = "Cancelled Notification received for call ";
                                  "entry that does not exist. Radio call id: ";
            message            += callId;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, message.c_str());
        }


        FUNCTION_EXIT;
    }



    void AuthorisationCallFactory::cancelledEntryActivated(
        unsigned long callEntryId)
    {
        FUNCTION_ENTRY("cancelledEntryActivated");

        //
        // When a previously cancelled entry is activated, delete it from the
        // banner.
        //
        deleteCallEntry(callEntryId);

        FUNCTION_EXIT;
    }

    void AuthorisationCallFactory::callEntryAccepted(
        unsigned long callEntryId)
    {
        //
        // When a call entry is accepted, delete it from the banner.
        //
        deleteCallEntry(callEntryId);
    }

    void AuthorisationCallFactory::callEntryRejected(
        unsigned long callEntryId)
    {
        //
        // When a call entry is rejected, delete it from the banner.
        //
        deleteCallEntry(callEntryId);
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

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, message.c_str());
        }
    }
    

    std::string AuthorisationCallFactory::getNotificationAsString(
        const TA_Base_Bus::IRadioSessionCorbaDef::CallAuthorisationNotification& notification)
    {
        TA_Base_Bus::IRadioSessionCorbaDef::CallAuthorisationRequest request = notification.request;

        std::string action = getActionAsString(notification.action);

        std::string sourceType =
            getRadioUserTypeAsString(request.sourceType);
        std::string destType =
            getRadioUserTypeAsString(request.sourceType);

        std::string sessionReference = getUnsignedLongAsString(request.sessionRef);

        std::ostringstream asStream;
        asStream <<
            
            action << " Authorisation Notification " <<
            "for call " << request.callID <<

            " from " <<
            request.callerName      << "(" << sourceType << ") to " <<
            request.calledName << "(" << destType   << ") " <<

            "with session reference '" << sessionReference << "'";

        return asStream.str();
    }


    std::string AuthorisationCallFactory::getActionAsString(
		TA_Base_Bus::IRadioSessionCorbaDef::EAction action)
    {
        switch(action)
        {
			case TA_Base_Bus::IRadioSessionCorbaDef::NEW_AUTHORISATION:
            {
                return "New";
            }
            case TA_Base_Bus::IRadioSessionCorbaDef::ACCEPTED_AUTHORISATION:
            {
                return "Accepted";
            }
            case TA_Base_Bus::IRadioSessionCorbaDef::REJECTED_AUTHORISATION:
            {
                return "Rejected";
            }
            case TA_Base_Bus::IRadioSessionCorbaDef::CANCELLED_AUTHORISATION:
            {
                return "Cancelled";
            }
            default:
            {
                return "Unrecognised";
            }
        }
    }

    std::string AuthorisationCallFactory::getRadioUserTypeAsString(
        TA_Base_Bus::IRadioSessionCorbaDef::ERadioUserType type)
    {
        switch(type)
        {
            case TA_Base_Bus::IRadioSessionCorbaDef::PORTABLE:
            {
                return "Portable";
            }
            case TA_Base_Bus::IRadioSessionCorbaDef::OPERATOR:
            {
                return "Operator";
            }
            case TA_Base_Bus::IRadioSessionCorbaDef::STATION:
            {
                return "Station";
            }
            default:
            {
                return "Unrecognised";
            }
        }
    }


    std::string AuthorisationCallFactory::getCallInformation(
            const TA_Base_Bus::IRadioSessionCorbaDef::CallAuthorisationRequest& call)
    {
        std::ostringstream ostr;
        // 'Calling Mr James (TSI) at CVC
        ostr << "Calling " <<   call.calledName.in()
             << " (" <<         call.calledTSI.in() << ")" 
             << " at " <<       call.calledLocation.in();

        return ostr.str();        
    }


    std::string AuthorisationCallFactory::getCallSourceAsString( 
            const TA_Base_Bus::IRadioSessionCorbaDef::CallAuthorisationRequest& call)
    {
        // TES #457: Source should show name and TSI
        std::ostringstream ostr;
        ostr << call.callerName.in() << " (" 
             << call.callerTSI.in() << ")";

        return ostr.str();
    }


}// TA_IRS_Bus

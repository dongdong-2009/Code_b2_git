/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/signs/RATISCallBannerLibrary/src/RATISCallFactory.cpp $
  * @author:  Rob Ashcroft
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/02/10 16:25:11 $
  * Last modified by:  $Author: builder $
  * 
  * This factory listens for incoming RATIS message notifications from the
  * TIS Agent.
  *
  *
  */



// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786 4250)
#endif // _MSC_VER


#include "bus/signs/RATISCallBannerLibrary/src/RATISCallFactory.h"
#include "bus/signs/RATISCallBannerLibrary/src/RATISCallEntry.h"
#include "bus/banner_pages/call_page_library/src/CallEntryDoesNotExistException.h"

#include <iomanip>

#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/TisComms_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/TISAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/STISEntityData.h"
#include "core\data_access_interface\entity_access\src\DataPointEntityData.h"
#include "core\data_access_interface\tis_agent_4669\IDL\src\TISMessageDataDefinitions.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

//using TA_Base_Core::CommsMessageCorbaDef;
//using TA_Base_Core::MessageSubscriptionManager;
//using TA_Base_Core::SpecialisedMessageSubscriber;
//using TA_Base_Core::EntityAccessFactory;
//using TA_Base_Core::RadioEntityData;
//using TA_Base_Core::IEntityData;
//using TA_Base_Core::ThreadGuard;
//using TA_Base_Core::DebugUtil;
//using TA_Base_Core::DataException;


namespace TA_IRS_Bus
{

    RATISCallFactory::RATISCallFactory(
		TA_IRS_Bus::CallEntryManager& callManager,
        unsigned long locationKey,
        long tisSessionKey)
        :
        m_callManager(callManager),
        m_tisSessionKey(tisSessionKey)
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
			TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
            
            throw;
        }
    }


    void RATISCallFactory::initialise(unsigned long locationKey)
    {
        // connect straight to the TIS Agent at OCC even if this is a station
        int occLocationKey = TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey();
        std::vector<TA_Base_Core::IEntityData*> entities =
			TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(
			TA_Base_Core::STISEntityData::getStaticType(), occLocationKey);

        if(entities.size() != 1)
        {
            const char* msg = "There is no TIS agent at this location.";
            const char* fld = "TIS Agent";
            TA_THROW(TA_Base_Core::DataException(msg, TA_Base_Core::DataException::MISSING_MANDATORY, fld));
        }


        unsigned long entityKey    = 0;
        unsigned long subsystemKey = 0;
        std::string locationName   = "";

        try
        {
            entityKey    = entities[0]->getKey();
            subsystemKey = entities[0]->getSubsystem();
            locationName = entities[0]->getLocationName();
        }
        catch(...)
        {
            delete entities[0];
            entities[0] = NULL;
            throw;
        }
        delete entities[0];
        entities[0] = NULL;

        m_locationName = locationName;
        
        //
        // Acquire the notifications lock before subscribing to messages
        // so that any notifications are received after the initial batch of
        // authorisation requests are retrieved.
        //
		TA_Base_Core::ThreadGuard guard(m_notificationsLock);

        //
        // Subscribe to RATIS notifications from the TIS Agent
        //

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Subscribing for IncomingRATISMessages from entity %lu of subsystem %lu at location %lu",
            entityKey, subsystemKey, occLocationKey);

		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
            TA_Base_Core::TISComms::IncomingRATISMessage,
            (TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*)(this),
            entityKey,
            subsystemKey,
            occLocationKey);  

		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
            TA_Base_Core::TISComms::RATISStatusUpdate,
            (TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*)(this),
            entityKey,
            subsystemKey,
            occLocationKey);  

        //
        // Get any existing RATIS messages from the TIS Agent
        //
        getExistingMessages();

        // Build up the vector of map of pid address to pid name
//        buildPIDMap();    
    }

/*
    void RATISCallFactory::buildPIDMap()
    {
	    std::vector<TA_Base_Core::IEntityData*> entityData;
        // Only take it if it has 5 parts, Ignore all data nodes with virtual addresses
        entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeWithNameLikeToken(
                        TA_Base_Core::DataPointEntityData::getStaticType(),
                        "%.TIS.%.%.%");

        // the names would be in the form of CVC.TIS.Platform.PDP08.diiTISC-LockStatus  
        // determine if the station already exist, 
        // if it hasn't then add it into the vector
        // add the map of address and shortname into this item.
		std::vector<std::string> entityNames;
	    std::vector<TA_Base_Core::IEntityData*>::iterator i;
	    for ( i = entityData.begin(); i != entityData.end(); i++)
        {
            // we only add it if the datanode has a valid integer address
            int address = atol((*i)->getAddress().c_str());
            if (address != 0)
            {
                std::string& name = (*i)->getName();
                unsigned int index = 0;
                unsigned int endpart = 0;
                std::vector<std::string> part;
                do 
                {
                    endpart = name.find(".", index);
                    part.push_back(name.substr(index, endpart - index));
                    index = endpart + 1;

                } while (endpart != std::string::npos);

                std::string& stationName = part[0];
                std::string& pidName = part[3];

                // The address that we're getting from the agent is something like 002, so we might
                // as well just build the map using 002 so that we don't have to parse the string
                // given by the agent during comparison
                std::stringstream paddedAddress;
                paddedAddress << std::setw(3) << std::setfill('0') << address;
                // if can't find the station, then add it along with a new map of pid address-name
                // the first one item would be station and last would be the pid shortname
                if (m_stationPidMap.find(stationName) == m_stationPidMap.end())
                {
                    PIDAddressNameMap newAddressNameMap;
                    newAddressNameMap.insert(PIDAddressNameMap::value_type(paddedAddress.str(), pidName));
                    m_stationPidMap.insert(StationPIDMap::value_type(stationName, newAddressNameMap));
                }
                else
                {
                    // otherwise it's already there, just stick in the new address-name map into it
                    PIDAddressNameMap& pidMap = m_stationPidMap[stationName];
                    if (pidMap.find(paddedAddress.str()) == pidMap.end())
                    {
                        // only insert it if it's not already mapped to something
                        pidMap.insert(PIDAddressNameMap::value_type(paddedAddress.str(), pidName));
                    }
                }
            }
	    }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "Map of STIS data node");
        for (StationPIDMap::const_iterator sta = m_stationPidMap.begin(); sta != m_stationPidMap.end(); sta++)
        {
            LOGMORE(SourceInfo, "%s", sta->first.c_str());

            const PIDAddressNameMap& pidMap = sta->second;
            for (PIDAddressNameMap::const_iterator pid = pidMap.begin(); pid != pidMap.end(); pid++)
            {
                LOGMORE(SourceInfo, "\t%s: %s", pid->first.c_str(), pid->second.c_str());
            }
        }
    }

    std::string RATISCallFactory::getPidName(std::string& station, std::string& address)
    {
        StationPIDMap::const_iterator sta = m_stationPidMap.find(station);
        if (sta == m_stationPidMap.end())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Can't find the station %s", station.c_str());
            return "";
        }

        PIDAddressNameMap::const_iterator addr = sta->second.find(address);
        if (addr == sta->second.end())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Can't find pid address %d on %s", address.c_str(), station.c_str());
            return "";
        }

        return addr->second;
    }
*/


    void RATISCallFactory::getExistingMessages()
    {
		TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageList_var calls;

		try
		{
            CORBA_CALL_RETURN( calls, TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent(), getAllIncomingRATISMessages, () );//limin
		}
		catch ( const TA_Base_Core::ObjectResolutionException& ore )
	    {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::ObjectResolutionException", ore.what() );

			return;
	    }
        catch (const TA_Base_Bus::ISTISManagerCorbaDef::STISGeneralFailureException& ex)
        {
	        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "STISGeneralFailureException",
                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ex ) );
        }
        catch( const CORBA::Exception& cex )
        {
	        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA",
                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

			return;
        }
		catch( ... )
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA",
                 "Caught an unknown exception while attempting to retrieve all existing RATIS messages" );

			return;
		}


        //
        // For each existing RATIS message retrieved from the TIS Agent that is
        // addressed to this console, treat it as though a new message had arrived
        //

		// TODO - need to check whether this is the OCC?
        for(unsigned long i = 0; i < calls->length(); i ++)
        {
            if( ( calls[i].sessionRef == m_tisSessionKey ) )
            {
                processMessageNotification(calls[i]);
            }
        }
    }


    RATISCallFactory::~RATISCallFactory()
    {
        //
        // Unsubscribe to messages
        //
		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
    }



    void RATISCallFactory::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
    {
        FUNCTION_ENTRY("receiveSpecialisedMessage");

        
        //
        // Acquire the notifications lock so that other threads can acquire
        // this lock (when released) to avoid being interrupted by incoming
        // notifications.
        //
		TA_Base_Core::ThreadGuard lock(m_notificationsLock);

        //
        // Make sure the message is an incoming RATIS message notification
        //
        if (0 == TA_Base_Core::TISComms::IncomingRATISMessage.getTypeKey().compare(message.messageTypeKey)) 
        {
            //
            // Extract to the message info
            //
            TA_Base_Bus::ISTISManagerCorbaDef::IncomingRATISEvent* notification;
            if( ( message.messageState >>= notification ) == 0 )
            {
                FUNCTION_EXIT;
                return;
            }
        
            //
            // If the notification is not addressed to this console, ignore it.
            //
            if( notification->sessionRef != m_tisSessionKey )
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

            // Process the incoming message

            try
            {
                TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails* details = NULL;
                CORBA_CALL_RETURN( details, TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent(), getIncomingRATISMessage, ( notification->messageID ) );//limin

                processMessageNotification(*details);
            }
            catch(const TA_Base_Bus::ISTISManagerCorbaDef::STISGeneralFailureException&)
            {
                std::string message = "Notification received for incoming RATIS message "
                                      "that does not exist at TISAgent. "
                                      "RATIS message id: ";
                message            += notification->messageID;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, message.c_str());
                FUNCTION_EXIT;
            }
            // TD17564
			catch( const CORBA::Exception& cex )
            {
	            LOG0( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA",
                     TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ).c_str() );
                FUNCTION_EXIT;
			    return;
            }
            // TD17564
		    catch( ... )
		    {
			    LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA",
                     "Caught an unknown exception while attempting to retrieve an incoming RATIS message" );
                FUNCTION_EXIT;
			    return;
		    }
        }
        else if (0 == TA_Base_Core::TISComms::RATISStatusUpdate.getTypeKey().compare(message.messageTypeKey))
        {
            //
            // Extract to the message info
            //
            TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageApprovalDetails* notification;
            if( ( message.messageState >>= notification ) == 0 )
            {
                FUNCTION_EXIT;
                return;
            }
        
            //
            // If the notification is not addressed to this console, ignore it.
            //
            if( notification->sessionRef != m_tisSessionKey )
            {
                FUNCTION_EXIT;
                return;
            }

            //
            // Log a message with the details of the notification.
            //
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Updating approval status of message %d to %d", 
                notification->messageID, notification->status);

            try
            {
                std::string callIdStr = getUnsignedLongAsString(notification->messageID);
                // see if the call exists first.. could have been a non-vetted message which we'll never display
                if (!doesCallIdExist(callIdStr) )
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Call does not exist on this banner", 
                        notification->messageID, notification->status);
                    FUNCTION_EXIT;
                    return;
                }
                unsigned long callId = retrieveCallId(callIdStr);
                if ((notification->status == TA_Base_Core::APPROVED) || (notification->status == TA_Base_Core::REJECTED))
                {
                    m_callManager.deleteCallEntry(callId, RATISCallEntry::CALL_ENTRY_TYPE);
                }
                else // we got to show the fail state if we've failed to approve it
                {
                    RATISCallEntry* entry = dynamic_cast<RATISCallEntry*>(&m_callManager.getCallEntry(callId));
                    entry->updateStatus(notification->status);
                    m_callManager.refreshCallEntry(callId);
                }
            }
            catch(const TA_IRS_Bus::CallEntryDoesNotExistException&)
            {
                std::string message = "Attempt to refresh/delete Call Entry that does not ";
                message            += "exist. Call entry id: '";
                message            += getUnsignedLongAsString(notification->messageID);
                message            += "'";

                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, message.c_str());
            }
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Received some unknown message, ignoring");
            FUNCTION_EXIT;
            return;
        }

        FUNCTION_EXIT;
    }


    void RATISCallFactory::processMessageNotification(TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails& details)
    {
        FUNCTION_ENTRY("processMessageNotification");

        std::string callIdStr = getUnsignedLongAsString(details.messageID);

        // if it's already exist, then just update it, otherwise add it
        if( doesCallIdExist(callIdStr) )
        {
            unsigned long entryId = retrieveCallId(callIdStr);
            RATISCallEntry* entry = dynamic_cast<RATISCallEntry*>(&m_callManager.getCallEntry(entryId));
            entry->update(details);
            m_callManager.refreshCallEntry(entryId);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Updated RATIS entry");
        }
        else
        {
            // create a new RATIS message entry.
            addRATISCallEntry(callIdStr, details);
        }
        FUNCTION_EXIT;
    }


    void RATISCallFactory::addRATISCallEntry(
        const std::string& callId,
        const TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails& details)
    {
        unsigned long entryId = getUniqueCallEntryId();
        TA_IRS_Bus::ICallEntry* newEntry = new RATISCallEntry(entryId, details, *this);
    

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

    std::string RATISCallFactory::getNotificationAsString(
        const TA_Base_Bus::ISTISManagerCorbaDef::IncomingRATISEvent& notification)
    {
        //std::string status = getCallStatusAsString(notification.status);

        std::ostringstream asStream;
        asStream << " RATIS message notification (" << notification.messageID << ")"
            " with session reference '" << notification.sessionRef << "'";

        return asStream.str();
    }


    std::string RATISCallFactory::getStatusAsString(
        TA_Base_Core::ERATISMessageStatus status)
    {
        switch(status)
        {
            case TA_Base_Core::APPROVED:
            {
                return "Approved";
            }
            case TA_Base_Core::NOT_APPROVED:
            {
                return "Not Approved";
            }
            case TA_Base_Core::REJECTED:
            {
                return "Rejected";
            }
            case TA_Base_Core::APPROVE_FAILED:
            {
                return "Approve Failed";
            }
            default:
            {
                return "Unrecognised";
            }
        }
    }


    std::string RATISCallFactory::getTypeAsString(
        TA_Base_Core::ERATISMessageType type)
    {
        switch(type)
        {
            case TA_Base_Core::RATIS_IN_NEW:
            {
                return "New-In";
            }
            case TA_Base_Core::RATIS_OUT_NEW:
            {
                return "New-Out";
            }
            case TA_Base_Core::RATIS_IN_UPDATE:
            {
                return "Update-In";
            }
            case TA_Base_Core::RATIS_OUT_UPDATE:
            {
                return "Update-Out";
            }
            case TA_Base_Core::RATIS_IN_CLEAR:
            {
                return "Clear-In";
            }
            case TA_Base_Core::RATIS_OUT_CLEAR:
            {
                return "Clear-Out";
            }
            default:
            {
                return "Unrecognised";
            }
        }
    }


    std::string RATISCallFactory::getMessageInformation(
            const TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails& call)
    {
        std::string mesg(call.messageText);
		//TD 9991
		//zhou yuan++
        if (mesg.empty() && (call.type == TA_Base_Core::RATIS_IN_CLEAR || call.type == TA_Base_Core::RATIS_OUT_CLEAR))//++zhou yuan
        {
            return "<Clear Request>";
        }
		return mesg;
    }


    void RATISCallFactory::deleteCallEntry(unsigned long callEntryId)
    {
        try
        {
            m_callManager.deleteCallEntry(callEntryId, RATISCallEntry::CALL_ENTRY_TYPE);
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


}// TA_IRS_Bus

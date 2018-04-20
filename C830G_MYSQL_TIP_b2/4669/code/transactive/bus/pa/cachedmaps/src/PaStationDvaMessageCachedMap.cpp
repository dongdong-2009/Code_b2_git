/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Maintains a cache of Pa Zone mappings, also listens
  * out for changes to database regarding the PA_ZONE table
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <algorithm>

#include "core/exceptions/src/PasExceptions.h"
#include "core/data_access_interface/pa_4669/src/IConfigPaDvaMessage.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "core/message/types/ConfigUpdate_MessageTypes.h"

using namespace TA_Base_Core;

namespace TA_IRS_Bus
{

    static void toUpper(std::string& convertMe)
    {
        std::transform(convertMe.begin(), convertMe.end(), convertMe.begin(), toupper);
    }


    PaStationDvaMessageCachedMap::PaStationDvaMessageCachedMap()
    :
    GenericCachedMap<StationDvaMessageRecord, TA_Base_Core::IPaDvaMessage>
        (TA_Base_Core::PA_DVA_MESSAGE, "StationDvaMessage"),
        m_configUpdateSender(0),
        m_locationKey(TA_Base_Core::DatabaseKey::getInvalidKey())
    {
    }

    PaStationDvaMessageCachedMap::~PaStationDvaMessageCachedMap()
    {
        try
        {
            delete m_configUpdateSender;
            m_configUpdateSender = 0;
        }
        catch (...)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }

    void PaStationDvaMessageCachedMap::setStationDvaMessageRecord(StationDvaMessageRecord& record, 
                                                                  bool writeChangesToDataBase)
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::DataConfigurationException)
    {
        // Only nonzero primary key present in database
        bool isExistingRecord = TA_Base_Core::DatabaseKey::isValidKey(record.m_pKey);

        TA_ASSERT( isExistingRecord || writeChangesToDataBase, "setStationDvaMessageRecord called with a new record without writeChangesToDataBase");


        if (writeChangesToDataBase)
        {
			TA_Base_Core::IConfigPaDvaMessage* configPaDvaMessage = NULL;

            if (isExistingRecord)
            {
               LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                   "Writing modified StationDvaMessageRecord for pKey=%d", 
                   record.m_pKey);

                TA_Base_Core::IPaDvaMessage* paDvaMessage = 
                    TA_Base_Core::PaDvaMessageAccessFactory::getInstance().getPaDvaMessage(record.m_pKey, true);
                configPaDvaMessage = dynamic_cast<TA_Base_Core::IConfigPaDvaMessage*>(paDvaMessage);

                TA_ASSERT(configPaDvaMessage, "dynamic_cast failed on a configPaDvaMessage");
            }
            else
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                   "Writing new StationDvaMessageRecord");

                configPaDvaMessage = TA_Base_Core::PaDvaMessageAccessFactory::getInstance().createPaDvaMessage();

                configPaDvaMessage->setId(record.m_id);
                configPaDvaMessage->setLocationKey(record.m_locationKey);
                configPaDvaMessage->setTisMessageTag(record.m_tisMessageTag);
                configPaDvaMessage->setTisLibrarySection(record.m_tisLibrarySection);
                configPaDvaMessage->setTisLibraryVersion(record.m_tisLibraryVersion);
            }

            try
            {
                char buff[2];
                buff[0] = record.m_type;
                buff[1] = 0;

                configPaDvaMessage->setLabel(record.m_label);
                configPaDvaMessage->setType(buff);

                configPaDvaMessage->applyChanges();

                std::vector<std::string> columnChanges;
                TA_Base_Core::ItemChanges changeMap = configPaDvaMessage->getAllItemChanges();
                for (TA_Base_Core::ItemChanges::iterator it = changeMap.begin();
                            it != changeMap.end();
                            it ++)
                {
                    columnChanges.push_back(it->first);
                }

                if (isExistingRecord)
                {
                    // Modification of existing
                    submitConfigUpdate(record.m_pKey, TA_Base_Core::Update, columnChanges);
                }
                else
                {
                    // New group added
                    submitConfigUpdate(configPaDvaMessage->getKey(), TA_Base_Core::Create, columnChanges);
                }

                delete configPaDvaMessage;
                configPaDvaMessage = NULL;
            }
            catch(...)
            {
                delete configPaDvaMessage;
                configPaDvaMessage = NULL;

                throw;
            }
        }
    
        // Update the local cached data - record may or may not exist currently..
        updateRecordMapWithItem(record.m_pKey, record);
    }


    bool PaStationDvaMessageCachedMap::processDvaMessageUpdates(
                    const TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData& updateData)                                    
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {    
        bool changesDetected = false;

        for (int i = 0; i < updateData.length(); i ++)
        {    
            LockedPacket<StationDvaMessageRecord> updateRecord =
                            getConfigurableCachedDbRecordFromKey(updateData[i].m_pKey);
        
            StationDvaMessageRecord oldRecord = updateRecord.getMutableData();

            // These functions will update the internal map directly
            // (hence why the record needs to be locked)
            updateRecord.getMutableData().m_locationKey         = updateData[i].m_locationKey;
            updateRecord.getMutableData().m_id                  = updateData[i].m_id;
            updateRecord.getMutableData().m_label               = updateData[i].m_label;
            updateRecord.getMutableData().m_type                = updateData[i].m_type;
            updateRecord.getMutableData().m_tisMessageTag       = updateData[i].m_tisMessageTag;
            updateRecord.getMutableData().m_tisLibrarySection   = updateData[i].m_tisLibrarySection;
            updateRecord.getMutableData().m_tisLibraryVersion   = updateData[i].m_tisLibraryVersion;                
        
            // != operator not defined..
            bool matchingData = (oldRecord == updateRecord.getMutableData());

            if (!matchingData)
            {
                // Something must have changed
                changesDetected = true;
            }

        }

        return changesDetected;
    }


    unsigned long PaStationDvaMessageCachedMap::getKeyFromStationDvaMessageIdAndLocation(unsigned long messageId, unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        FUNCTION_ENTRY("getKeyFromStationDvaMessageIdAndLocation");

        FilterStationDvaMessageByIdAndLocation filter(messageId, locationKey);

        StationDvaMessageRecords matchingMessages = getFirstMatchingRecord(filter);

        if (matchingMessages.size() > 0)
        {
            // Matching item found
            return matchingMessages.front().m_pKey;
        }

        // Record not found
        std::ostringstream msg;

        msg << "Failed to locate StationDvaMessageRecord with locationKey=" 
            << locationKey << " and messageId=" 
            << messageId << " in KeyToStationDvaMessageRecordMap";
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));

        return 0;
        FUNCTION_EXIT;
    }


    PaStationDvaMessageCachedMap::FilterStationDvaMessageByIdAndLocation::FilterStationDvaMessageByIdAndLocation(
        unsigned long messageId, 
        unsigned long locationKey)
        :
        m_id(messageId),
        m_locationKey(locationKey)
    {}


    bool PaStationDvaMessageCachedMap::FilterStationDvaMessageByIdAndLocation::isMatch(
            const StationDvaMessageRecord& record) const
    {
        return record.m_id == m_id &&
               record.m_locationKey == m_locationKey;
    }


    TA_Base_Core::IPaDvaMessage* PaStationDvaMessageCachedMap::getRecordFromDatabaseUsingKey(unsigned long pKey, bool readWrite)
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException)
    {
        return TA_Base_Core::PaDvaMessageAccessFactory::getInstance().getPaDvaMessage(pKey, readWrite);
    }


    void PaStationDvaMessageCachedMap::validateRecordData(const StationDvaMessageRecord& dvaMessage)
    {
        // Validate various parameters (failure on a true condition met)
        performDatavalueValidation(0 == dvaMessage.m_locationKey,           dvaMessage.m_pKey);
        performDatavalueValidation(dvaMessage.m_id == 0,                    dvaMessage.m_pKey);

        // The operator can enter empty ad-hoc labels if they want..
        //performDatavalueValidation(dvaMessage.m_label.length() == 0,        dvaMessage.m_pKey);
    
        performDatavalueValidation(dvaMessage.m_type != 'N' && 
                                    dvaMessage.m_type != 'E' &&
                                    dvaMessage.m_type != 'X',
                                        dvaMessage.m_pKey);

        // Can't validate entity key value, since it may be null
    }
         

    StationDvaMessageRecord PaStationDvaMessageCachedMap::createRecordFromInterface(TA_Base_Core::IPaDvaMessage& item)
         //throw (TA_Base_Core::InvalidPasConfigurationException)
    {
        StationDvaMessageRecord result;

        result.m_pKey = item.getKey();
        result.m_locationKey = item.getLocationKey();
        result.m_id = item.getId();
        result.m_label = item.getLabel();

        // Should be a single character for type
        performDatavalueValidation(item.getType().length() != 1, item.getKey());

        result.m_type = item.getType()[0];
        result.m_tisMessageTag = item.getTisMessageTag();
        result.m_tisLibrarySection = item.getTisLibrarySection();
        result.m_tisLibraryVersion = item.getTisLibraryVersion();

        return result;
    }


    StationDvaMessageRecordPointers PaStationDvaMessageCachedMap::getClonedPrerecordedMessageRecords
        ( TA_Base_Bus::IPAAgentCorbaDef::EDvaSet dvaSet, 
          char messageType, 
          const std::string& filterKeyword )
    {

        FUNCTION_ENTRY("getClonedPrerecordedMessageRecords");
        TA_ASSERT((messageType == 'N' || messageType == 'E'), "Unrecognised messageType");

        unsigned long minMessageId, maxMessageId;

        switch (dvaSet)
        {
        case TA_Base_Bus::IPAAgentCorbaDef::SetA:
            minMessageId = TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 1;

            maxMessageId = TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                           TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC;
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::SetB:
            minMessageId = TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                           TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC +
                           TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 1;

            maxMessageId = TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                           TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC +
                           TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                           TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC;
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::SetD:
            minMessageId = TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 1;

            maxMessageId = TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                           TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC;
            break;
        default:
            TA_ASSERT(false, "Invalid DVA Set specified");
        }

        std::string ucFilterKeyword(filterKeyword);
        toUpper(ucFilterKeyword);

        StationDvaMessageRecords allMessages = getAllRecords();
        StationDvaMessageRecordPointers result;

        for (StationDvaMessageRecordsIt it = allMessages.begin(); 
                it != allMessages.end();
                ++it)
        {
            unsigned long messageId = (*it).m_id;

            if (messageId >= minMessageId && messageId <= maxMessageId)
            {
                // MessageId is in range

                if ((*it).m_type == messageType)
                {
                    // MessageType matches the one specified
                
                    if (!ucFilterKeyword.empty())
                    {
                        std::string ucSource ((*it).m_label);
		                toUpper(ucSource);
		                
		                if (std::string::npos != ucSource.find(ucFilterKeyword))
		                {
			                // Found keyword in description
                            result.push_back(new StationDvaMessageRecord(*it));

		                }
                    }
                    else
                    {
                        result.push_back(new StationDvaMessageRecord(*it));
                    }
                }
            }
        }

        FUNCTION_EXIT;
        return result;
    }


    bool PaStationDvaMessageCachedMap::isAdhocMessageId(TA_Base_Bus::IPAAgentCorbaDef::EDvaSet dvaSet,
                                                        unsigned long id)
    {
        unsigned long minMessageId, maxMessageId;
        getMinMaxAdhocRange(dvaSet, minMessageId, maxMessageId);

        return id >= minMessageId && id <= maxMessageId;
    }


    void PaStationDvaMessageCachedMap::getMinMaxAdhocRange(TA_Base_Bus::IPAAgentCorbaDef::EDvaSet dvaSet,
                                                           unsigned long& minMessageId,
                                                           unsigned long& maxMessageId)
    {
        switch (dvaSet)
        {
        case TA_Base_Bus::IPAAgentCorbaDef::SetA:
            minMessageId = 1;
            maxMessageId = TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC;
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::SetB:
            minMessageId = TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                           TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC + 1;

            maxMessageId = TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                           TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC +
                           TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC;
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::SetD:
            minMessageId = 1;
            maxMessageId = TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC;
            break;
        default:
            TA_ASSERT(false, "Invalid DVA Set specified");
        }
    }


    StationDvaMessageRecordPointers PaStationDvaMessageCachedMap::getClonedAdhocMessageRecords
        ( TA_Base_Bus::IPAAgentCorbaDef::EDvaSet dvaSet )
    {
        FUNCTION_ENTRY("getClonedAdhocMessageRecords");

        unsigned long minMessageId, maxMessageId;
        getMinMaxAdhocRange(dvaSet, minMessageId, maxMessageId);

        StationDvaMessageRecords allMessages = getAllRecords();
        StationDvaMessageRecordPointers result;

        for (StationDvaMessageRecordsIt it = allMessages.begin(); 
                it != allMessages.end();
                it ++)
        {
            unsigned long messageId = (*it).m_id;

            if (messageId >= minMessageId && messageId <= maxMessageId)
            {
                // MessageId is in range     
                result.push_back(new StationDvaMessageRecord(*it));
            }
        }

        FUNCTION_EXIT;
        return result;
    }


    void PaStationDvaMessageCachedMap::refreshKeyToStationDvaMessageRecordMap(unsigned long locationKey) 
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        FUNCTION_ENTRY("refreshKeyToStationDvaMessageRecordMap");

        m_locationKey = locationKey;

        // Obtain read only array of dva messages
        TA_Base_Core::IPaDvaMessages dvaMessages;
    
        if (-1 == locationKey)
        {
            dvaMessages = TA_Base_Core::PaDvaMessageAccessFactory::getInstance().getAllPaDvaMessages(false);
        }
        else
        {
            dvaMessages = TA_Base_Core::PaDvaMessageAccessFactory::getInstance().getPaDvaMessagesByLocationKey(locationKey, false);
        }

        try
        {
            refreshKeyToRecordMapUsing(dvaMessages);
        }
        catch (...)
        {
            // Clean up (this is defined in GenericCachedMap.inl)
            deleteAllItemsInArray(dvaMessages);
            throw;
        }
    
        // Clean up (this is defined in GenericCachedMap.inl)
        deleteAllItemsInArray(dvaMessages);

        FUNCTION_EXIT;
    }

    // ExceptionChecked
    void PaStationDvaMessageCachedMap::submitConfigUpdate( unsigned long recordKey, 
                                                    TA_Base_Core::EModificationType modificationType, 
                                                    const std::vector<std::string>& changes)
    {
        FUNCTION_ENTRY("submitConfigUpdate");   

        if (0 == m_configUpdateSender)
        {
            // 4669FAT TD #782: Postponed object creation (out of constructor), so only
            // created on an as-needed basis (config editor doesn't require this)
            m_configUpdateSender = 
                TA_Base_Core::MessagePublicationManager::getInstance().
                getConfigUpdateMessageSender( TA_Base_Core::ConfigUpdate::Context );
        }
        
        TA_ASSERT(m_configUpdateSender, "m_configUpdateSender is NULL");

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Sending ConfigPaDvaMessage ConfigUpdate message to subscribers");

        try
        {
            m_configUpdateSender->sendConfigUpdateMessage(
                TA_Base_Core::ConfigUpdate::ConfigPaDvaMessage,          // Message Type
                recordKey,                                          // Key of changed item
                modificationType,                                   // EModificationType (upd/del)
                changes,                                            // Desc of changes (col names)
                0);                                                 // FilterableData

            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "PA_DVA_MESSAGE changed");
        } // try block
        catch(...)
        {
            std::ostringstream msg;
            msg << "Failed to submit the PaStationDvaMessageCachedMap Config Update Message";
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, msg.str().c_str());
        }    

        FUNCTION_EXIT;
    }



    PaStationDvaMessageCachedMap::FilterStationDvaMessageIdRange::FilterStationDvaMessageIdRange(
        unsigned long locationKey,
        unsigned long minId,
        unsigned long maxId)
        :
        m_locationKey(locationKey),
        m_minId(minId),
        m_maxId(maxId)
    {}


    bool PaStationDvaMessageCachedMap::FilterStationDvaMessageIdRange::isMatch(
            const StationDvaMessageRecord& record) const
    {
        if (TA_Base_Core::DatabaseKey::isValidKey(m_locationKey))
        {
            // If the location key filter is properly set, reject non-matches
            if (m_locationKey != record.m_locationKey)
            {
                return false;
            }    
        }

        return record.m_id >= m_minId && record.m_id <= m_maxId;
    }


    bool PaStationDvaMessageCachedMap::processDatabaseRemovalNotification(unsigned long pKey)
    {
        bool forThisLocation = false;

        // If no location key specificed locally, assume a match
        if (TA_Base_Core::DatabaseKey::getInvalidKey() == m_locationKey)
        {
            forThisLocation = true;
        }

        if (doesRecordExistInMap(pKey))
        {
            if (getCachedDbRecordFromKey(pKey).m_locationKey == m_locationKey)
            {
                forThisLocation = true;
            }
        }

        bool changed = GenericCachedMap<StationDvaMessageRecord, TA_Base_Core::IPaDvaMessage>::processDatabaseRemovalNotification(pKey);
        
        if (!forThisLocation)
        {
            // Return no changes - because change was completely for a different station
            return false;
        }
        else
        {
            return changed;
        }
    }


    bool PaStationDvaMessageCachedMap::processDatabaseUpdateCreateNotification(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        // If the operator has changed the location key of a message from a foreign location to 'this'
        // location, we will never receive the update (because we weren't listening out for that item)
        bool forThisLocation = false;

        // If no location key specificed locally, assume a match
        if (TA_Base_Core::DatabaseKey::getInvalidKey() == m_locationKey)
        {
            forThisLocation = true;
        }

        if (!forThisLocation && doesRecordExistInMap(updateEvent.getKey()) )
        {
            if (getCachedDbRecordFromKey(updateEvent.getKey()).m_locationKey == m_locationKey)
            {
                forThisLocation = true;
            }
        }

        bool changed = GenericCachedMap<StationDvaMessageRecord, TA_Base_Core::IPaDvaMessage>::processDatabaseUpdateCreateNotification(updateEvent);

        StationDvaMessageRecord newRecord = getCachedDbRecordFromKey(updateEvent.getKey());
        
        if (!forThisLocation)
        {
            if (getCachedDbRecordFromKey(updateEvent.getKey()).m_locationKey == m_locationKey)
            {
                forThisLocation = true;
            }
        }

        if (!forThisLocation)
        {
            // Return no changes - because change was completely for a different station
            return false;
        }
        else
        {
            return changed;
        }
    }

} // end namespace TA_IRS_Bus
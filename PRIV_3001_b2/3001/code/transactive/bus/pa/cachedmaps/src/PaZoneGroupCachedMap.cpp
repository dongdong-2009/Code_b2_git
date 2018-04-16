/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/bus/pa/cachedmaps/src/PaZoneGroupCachedMap.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Maintains a cache of Pa Zone mappings, also listens
  * out for changes to database regarding the PA_ZONE table
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4284)
#endif // defined _MSC_VER

#include "bus/pa/cachedmaps/src/PaZoneGroupCachedMap.h"
#include "core/exceptions/src/PasExceptions.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/pa_4669/src/PaZoneAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/ConfigPaZoneGroup.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/message/types/ConfigUpdate_MessageTypes.h"


namespace TA_IRS_Bus
{

    PaZoneGroupCachedMap::PaZoneGroupCachedMap()
    :
    GenericCachedMap<PaZoneGroupRecord, TA_Base_Core::IPaZoneGroup>
        (TA_Base_Core::PA_ZONE_GROUP, "PaZoneGroup"),
        m_configUpdateSender(0),
        m_refreshLocationKey(-1)
    {
    }

    PaZoneGroupCachedMap::~PaZoneGroupCachedMap()
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

    void PaZoneGroupCachedMap::refreshKeyToPaZoneGroupRecordMap(unsigned long locationKey) 
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        m_refreshLocationKey = locationKey;

        // Obtain read only array of dva messages
        TA_Base_Core::IPaZoneGroups paZoneGroups;
    
        if (0 == locationKey)
        {
            paZoneGroups = TA_Base_Core::PaZoneGroupAccessFactory::getInstance().getAllPaZoneGroups(false);
        }
        else
        {
            paZoneGroups = TA_Base_Core::PaZoneGroupAccessFactory::getInstance().getPaZoneGroupsByLocationKey(locationKey, false);
        }

        try
        {
            refreshKeyToRecordMapUsing(paZoneGroups);
        }
        catch (...)
        {
            // Clean up (this is defined in GenericCachedMap.inl)
            deleteAllItemsInArray(paZoneGroups);
            throw;
        }

        // Clean up (this is defined in GenericCachedMap.inl)
        deleteAllItemsInArray(paZoneGroups);
    }


    TA_Base_Core::IPaZoneGroup* PaZoneGroupCachedMap::getRecordFromDatabaseUsingKey(
                                                 unsigned long pKey, 
                                                 bool readWrite)
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException)
    {
        return TA_Base_Core::PaZoneGroupAccessFactory::getInstance().getPaZoneGroup(pKey, readWrite);
    }


    PaZoneGroupRecord PaZoneGroupCachedMap::createRecordFromInterface(TA_Base_Core::IPaZoneGroup& item)
                            //throw (TA_Base_Core::InvalidPasConfigurationException)
    {
        PaZoneGroupRecord result;

        result.m_pKey = item.getKey();
        result.m_locationKey = item.getLocationKey();
        result.m_label = item.getLabel();
  
        // isEventGroup should be boolean value
        performDatavalueValidation(
                item.getIsEventGroup() != 0 && item.getIsEventGroup() != 1,
                item.getKey());

        result.m_isEventGroup = (0 != item.getIsEventGroup());

        return result;
    }


    void PaZoneGroupCachedMap::validateRecordData(const PaZoneGroupRecord& paZoneGroup)
    {
        // Validate various parameters (failure on a true condition met)
        performDatavalueValidation(0 == paZoneGroup.m_locationKey,    paZoneGroup.m_pKey);
        performDatavalueValidation(paZoneGroup.m_label.length() == 0, paZoneGroup.m_pKey);
    } 


    TA_Base_Core::PrimitiveWrapper<PaZoneGroupRecord>
    PaZoneGroupCachedMap::getKeyFromLocationAndLabel(unsigned long locationKey,
                                                     const std::string& label)
    {
        TA_Base_Core::PrimitiveWrapper<PaZoneGroupRecord> result;

        FilterPaZoneGroupByLocationAndLabel filter(locationKey, label);

        PaZoneGroupRecords matchingGroups = getFirstMatchingRecord(filter);

        if (matchingGroups.size() > 0)
        {
            // Found match
            result.setValue(matchingGroups.front());
        }

        return result;
    }

    PaZoneGroupCachedMap::FilterPaZoneGroupByLocationAndLabel::FilterPaZoneGroupByLocationAndLabel(
                unsigned long locationKey,
                const std::string& label)
                : 
                m_locationKey(locationKey),
                m_label(label)
    {
    }

    bool PaZoneGroupCachedMap::FilterPaZoneGroupByLocationAndLabel::isMatch(
               const PaZoneGroupRecord& record) const
    {
        return record.m_locationKey == m_locationKey && 
                record.m_label == m_label;
    }


    void PaZoneGroupCachedMap::removePaZoneGroup(unsigned int locationKey, const std::string& label)
    {
        TA_Base_Core::PrimitiveWrapper<PaZoneGroupRecord> zoneGroupKey = 
                    getKeyFromLocationAndLabel(locationKey, label);

        if (!zoneGroupKey.hasBeenSet())
        {
            // Item not present
            TA_THROW(TA_Base_Core::DataException("PA Zone Group Missing",
                                            TA_Base_Core::DataException::NO_VALUE,
                                            "Location / Label"));
        }

        std::auto_ptr<TA_Base_Core::IConfigPaZoneGroup> group
                (dynamic_cast<TA_Base_Core::IConfigPaZoneGroup*>
                    (TA_Base_Core::PaZoneGroupAccessFactory::getInstance().
                             getPaZoneGroup(zoneGroupKey.getValue().m_pKey, true)));
    
        if (0 != group.get())
        {
            group->deleteThisObject();     

            // Remove item from local cache
            removeRecordMapItem(zoneGroupKey.getValue().m_pKey);

            std::vector<std::string> nullChanges;

            submitConfigUpdate(zoneGroupKey.getValue().m_pKey, TA_Base_Core::Delete, nullChanges);
        }
        else
        {
            TA_THROW(TA_Base_Core::DataException("PA Zone Group Missing",
                                            TA_Base_Core::DataException::NO_VALUE,
                                            "Location / Label"));
        }
    }


    unsigned long PaZoneGroupCachedMap::createNewPaZoneGroup
    (
     unsigned int locationKey, 
     std::string label, 
     bool isEventGroup,
     const std::list<unsigned long>& zoneKeys,
     TA_Base_Core::PrimitiveWrapper<unsigned long> existingKey
    )
    //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        std::auto_ptr<TA_Base_Core::IConfigPaZoneGroup> group(0);

        if (existingKey.hasBeenSet())
        {
            // Existing item, overwrite it (by obtaining readonly version)
            group = std::auto_ptr<TA_Base_Core::IConfigPaZoneGroup>
                     (dynamic_cast<TA_Base_Core::IConfigPaZoneGroup*>
                        (TA_Base_Core::PaZoneGroupAccessFactory::getInstance().
                             getPaZoneGroup(existingKey.getValue(), true)));

            // Remove all existing zone group helper records (since we're
            // overwriting them with more up to date data)
            if (0 != group.get())
            {          
                group->destroyExistingPaZoneGroupHelperRecords();
            }
            else
            {
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to retrieve specified group" );
				return 0;
            }
        }
        else
        {
            // No item exists with matching location/label, create a new one
            group = std::auto_ptr<TA_Base_Core::IConfigPaZoneGroup>
                    (TA_Base_Core::PaZoneGroupAccessFactory::getInstance().createPaZoneGroup());
        }

        if (0 == group.get())
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No group to work with" );
            return 0;
        }
    
        group->setLocationKey(locationKey);
        group->setLabel(label);
        group->setIsEventGroup(isEventGroup);
    
        std::vector<std::string> columnChanges;
        TA_Base_Core::ItemChanges changeMap = group->getAllItemChanges();
        for (TA_Base_Core::ItemChanges::iterator it = changeMap.begin();
                    it != changeMap.end();
                    it ++)
        {
            columnChanges.push_back(it->first);
        }
    
        // Write the group database - may fail if someone took this 
        // label/location before we did (provided we're creating a new group)
        group->applyChanges();
    
        // Create the associated helper records to associate this group
        // With its related zones
        group->createPaZoneGroupHelperRecords(zoneKeys);

        bool isNew = !existingKey.hasBeenSet();

        // Refresh the local cache (so up to date immediately upon this change)
        updateRecordMapWithItemFromDatabase(*group.get(), isNew);

        // Post configuration update
        if (existingKey.hasBeenSet())
        {
            // Modification of existing
            submitConfigUpdate(group->getKey(), TA_Base_Core::Update, columnChanges);
        }
        else
        {
            // New group added
            submitConfigUpdate(group->getKey(), TA_Base_Core::Create, columnChanges);
        }

        return group->getKey();
    }
    

    // ExceptionChecked
    void PaZoneGroupCachedMap::submitConfigUpdate(unsigned long recordKey, 
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
        
        try
        {
			if (NULL == m_configUpdateSender)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_configUpdateSender is NULL" );

				FUNCTION_EXIT;
				return;
			}

            m_configUpdateSender->sendConfigUpdateMessage(
                TA_Base_Core::ConfigUpdate::ConfigPaZoneGroup,           // Message Type
                recordKey,                                          // Key of changed item
                modificationType,                                   // EModificationType (upd/del)
                changes,                                            // Desc of changes (col names)
                0);                                                 // FilterableData

            // ToDo - does sendAuditMessage automatically log?
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Zone Group Changed");
        } // try block
        catch(...)
        {
            std::ostringstream msg;
            msg << "Failed to submit the PaZoneGroupCachedMap Config Update Message";
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, msg.str().c_str());
        }    

        FUNCTION_EXIT;
    }


    std::list<unsigned long> PaZoneGroupCachedMap::getZonesAssociatedWithGroup(
                                                           unsigned long locationKey, 
                                                           const std::string& label)
    {
        TA_Base_Core::PrimitiveWrapper<PaZoneGroupRecord> keyForGroup =
                    getKeyFromLocationAndLabel(locationKey, label);

        if (!keyForGroup.hasBeenSet())
        {
            TA_THROW(TA_Base_Core::DataException("PA Zone Group Missing",
                                            TA_Base_Core::DataException::NO_VALUE,
                                            "Location / Label"));

        }

        // Grab all zone keys, allow exceptions to propogated
        std::list<unsigned long> result = TA_Base_Core::PaZoneAccessFactory::getInstance().
                                            getPaZoneKeysByGroupId(keyForGroup.getValue().m_pKey);

        return result;
    }


    PaZoneGroupRecords PaZoneGroupCachedMap::getPaZoneGroupsByEventFlag(bool isEventGroup)
    {
        FilterPaZoneGroupByLocationKeyAndEventGroup filter(m_refreshLocationKey, isEventGroup);

        return getAllRecords(&filter);
    }


    PaZoneGroupCachedMap::FilterPaZoneGroupByLocationKeyAndEventGroup::
                FilterPaZoneGroupByLocationKeyAndEventGroup(unsigned long locationKey, 
                                                            bool isEventGroup)
        :
        m_isEventGroup(isEventGroup),
        m_locationKey(locationKey)
    {}


    bool PaZoneGroupCachedMap::FilterPaZoneGroupByLocationKeyAndEventGroup::isMatch(
            const PaZoneGroupRecord& record) const
    {
        return record.m_locationKey == m_locationKey &&
                record.m_isEventGroup == m_isEventGroup;
    }


    PaZoneGroupCachedMap::ZoneGroupFilter::ZoneGroupFilter()
    {
    }


    bool PaZoneGroupCachedMap::ZoneGroupFilter::isMatch(
            const PaZoneRecord& record) const
    {
        // Filter out all elements with an invalid entity name
        if (0 == record.m_stnEquipmentEntityName.length() &&
            0 == record.m_occEquipmentEntityName.length() )
        {
            // Exclude items with invalid names
            return false;
        }
        else
        {
            // T4669FAT TD #680: need to reject DEPOT zones, if at the Overall schematic
            // (because when ALL keyword sent from schematics, depot zones appear in PA List
            // despite not being visible on the HMIs overall schematic)
            // [TBD] C955 doesn't have such requirement yet
            //if (m_isOccZoneGroup && m_depotLocationKey == record.m_locationKey)
            //{
            //    // Reject depot locations whilst at OCC
            //    return false;
            //}
            return true;
        }
    }

} // end namespace TA_IRS_Bus


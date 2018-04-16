/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Karen Graham
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This class uses the Data Access Interface to retrieve DB information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"

#include "core/data_access_interface/pa/src/IConfigPaFasTriggeredBroadcast.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/src/DatabaseKey.h"

#include "bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaZoneGroupCachedMap.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/pa/pa_agent/IDL/src/IPAAgentCorbaDef.h"

#include "app/pa/PAAgent/src/GlobalConstants.h"

using namespace TA_Base_Core;
//using namespace TA_IRS_Core;
using namespace TA_IRS_Bus;
using namespace TA_Base_Bus;

namespace TA_IRS_App
{


    PaFasTriggeredBroadcastDatabaseAccessor::PaFasTriggeredBroadcastDatabaseAccessor() : 
    m_configItems(),
    m_isLoaded(false),
    m_stationDvaMessageCachedMap(0),
    m_zoneGroupCachedMap(0),
    m_fasAlarmEntities()
    {
        m_configItems.clear();
    }


    PaFasTriggeredBroadcastDatabaseAccessor::~PaFasTriggeredBroadcastDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (ConfigItemsIt it = m_configItems.begin(); it != m_configItems.end(); ++it)
            {
                if( (it->second) != NULL )
                {
                    delete it->second;
                    it->second = NULL;
                }
            }
            m_configItems.clear();
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        for ( FasAlarmEntityMap::iterator itLoop = m_fasAlarmEntities.begin(); m_fasAlarmEntities.end() != itLoop; ++itLoop )
        {
            delete itLoop->second;
            itLoop->second = NULL;
        }
        m_fasAlarmEntities.clear();
    
        delete m_stationDvaMessageCachedMap;
        m_stationDvaMessageCachedMap = 0;

        delete m_zoneGroupCachedMap;
        m_zoneGroupCachedMap = 0;
        
        FUNCTION_EXIT;
    }


    void PaFasTriggeredBroadcastDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the PA Zones and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (ConfigItemsIt assocToDelete = m_configItems.begin(); assocToDelete != m_configItems.end(); ++assocToDelete)
        {
            if( (assocToDelete->second) != NULL )
            {
                delete assocToDelete->second;
                assocToDelete->second = NULL;
            }
        }
        m_configItems.clear();

        // Now reload all the PA Zones
        IPaFasTriggeredBroadcasts items = PaFasTriggeredBroadcastAccessFactory::getInstance().getAllPaFasTriggeredBroadcasts(true);

        // And insert the PA Zones into our map. At the same time cast them to IConfigPaFasTriggeredBroadcast*
        // instead of IPaFasTriggeredBroadcast*.
        for (IPaFasTriggeredBroadcastsIt it = items.begin(); it != items.end(); ++it)
        {
            TA_Base_Core::IPaFasTriggeredBroadcast* item = *it;
            TA_Base_Core::IConfigPaFasTriggeredBroadcast* configItem = dynamic_cast<TA_Base_Core::IConfigPaFasTriggeredBroadcast*>(item);
            if( configItem != NULL )
            {
                unsigned long key = configItem->getUniqueIdentifier();
                m_configItems[key] = configItem;
            }
        }

        refreshCachedMaps();

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> PaFasTriggeredBroadcastDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for this class");

        std::multimap<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long, PaFasTriggeredBroadcastDatabaseAccessor::CfgPaFasTriggeredBroadcastRecord> PaFasTriggeredBroadcastDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long, CfgPaFasTriggeredBroadcastRecord> details; 

        // The maximum range is the number of locations times by 2. This is because we want to step for each
        // Location while it is loaded and then again while each Location is added to the list.
        progress.SetRange( 0, m_configItems.size() * 2 );

        // Step through all the objects and retrieve the details for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.

        CString buffer;
        for (ConfigItemsIt it = m_configItems.begin(); it != m_configItems.end(); ++it)
        {
            if (it->second != NULL)
            {
                CfgPaFasTriggeredBroadcastRecord record = getRecordFrom(*it->second);

                details.insert( std::map<unsigned long, CfgPaFasTriggeredBroadcastRecord>::value_type( it->second->getUniqueIdentifier(), record ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    PaFasTriggeredBroadcastDatabaseAccessor::CfgPaFasTriggeredBroadcastRecord PaFasTriggeredBroadcastDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the record matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(identifier);
        if (matching == m_configItems.end() || 0 == matching->second)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the PA Zone with identifier %lu",identifier);
            FUNCTION_EXIT;
            CfgPaFasTriggeredBroadcastRecord record;
            return record;
        }

        CfgPaFasTriggeredBroadcastRecord record = getRecordFrom(*matching->second);

        FUNCTION_EXIT;
        return record;
    }


    PaFasTriggeredBroadcastDatabaseAccessor::CfgPaFasTriggeredBroadcastRecord PaFasTriggeredBroadcastDatabaseAccessor::getRecordFrom(TA_Base_Core::IConfigPaFasTriggeredBroadcast& item)
    {
        CfgPaFasTriggeredBroadcastRecord record;

        if (TA_Base_Core::DatabaseKey::isInvalidKey(item.getKey()))
        {
            record.key = "";
        }
        else
        {
            record.key.Format("%ld", item.getKey()); 
        }

        if (TA_Base_Core::DatabaseKey::isInvalidKey(item.getLocationKey()))
        {
            record.location = "";
        }
        else
        {        
            record.location = getLocDisplayName(item.getLocationKey()).c_str();
        }

        if (TA_Base_Core::DatabaseKey::isInvalidKey(item.getPaCyclicTimes()))
        {
            record.cyclicTimes = "";
        }
        else
        {
            record.cyclicTimes.Format("%ld", item.getPaCyclicTimes());
        }

        if (TA_Base_Core::DatabaseKey::isInvalidKey(item.getFasAlarmEntityKey()))
        {
            record.fasAlarmEntityName = "";
        }
        else
        {
            record.fasAlarmEntityName = getFasAlarmEntityName(item.getFasAlarmEntityKey());
        }
        
        if (TA_Base_Core::DatabaseKey::isInvalidKey(item.getPaZoneGroupKey()))
        {
            record.zoneGroupName = "";
        }
        else
        {
            record.zoneGroupName = getZoneGroupName(item.getPaZoneGroupKey());
        }
  
        if (TA_Base_Core::DatabaseKey::isInvalidKey(item.getPaDvaMessageKey()))
        {
            record.dvaMessage = "";
        }
        else
        {
            record.dvaMessage = getDvaMessageName(item.getPaDvaMessageKey());
        }

        if (item.getIsEnabledParameter())
        {
            record.isEnabled = "是";
        }
        else
        {
            record.isEnabled = "否";
        }

        return record;
    }

    
    TA_Base_Core::IConfigItem* PaFasTriggeredBroadcastDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the PA Zone matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the PA Zone with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void PaFasTriggeredBroadcastDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the PA Zone matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the PA Zone with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored PA Zone was equal to NULL");

        if (!matching->second->isNew())
        {
            // TODO - check asserts
            matching->second->deleteThisObject();
        }
        delete matching->second;
        matching->second = NULL;
        m_configItems.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* PaFasTriggeredBroadcastDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigPaFasTriggeredBroadcast* newAssoc = TA_Base_Core::PaFasTriggeredBroadcastAccessFactory::getInstance().createPaFasTriggeredBroadcast();
        
        m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );

        return newAssoc;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* PaFasTriggeredBroadcastDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigPaFasTriggeredBroadcast* newAssoc = NULL;

        // Attempt to find the PA Zone matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(idOfItemToCopy);

        TA_ASSERT(matching != m_configItems.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigPaFasTriggeredBroadcast stored in the map");

        newAssoc = TA_Base_Core::PaFasTriggeredBroadcastAccessFactory::getInstance().copyPaFasTriggeredBroadcast(matching->second);

        if (newAssoc != NULL)
        {
            m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
        }
        
        return newAssoc;

        FUNCTION_EXIT;
    }


    bool PaFasTriggeredBroadcastDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(itemsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (ConfigItemsIt it = m_configItems.begin(); it != m_configItems.end();  ++it)
        {
            if( (it->second != NULL) && (it->second->hasChanged() || it->second->isNew()) )
            {
                // We just add a single empty item if we know a change has been made. We don't list each
                // individual change for PA Zones
                itemsNotApplied.push_back("");
                break;
            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }


    void PaFasTriggeredBroadcastDatabaseAccessor::refreshCachedMaps()
    {
        FUNCTION_ENTRY("refreshCachedMaps");

        // First retrieve all the subsystems from the database
        try
        {
            std::vector<ILocation*> vecLocations = LocationAccessFactory::getInstance().getAllLocations();
            for ( std::vector<ILocation*>::iterator itLoop = vecLocations.begin(); vecLocations.end() != itLoop; ++itLoop )
            {
                m_mapLocationKeyToDisplayName[(*itLoop)->getKey()] = (*itLoop)->getDisplayName();
            }
            // The cache will delete the vecLocations
            m_locationNameCache.refreshUsing(vecLocations);
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No Locations will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No Locations will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return;
        }

        /////////////////////////////////
        // Set up Station DVA Messages (all locations)  
        if ( NULL == m_stationDvaMessageCachedMap )
        {
            m_stationDvaMessageCachedMap = new PaStationDvaMessageCachedMap();
        }
        m_stationDvaMessageCachedMap->refreshKeyToStationDvaMessageRecordMap();

        /////////////////////////////////
        // Set up PA Zone Groups (all locations)  
        if ( NULL == m_zoneGroupCachedMap )
        {
            m_zoneGroupCachedMap = new PaZoneGroupCachedMap();
        }
        m_zoneGroupCachedMap->refreshKeyToPaZoneGroupRecordMap();
        /////////////////////////////////

        /////////////////////////////////
        // Set up FAS Alarm Entities
        try
        {
            for ( FasAlarmEntityMap::iterator itFasLoop = m_fasAlarmEntities.begin(); m_fasAlarmEntities.end() != itFasLoop; ++itFasLoop )
            {
                delete itFasLoop->second;
                itFasLoop->second = NULL;
            }
            m_fasAlarmEntities.clear();

            IEntityDataList vecFasEntities = EntityAccessFactory::getInstance().getFasAlarmEntities();
            for ( IEntityDataList::iterator itLoop = vecFasEntities.begin(); vecFasEntities.end() != itLoop; ++itLoop )
            {
                TA_Base_Core::PaFasTriggeredBroadcastAccessFactory::FasAlarmEntity* pNewRecord = 
                    new TA_Base_Core::PaFasTriggeredBroadcastAccessFactory::FasAlarmEntity();
                pNewRecord->pKey = (*itLoop)->getKey();
                pNewRecord->locaionKey = (*itLoop)->getLocation();
                pNewRecord->entityName = (*itLoop)->getName();
                pNewRecord->entityDescription = (*itLoop)->getDescription();
                m_fasAlarmEntities[(*itLoop)->getKey()] = pNewRecord;
            }
        }
        catch (...)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Unknown reason");
        }
        /////////////////////////////////

        FUNCTION_EXIT;
    }

    std::string PaFasTriggeredBroadcastDatabaseAccessor::getLocationName(unsigned long locationKey) const
    {
        return m_locationNameCache.getName(locationKey);
    }

    std::string PaFasTriggeredBroadcastDatabaseAccessor::getLocDisplayName(unsigned long locationKey) const
    {
        std::map<unsigned long, std::string>::const_iterator itFound = m_mapLocationKeyToDisplayName.find( locationKey );
        if ( m_mapLocationKeyToDisplayName.end() != itFound )
        {
            return itFound->second;
        }

        return "";
    }

        
    unsigned long PaFasTriggeredBroadcastDatabaseAccessor::getLocationKey(const std::string& locationName)
    {
        return m_locationNameCache.getKey(locationName);
    }  

    const TA_Base_App::MultipleChangeDialog::ItemsForSelection 
    PaFasTriggeredBroadcastDatabaseAccessor::getDataForSelection(EDataSet dataSet, unsigned long locationKeyFilter) const 
    {
        // locationKeyFilter defines a filter by location, -1 if no location
        switch (dataSet)
        {
            case DATA_ZONE_GROUP:
                return getZoneGroupDataForSelection(locationKeyFilter);
            case DATA_DVA_MESSAGE:
                return getDvaMessageDataForSelection(locationKeyFilter);
            case DATA_FAS_ALARM_ENTITY:
                return getFasAlarmEntityDataForSelection(locationKeyFilter);
            default:
                TA_ASSERT(FALSE, "getDataForSelection: Invalid data set specified");
        }
		return TA_Base_App::MultipleChangeDialog::ItemsForSelection();
    }


    const TA_Base_App::MultipleChangeDialog::ItemsForSelection 
    PaFasTriggeredBroadcastDatabaseAccessor::getZoneGroupDataForSelection(unsigned long locationKeyFilter) const
    {   
        TA_ASSERT(m_zoneGroupCachedMap != 0, "Zone Group Cached Map is NULL");
        
        TA_Base_App::MultipleChangeDialog::ItemsForSelection result;

        // Only want to have the event groups listing
        const bool isEventGroup = true;
        
        PaZoneGroupRecords groupData;

        if (TA_Base_Core::DatabaseKey::isInvalidKey(locationKeyFilter))
        {
            // Disregard the location filter
            PaZoneGroupCachedMap::FilterPaZoneGroupByEventGroup filter(isEventGroup);
            groupData = m_zoneGroupCachedMap->getAllRecords(&filter);
        }
        else
        {
            PaZoneGroupCachedMap::FilterPaZoneGroupByLocationKeyAndEventGroup filter(locationKeyFilter, isEventGroup);
            groupData.swap(m_zoneGroupCachedMap->getAllRecords(&filter));
        }

        for (PaZoneGroupRecordsIt itr = groupData.begin(); itr != groupData.end(); itr ++)
        {
            std::vector<std::string> vec;

            vec.push_back(getLocDisplayName(itr->m_locationKey));
            vec.push_back(itr->m_label);
            result[itr->m_pKey] = vec;
        }

        return result;
    }


    const TA_Base_App::MultipleChangeDialog::ItemsForSelection 
    PaFasTriggeredBroadcastDatabaseAccessor::getDvaMessageDataForSelection(unsigned long locationKeyFilter) const
    {
        TA_ASSERT(0 != m_stationDvaMessageCachedMap, "Station DVA Cached map is NULL");

        TA_Base_App::MultipleChangeDialog::ItemsForSelection result;

        DWORD idMin = PA_DVA_MSG_ID_MIN;                

        DWORD idMax = PA_DVA_MSG_ID_MAX;

        PaStationDvaMessageCachedMap::FilterStationDvaMessageIdRange filter(locationKeyFilter, idMin, idMax);
        StationDvaMessageRecords dvaMessages = m_stationDvaMessageCachedMap->getAllRecords(&filter);
                
        for (StationDvaMessageRecords::const_iterator itr = dvaMessages.begin();
                itr != dvaMessages.end();
                itr ++)
        {
            std::vector<std::string> vec;
            
            // Result should have "location/type/label"
            vec.push_back(getLocDisplayName(itr->ulLocationKey));
            //vec.push_back(itr->getTypeAsString());
            vec.push_back(itr->strMsgName);

            result[itr->ulKey] = vec;
        }

        return result;
    }

    const TA_Base_App::MultipleChangeDialog::ItemsForSelection 
    PaFasTriggeredBroadcastDatabaseAccessor::getFasAlarmEntityDataForSelection( unsigned long locationKeyFilter ) const
    {
        TA_Base_App::MultipleChangeDialog::ItemsForSelection result;

        std::map<unsigned long, unsigned long> mapExistAlarmEntityFilter;
        for ( ConfigItems::const_iterator itLoop = m_configItems.begin(); m_configItems.end() != itLoop; ++itLoop )
        {
            mapExistAlarmEntityFilter[itLoop->second->getFasAlarmEntityKey()] = itLoop->second->getFasAlarmEntityKey();
        }

        for ( FasAlarmEntityMap::const_iterator itLoop = m_fasAlarmEntities.begin(); m_fasAlarmEntities.end() != itLoop; ++itLoop )
        {
            unsigned long ulFasAlarmEntity = itLoop->second->pKey;
            std::map<unsigned long, unsigned long>::iterator itExist = mapExistAlarmEntityFilter.find( ulFasAlarmEntity );
            if ( mapExistAlarmEntityFilter.end() != itExist )
            {
                continue;
            }

            if ( TA_Base_Core::DatabaseKey::getInvalidKey() == locationKeyFilter )
            {
                std::vector<std::string> vec;
                // Result should have "location/type/label"
                vec.push_back(getLocDisplayName(itLoop->second->locaionKey));
                vec.push_back(itLoop->second->entityName);
                vec.push_back(itLoop->second->entityDescription);
                result[ulFasAlarmEntity] = vec;
            }
            else if ( itLoop->second->locaionKey == locationKeyFilter )
            {
                std::vector<std::string> vec;
                // Result should have "location/type/label"
                vec.push_back(getLocDisplayName(locationKeyFilter));
                vec.push_back(itLoop->second->entityName);
                vec.push_back(itLoop->second->entityDescription);
                result[ulFasAlarmEntity] = vec;
            }
        }

        return result;
    }


    std::vector<TA_Base_App::MultipleChangeDialog::HeaderProperties>
    PaFasTriggeredBroadcastDatabaseAccessor::getDataSetHeaderProperties(EDataSet dataSet)
    {
        std::vector<TA_Base_App::MultipleChangeDialog::HeaderProperties> props;
        TA_Base_App::MultipleChangeDialog::HeaderProperties hp;

        // NOTE: Any changes here need to be reflected in the function getDataForSelection
        switch (dataSet)
        {
            case DATA_ZONE_GROUP:
                hp.label = "车站";
                hp.width = 70;
                props.push_back(hp);
                hp.label = "广播区域组";
                hp.width = LVSCW_AUTOSIZE_USEHEADER;
                props.push_back(hp);
                break;
            case DATA_DVA_MESSAGE:
                hp.label = "车站";
                hp.width = 70;
                props.push_back(hp);
                hp.label = "类型";
                hp.width = 100;
                props.push_back(hp);
                hp.label = "名称";
                hp.width = LVSCW_AUTOSIZE_USEHEADER;
                props.push_back(hp);
                break;
            case DATA_FAS_ALARM_ENTITY:   
                hp.label = "车站";
                hp.width = 70;
                props.push_back(hp);
                hp.label = "火灾报警点";
                hp.width = 280;
                props.push_back(hp);
                hp.label = "描述";
                hp.width = LVSCW_AUTOSIZE_USEHEADER;
                props.push_back(hp);
                break;
            default:
                TA_ASSERT(FALSE, "Unrecognized enum ID");
        }

        return props;
    }


    CString PaFasTriggeredBroadcastDatabaseAccessor::getFasAlarmEntityName(unsigned long key, unsigned long* locationKeyOut)
    {
        FasAlarmEntityMap::iterator itr = m_fasAlarmEntities.find(key);

        if (itr != m_fasAlarmEntities.end())
        {
            if (NULL != locationKeyOut)
            {
                *locationKeyOut = itr->second->locaionKey;
            }

            return itr->second->entityName.c_str();
        }

        return "#未知#";
    }


    CString PaFasTriggeredBroadcastDatabaseAccessor::getZoneGroupName(unsigned long key, unsigned long* locationKeyOut)
    {
        TA_ASSERT(m_zoneGroupCachedMap != 0, "Zone Group Cached Map is NULL");

        try
        {
            PaZoneGroupRecord group = m_zoneGroupCachedMap->getCachedDbRecordFromKey(key);
            
            if (NULL != locationKeyOut)
            {
                *locationKeyOut = group.m_locationKey;
            }

            return group.m_label.c_str();
        }
        catch (const TA_Base_Core::CachedMappingNotFoundException&)
        {
            if (NULL != locationKeyOut)
            {
                *locationKeyOut = TA_Base_Core::DatabaseKey::getInvalidKey();
            }

            return "#未知#";
        }
    }


    CString PaFasTriggeredBroadcastDatabaseAccessor::getDvaMessageName(unsigned long key, unsigned long* locationKeyOut)
    {
        TA_ASSERT(0 != m_stationDvaMessageCachedMap, "Station DVA Cached map is NULL");

        try
        {
            StationDvaMessageRecord message = m_stationDvaMessageCachedMap->getCachedDbRecordFromKey(key);

            if (NULL != locationKeyOut)
            {
                *locationKeyOut = message.ulLocationKey;
            }

            return message.strMsgName.c_str();
        }
        catch (const TA_Base_Core::CachedMappingNotFoundException&)
        {
            if (NULL != locationKeyOut)
            {
                *locationKeyOut = TA_Base_Core::DatabaseKey::getInvalidKey();
            }

            return "#未知#";
        }
    }
}

    

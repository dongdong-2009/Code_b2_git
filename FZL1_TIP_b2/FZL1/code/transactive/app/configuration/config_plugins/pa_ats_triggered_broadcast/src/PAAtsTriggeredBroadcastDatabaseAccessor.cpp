/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/pa_ats_triggered_broadcast/src/PAAtsTriggeredBroadcastDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve DB information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"

#include "core/data_access_interface/pa/src/IConfigPaAtsTriggeredBroadcast.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "bus/ats/ats_agent/IDL/src/IAtsPasCorbaDef.h"

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


    PaAtsTriggeredBroadcastDatabaseAccessor::PaAtsTriggeredBroadcastDatabaseAccessor()
        : 
        m_isLoaded(false),
        m_stationDvaMessageCachedMap(0),
        m_zoneGroupCachedMap(0)
    {
        m_configItems.clear();
    }


    PaAtsTriggeredBroadcastDatabaseAccessor::~PaAtsTriggeredBroadcastDatabaseAccessor()
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
    
        delete m_stationDvaMessageCachedMap;
        m_stationDvaMessageCachedMap = 0;

        delete m_zoneGroupCachedMap;
        m_zoneGroupCachedMap = 0;
        
        FUNCTION_EXIT;
    }


    void PaAtsTriggeredBroadcastDatabaseAccessor::loadItems()
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
        IPaAtsTriggeredBroadcasts items = PaAtsTriggeredBroadcastAccessFactory::getInstance().getAllPaAtsTriggeredBroadcasts(true);

        // And insert the PA Zones into our map. At the same time cast them to IConfigPaAtsTriggeredBroadcast*
        // instead of IPaAtsTriggeredBroadcast*.
        for (IPaAtsTriggeredBroadcastsIt it = items.begin(); it != items.end(); ++it)
        {
            TA_Base_Core::IPaAtsTriggeredBroadcast* item = *it;
            TA_Base_Core::IConfigPaAtsTriggeredBroadcast* configItem = dynamic_cast<TA_Base_Core::IConfigPaAtsTriggeredBroadcast*>(item);
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


    std::multimap<CString,unsigned long> PaAtsTriggeredBroadcastDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for this class");

        std::multimap<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long, PaAtsTriggeredBroadcastDatabaseAccessor::CfgPaAtsTriggeredBroadcastRecord> PaAtsTriggeredBroadcastDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long, CfgPaAtsTriggeredBroadcastRecord> details; 

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
                CfgPaAtsTriggeredBroadcastRecord record = getRecordFrom(*it->second);

                details.insert( std::map<unsigned long, CfgPaAtsTriggeredBroadcastRecord>::value_type( it->second->getUniqueIdentifier(), record ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    PaAtsTriggeredBroadcastDatabaseAccessor::CfgPaAtsTriggeredBroadcastRecord PaAtsTriggeredBroadcastDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the record matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(identifier);
        if (matching == m_configItems.end() || 0 == matching->second)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the PA Zone with identifier %lu",identifier);
            FUNCTION_EXIT;
            CfgPaAtsTriggeredBroadcastRecord record;
            return record;
        }

        CfgPaAtsTriggeredBroadcastRecord record = getRecordFrom(*matching->second);

        FUNCTION_EXIT;
        return record;
    }


    PaAtsTriggeredBroadcastDatabaseAccessor::CfgPaAtsTriggeredBroadcastRecord PaAtsTriggeredBroadcastDatabaseAccessor::getRecordFrom(TA_Base_Core::IConfigPaAtsTriggeredBroadcast& item)
    {
        CfgPaAtsTriggeredBroadcastRecord record;

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

        if (TA_Base_Core::DatabaseKey::isInvalidKey(item.getDestinationLocationKey()))
        {
            record.destLocation = "";
        }
        else
        {
            record.destLocation = getLocDisplayName(item.getDestinationLocationKey()).c_str();
        }

        if (TA_Base_Core::DatabaseKey::isInvalidKey(item.getAtsEventType()))
        {
            record.eventType = "";
        }
        else
        {
            record.eventType = getAtsEventTypeName(item.getAtsEventType());
        }

        if (TA_Base_Core::DatabaseKey::isInvalidKey(item.getPlatformId()))
        {
            record.platformId = "";
        }
        else
        {
            record.platformId = getPlatformName(item.getPlatformId());
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
    
        if (item.getIsLastTrain())
        {
            record.isLastTrain = "是";
        }
        else
        {
            record.isLastTrain = "否";
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

    
    TA_Base_Core::IConfigItem* PaAtsTriggeredBroadcastDatabaseAccessor::getItem(unsigned long key)
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


    void PaAtsTriggeredBroadcastDatabaseAccessor::deleteItem(unsigned long key)
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


    TA_Base_Core::IConfigItem* PaAtsTriggeredBroadcastDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigPaAtsTriggeredBroadcast* newAssoc = TA_Base_Core::PaAtsTriggeredBroadcastAccessFactory::getInstance().createPaAtsTriggeredBroadcast();
        
        m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );

        return newAssoc;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* PaAtsTriggeredBroadcastDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigPaAtsTriggeredBroadcast* newAssoc = NULL;

        // Attempt to find the PA Zone matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(idOfItemToCopy);

        TA_ASSERT(matching != m_configItems.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigPaAtsTriggeredBroadcast stored in the map");

        newAssoc = TA_Base_Core::PaAtsTriggeredBroadcastAccessFactory::getInstance().copyPaAtsTriggeredBroadcast(matching->second);

        if (newAssoc != NULL)
        {
            m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
        }
        
        return newAssoc;

        FUNCTION_EXIT;
    }


    bool PaAtsTriggeredBroadcastDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
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


    void PaAtsTriggeredBroadcastDatabaseAccessor::refreshCachedMaps()
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

        // Set up the event group mapping
        m_eventTypes[TA_IRS_Bus::IAtsPasCorbaDef::PreArrival] = "列车即将到站";        
        m_eventTypes[TA_IRS_Bus::IAtsPasCorbaDef::Arrival] = "列车到站";
        m_eventTypes[TA_IRS_Bus::IAtsPasCorbaDef::PreDeparture] = "列车即将离站";
        m_eventTypes[TA_IRS_Bus::IAtsPasCorbaDef::Departure] = "列车离站";
        m_eventTypes[TA_IRS_Bus::IAtsPasCorbaDef::Skip] = "列车过站不停";
        m_eventTypes[TA_IRS_Bus::IAtsPasCorbaDef::Hold] = "列车停留";
        m_eventTypes[TA_IRS_Bus::IAtsPasCorbaDef::Evacuation] = "疏散列车乘客";
        m_eventTypes[TA_IRS_Bus::IAtsPasCorbaDef::TrainOverrun] = "列车超出范围";

        /////////////////////////////////
        // Set up Platform IDs
        PaAtsTriggeredBroadcastAccessFactory::PlatformIds platforms = 
                PaAtsTriggeredBroadcastAccessFactory::getInstance().enumeratePlatformIds();

        for (PaAtsTriggeredBroadcastAccessFactory::PlatformIds::iterator itr = platforms.begin();
                    itr != platforms.end();
                    itr ++)
        {
			m_platformIds[itr->platformId] = (*itr);
        }

        FUNCTION_EXIT;
    }

    std::string PaAtsTriggeredBroadcastDatabaseAccessor::getLocationName(unsigned long locationKey) const
    {   
        return m_locationNameCache.getName(locationKey);
    }

    std::string PaAtsTriggeredBroadcastDatabaseAccessor::getLocDisplayName(unsigned long locationKey) const
    {
        std::map<unsigned long, std::string>::const_iterator itFound = m_mapLocationKeyToDisplayName.find( locationKey );
        if ( m_mapLocationKeyToDisplayName.end() != itFound )
        {
            return itFound->second;
        }

        return "";
    }

        
    unsigned long PaAtsTriggeredBroadcastDatabaseAccessor::getLocationKey(const std::string& locationName)
    {
        return m_locationNameCache.getKey(locationName);
    }  

    const TA_Base_App::MultipleChangeDialog::ItemsForSelection 
    PaAtsTriggeredBroadcastDatabaseAccessor::getDataForSelection(EDataSet dataSet, unsigned long locationKeyFilter) const 
    {
        // locationKeyFilter defines a filter by location, -1 if no location
        switch (dataSet)
        {
            case DATA_ZONE_GROUP:
                return getZoneGroupDataForSelection(locationKeyFilter);
            case DATA_PLATFORM_ID:
                return getPlatformIdDataForSelection(locationKeyFilter);
            case DATA_DVA_MESSAGE:
                return getDvaMessageDataForSelection(locationKeyFilter);
            case DATA_EVENT_TYPE:
                return getEventTypeDataForSelection();
            default:
                TA_ASSERT(FALSE, "getDataForSelection: Invalid data set specified");
        }
		return TA_Base_App::MultipleChangeDialog::ItemsForSelection();
    }


    const TA_Base_App::MultipleChangeDialog::ItemsForSelection 
    PaAtsTriggeredBroadcastDatabaseAccessor::getZoneGroupDataForSelection(unsigned long locationKeyFilter) const
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
    PaAtsTriggeredBroadcastDatabaseAccessor::getPlatformIdDataForSelection(unsigned long locationKeyFilter) const
    {
        TA_Base_App::MultipleChangeDialog::ItemsForSelection result;

        for (PlatformMap::const_iterator itr = m_platformIds.begin();
                itr != m_platformIds.end();
                itr ++)
        {
            if (TA_Base_Core::DatabaseKey::isValidKey(locationKeyFilter) &&
                    itr->second.stationId != locationKeyFilter)
            {
                continue;
            }

            std::vector<std::string> vec;
            std::ostringstream ostr;
            
            // Stores the Location/ID/name combination
            vec.push_back(getLocDisplayName(itr->second.stationId));

            ostr << itr->second.platformId;
            vec.push_back(ostr.str());

            vec.push_back(itr->second.platformName);

			result[itr->second.platformId] = vec;
        }

        return result;
    }


    const TA_Base_App::MultipleChangeDialog::ItemsForSelection 
    PaAtsTriggeredBroadcastDatabaseAccessor::getDvaMessageDataForSelection(unsigned long locationKeyFilter) const
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
    PaAtsTriggeredBroadcastDatabaseAccessor::getEventTypeDataForSelection() const
    {
        TA_Base_App::MultipleChangeDialog::ItemsForSelection result;

        for (EventTypeMap::const_iterator itr = m_eventTypes.begin();
                itr != m_eventTypes.end();
                itr ++)
        {
            std::vector<std::string> vec(1, itr->second);
            result[itr->first] = vec;
        }
    
        return result;
    }


    std::vector<TA_Base_App::MultipleChangeDialog::HeaderProperties>
    PaAtsTriggeredBroadcastDatabaseAccessor::getDataSetHeaderProperties(EDataSet dataSet)
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
            case DATA_PLATFORM_ID:
                hp.label = "车站";
                hp.width = 70;
                props.push_back(hp);
                hp.label = "编号";
                hp.width = 50;
                props.push_back(hp);
                hp.label = "名称";
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
            case DATA_EVENT_TYPE:   
                hp.label = "信号广播组";
                hp.width = LVSCW_AUTOSIZE_USEHEADER;
                props.push_back(hp);
                break;
            default:
                TA_ASSERT(FALSE, "Unrecognized enum ID");
        }

        return props;
    }

    CString PaAtsTriggeredBroadcastDatabaseAccessor::getAtsEventTypeName(unsigned long key)
    {
        EventTypeMap::iterator itr = m_eventTypes.find((TA_IRS_Bus::IAtsPasCorbaDef::EEventType)key);

        if (itr != m_eventTypes.end())
        {
            return itr->second.c_str();
        }
        else
        {
            return "#未知#";
        }
    }


    CString PaAtsTriggeredBroadcastDatabaseAccessor::getPlatformName(unsigned long key, unsigned long* locationKeyOut)
    {
        PlatformMap::iterator itr = m_platformIds.find(key);

        if (itr != m_platformIds.end())
        {
            if (0 != locationKeyOut)
            {
                *locationKeyOut = itr->second.stationId;
            }

            return itr->second.platformName.c_str();
        }
        else
        {
            if (0 != locationKeyOut)
            {
                *locationKeyOut = TA_Base_Core::DatabaseKey::getInvalidKey();
            }

            return "#未知#";
        }
    }


    CString PaAtsTriggeredBroadcastDatabaseAccessor::getZoneGroupName(unsigned long key, unsigned long* locationKeyOut)
    {
        TA_ASSERT(m_zoneGroupCachedMap != 0, "Zone Group Cached Map is NULL");

        try
        {
            PaZoneGroupRecord group = m_zoneGroupCachedMap->getCachedDbRecordFromKey(key);
            
            if (0 != locationKeyOut)
            {
                *locationKeyOut = group.m_locationKey;
            }

            return group.m_label.c_str();
        }
        catch (const TA_Base_Core::CachedMappingNotFoundException&)
        {
            if (0 != locationKeyOut)
            {
                *locationKeyOut = TA_Base_Core::DatabaseKey::getInvalidKey();
            }

            return "#未知#";
        }
    }


    CString PaAtsTriggeredBroadcastDatabaseAccessor::getDvaMessageName(unsigned long key, unsigned long* locationKeyOut)
    {
        TA_ASSERT(0 != m_stationDvaMessageCachedMap, "Station DVA Cached map is NULL");

        try
        {
            StationDvaMessageRecord message = m_stationDvaMessageCachedMap->getCachedDbRecordFromKey(key);

            if (0 != locationKeyOut)
            {
                *locationKeyOut = message.ulLocationKey;
            }

            return message.strMsgName.c_str();
        }
        catch (const TA_Base_Core::CachedMappingNotFoundException&)
        {
            if (0 != locationKeyOut)
            {
                *locationKeyOut = TA_Base_Core::DatabaseKey::getInvalidKey();
            }

            return "#未知#";
        }
    }
}

    

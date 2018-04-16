/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/pa_zone_group/src/PAZoneGroupDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #7 $
  *
  * Last modification: $DateTime: 2014/06/13 11:13:04 $
  * Last modified by:  $Author: huangjian $
  * 
  * This class uses the Data Access Interface to retrieve DB information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"

#include "core/data_access_interface/pa_4669/src/IConfigPaZoneGroup.h"
#include "core/data_access_interface/pa_4669/src/PaZoneGroupAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"

using namespace TA_Base_Core;
using namespace TA_Base_Bus;
using namespace TA_IRS_Bus;

namespace TA_IRS_App
{


    PaZoneGroupDatabaseAccessor::PaZoneGroupDatabaseAccessor()
        : m_isLoaded(false)
    {
        m_configItems.clear();
    }


    PaZoneGroupDatabaseAccessor::~PaZoneGroupDatabaseAccessor()
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

        FUNCTION_EXIT;
    }


    void PaZoneGroupDatabaseAccessor::loadItems()
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
        IPaZoneGroups items = PaZoneGroupAccessFactory::getInstance().getAllPaZoneGroups(true);

        // And insert the PA Zones into our map. At the same time cast them to IConfigPaZoneGroup*
        // instead of IPaZoneGroup*.
        for (IPaZoneGroupsIt it = items.begin(); it != items.end(); ++it)
        {
            IPaZoneGroup* item = *it;
            TA_Base_Core::IConfigPaZoneGroup* configItem = dynamic_cast<IConfigPaZoneGroup*>(item);
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


    std::map<CString,unsigned long> PaZoneGroupDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for this class");

        std::map<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long,PaZoneGroupDatabaseAccessor::PaZoneGroupRecord> PaZoneGroupDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,PaZoneGroupRecord> details; 

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
                PaZoneGroupRecord record = getRecordFrom(*it->second);

                details.insert( std::map<unsigned long, PaZoneGroupRecord>::value_type( it->second->getUniqueIdentifier(), record ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    PaZoneGroupDatabaseAccessor::PaZoneGroupRecord PaZoneGroupDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the record matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(identifier);
        if (matching == m_configItems.end() || 0 == matching->second)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the PA Zone with identifier %lu",identifier);
            FUNCTION_EXIT;
            PaZoneGroupRecord record;
            return record;
        }

        PaZoneGroupRecord record = getRecordFrom(*matching->second);

        FUNCTION_EXIT;
        return record;
    }


    PaZoneGroupDatabaseAccessor::PaZoneGroupRecord PaZoneGroupDatabaseAccessor::getRecordFrom(TA_Base_Core::IConfigPaZoneGroup& item)
    {
        PaZoneGroupRecord record;

        CString buffer;       
        if (TA_Base_Core::DatabaseKey::isInvalidKey(item.getKey()) )
        {
            record.key = "";
        }
        else
        {
            buffer.Format("%ld",item.getKey()); 
            record.key = buffer;
        }

        if (TA_Base_Core::DatabaseKey::isInvalidKey(item.getLocationKey()))
        {
            record.location = "";
        }
        else
        {        
            record.location = getLocationName(item.getLocationKey()).c_str();
        }

        record.label = item.getLabel().c_str();
        if (item.getIsEventGroup())
        {
            record.isEventGroup = "Y";
        }
        else
        {
            record.isEventGroup = "N";
        }

        return record;
    }


    TA_Base_Core::IConfigItem* PaZoneGroupDatabaseAccessor::getItem(unsigned long key)
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


    void PaZoneGroupDatabaseAccessor::deleteItem(unsigned long key)
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
            matching->second->deleteThisObject();
        }
        delete matching->second;
        matching->second = NULL;
        m_configItems.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* PaZoneGroupDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigPaZoneGroup* newAssoc = TA_Base_Core::PaZoneGroupAccessFactory::getInstance().createPaZoneGroup();

        m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );

        return newAssoc;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* PaZoneGroupDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigPaZoneGroup* newAssoc = NULL;

        // Attempt to find the PA Zone matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(idOfItemToCopy);

        TA_ASSERT(matching != m_configItems.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigPaZoneGroup stored in the map");

        newAssoc = TA_Base_Core::PaZoneGroupAccessFactory::getInstance().copyPaZoneGroup(matching->second);

        if (newAssoc != NULL)
        {
            m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
        }
        
        return newAssoc;

        FUNCTION_EXIT;
    }


    bool PaZoneGroupDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
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


    void PaZoneGroupDatabaseAccessor::refreshCachedMaps()
    {
        FUNCTION_ENTRY("refreshCachedMaps");

        // First retrieve all the subsystems from the database
        try
        {
            m_locationNameCache.refreshUsing(LocationAccessFactory::getInstance().getAllLocations());
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

        // Refresh the cached map - obtaining all the zones
        TA_IRS_Bus::CachedMaps::getInstance()->refreshKeyToPaZoneRecordMap();

        FUNCTION_EXIT;
    }

    std::string PaZoneGroupDatabaseAccessor::getLocationName(unsigned long locationKey)
    {   
        return m_locationNameCache.getName(locationKey);
    }
        
    unsigned long PaZoneGroupDatabaseAccessor::getLocationKey(const std::string& locationName)
    {
        return m_locationNameCache.getKey(locationName);
    }


    bool PaZoneGroupDatabaseAccessor::isOccLocationKey(unsigned long locationKey)
    {
		std::map<unsigned long, ILocation::ELocationType>::iterator itFound = m_locationTypeMap.find(locationKey);
		if (itFound == m_locationTypeMap.end())
		{
			ILocation::ELocationType eLocationType = LocationAccessFactory::getInstance().getLocationType(locationKey);
			itFound = m_locationTypeMap.insert(std::make_pair(locationKey,eLocationType)).first;
		}
		return ( ILocation::OCC == itFound->second);
    }

    PaZoneRecords PaZoneGroupDatabaseAccessor::getAllZonesForLocation(unsigned long locationKey)
    {
        if (isOccLocationKey(locationKey))
        {
            return TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneCachedMap().getAllRecords();
        }
        else
        {
            return TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneCachedMap().getPaZoneRecordsFromLocationKey(locationKey);
        }
    }


    PaZoneRecord PaZoneGroupDatabaseAccessor::getZoneFromKey(unsigned long zoneKey)
    {
        return TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneCachedMap().getCachedDbRecordFromKey(zoneKey);
    }
}

    

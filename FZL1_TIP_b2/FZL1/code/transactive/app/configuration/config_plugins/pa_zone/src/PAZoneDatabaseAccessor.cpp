/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/pa_zone/src/PAZoneDatabaseAccessor.cpp $
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

#include "core/data_access_interface/pa/src/IConfigPaZone.h"
#include "core/data_access_interface/pa/src/PaZoneAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/data_access_interface/src/DatabaseKey.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{


    PaZoneDatabaseAccessor::PaZoneDatabaseAccessor()
        : m_isLoaded(false)
    {
        m_configItems.clear();
    }


    PaZoneDatabaseAccessor::~PaZoneDatabaseAccessor()
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


    void PaZoneDatabaseAccessor::loadItems()
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
        IPaZones items = PaZoneAccessFactory::getInstance().getAllPaZones(true);

        // And insert the PA Zones into our map. At the same time cast them to IConfigPaZone*
        // instead of IPaZone*.
        for (IPaZonesIt it = items.begin(); it != items.end(); ++it)
        {
            IPaZone* item = *it;
            IConfigPaZone* configItem = dynamic_cast<IConfigPaZone*>(item);
            if( configItem != NULL )
            {
                unsigned long key = configItem->getUniqueIdentifier();
                m_configItems[key] = configItem;
            }
        }

        refreshKeyToNameMaps();

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> PaZoneDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for this class");

        std::multimap<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long,PaZoneDatabaseAccessor::PaZoneRecord> PaZoneDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,PaZoneRecord> details; 

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
                PaZoneRecord record = getRecordFrom(*it->second);

                details.insert( std::map<unsigned long, PaZoneRecord>::value_type( it->second->getUniqueIdentifier(), record ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    PaZoneDatabaseAccessor::PaZoneRecord PaZoneDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the record matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(identifier);
        if (matching == m_configItems.end() || 0 == matching->second)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the PA Zone with identifier %lu",identifier);
            FUNCTION_EXIT;
            PaZoneRecord record;
            return record;
        }

        PaZoneRecord record = getRecordFrom(*matching->second);

        FUNCTION_EXIT;
        return record;
    }


    PaZoneDatabaseAccessor::PaZoneRecord PaZoneDatabaseAccessor::getRecordFrom(TA_Base_Core::IConfigPaZone& item)
    {
        PaZoneRecord record;

        CString buffer;       
        if (TA_Base_Core::DatabaseKey::getInvalidKey() == item.getKey())
        {
            record.key = "";
        }
        else
        {
            buffer.Format("%ld",item.getKey()); 
            record.key = buffer;
        }
        
        if (TA_Base_Core::DatabaseKey::getInvalidKey() == item.getLocationKey())
        {
            record.location = "";
        }
        else
        {
            record.location = getLocationName(item.getLocationKey()).c_str();
        }
        
        if (TA_Base_Core::DatabaseKey::getInvalidKey() == item.getId())
        {
            record.id = "";
        }
        else
        {
            buffer.Format("%ld",item.getId()); 
            record.id = buffer;
        }

        record.label = item.getLabel().c_str();

        record.stnEquipmentEntityName = item.getStnEntityName().c_str();
        record.occEquipmentEntityName = item.getOccEntityName().c_str();

        return record;
    }


    TA_Base_Core::IConfigItem* PaZoneDatabaseAccessor::getItem(unsigned long key)
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


    void PaZoneDatabaseAccessor::deleteItem(unsigned long key)
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


    TA_Base_Core::IConfigItem* PaZoneDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigPaZone* newAssoc = TA_Base_Core::PaZoneAccessFactory::getInstance().createPaZone();

        m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );

        return newAssoc;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* PaZoneDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigPaZone* newAssoc = NULL;

        // Attempt to find the PA Zone matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(idOfItemToCopy);

        TA_ASSERT(matching != m_configItems.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigPaZone stored in the map");

        newAssoc = TA_Base_Core::PaZoneAccessFactory::getInstance().copyPaZone(matching->second);

        if (newAssoc != NULL)
        {
            m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
        }
        
        return newAssoc;

        FUNCTION_EXIT;
    }


    bool PaZoneDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
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


    void PaZoneDatabaseAccessor::refreshKeyToNameMaps()
    {
        FUNCTION_ENTRY("refreshKeyToNameMaps");

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

        try
        {
            // The equipment entity associated with PA Zones is always a data node
            std::vector<IEntityData*> entities = 
                EntityAccessFactory::getInstance().getEntitiesOfType(TA_Base_Core::DataNodeEntityData::getStaticType());
            
            // Initially cache all the key to location data
            for (std::vector<IEntityData*>::iterator itr = entities.begin(); itr != entities.end(); itr ++)
            {
                m_entityKeyToLocationMap[(*itr)->getKey()] = (*itr)->getLocation();
            }

            // The cache class will automatically delete the individual entity pointers
            m_entityNameCache.refreshUsing(entities);               
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

        FUNCTION_EXIT;
    }

    std::string PaZoneDatabaseAccessor::getLocationName(unsigned long locationKey)
    {   
        return m_locationNameCache.getName(locationKey);
    }


    std::string PaZoneDatabaseAccessor::getEntityName(unsigned long entityKey)
    {
        return m_entityNameCache.getName(entityKey);
    }
        
    unsigned long PaZoneDatabaseAccessor::getLocationKey(const std::string& locationName)
    {
        return m_locationNameCache.getKey(locationName);
    }

    unsigned long PaZoneDatabaseAccessor::getEntityKey(const std::string& entityName)
    {
        return m_entityNameCache.getKey(entityName);
    }

    unsigned long PaZoneDatabaseAccessor::getEntityLocation(unsigned long entityKey)
    {
        if (entityKey >= 0)
        {
            std::map<unsigned long, unsigned long>::iterator itr = m_entityKeyToLocationMap.find(entityKey);
            if (itr != m_entityKeyToLocationMap.end())
            {
                // Return the associated location
                return itr->second;
            }
        }

        return -1;
    }


    unsigned long PaZoneDatabaseAccessor::getOccLocationKey() const
    {
        if (!m_occLocationKey.hasBeenSet())
        {
            m_occLocationKey.setValue(LocationAccessFactory::getInstance().getOccLocationKey());
        }

        return m_occLocationKey.getValue();
    }


    PaZoneDatabaseAccessor::EntityKeyToNameMap 
        PaZoneDatabaseAccessor::getAllEntitiesAtLocation(unsigned long locationKey, bool fetchPaEntitiesOnly)
    {        
        // If the zone is located at the OCC, still can only link to OCC equipment
        /*       
        bool showAllEntities = false;
        if (LocationAccessFactory::getInstance().getOccLocationKey() == locationKey)
        {
            showAllEntities = true;
        }
        */
    
        // Only return entities at the same location as the PA_ZONE
        const EntityKeyToNameMap& map = m_entityNameCache.getInternalMap();
        EntityKeyToNameMap result;

        for (EntityKeyToNameMap::const_iterator itr = map.begin(); itr != map.end(); itr ++)
        {
            if (fetchPaEntitiesOnly)
            {            
                if (!isFormattedAsPaZoneEntity(itr->second))
                {
                    continue;
                }
            }       

            if (getEntityLocation (itr->first) == locationKey)
            {
                result[itr->first] = itr->second;
            }
        }
        
        return result;
    }


    bool PaZoneDatabaseAccessor::isFormattedAsPaZoneEntity(const std::string entityName)
    {
        // PA Zone entities must be in the format:
        // be in the format <STN>.PA.<Area>.<ZoneShortName>
        CString text(entityName.c_str());
        
        const std::string token(".PA.");
        int npos = text.Find(token.c_str());
        
        if (npos < 0 || npos != text.Find("."))
        {
            // If token not found, or '.' found before it, invalid format
            return false;
        }
        
        // Should be a single '.' after this token
        npos = text.Find(".", npos + token.length());
        if (npos < 0)
        {
            return false;
        }
        
        // There should be no further full stops
        if (text.Find(".", npos + 1) >= 0)
        {
            // Some more full stops, invalid
            return false;
        }

        // Otherwise as far as we can tell this is a valid format for a PA Zone
        return true;
    }
}

    

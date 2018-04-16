/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/ecs_zone/src/ECSZoneDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve ECS Zone information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ecs_zone/src/StdAfx.h"
#include "app/configuration/config_plugins/ecs_zone/src/ECSZoneDatabaseAccessor.h"


#include "core/data_access_interface/ecs/src/IConfigECSZone.h"
#include "core/data_access_interface/ecs/src/ECSZoneAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using namespace TA_Base_Core; using namespace TA_IRS_Core;

namespace TA_IRS_App
{

    static const std::string ECS_ZONE_DATANODES_TOKEN = "%.ECS.ZON.%";
    static const std::string DATANODE_TYPE            = "DataNode";

    ECSZoneDatabaseAccessor::ECSZoneDatabaseAccessor()
        : m_isLoaded(false)
    {
        m_configItems.clear();
        m_dataNodeMap.clear();
    }


    ECSZoneDatabaseAccessor::~ECSZoneDatabaseAccessor()
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


    void ECSZoneDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the ECS Zones and this is not a refresh so we exit");
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


        // Now reload all the ECS Zones
        IECSZones items = ECSZoneAccessFactory::getInstance().getAllECSZones(true);

        // And insert the ECS Zones into our map. At the same time cast them to IConfigECSZone*
        // instead of IECSZone*.
        for (IECSZonesIt it = items.begin(); it != items.end(); ++it)
        {
            IECSZone* item = *it;
            IConfigECSZone* configItem = dynamic_cast<IConfigECSZone*>(item);
            if( configItem != NULL )
            {
                unsigned long key = configItem->getUniqueIdentifier();
                m_configItems[key] = configItem;
            }
        }

        refreshDataNodeMap();

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::map<CString,unsigned long> ECSZoneDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for ECS Zones");

        std::map<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long,ECSZoneDatabaseAccessor::ECSZoneRecord> ECSZoneDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,ECSZoneRecord> details; 

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
                ECSZoneRecord record;

                if (it->second->getECSZoneId() == -1)
                {
                    record.ecsZoneId = "";
                }
                else
                {
                    buffer.Format("%d",it->second->getECSZoneId()); 
                    record.ecsZoneId = buffer;
                }
                
                record.ecsLocation = it->second->getECSLocation().c_str();
                
                record.track = it->second->getTrack().c_str();
                
                record.occEquipment = getDataNodeName(it->second->getOccEquipmentEntityKey()).c_str();
                record.stnEquipment = getDataNodeName(it->second->getStnEquipmentEntityKey()).c_str();
				//TD17826 - marc_bugfix_start
				//Support IS_IGNORING_CONGESTION field
				record.bIgnoringCongestion = it->second->getIsIgnoringCongestion(); 
				//TD17826 - marc_bugfix_end


                details.insert( std::map<unsigned long, ECSZoneRecord>::value_type( it->second->getUniqueIdentifier(), record ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    ECSZoneDatabaseAccessor::ECSZoneRecord ECSZoneDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the ECS Zone matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(identifier);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ECS Zone with identifier %lu",identifier);
            FUNCTION_EXIT;
            ECSZoneRecord record;
            return record;
        }

        ECSZoneRecord record;

        if (matching->second->getECSZoneId() == -1)
        {
            record.ecsZoneId = "";
        }
        else
        {
            CString buffer;
            buffer.Format("%d",matching->second->getECSZoneId()); 
            record.ecsZoneId = buffer;
        }
        
        record.ecsLocation = matching->second->getECSLocation().c_str();
        
        record.track = matching->second->getTrack().c_str();
 
        record.occEquipment = getDataNodeName(matching->second->getOccEquipmentEntityKey()).c_str();
        record.stnEquipment = getDataNodeName(matching->second->getStnEquipmentEntityKey()).c_str();

		//TD17826 - marc_bugfix_start
		//Support IS_IGNORING_CONGESTION field
		record.bIgnoringCongestion = matching->second->getIsIgnoringCongestion(); 
		//TD17826 - marc_bugfix_end


        FUNCTION_EXIT;
        return record;
    }


    TA_Base_Core::IConfigItem* ECSZoneDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the ECS Zone matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ECS Zone with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void ECSZoneDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the ECS Zone matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ECS Zone with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored ECS Zone was equal to NULL");

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


    TA_Base_Core::IConfigItem* ECSZoneDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_IRS_Core::IConfigECSZone* newAssoc = TA_IRS_Core::ECSZoneAccessFactory::getInstance().createECSZone();

        m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );

        return newAssoc;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ECSZoneDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_IRS_Core::IConfigECSZone* newAssoc = NULL;

        // Attempt to find the ECS Zone matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(idOfItemToCopy);

        TA_ASSERT(matching != m_configItems.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigECSZone stored in the map");

        newAssoc = TA_IRS_Core::ECSZoneAccessFactory::getInstance().copyECSZone(matching->second);

        if (newAssoc != NULL)
        {
            m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
        }
        
        return newAssoc;

        FUNCTION_EXIT;
    }


    bool ECSZoneDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
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
                // individual change for ECS Zones
                itemsNotApplied.push_back("");
                break;
            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }


    void ECSZoneDatabaseAccessor::refreshDataNodeMap()
    {
        FUNCTION_ENTRY("refreshDataNodeMap");

        m_dataNodeMap.clear();

        // First retrieve all the subsystems from the database
        IEntityDataList dataNodeEntities;
        try
        {
            dataNodeEntities = EntityAccessFactory::getInstance().getEntitiesOfTypeWithNameLikeToken(DATANODE_TYPE, ECS_ZONE_DATANODES_TOKEN);
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No ECS Zone datanodes will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No ECS Zone datanodes will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want ot return
        // 2) Delete the DataNodeEntity object when we're done
        for (IEntityDataList::iterator it = dataNodeEntities.begin(); it != dataNodeEntities.end(); ++it)
        {
            if (*it != NULL)
            {
                try
                {
                    m_dataNodeMap.insert( DataNodeKeyToNamesMap::value_type( (*it)->getKey(), (*it)->getName() ) );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This ECS Zone datanode will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This ECS Zone datanode will not be added to the map");
                }

                // Now we're finished with this iterator so we might as well delete what it contains so we
                // don't have to do it later.
                delete *it;
                *it = NULL;
            }
        }
        dataNodeEntities.clear();

        FUNCTION_EXIT;
    }

    std::string ECSZoneDatabaseAccessor::getDataNodeName(unsigned long dataNodeKey)
    {
        DataNodeKeyToNamesMapIt it = m_dataNodeMap.begin();
        for (; it != m_dataNodeMap.end(); ++it)
        {
            if (it->first == dataNodeKey)
            {
                return it->second;
            }
        }

        // Can't find it
        return "";
    }

    unsigned long ECSZoneDatabaseAccessor::getDataNodeKey(const std::string& dataNodeName)
    {
        DataNodeKeyToNamesMapIt it = m_dataNodeMap.begin();
        for (; it != m_dataNodeMap.end(); ++it)
        {
            if (it->second == dataNodeName)
            {
                return it->first;
            }
        }

        // Can't find it
        return -1;
    }

}

    

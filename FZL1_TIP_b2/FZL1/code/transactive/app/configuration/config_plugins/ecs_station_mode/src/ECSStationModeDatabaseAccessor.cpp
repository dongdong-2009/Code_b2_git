/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/ecs_station_mode/src/ECSStationModeDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve ECS Station Mode information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ecs_station_mode/src/StdAfx.h"
#include "app/configuration/config_plugins/ecs_station_mode/src/ECSStationModeDatabaseAccessor.h"


#include "core/data_access_interface/ecs/src/IConfigECSStationMode.h"
#include "core/data_access_interface/ecs/src/ECSStationModeAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using namespace TA_Base_Core; using namespace TA_IRS_Core;

namespace TA_IRS_App
{


    ECSStationModeDatabaseAccessor::ECSStationModeDatabaseAccessor()
        : m_isLoaded(false)
    {
        m_configItems.clear();
        m_locationMap.clear();
    }


    ECSStationModeDatabaseAccessor::~ECSStationModeDatabaseAccessor()
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


    void ECSStationModeDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the ECS Station Modes and this is not a refresh so we exit");
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


        // Now reload all the ECS Station Modes
        IECSStationModes items = ECSStationModeAccessFactory::getInstance().getAllECSStationModes(true);

        // And insert the ECS Station Modes into our map. At the same time cast them to IConfigECSStationMode*
        // instead of IECSStationMode*.
        for (IECSStationModesIt it = items.begin(); it != items.end(); ++it)
        {
            IECSStationMode* item = *it;
            IConfigECSStationMode* configItem = dynamic_cast<IConfigECSStationMode*>(item);
            if( configItem != NULL )
            {
                unsigned long key = configItem->getUniqueIdentifier();
                m_configItems[key] = configItem;
            }
        }

        refreshLocationMap();

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::map<CString,unsigned long> ECSStationModeDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for ECS Station Modes");

        std::map<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long,ECSStationModeDatabaseAccessor::ECSStationModeRecord> ECSStationModeDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,ECSStationModeRecord> details; 

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
                ECSStationModeRecord record;

                if (it->second->getKey() == 0)
                {
                    record.key = "";
                }
                else
                {
                    buffer.Format("%ld",it->second->getKey()); 
                    record.key = buffer;
                }

                buffer.Format("%ld",it->second->getStationMode()); 
                record.stationMode = buffer;
                
                record.location = getLocationName(it->second->getLocationKey()).c_str();
                record.operation = it->second->getOperation().c_str();
                record.track = it->second->getTrack().c_str();
                record.condition = it->second->getCondition().c_str();
                record.congestionLogic = it->second->getCongestionLogic().c_str();

                details.insert( std::map<unsigned long, ECSStationModeRecord>::value_type( it->second->getUniqueIdentifier(), record ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    ECSStationModeDatabaseAccessor::ECSStationModeRecord ECSStationModeDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the ECS Station Mode matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(identifier);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ECS Station Mode with identifier %lu",identifier);
            FUNCTION_EXIT;
            ECSStationModeRecord record;
            return record;
        }

        ECSStationModeRecord record;

        CString buffer;
        if (matching->second->getKey() == 0)
        {
            record.key = "";
        }
        else
        {
            buffer.Format("%ld",matching->second->getKey()); 
            record.key = buffer;
        }

        buffer.Format("%ld",matching->second->getStationMode()); 
        record.stationMode = buffer;
        
        record.location = getLocationName(matching->second->getLocationKey()).c_str();
        record.operation = matching->second->getOperation().c_str();
        record.track = matching->second->getTrack().c_str();
        record.condition = matching->second->getCondition().c_str();
        record.congestionLogic = matching->second->getCongestionLogic().c_str();


        FUNCTION_EXIT;
        return record;
    }


    TA_Base_Core::IConfigItem* ECSStationModeDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the ECS Station Mode matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ECS Station Mode with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void ECSStationModeDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the ECS Station Mode matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ECS Station Mode with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored ECS Station Mode was equal to NULL");

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


    TA_Base_Core::IConfigItem* ECSStationModeDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_IRS_Core::IConfigECSStationMode* newAssoc = TA_IRS_Core::ECSStationModeAccessFactory::getInstance().createECSStationMode();

        m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );

        return newAssoc;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ECSStationModeDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_IRS_Core::IConfigECSStationMode* newAssoc = NULL;

        // Attempt to find the ECS Station Mode matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(idOfItemToCopy);

        TA_ASSERT(matching != m_configItems.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigECSStationMode stored in the map");

        newAssoc = TA_IRS_Core::ECSStationModeAccessFactory::getInstance().copyECSStationMode(matching->second);

        if (newAssoc != NULL)
        {
            m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
        }
        
        return newAssoc;

        FUNCTION_EXIT;
    }


    bool ECSStationModeDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
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
                // individual change for ECS Station Modes
                itemsNotApplied.push_back("");
                break;
            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }


    void ECSStationModeDatabaseAccessor::refreshLocationMap()
    {
        FUNCTION_ENTRY("refreshLocationMap");

        m_locationMap.clear();

        // First retrieve all the subsystems from the database
        std::vector<ILocation*> locations;
        std::vector<ILocation*>::iterator it;

        try
        {
            locations = LocationAccessFactory::getInstance().getAllLocations();
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

        // Now step through each one and:
        // 1) Add it's name and key to the map we want ot return
        // 2) Delete the ILocation object when we're done
        for (it = locations.begin(); it != locations.end(); ++it)
        {
            if (*it != NULL)
            {
                try
                {
                    m_locationMap.insert( LocationKeyToNameMap::value_type( (*it)->getKey(), (*it)->getName() ) );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This Location will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This Location will not be added to the map");
                }

                // Now we're finished with this iterator so we might as well delete what it contains so we
                // don't have to do it later.
                delete *it;
                *it = NULL;
            }
        }
        locations.clear();

        FUNCTION_EXIT;
    }

    std::string ECSStationModeDatabaseAccessor::getLocationName(unsigned long locationKey)
    {
        LocationKeyToNameMapIt it = m_locationMap.begin();
        for (; it != m_locationMap.end(); ++it)
        {
            if (it->first == locationKey)
            {
                return it->second;
            }
        }

        // Can't find it
        return "";
    }

    unsigned long ECSStationModeDatabaseAccessor::getLocationKey(const std::string& locationName)
    {
        LocationKeyToNameMapIt it = m_locationMap.begin();
        for (; it != m_locationMap.end(); ++it)
        {
            if (it->second == locationName)
            {
                return it->first;
            }
        }

        // Can't find it
        return -1;
    }

}

    

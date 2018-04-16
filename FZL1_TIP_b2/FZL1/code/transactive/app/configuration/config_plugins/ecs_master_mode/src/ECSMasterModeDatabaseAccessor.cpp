/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/ecs_master_mode/src/ECSMasterModeDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve ECS Master Mode information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ecs_master_mode/src/StdAfx.h"
#include "app/configuration/config_plugins/ecs_master_mode/src/ECSMasterModeDatabaseAccessor.h"


#include "core/data_access_interface/ecs/src/IConfigECSMasterMode.h"
#include "core/data_access_interface/ecs/src/ECSMasterModeAccessFactory.h"
#include "core/data_access_interface/ecs/src/ECSZoneAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using namespace TA_Base_Core; using namespace TA_IRS_Core;

namespace TA_IRS_App
{


    ECSMasterModeDatabaseAccessor::ECSMasterModeDatabaseAccessor()
        : m_isLoaded(false)
    {
        m_configItems.clear();
        m_zoneMap.clear();
    }


    ECSMasterModeDatabaseAccessor::~ECSMasterModeDatabaseAccessor()
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


    void ECSMasterModeDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the ECS Master Modes and this is not a refresh so we exit");
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


        // Now reload all the ECS Master Modes
        IECSMasterModes items = ECSMasterModeAccessFactory::getInstance().getAllECSMasterModes(true);

        // And insert the ECS Master Modes into our map. At the same time cast them to IConfigECSMasterMode*
        // instead of IECSMasterMode*.
        for (IECSMasterModesIt it = items.begin(); it != items.end(); ++it)
        {
            IECSMasterMode* item = *it;
            IConfigECSMasterMode* configItem = dynamic_cast<IConfigECSMasterMode*>(item);
            if( configItem != NULL )
            {
                unsigned long key = configItem->getUniqueIdentifier();
                m_configItems[key] = configItem;
            }
        }

        refreshZoneMap();

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::map<CString,unsigned long> ECSMasterModeDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for ECS Master Modes");

        std::map<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long,ECSMasterModeDatabaseAccessor::ECSMasterModeRecord> ECSMasterModeDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,ECSMasterModeRecord> details; 

        // The maximum range is the number of record times by 2. This is because we want to step for each
        // record while it is loaded and then again while each record is added to the list.
        progress.SetRange( 0, m_configItems.size() * 2 );

        // Step through all the objects and retrieve the details for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.

        CString buffer;
        for (ConfigItemsIt it = m_configItems.begin(); it != m_configItems.end(); ++it)
        {
            if (it->second != NULL)
            {
                ECSMasterModeRecord record;

                if (it->second->getKey() == 0)
                {
                    record.key = "";
                }
                else
                {
                    buffer.Format("%ld",it->second->getKey()); 
                    record.key = buffer;
                }

                buffer.Format("%ld",it->second->getMasterMode()); 
                record.masterMode = buffer;
                
                record.condition  = it->second->getCondition().c_str();
                record.ecsZone    = getZoneName(it->second->getECSZoneId()).c_str();
                record.evacuation = it->second->getEvacuationDirection().c_str();
                record.airflow    = it->second->getAirflowDirection().c_str();
                record.display    = it->second->getDisplayDirection().c_str();
                record.verificationMsg = it->second->getVerificationMessage().c_str();
                
                details.insert( std::map<unsigned long, ECSMasterModeRecord>::value_type( it->second->getUniqueIdentifier(), record ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    ECSMasterModeDatabaseAccessor::ECSMasterModeRecord ECSMasterModeDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the ECS Master Mode matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(identifier);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ECS Master Mode with identifier %lu",identifier);
            FUNCTION_EXIT;
            ECSMasterModeRecord record;
            return record;
        }

        ECSMasterModeRecord record;

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

        buffer.Format("%ld",matching->second->getMasterMode()); 
        record.masterMode = buffer;
        
        record.condition  = matching->second->getCondition().c_str();
        record.ecsZone    = getZoneName(matching->second->getECSZoneId()).c_str();
        record.evacuation = matching->second->getEvacuationDirection().c_str();
        record.airflow    = matching->second->getAirflowDirection().c_str();
        record.display    = matching->second->getDisplayDirection().c_str();
        record.verificationMsg = matching->second->getVerificationMessage().c_str();


        FUNCTION_EXIT;
        return record;
    }


    TA_Base_Core::IConfigItem* ECSMasterModeDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the ECS Master Mode matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ECS Master Mode with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void ECSMasterModeDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the ECS Master Mode matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ECS Master Mode with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored ECS Master Mode was equal to NULL");

        if (!matching->second->isNew())
        {
            // TODO - check asserts

            // Force the deletion of all references to this Master ECS Mode in the EC_MASTER_MODE_HELPER
            // table by setting the cascade parameter to true
            matching->second->deleteThisObject(/* cascade = */true); 
            
        }
        delete matching->second;
        matching->second = NULL;
        m_configItems.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ECSMasterModeDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_IRS_Core::IConfigECSMasterMode* newAssoc = TA_IRS_Core::ECSMasterModeAccessFactory::getInstance().createECSMasterMode();

        m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );

        return newAssoc;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ECSMasterModeDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_IRS_Core::IConfigECSMasterMode* newAssoc = NULL;

        // Attempt to find the ECS Master Mode matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(idOfItemToCopy);

        TA_ASSERT(matching != m_configItems.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigECSMasterMode stored in the map");

        newAssoc = TA_IRS_Core::ECSMasterModeAccessFactory::getInstance().copyECSMasterMode(matching->second);

        if (newAssoc != NULL)
        {
            m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
        }
        
        return newAssoc;

        FUNCTION_EXIT;
    }


    bool ECSMasterModeDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
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
                // individual change for ECS Master Modes
                itemsNotApplied.push_back("");
                break;
            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }


    void ECSMasterModeDatabaseAccessor::refreshZoneMap()
    {
        FUNCTION_ENTRY("refreshZoneMap");

        m_zoneMap.clear();

        // First retrieve all the subsystems from the database
        std::vector<IECSZone*> zones;
        std::vector<IECSZone*>::iterator it;

        try
        {
            zones = ECSZoneAccessFactory::getInstance().getAllECSZones();
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No ECS Zones will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No ECS Zones will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want ot return
        // 2) Delete the IECSZone object when we're done
        for (it = zones.begin(); it != zones.end(); ++it)
        {
            if (*it != NULL)
            {
                try
                {
                    m_zoneMap.insert( ZoneKeyToNameMap::value_type( (*it)->getKey(), (*it)->getName() ) );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This Zone will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This Zone will not be added to the map");
                }

                // Now we're finished with this iterator so we might as well delete what it contains so we
                // don't have to do it later.
                delete *it;
                *it = NULL;
            }
        }
        zones.clear();

        FUNCTION_EXIT;
    }

    std::string ECSMasterModeDatabaseAccessor::getZoneName(unsigned long zoneKey)
    {
        ZoneKeyToNameMapIt it = m_zoneMap.begin();
        for (; it != m_zoneMap.end(); ++it)
        {
            if (it->first == zoneKey)
            {
                return it->second;
            }
        }

        // Can't find it
        return "";
    }

    unsigned long ECSMasterModeDatabaseAccessor::getZoneKey(const std::string& zoneName)
    {
        if (zoneName == "")
        {
            return -1;
        }

        ZoneKeyToNameMapIt it = m_zoneMap.begin();
        for (; it != m_zoneMap.end(); ++it)
        {
            if (it->second == zoneName)
            {
                return it->first;
            }
        }

        // Can't find it (-1 is a null entity field)
        return -2;
    }

}

    

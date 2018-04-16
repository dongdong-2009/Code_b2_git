/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/ats_platform/src/AtsPlatformDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve Ats Platform information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ats_platform/src/StdAfx.h"
#include "app/configuration/config_plugins/ats_platform/src/AtsPlatformDatabaseAccessor.h"


#include "core/data_access_interface/ats/src/IConfigAtsPlatform.h"
#include "core/data_access_interface/ats/src/AtsPlatformAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using namespace TA_Base_Core;
using namespace TA_IRS_Core;

namespace TA_IRS_App
{
    AtsPlatformDatabaseAccessor::AtsPlatformDatabaseAccessor()
        : m_isLoaded(false)
    {
        m_configItems.clear();
    }


    AtsPlatformDatabaseAccessor::~AtsPlatformDatabaseAccessor()
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


    void AtsPlatformDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the Ats Platforms and this is not a refresh so we exit");
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


        // Now reload all the Ats Platforms
        IAtsPlatforms items = AtsPlatformAccessFactory::getInstance().getAllAtsPlatforms(true);

        // And insert the Ats Platforms into our map. At the same time cast them to IConfigAtsPlatform*
        // instead of IAtsPlatform*.
        for (IAtsPlatformsIt it = items.begin(); it != items.end(); ++it)
        {
            IAtsPlatform* item = *it;
            IConfigAtsPlatform* configItem = dynamic_cast<IConfigAtsPlatform*>(item);
            if( configItem != NULL )
            {
                unsigned long key = configItem->getUniqueIdentifier();
                m_configItems[key] = configItem;
            }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> AtsPlatformDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for Ats Platforms");

        std::multimap<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long,AtsPlatformDatabaseAccessor::AtsPlatformRecord> AtsPlatformDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,AtsPlatformRecord> details; 

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
                AtsPlatformRecord record;

				record.key = it->second->getKey();

				std::ostringstream platformId;
				record.platformIdValue = it->second->getPlatformId();
				platformId << record.platformIdValue;
				record.platformId = platformId.str().c_str();


                std::ostringstream tisPlatformId;
				record.tisPlatformIdValue = it->second->getTisPlatformId();
				tisPlatformId << record.tisPlatformIdValue;
				record.tisPlatformId = tisPlatformId.str().c_str();

				std::ostringstream station;
				try
				{
					unsigned long locationId = it->second->getStationId();
					TA_Base_Core::ILocation* location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationId);

					station << location->getName().c_str();

					delete location;
				}
				catch (...)
				{
					station << it->second->getStationId();
				}

                record.station = station.str().c_str();
				
                record.platformName = it->second->getName().c_str();
                
                details.insert( std::map<unsigned long, AtsPlatformRecord>::value_type( it->second->getUniqueIdentifier(), record ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    AtsPlatformDatabaseAccessor::AtsPlatformRecord AtsPlatformDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the Ats Platform matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(identifier);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Ats Platform with identifier %lu",identifier);
            FUNCTION_EXIT;
            AtsPlatformRecord record;
            return record;
        }

        AtsPlatformRecord record;

		record.key = matching->second->getKey();

		std::ostringstream platformId;
		record.platformIdValue = matching->second->getPlatformId();
		platformId << record.platformIdValue;
		record.platformId = platformId.str().c_str();

        std::ostringstream tisPlatformId;
		record.tisPlatformIdValue = matching->second->getTisPlatformId();
		tisPlatformId << record.tisPlatformIdValue;
		record.tisPlatformId = tisPlatformId.str().c_str();

		std::ostringstream station;
		try
		{
			unsigned long locationId = matching->second->getStationId();
			TA_Base_Core::ILocation* location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationId);

			station << location->getName().c_str();

			delete location;
		}
		catch (...)
		{
			station << matching->second->getStationId();
		}

        record.station = station.str().c_str();

		record.platformName = matching->second->getName().c_str();
 
        FUNCTION_EXIT;
        return record;
    }


    TA_Base_Core::IConfigItem* AtsPlatformDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the Ats Platform matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Ats Platform with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void AtsPlatformDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the Ats Platform matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Ats Platform with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored Ats Platform was equal to NULL");

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


    TA_Base_Core::IConfigItem* AtsPlatformDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_IRS_Core::IConfigAtsPlatform* newAssoc = TA_IRS_Core::AtsPlatformAccessFactory::getInstance().createAtsPlatform();

        m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );

        return newAssoc;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* AtsPlatformDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_IRS_Core::IConfigAtsPlatform* newAssoc = NULL;

        // Attempt to find the Ats Platform matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(idOfItemToCopy);

        TA_ASSERT(matching != m_configItems.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigAtsPlatform stored in the map");

        newAssoc = TA_IRS_Core::AtsPlatformAccessFactory::getInstance().copyAtsPlatform(matching->second);

        if (newAssoc != NULL)
        {
            m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
        }
        
        return newAssoc;

        FUNCTION_EXIT;
    }


    bool AtsPlatformDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
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
                // individual change for Ats Platforms
                itemsNotApplied.push_back("");
                break;
            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }
}

    

    

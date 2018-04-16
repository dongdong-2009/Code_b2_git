/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/ats_psd_entity_map/src/AtsPsdEntityMapDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve ATS PSD Entity Map information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ats_psd_entity_map/src/StdAfx.h"
#include "app/configuration/config_plugins/ats_psd_entity_map/src/AtsPsdEntityMapDatabaseAccessor.h"


#include "core/data_access_interface/ats/src/IConfigAtsPSDData.h"
#include "core/data_access_interface/ats/src/AtsPSDDataFactory.h"
#include "core/data_access_interface/ats/src/AtsPlatformAccessFactory.h"
#include "core/data_access_interface/ats/src/AtsMssZoneAccessFactory.h"
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

    static const std::string PSD_DATAPOINTS_TOKEN		= "%.PSD.%";
    static const std::string DATAPOINT_TYPE				= "DataPoint";

    AtsPsdEntityMapDatabaseAccessor::AtsPsdEntityMapDatabaseAccessor()
        : m_isLoaded(false)
    {
        m_configItems.clear();
        m_dataPointMap.clear();
    }


    AtsPsdEntityMapDatabaseAccessor::~AtsPsdEntityMapDatabaseAccessor()
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


    void AtsPsdEntityMapDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the ATS PSD Entity Maps and this is not a refresh so we exit");
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


        // Now reload all the ATS PSD Entity Maps
        IAtsPSDDataVector items = AtsPSDDataFactory::getInstance().getAllPSDData(true);

        // And insert the ATS PSD Entity Maps into our map. At the same time cast them to IConfigAtsPSDData*
        // instead of IAtsPSDData*.
        for (IAtsPSDDataVectorIt it = items.begin(); it != items.end(); ++it)
        {
            IAtsPSDData* item = *it;
            IConfigAtsPSDData* configItem = dynamic_cast<IConfigAtsPSDData*>(item);
            if( configItem != NULL )
            {
                unsigned long key = configItem->getUniqueIdentifier();
                m_configItems[key] = configItem;
            }
        }

        refreshDataPointMap();

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> AtsPsdEntityMapDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for ATS PSD Entity Maps");

        std::multimap<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long,AtsPsdEntityMapDatabaseAccessor::AtsPsdEntityMapRecord> AtsPsdEntityMapDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,AtsPsdEntityMapRecord> details; 

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
                AtsPsdEntityMapRecord record = formatItemDetails(it->second);
                
                details.insert( std::map<unsigned long, AtsPsdEntityMapRecord>::value_type( it->second->getUniqueIdentifier(), record ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    AtsPsdEntityMapDatabaseAccessor::AtsPsdEntityMapRecord AtsPsdEntityMapDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the ATS PSD Entity Map matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(identifier);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ATS PSD Entity Map with identifier %lu",identifier);
            FUNCTION_EXIT;
            AtsPsdEntityMapRecord record;
            return record;
        }

        AtsPsdEntityMapRecord record = formatItemDetails(matching->second);

        FUNCTION_EXIT;
        return record;
    }

	AtsPsdEntityMapDatabaseAccessor::AtsPsdEntityMapRecord AtsPsdEntityMapDatabaseAccessor::formatItemDetails(TA_IRS_Core::IConfigAtsPSDData* config)
	{
		AtsPsdEntityMapRecord record;

		record.key = config->getKey();

		std::string platformName;
		std::string station;
		std::string mssZone;
		try
		{
			unsigned int platformId = config->getPlatformId();

			TA_IRS_Core::IAtsPlatforms platforms = 
				TA_IRS_Core::AtsPlatformAccessFactory::getInstance().getAtsPlatformsWhere(false, platformId);

			if (platforms.size() == 1)
			{
				unsigned long locationId = platforms[0]->getStationId();
				TA_Base_Core::ILocation* location = 
					TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationId);

				station = location->getName().c_str();

				delete location;

				TA_IRS_Core::IAtsMssZones mssZones = 
					TA_IRS_Core::AtsMssZoneAccessFactory::getInstance().getAtsMssZonesWhere(false, locationId);

				if (mssZones.size() == 1)
				{
					mssZone = mssZones[0]->getMssZone();
				}

				for (IAtsMssZonesIt it = mssZones.begin();
					 it != mssZones.end();
				     it++)
				{
					delete (*it);
					(*it) = NULL;
				}

				platformName = platforms[0]->getName();
			}

			for (IAtsPlatformsIt it = platforms.begin();
				 it != platforms.end();
				 it++)
			{
				delete (*it);
				(*it) = NULL;
			}
		}
		catch (...)
		{
            std::ostringstream stationNumber;
            stationNumber << config->getStationId();
			station = stationNumber.str();
		}

		record.station = station.c_str();
		record.mssZone = mssZone.c_str();
		record.platformName = platformName.c_str();

		if (config->getPSDNumber() != -1)
		{
			std::ostringstream psdNumber;
			psdNumber << config->getPSDNumber();
			record.psdNumber = psdNumber.str().c_str();
		}
		else
		{
			record.psdNumber = "";
		}

		record.dpType = config->getPSDDataPointNameOfType().c_str();
		record.dpInputOutput = config->getPSDDataPointType().c_str();

		record.dpName = getDataPointName(config->getPSDDataPointEntityKey()).c_str();

        //CString dataPointLabel;		// The label of the DataPoint

		return record;
	}	

    TA_Base_Core::IConfigItem* AtsPsdEntityMapDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the ATS PSD Entity Map matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ATS PSD Entity Map with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void AtsPsdEntityMapDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the ATS PSD Entity Map matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ATS PSD Entity Map with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored ATS PSD Entity Map was equal to NULL");

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


    TA_Base_Core::IConfigItem* AtsPsdEntityMapDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_IRS_Core::IConfigAtsPSDData* newAssoc = TA_IRS_Core::AtsPSDDataFactory::getInstance().createAtsPSDData();

        m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );

        return newAssoc;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* AtsPsdEntityMapDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_IRS_Core::IConfigAtsPSDData* newAssoc = NULL;

        // Attempt to find the ATS PSD Entity Map matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(idOfItemToCopy);

        TA_ASSERT(matching != m_configItems.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigAtsPsdEntityMap stored in the map");

        newAssoc = TA_IRS_Core::AtsPSDDataFactory::getInstance().copyAtsPSDData(matching->second);

        if (newAssoc != NULL)
        {
            m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
        }
        
        return newAssoc;

        FUNCTION_EXIT;
    }


    bool AtsPsdEntityMapDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
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
                // individual change for ATS PSD Entity Maps
                itemsNotApplied.push_back("");
                break;
            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }


    void AtsPsdEntityMapDatabaseAccessor::refreshDataPointMap()
    {
        FUNCTION_ENTRY("refreshDataPointMap");

        m_dataPointMap.clear();

        // First retrieve all the subsystems from the database
        IEntityDataList dataPointEntities;
        try
        {
            dataPointEntities = EntityAccessFactory::getInstance().getEntitiesOfTypeWithNameLikeToken(DATAPOINT_TYPE, PSD_DATAPOINTS_TOKEN);
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No ATS PSD Entity Map datapoints will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No ATS PSD Entity Map datapoints will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return;
        }

        // Now step through each one and:
        // 1) Add it's name and key to the map we want ot return
        // 2) Delete the DataPointEntity object when we're done
        for (IEntityDataList::iterator it = dataPointEntities.begin(); it != dataPointEntities.end(); ++it)
        {
            if (*it != NULL)
            {
                try
                {
                    m_dataPointMap.insert( DataPointKeyToNamesMap::value_type( (*it)->getKey(), (*it)->getName() ) );
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "This ATS PSD Entity Map datapoint will not be added to the map");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "This ATS PSD Entity Map datapoint will not be added to the map");
                }

                // Now we're finished with this iterator so we might as well delete what it contains so we
                // don't have to do it later.
                delete *it;
                *it = NULL;
            }
        }
        dataPointEntities.clear();

        FUNCTION_EXIT;
    }

    std::string AtsPsdEntityMapDatabaseAccessor::getDataPointName(unsigned long dataPointKey)
    {
        DataPointKeyToNamesMapIt it = m_dataPointMap.begin();
        for (; it != m_dataPointMap.end(); ++it)
        {
            if (it->first == dataPointKey)
            {
                return it->second;
            }
        }

        // Can't find it
        return "";
    }

    unsigned long AtsPsdEntityMapDatabaseAccessor::getDataPointKey(const std::string& dataPointName)
    {
        DataPointKeyToNamesMapIt it = m_dataPointMap.begin();
        for (; it != m_dataPointMap.end(); ++it)
        {
            if (it->second == dataPointName)
            {
                return it->first;
            }
        }

        // Can't find it
        return -1;
    }

}

    

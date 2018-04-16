/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/ats_fixed_block_map/src/AtsFixedBlockMapDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/07/02 16:00:02 $
  * Last modified by:  $Author: peter.wong $
  * 
  * This class uses the Data Access Interface to retrieve Ats Fixed Block Map information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ats_fixed_block_map/src/StdAfx.h"
#include "app/configuration/config_plugins/ats_fixed_block_map/src/AtsFixedBlockMapDatabaseAccessor.h"


#include "core/data_access_interface/ats/src/IConfigAtsFbMapRecord.h"
#include "core/data_access_interface/ats/src/AtsFixedBlockMapAccessFactory.h"
#include "core/data_access_interface/ats/src/AtsBranchTrackAccessFactory.h"
#include "core/data_access_interface/ecs/src/ECSZoneAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/BVSStage.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using namespace TA_Base_Core;
using namespace TA_IRS_Core;

namespace TA_IRS_App
{
    AtsFixedBlockMapDatabaseAccessor::AtsFixedBlockMapDatabaseAccessor()
        : m_isLoaded(false)
    {
        m_configItems.clear();
    }


    AtsFixedBlockMapDatabaseAccessor::~AtsFixedBlockMapDatabaseAccessor()
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


    void AtsFixedBlockMapDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the Ats Fixed Block Maps and this is not a refresh so we exit");
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


        // Now reload all the Ats Fixed Block Maps
        IAtsFbMapRecords items = AtsFixedBlockMapAccessFactory::getInstance().getAllAtsFbMapRecords(true);

        // And insert the Ats Fixed Block Maps into our map. At the same time cast them to IConfigAtsFixedBlockMap*
        // instead of IAtsFixedBlockMap*.
        for (IAtsFbMapRecordsIt it = items.begin(); it != items.end(); ++it)
        {
            IAtsFbMapRecord* item = *it;
            IConfigAtsFbMapRecord* configItem = dynamic_cast<IConfigAtsFbMapRecord*>(item);
            if( configItem != NULL )
            {
                unsigned long key = configItem->getUniqueIdentifier();
                m_configItems[key] = configItem;
            }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::map<CString,unsigned long> AtsFixedBlockMapDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for Ats Fixed Block Maps");

        std::map<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long,AtsFixedBlockMapDatabaseAccessor::AtsFixedBlockMapRecord> AtsFixedBlockMapDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,AtsFixedBlockMapRecord> details; 

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
                AtsFixedBlockMapRecord record = formatItemDetails(it->second);

                details.insert( std::map<unsigned long, AtsFixedBlockMapRecord>::value_type( it->second->getUniqueIdentifier(), record ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


	AtsFixedBlockMapDatabaseAccessor::AtsFixedBlockMapRecord AtsFixedBlockMapDatabaseAccessor::formatItemDetails(TA_IRS_Core::IConfigAtsFbMapRecord* config)
	{
		AtsFixedBlockMapRecord record;

		record.key = config->getKey();
		record.zoneType = config->getZoneTypeName().c_str();

		std::ostringstream zoneId;

		if (record.zoneType == "RC_ZONES")
		{
			try
			{
				unsigned long locationId = (unsigned long)config->getZoneId();
				TA_Base_Core::ILocation* location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationId);

				zoneId << location->getName().c_str();

				delete location;
			}
			catch (...)
			{
			}
		}
		else if (record.zoneType == "VI_ZONES")
		{
			try
			{
				unsigned long locationId1, locationId2;
				char dot;
				bool negative=false;
				double configZone = config->getZoneId();
				if ( configZone < 0 )
				{
					negative=true;
					configZone *= -1;
				}
				std::stringstream zoneIdExtract;
				zoneIdExtract << configZone;

				if (zoneIdExtract >> locationId1)
				{					
					TA_Base_Core::IEntityDataList bvsStages = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(TA_Base_Core::BVSStage::getStaticType());
					
					TA_Base_Core::IEntityDataList::iterator it;
					for (it = bvsStages.begin(); it != bvsStages.end(); it++)
					{
						if(locationId1 == atoi((*it)->getAddress().c_str()))
						{
							 break;
						}	
					}
					if(it!=bvsStages.end())
					{
						IEntityData* stage1 = *it;

						zoneId << stage1->getName().c_str();
						
						if (zoneIdExtract >> dot && zoneIdExtract >> locationId2)
						{
							TA_Base_Core::IEntityDataList::iterator it2;
							for (it2 = bvsStages.begin(); it2 != bvsStages.end(); it2++)
							{
								if(locationId2 == atoi((*it2)->getAddress().c_str()))
								{
									 break;
								}	
							}
							if(it2!=bvsStages.end())
							{
								IEntityData* stage2 = *it2;
								
								zoneId << "/" << stage2->getName().c_str();
								if(negative)
								{
									zoneId<< " PreConflict";
								}
								else
								{
									zoneId << " Conflict";
								}
							}
						}
					}
				}
			}
			catch (...)
			{
			}
		}
		else if (record.zoneType == "EC_ZONES")
		{
			try
			{
				unsigned long ecsZoneId = (unsigned long)config->getZoneId();
				TA_IRS_Core::IECSZone* ecsZone = TA_IRS_Core::ECSZoneAccessFactory::getInstance().getECSZone(ecsZoneId);
				zoneId << ecsZone->getECSLocation().c_str();
				zoneId << " - " << ecsZone->getTrack().c_str();
				delete ecsZone;
			}
			catch (...)
			{
			}
		}
		
		zoneId << " (" << config->getZoneId() << ")";
		record.zoneId = zoneId.str().c_str();

		try
		{
			TA_IRS_Core::IAtsBranchTrack* branchTrack = 
				TA_IRS_Core::AtsBranchTrackAccessFactory::getInstance().getAtsBranchTrack(config->getBranchId());

			record.track = branchTrack->getTrack().c_str();

			delete branchTrack;
		}
		catch (...)
		{
			record.track = config->getTrack().c_str();
		}

        record.branchId = config->getBranchIdAsString().c_str();

		std::ostringstream minAbscissa;
		minAbscissa << config->getMinAbscissa();
		record.minAbscissa = minAbscissa.str().c_str();

		std::ostringstream maxAbscissa;
		maxAbscissa << config->getMaxAbscissa();
		record.maxAbscissa = maxAbscissa.str().c_str();

		std::ostringstream armId;
		armId << config->getArmId();
		record.armId = armId.str().c_str();

		return record;
	}	

    AtsFixedBlockMapDatabaseAccessor::AtsFixedBlockMapRecord AtsFixedBlockMapDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the Ats Fixed Block Map matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(identifier);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Ats Fixed Block Map with identifier %lu",identifier);
            FUNCTION_EXIT;
            AtsFixedBlockMapRecord record;
            return record;
        }

        AtsFixedBlockMapRecord record = formatItemDetails(matching->second);
 
        FUNCTION_EXIT;
        return record;
    }


    TA_Base_Core::IConfigItem* AtsFixedBlockMapDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the Ats Fixed Block Map matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Ats Fixed Block Map with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void AtsFixedBlockMapDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the Ats Fixed Block Map matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the Ats Fixed Block Map with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored Ats Fixed Block Map was equal to NULL");

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


    TA_Base_Core::IConfigItem* AtsFixedBlockMapDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_IRS_Core::IConfigAtsFbMapRecord* newAssoc = TA_IRS_Core::AtsFixedBlockMapAccessFactory::getInstance().createAtsFbMapRecord();

        m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );

        return newAssoc;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* AtsFixedBlockMapDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_IRS_Core::IConfigAtsFbMapRecord* newAssoc = NULL;

        // Attempt to find the Ats Fixed Block Map matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(idOfItemToCopy);

        TA_ASSERT(matching != m_configItems.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigAtsFixedBlockMap stored in the map");

        newAssoc = TA_IRS_Core::AtsFixedBlockMapAccessFactory::getInstance().copyAtsFbMapRecord(matching->second);

        if (newAssoc != NULL)
        {
            m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
        }
        
        return newAssoc;

        FUNCTION_EXIT;
    }


    bool AtsFixedBlockMapDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
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
                // individual change for Ats Fixed Block Maps
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

    

    

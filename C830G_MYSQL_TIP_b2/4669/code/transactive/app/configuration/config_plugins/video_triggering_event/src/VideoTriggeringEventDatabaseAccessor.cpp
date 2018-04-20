/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/video_triggering_event/src/VideoTriggeringEventDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve DB information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"

#include "core/data_access_interface/video_switch_agent/src/IConfigVideoTriggeringEvent.h"
#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEventAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/VideoStationIdAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/src/DatabaseKey.h"

using namespace TA_Base_Core; using namespace TA_IRS_Core;

namespace TA_IRS_App
{

	const std::string VideoTriggeringEventDatabaseAccessor::CCTV_ALARM_TYPE = "CctvAlarm";

    VideoTriggeringEventDatabaseAccessor::VideoTriggeringEventDatabaseAccessor()
        : m_isLoaded(false)
    {
        m_configItems.clear();
		m_OccConfigItems.clear();
    }


    VideoTriggeringEventDatabaseAccessor::~VideoTriggeringEventDatabaseAccessor()
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

			// TD15848
			for (OccConfigItems::iterator occIt = m_OccConfigItems.begin(); occIt != m_OccConfigItems.end(); ++occIt)
			{
				if (occIt->second != NULL)
				{
					delete occIt->second;
					occIt->second = NULL;	
				}
			}
			m_OccConfigItems.clear();
			// TD15848
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }


        FUNCTION_EXIT;
    }


    void VideoTriggeringEventDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the Triggering Events and this is not a refresh so we exit");
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


        // Now reload all the triggering events
        IVideoTriggeringEvents items = VideoTriggeringEventAccessFactory::getInstance().getAllVideoTriggeringEvents(true);

        // And insert the triggering events into our map. At the same time cast them to IConfigVideoTriggeringEvent*
        // instead of IVideoTriggeringEvent*.
        for (IVideoTriggeringEvents::iterator it = items.begin(); it != items.end(); ++it)
        {
            IVideoTriggeringEvent* item = *it;
            IConfigVideoTriggeringEvent* configItem = dynamic_cast<IConfigVideoTriggeringEvent*>(item);

            if( configItem != NULL )
            {
				// TD16066
				unsigned long key = configItem->getUniqueIdentifier();

				TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(configItem->getEntityKey());
				if (entityData->getLocation() != 1) // must be Station location
				{
					m_configItems[key] = configItem;					
				}
				else
				{
					// OCC alarm entities must not be visible to operators
					// create a separate map for the OCC alarm entities for Add/Modify in CE
					EventState ev;
					ev.eventId = configItem->getEventId();
					ev.stationId = configItem->getStationId();
					m_OccConfigItems[ev] = configItem;
				}
				delete entityData;
				// TD16066
            }
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Loaded %d Triggering Events", m_configItems.size());


        // build a map of video station keys to location names
        IVideoStationIds videoStations;
        std::vector<ILocation*> allLocations;

        try
        {
            // load all video stations
            IVideoStationIds videoStations = 
                VideoStationIdAccessFactory::getInstance().getAllVideoStationIds();

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Loaded %d Video Locations", videoStations.size());

            // now load all locations
            std::vector<ILocation*> allLocations =
                LocationAccessFactory::getInstance().getAllLocations();

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Loaded %d Locations", allLocations.size());

            // build the map of station id pkeys to location names
            for (IVideoStationIds::iterator vsIter = videoStations.begin();
                 vsIter != videoStations.end(); vsIter++)
            {
                unsigned int locationId = (*vsIter)->getTaLocation();
                // get the location name
                for (std::vector<ILocation*>::iterator locIter = allLocations.begin();
                     locIter != allLocations.end(); locIter++)
                {
                    if ((*locIter)->getKey() == locationId)
                    {
                        // found it
                        m_locationNameCache[(*vsIter)->getStationId()] = (*locIter)->getName();
                        break;
                    }
                }
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Station Key to location name map has %d entries", m_locationNameCache.size());
        }
        catch( ... )
        {
            // clean up
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Caught exception while loading video stations and locations, throwing on." );

            for (IVideoStationIds::iterator vsIter = videoStations.begin();
                 vsIter != videoStations.end(); vsIter++)
            {
                delete *vsIter;
                *vsIter = NULL;
            }
            videoStations.clear();

            for (std::vector<ILocation*>::iterator locIter = allLocations.begin();
                 locIter != allLocations.end(); locIter++)
            {
                delete *locIter;
                *locIter = NULL;
            }
            allLocations.clear();

            FUNCTION_EXIT;
            throw;
        }

        // clean up
        for (IVideoStationIds::iterator vsIter = videoStations.begin();
             vsIter != videoStations.end(); vsIter++)
        {
            delete *vsIter;
            *vsIter = NULL;
        }
        videoStations.clear();

        for (std::vector<ILocation*>::iterator locIter = allLocations.begin();
             locIter != allLocations.end(); locIter++)
        {
            delete *locIter;
            *locIter = NULL;
        }
        allLocations.clear();

		// TD15848 load CCTV alarm entities 
		std::vector<TA_Base_Core::IEntityData*> entityList = 
				TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(CCTV_ALARM_TYPE);

		for (int i = 0; i < entityList.size(); ++i)
		{
			if (entityList[i]->getLocation() != 1) // display alarm entities for STATION only
			{
				m_alarmtypeCache[ entityList[i]->getKey() ] = entityList[i]->getName();
			}
			delete entityList[i];
		}		
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"CCTV Alarm entities has %d entries", m_alarmtypeCache.size());

		// TD15848

        m_isLoaded = true;

        FUNCTION_EXIT;
    }


    std::map<CString,unsigned long> VideoTriggeringEventDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for this class");

        std::map<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long,VideoTriggeringEventDatabaseAccessor::VideoTriggeringEventRecord> VideoTriggeringEventDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,VideoTriggeringEventRecord> details; 

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
                VideoTriggeringEventRecord record = getRecordFrom(*it->second);

                details.insert( std::map<unsigned long, VideoTriggeringEventRecord>::value_type( it->second->getUniqueIdentifier(), record ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    VideoTriggeringEventDatabaseAccessor::VideoTriggeringEventRecord VideoTriggeringEventDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the record matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(identifier);
        if (matching == m_configItems.end() || 0 == matching->second)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the triggering event with identifier %lu",identifier);
            FUNCTION_EXIT;
            VideoTriggeringEventRecord record;
            return record;
        }

        try
        {
            VideoTriggeringEventRecord record = getRecordFrom(*matching->second);

            FUNCTION_EXIT;
            return record;
        }
        catch (TA_Base_Core::TransactiveException& te)
        {
            std::ostringstream err;
            err << "Unexpected Exception coming from an item already loaded and validated. "
                << te.what();

            TA_ASSERT(false, err.str().c_str());
        }

        // shouldnt be necessary
        VideoTriggeringEventRecord record;
        return record;
    }


    VideoTriggeringEventDatabaseAccessor::VideoTriggeringEventRecord VideoTriggeringEventDatabaseAccessor::getRecordFrom(TA_IRS_Core::IConfigVideoTriggeringEvent& item)
    {
        VideoTriggeringEventRecord record;

        record.stationName = getLocationName(item.getStationId()).c_str();
        record.eventId.Format("%ld", item.getEventId());
        record.location = item.getLocation().c_str();
        record.cctvTriggering = item.getCctvTriggering().c_str();
        record.manualClosing = item.getManualClosing() ? "Yes" : "No";

		record.alarmType = getAlarmTypeName(item.getEntityKey()).c_str(); // TD15848

        return record;
    }


    TA_Base_Core::IConfigItem* VideoTriggeringEventDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the Video Zone matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the triggering event with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void VideoTriggeringEventDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the Video Zone matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the triggering event with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored triggering event was equal to NULL");

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


    TA_Base_Core::IConfigItem* VideoTriggeringEventDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        // Create a configurable item with an invalid key (set it within config page)
        TA_IRS_Core::IConfigVideoTriggeringEvent* newAssoc = 
            TA_IRS_Core::VideoTriggeringEventAccessFactory::getInstance().createVideoTriggeringEvent();

        m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );

        return newAssoc;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* VideoTriggeringEventDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_IRS_Core::IConfigVideoTriggeringEvent* newAssoc = NULL;

        // Attempt to find the Video Zone matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(idOfItemToCopy);

        TA_ASSERT(matching != m_configItems.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigVideoTriggeringEvent stored in the map");

        newAssoc = TA_IRS_Core::VideoTriggeringEventAccessFactory::getInstance().copyVideoTriggeringEvent(matching->second);

        if (newAssoc != NULL)
        {
            m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
        }
        
        return newAssoc;

        FUNCTION_EXIT;
    }


    bool VideoTriggeringEventDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
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
                // individual change for Video Zones
                itemsNotApplied.push_back("");
                break;
            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }


    std::string VideoTriggeringEventDatabaseAccessor::getLocationName(unsigned long stationId)
    {
        // find the location 
        StationIdToLocationNameMap::const_iterator it = m_locationNameCache.find(stationId);
        if (it != m_locationNameCache.end())
        {
            return it->second;
        }

        // Can't find it
        return "";
    }


    unsigned long VideoTriggeringEventDatabaseAccessor::getStationId(const std::string& locationName)
    {
        StationIdToLocationNameMap::iterator it = m_locationNameCache.begin();
        for (; it != m_locationNameCache.end(); ++it)
        {
            if (it->second == locationName)
            {
                return it->first;
            }
        }
    
        // Can't find it
        return -1;
    }

	// TD15848
	std::string VideoTriggeringEventDatabaseAccessor::getAlarmTypeName(unsigned long entitykey)
	{
		std::map<unsigned long, std::string>::const_iterator it = m_alarmtypeCache.find(entitykey);
        if (it != m_alarmtypeCache.end())
        {
            return it->second;
        }

		return "";
	}

	unsigned long VideoTriggeringEventDatabaseAccessor::getAlarmEntityKey(const std::string& entityname)
	{
		std::map<unsigned long, std::string>::iterator it = m_alarmtypeCache.begin();
        for (; it != m_alarmtypeCache.end(); ++it)
        {
            if (it->second == entityname)
            {
                return it->first;
            }
        }

		return -1;
	}

	TA_IRS_Core::IConfigVideoTriggeringEvent* VideoTriggeringEventDatabaseAccessor::getOccEntity(unsigned long eventId, unsigned long stationId)
	{
		IConfigVideoTriggeringEvent* configItem = NULL;
		
		EventState ev;
		ev.eventId = eventId;
		ev.stationId = stationId;

		OccConfigItems::iterator it = m_OccConfigItems.find(ev);
		if (it != m_OccConfigItems.end())
		{
			configItem = it->second;
		}

		return configItem;
	}
	// TD15848

}

    

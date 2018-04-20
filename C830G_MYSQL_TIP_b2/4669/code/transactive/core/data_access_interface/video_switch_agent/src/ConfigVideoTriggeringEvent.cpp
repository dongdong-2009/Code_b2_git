/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/video_switch_agent/src/ConfigVideoTriggeringEvent.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ConfigVideoTriggeringEvent is a concrete implementation of IConfigVideoTriggeringEvent, which is in turn an implementation
  * of IVideoTriggeringEvent. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for VideoTriggeringEvents.
  */
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/video_switch_agent/src/ConfigVideoTriggeringEvent.h"
#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEventHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h" // TD15848

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string STATION_ID					= "Station Id";
    static const std::string EVENT_ID					= "Event Id";
    static const std::string LOCATION					= "Location";
    static const std::string TRIGGERING_EVENT           = "Triggering Event";
    static const std::string MANUAL_CLOSING             = "Manual Closing";
	// TD 15200
	static const std::string ENTITYKEY					= "Entity Key";
    // TD 15200
    long ConfigVideoTriggeringEvent::s_nextAvailableIdentifier = 0;


    ConfigVideoTriggeringEvent::ConfigVideoTriggeringEvent()
     : m_VideoTriggeringEventHelper( new VideoTriggeringEventHelper() ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier ),
	   m_occEntity(NULL)
    {
    }


    ConfigVideoTriggeringEvent::ConfigVideoTriggeringEvent(const unsigned long key)
     : m_VideoTriggeringEventHelper( new VideoTriggeringEventHelper(key) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier ),
	   m_occEntity(NULL)
    {
    }

    ConfigVideoTriggeringEvent::ConfigVideoTriggeringEvent( const ConfigVideoTriggeringEvent& theConfigVideoTriggeringEvent)
     : m_VideoTriggeringEventHelper( new VideoTriggeringEventHelper( *(theConfigVideoTriggeringEvent.m_VideoTriggeringEventHelper)) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier ),
	   m_occEntity(NULL)
    {
    }

    ConfigVideoTriggeringEvent::ConfigVideoTriggeringEvent(unsigned long row, TA_Base_Core::IData& data)
     : m_VideoTriggeringEventHelper( new VideoTriggeringEventHelper(row, data)),
       m_uniqueIdentifier(++s_nextAvailableIdentifier),
	   m_occEntity(NULL)
    {
    }

    ConfigVideoTriggeringEvent::~ConfigVideoTriggeringEvent()
    {
        try
        {
            delete m_VideoTriggeringEventHelper;
            m_VideoTriggeringEventHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigVideoTriggeringEvent::isNew()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");

        return m_VideoTriggeringEventHelper->isNew();
    }

    void ConfigVideoTriggeringEvent::setStationId(unsigned long stationId)
    {
        FUNCTION_ENTRY("setStationId");
        
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");

        std::ostringstream oldId;
        oldId << m_VideoTriggeringEventHelper->getStationId();

        std::ostringstream newId;
        newId << stationId;

        updateVideoTriggeringEventChanges(STATION_ID, oldId.str(), newId.str());

        m_VideoTriggeringEventHelper->setStationId(stationId);

        FUNCTION_EXIT;
    }
    
    void ConfigVideoTriggeringEvent::setEventId(unsigned long eventId)
    {
        FUNCTION_ENTRY("setEventId");
        
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
    
        std::ostringstream oldEventId;
        oldEventId << m_VideoTriggeringEventHelper->getEventId();

        std::ostringstream newEventId;
        newEventId << eventId;

        updateVideoTriggeringEventChanges(EVENT_ID, oldEventId.str(), newEventId.str());

        m_VideoTriggeringEventHelper->setEventId(eventId);

        FUNCTION_EXIT;
    }

    void ConfigVideoTriggeringEvent::setLocation(const std::string& location)
    {
        FUNCTION_ENTRY("setLocation");
        
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
    
        updateVideoTriggeringEventChanges(LOCATION, m_VideoTriggeringEventHelper->getLocation(), location);

        m_VideoTriggeringEventHelper->setLocation(location);

        FUNCTION_EXIT;
    }

    void ConfigVideoTriggeringEvent::setCctvTriggering(const std::string& triggeringEvent)
    {
        FUNCTION_ENTRY("setCctvTriggering");
        
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
    
        updateVideoTriggeringEventChanges(TRIGGERING_EVENT, m_VideoTriggeringEventHelper->getTriggeringEvent(), triggeringEvent);

        m_VideoTriggeringEventHelper->setTriggeringEvent(triggeringEvent);

        FUNCTION_EXIT;
    }

    void ConfigVideoTriggeringEvent::setManualClosing(bool manualClosing)
    {
        FUNCTION_ENTRY("setManualClosing");
        
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");

        std::ostringstream oldMC;
        oldMC << m_VideoTriggeringEventHelper->getManualClosing();

        std::ostringstream newMC;
        newMC << manualClosing;
    
        updateVideoTriggeringEventChanges(MANUAL_CLOSING, oldMC.str(), newMC.str());

        m_VideoTriggeringEventHelper->setManualClosing(manualClosing);

        FUNCTION_EXIT;
    }
	
	// TD 15200
	void ConfigVideoTriggeringEvent::setEntityKey(unsigned long entityKey)
    {
        FUNCTION_ENTRY("setEntityKey");
        
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
    
        std::ostringstream oldEntityKey;
        oldEntityKey << m_VideoTriggeringEventHelper->getEntityKey();

        std::ostringstream newEntityKey;
        newEntityKey << entityKey;

        updateVideoTriggeringEventChanges(ENTITYKEY, oldEntityKey.str(), newEntityKey.str());

        m_VideoTriggeringEventHelper->setEntityKey(entityKey);

        FUNCTION_EXIT;
    }
	// TD 15200

    unsigned long ConfigVideoTriggeringEvent::getKey()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        return m_VideoTriggeringEventHelper->getKey();
    }

    std::string ConfigVideoTriggeringEvent::getName()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        return m_VideoTriggeringEventHelper->getName();
    }

    unsigned long ConfigVideoTriggeringEvent::getStationId()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        return m_VideoTriggeringEventHelper->getStationId();
    }


    unsigned long ConfigVideoTriggeringEvent::getEventId()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        return m_VideoTriggeringEventHelper->getEventId();
    }

	std::string ConfigVideoTriggeringEvent::getLocation()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        return m_VideoTriggeringEventHelper->getLocation();
    }

    std::string ConfigVideoTriggeringEvent::getCctvTriggering()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        return m_VideoTriggeringEventHelper->getTriggeringEvent();
    }

    bool ConfigVideoTriggeringEvent::getManualClosing()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        return m_VideoTriggeringEventHelper->getManualClosing();
    }

	// TD 15200
	unsigned long ConfigVideoTriggeringEvent::getEntityKey()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        return m_VideoTriggeringEventHelper->getEntityKey();
    }
	// TD 15200

    time_t ConfigVideoTriggeringEvent::getDateCreated()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
		return m_VideoTriggeringEventHelper->getDateCreated();
    }


    time_t ConfigVideoTriggeringEvent::getDateModified()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
		return m_VideoTriggeringEventHelper->getDateModified();
    }

    void ConfigVideoTriggeringEvent::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigVideoTriggeringEvent::invalidate");
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");

        m_VideoTriggeringEventHelper->invalidate();
        m_VideoTriggeringEventChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigVideoTriggeringEvent::invalidate");
    }


    void ConfigVideoTriggeringEvent::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigVideoTriggeringEvent::deleteThisObject");
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");

        // Delete the VideoTriggeringEvent from the database
        m_VideoTriggeringEventHelper->deleteVideoTriggeringEvent(cascade);
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete VideoTriggeringEventHelper pointer m_VideoTriggeringEventHelper");

        delete m_VideoTriggeringEventHelper;
        m_VideoTriggeringEventHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigVideoTriggeringEvent::deleteThisObject");
    }


    void ConfigVideoTriggeringEvent::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigVideoTriggeringEvent::applyChanges");
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL, "applyChanges() has been called after VideoTriggeringEvent was deleted");

        m_VideoTriggeringEventHelper->writeVideoTriggeringEventData();

		// TD15848
		if (m_occEntity != NULL)
		{
			TA_Base_Core::IEntityData* stnEntityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_VideoTriggeringEventHelper->getEntityKey());
			TA_ASSERT(stnEntityData != NULL,"The entity of the station alarm is null.");

			std::string occStr = "Occ_" + stnEntityData->getName();
			delete stnEntityData;

			std::ostringstream data;
			data << "[TD15848] Write OCC entity data for ID = " << m_occEntity->getKey() 
				 << " EntityKey =" << occStr;
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, data.str().c_str());

			TA_Base_Core::IEntityData* occEntityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(occStr);
			TA_ASSERT(occEntityData != NULL,"The entity of the OCC alarm is null.");

			m_occEntity->setEntityKey(occEntityData->getKey());
			delete occEntityData;

			VideoTriggeringEventHelper occEventHelper(m_occEntity->getKey());
			occEventHelper.setEntityKey(m_occEntity->getEntityKey());
			occEventHelper.setEventId( m_VideoTriggeringEventHelper->getEventId() );
			occEventHelper.setStationId( m_VideoTriggeringEventHelper->getStationId());
			occEventHelper.setLocation( m_VideoTriggeringEventHelper->getLocation());
			occEventHelper.setTriggeringEvent( m_VideoTriggeringEventHelper->getTriggeringEvent());
			occEventHelper.setManualClosing( m_VideoTriggeringEventHelper->getManualClosing());
			occEventHelper.writeVideoTriggeringEventData();
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "No OCC Data corresponding to key = %d", m_VideoTriggeringEventHelper->getKey());	
		}
		// TD15848

		m_VideoTriggeringEventChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigVideoTriggeringEvent::applyChanges");
    }


    void ConfigVideoTriggeringEvent::updateVideoTriggeringEventChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateVideoTriggeringEventChanges");

		std::string oldValueString;
		std::string newValueString;
		
		// Search for this name in the map
		TA_Base_Core::ItemChanges::iterator matching = m_VideoTriggeringEventChanges.find(name);

        if (matching != m_VideoTriggeringEventChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_VideoTriggeringEventChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if (oldValue != newValue)
        {
            // If the old and new values are different then we can add this item to the map
			TA_Base_Core::Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_VideoTriggeringEventChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }


    void ConfigVideoTriggeringEvent::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }

    
} // closes TA_IRS_Core


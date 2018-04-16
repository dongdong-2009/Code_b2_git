/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/video_switch_agent/src/ConfigVideoStationId.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * ConfigVideoStationId is a concrete implementation of IConfigVideoStationId, which is in turn an implementation
  * of IVideoStationId. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for VideoStationIds.
  */
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/video_switch_agent/src/ConfigVideoStationId.h"
#include "core/data_access_interface/video_switch_agent/src/VideoStationIdHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string STATIONID          = "Station Id";
    static const std::string TALOCATION             = "Transactive Location";
    
    long ConfigVideoStationId::s_nextAvailableIdentifier = 0;


    ConfigVideoStationId::ConfigVideoStationId()
     : m_VideoStationIdHelper( new VideoStationIdHelper() ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }


    ConfigVideoStationId::ConfigVideoStationId(const unsigned long pKey)
     : m_VideoStationIdHelper( new VideoStationIdHelper(pKey) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigVideoStationId::ConfigVideoStationId( const ConfigVideoStationId& theConfigVideoStationId)
     : m_VideoStationIdHelper( new VideoStationIdHelper( *(theConfigVideoStationId.m_VideoStationIdHelper)) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigVideoStationId::ConfigVideoStationId(unsigned long row, TA_Base_Core::IData& data)
     : m_VideoStationIdHelper( new VideoStationIdHelper(row, data)),
       m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
    }

    ConfigVideoStationId::~ConfigVideoStationId()
    {
        try
        {
            delete m_VideoStationIdHelper;
            m_VideoStationIdHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigVideoStationId::isNew()
    {
        TA_ASSERT(m_VideoStationIdHelper != NULL,"The VideoStationIdHelper pointer is null.");

        return m_VideoStationIdHelper->isNew();
    }

    unsigned long ConfigVideoStationId::getKey()
    {
        TA_ASSERT(m_VideoStationIdHelper != NULL,"The VideoStationIdHelper pointer is null.");
        return m_VideoStationIdHelper->getKey();
    }

    void ConfigVideoStationId::setStationId(unsigned long stationId)
    {
        FUNCTION_ENTRY("setVideoStationIdId");
        
        TA_ASSERT(m_VideoStationIdHelper != NULL,"The VideoStationIdHelper pointer is null.");

        std::ostringstream oldId;
        oldId << m_VideoStationIdHelper->getStationId();

        std::ostringstream newId;
        newId << stationId;

        updateVideoStationIdChanges(STATIONID, oldId.str(), newId.str());

        m_VideoStationIdHelper->setStationId(stationId);

        FUNCTION_EXIT;
    }
    
    void ConfigVideoStationId::setTaLocation(unsigned long taLocation)
    {
        FUNCTION_ENTRY("setTaLocation");
        
        TA_ASSERT(m_VideoStationIdHelper != NULL,"The VideoStationIdHelper pointer is null.");
    
        std::ostringstream oldLocation;
        oldLocation << m_VideoStationIdHelper->getTaLocation();

        std::ostringstream newLocation;
        newLocation << taLocation;

        updateVideoStationIdChanges(TALOCATION, oldLocation.str(), newLocation.str());

        m_VideoStationIdHelper->setTaLocation(taLocation);

        FUNCTION_EXIT;
    }

    unsigned long ConfigVideoStationId::getStationId()
    {
        TA_ASSERT(m_VideoStationIdHelper != NULL,"The VideoStationIdHelper pointer is null.");
        return m_VideoStationIdHelper->getStationId();
    }


    unsigned long ConfigVideoStationId::getTaLocation()
    {
        TA_ASSERT(m_VideoStationIdHelper != NULL,"The VideoStationIdHelper pointer is null.");
        return m_VideoStationIdHelper->getTaLocation();
    }


    time_t ConfigVideoStationId::getDateCreated()
    {
        TA_ASSERT(m_VideoStationIdHelper != NULL,"The VideoStationIdHelper pointer is null.");
		return m_VideoStationIdHelper->getDateCreated();
    }


    time_t ConfigVideoStationId::getDateModified()
    {
        TA_ASSERT(m_VideoStationIdHelper != NULL,"The VideoStationIdHelper pointer is null.");
		return m_VideoStationIdHelper->getDateModified();
    }


    void ConfigVideoStationId::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }


    std::string ConfigVideoStationId::getName()
    {
        // just return the station id
        std::ostringstream name;
        name << getStationId();
        return name.str();
    }


    void ConfigVideoStationId::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigVideoStationId::invalidate");
        TA_ASSERT(m_VideoStationIdHelper != NULL,"The VideoStationIdHelper pointer is null.");

        m_VideoStationIdHelper->invalidate();
        m_VideoStationIdChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigVideoStationId::invalidate");
    }


    void ConfigVideoStationId::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigVideoStationId::deleteThisObject");
        TA_ASSERT(m_VideoStationIdHelper != NULL,"The VideoStationIdHelper pointer is null.");

        // Delete the VideoStationId from the database
        m_VideoStationIdHelper->deleteVideoStationId(cascade);
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete VideoStationIdHelper pointer m_VideoStationIdHelper");

        delete m_VideoStationIdHelper;
        m_VideoStationIdHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigVideoStationId::deleteThisObject");
    }


    void ConfigVideoStationId::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigVideoStationId::applyChanges");
        TA_ASSERT(m_VideoStationIdHelper != NULL, "applyChanges() has been called after VideoStationId was deleted");

        m_VideoStationIdHelper->writeVideoStationIdData();

        m_VideoStationIdChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigVideoStationId::applyChanges");
    }


    void ConfigVideoStationId::updateVideoStationIdChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateVideoStationIdChanges");

		std::string oldValueString;
		std::string newValueString;
		
		// Search for this name in the map
		TA_Base_Core::ItemChanges::iterator matching = m_VideoStationIdChanges.find(name);

        if (matching != m_VideoStationIdChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_VideoStationIdChanges.erase(matching);
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
            m_VideoStationIdChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    
} // closes TA_IRS_Core


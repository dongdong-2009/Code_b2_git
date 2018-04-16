/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/core/data_access_interface/video_switch_agent/src/ConfigTVSSStationId.cpp $
  * @author Zhang hongzhi
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/02/5 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * ConfigTVSSStationId is a concrete implementation of IConfigTVSSStationId, which is in turn an implementation
  * of ITVSSStationId. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for TVSSStationIds.
  */
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/video_switch_agent/src/ConfigTVSSStationId.h"
#include "core/data_access_interface/video_switch_agent/src/TVSSStationIdHelper.h"

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
    
    long ConfigTVSSStationId::s_nextAvailableIdentifier = 0;


    ConfigTVSSStationId::ConfigTVSSStationId()
     : m_TVSSStationIdHelper( new TVSSStationIdHelper() ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }


    ConfigTVSSStationId::ConfigTVSSStationId(const unsigned long pKey)
     : m_TVSSStationIdHelper( new TVSSStationIdHelper(pKey) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigTVSSStationId::ConfigTVSSStationId( const ConfigTVSSStationId& theConfigTVSSStationId)
     : m_TVSSStationIdHelper( new TVSSStationIdHelper( *(theConfigTVSSStationId.m_TVSSStationIdHelper)) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigTVSSStationId::ConfigTVSSStationId(unsigned long row, TA_Base_Core::IData& data)
     : m_TVSSStationIdHelper( new TVSSStationIdHelper(row, data)),
       m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
    }

    ConfigTVSSStationId::~ConfigTVSSStationId()
    {
        try
        {
            delete m_TVSSStationIdHelper;
            m_TVSSStationIdHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigTVSSStationId::isNew()
    {
        TA_ASSERT(m_TVSSStationIdHelper != NULL,"The TVSSStationIdHelper pointer is null.");

        return m_TVSSStationIdHelper->isNew();
    }

    unsigned long ConfigTVSSStationId::getKey()
    {
        TA_ASSERT(m_TVSSStationIdHelper != NULL,"The TVSSStationIdHelper pointer is null.");
        return m_TVSSStationIdHelper->getKey();
    }

    void ConfigTVSSStationId::setStationId(unsigned long stationId)
    {
        FUNCTION_ENTRY("setTVSSStationIdId");
        
        TA_ASSERT(m_TVSSStationIdHelper != NULL,"The TVSSStationIdHelper pointer is null.");

        std::ostringstream oldId;
        oldId << m_TVSSStationIdHelper->getStationId();

        std::ostringstream newId;
        newId << stationId;

        updateTVSSStationIdChanges(STATIONID, oldId.str(), newId.str());

        m_TVSSStationIdHelper->setStationId(stationId);

        FUNCTION_EXIT;
    }
    
    void ConfigTVSSStationId::setTaLocation(unsigned long taLocation)
    {
        FUNCTION_ENTRY("setTaLocation");
        
        TA_ASSERT(m_TVSSStationIdHelper != NULL,"The TVSSStationIdHelper pointer is null.");
    
        std::ostringstream oldLocation;
        oldLocation << m_TVSSStationIdHelper->getTaLocation();

        std::ostringstream newLocation;
        newLocation << taLocation;

        updateTVSSStationIdChanges(TALOCATION, oldLocation.str(), newLocation.str());

        m_TVSSStationIdHelper->setTaLocation(taLocation);

        FUNCTION_EXIT;
    }

    unsigned long ConfigTVSSStationId::getStationId()
    {
        TA_ASSERT(m_TVSSStationIdHelper != NULL,"The TVSSStationIdHelper pointer is null.");
        return m_TVSSStationIdHelper->getStationId();
    }


    unsigned long ConfigTVSSStationId::getTaLocation()
    {
        TA_ASSERT(m_TVSSStationIdHelper != NULL,"The TVSSStationIdHelper pointer is null.");
        return m_TVSSStationIdHelper->getTaLocation();
    }


    time_t ConfigTVSSStationId::getDateCreated()
    {
        TA_ASSERT(m_TVSSStationIdHelper != NULL,"The TVSSStationIdHelper pointer is null.");
		return m_TVSSStationIdHelper->getDateCreated();
    }


    time_t ConfigTVSSStationId::getDateModified()
    {
        TA_ASSERT(m_TVSSStationIdHelper != NULL,"The TVSSStationIdHelper pointer is null.");
		return m_TVSSStationIdHelper->getDateModified();
    }


    void ConfigTVSSStationId::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }


    std::string ConfigTVSSStationId::getName()
    {
        // just return the station id
        std::ostringstream name;
        name << getStationId();
        return name.str();
    }


    void ConfigTVSSStationId::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigTVSSStationId::invalidate");
        TA_ASSERT(m_TVSSStationIdHelper != NULL,"The TVSSStationIdHelper pointer is null.");

        m_TVSSStationIdHelper->invalidate();
        m_TVSSStationIdChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigTVSSStationId::invalidate");
    }


    void ConfigTVSSStationId::deleteThisObject()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigTVSSStationId::deleteThisObject");
        TA_ASSERT(m_TVSSStationIdHelper != NULL,"The TVSSStationIdHelper pointer is null.");

        // Delete the TVSSStationId from the database
        m_TVSSStationIdHelper->deleteTVSSStationId();
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete TVSSStationIdHelper pointer m_TVSSStationIdHelper");

        delete m_TVSSStationIdHelper;
        m_TVSSStationIdHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigTVSSStationId::deleteThisObject");
    }


    void ConfigTVSSStationId::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigTVSSStationId::applyChanges");
        TA_ASSERT(m_TVSSStationIdHelper != NULL, "applyChanges() has been called after TVSSStationId was deleted");

        m_TVSSStationIdHelper->writeTVSSStationIdData();

        m_TVSSStationIdChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigTVSSStationId::applyChanges");
    }


    void ConfigTVSSStationId::updateTVSSStationIdChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateTVSSStationIdChanges");

		std::string oldValueString;
		std::string newValueString;
		
		// Search for this name in the map
		TA_Base_Core::ItemChanges::iterator matching = m_TVSSStationIdChanges.find(name);

        if (matching != m_TVSSStationIdChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_TVSSStationIdChanges.erase(matching);
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
            m_TVSSStationIdChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    
} // closes TA_IRS_Core


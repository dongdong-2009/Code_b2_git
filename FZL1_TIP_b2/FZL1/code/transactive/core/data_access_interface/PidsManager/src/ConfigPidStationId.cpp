/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/PidsManager/src/ConfigPidStationId.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * ConfigPidStationId is a concrete implementation of IConfigPidStationId, which is in turn an implementation
  * of IPidStationId. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PidStationIds.
  */
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/PidsManager/src/ConfigPidStationId.h"
#include "core/data_access_interface/PidsManager/src/PidStationIdHelper.h"

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
    
    long ConfigPidStationId::s_nextAvailableIdentifier = 0;


    ConfigPidStationId::ConfigPidStationId()
     : m_PidStationIdHelper( new PidStationIdHelper() ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }


    ConfigPidStationId::ConfigPidStationId(const unsigned long pKey)
     : m_PidStationIdHelper( new PidStationIdHelper(pKey) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigPidStationId::ConfigPidStationId( const ConfigPidStationId& theConfigPidStationId)
     : m_PidStationIdHelper( new PidStationIdHelper( *(theConfigPidStationId.m_PidStationIdHelper)) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigPidStationId::ConfigPidStationId(unsigned long row, TA_Base_Core::IData& data)
     : m_PidStationIdHelper( new PidStationIdHelper(row, data)),
       m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
    }

    ConfigPidStationId::~ConfigPidStationId()
    {
        try
        {
            delete m_PidStationIdHelper;
            m_PidStationIdHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigPidStationId::isNew()
    {
        TA_ASSERT(m_PidStationIdHelper != NULL,"The PidStationIdHelper pointer is null.");

        return m_PidStationIdHelper->isNew();
    }

    unsigned long ConfigPidStationId::getKey()
    {
        TA_ASSERT(m_PidStationIdHelper != NULL,"The PidStationIdHelper pointer is null.");
        return m_PidStationIdHelper->getKey();
    }

    void ConfigPidStationId::setStationId(unsigned long stationId)
    {
        FUNCTION_ENTRY("setPidStationIdId");
        
        TA_ASSERT(m_PidStationIdHelper != NULL,"The PidStationIdHelper pointer is null.");

        std::ostringstream oldId;
        oldId << m_PidStationIdHelper->getStationId();

        std::ostringstream newId;
        newId << stationId;

        updatePidStationIdChanges(STATIONID, oldId.str(), newId.str());

        m_PidStationIdHelper->setStationId(stationId);

        FUNCTION_EXIT;
    }
    
    void ConfigPidStationId::setTaLocation(unsigned long taLocation)
    {
        FUNCTION_ENTRY("setTaLocation");
        
        TA_ASSERT(m_PidStationIdHelper != NULL,"The PidStationIdHelper pointer is null.");
    
        std::ostringstream oldLocation;
        oldLocation << m_PidStationIdHelper->getTaLocation();

        std::ostringstream newLocation;
        newLocation << taLocation;

        updatePidStationIdChanges(TALOCATION, oldLocation.str(), newLocation.str());

        m_PidStationIdHelper->setTaLocation(taLocation);

        FUNCTION_EXIT;
    }

    unsigned long ConfigPidStationId::getStationId()
    {
        TA_ASSERT(m_PidStationIdHelper != NULL,"The PidStationIdHelper pointer is null.");
        return m_PidStationIdHelper->getStationId();
    }


    unsigned long ConfigPidStationId::getTaLocation()
    {
        TA_ASSERT(m_PidStationIdHelper != NULL,"The PidStationIdHelper pointer is null.");
        return m_PidStationIdHelper->getTaLocation();
    }


    time_t ConfigPidStationId::getDateCreated()
    {
        TA_ASSERT(m_PidStationIdHelper != NULL,"The PidStationIdHelper pointer is null.");
		return m_PidStationIdHelper->getDateCreated();
    }


    time_t ConfigPidStationId::getDateModified()
    {
        TA_ASSERT(m_PidStationIdHelper != NULL,"The PidStationIdHelper pointer is null.");
		return m_PidStationIdHelper->getDateModified();
    }


    void ConfigPidStationId::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }


    std::string ConfigPidStationId::getName()
    {
        // just return the station id
        std::ostringstream name;
        name << getStationId();
        return name.str();
    }


    void ConfigPidStationId::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPidStationId::invalidate");
        TA_ASSERT(m_PidStationIdHelper != NULL,"The PidStationIdHelper pointer is null.");

        m_PidStationIdHelper->invalidate();
        m_PidStationIdChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPidStationId::invalidate");
    }


    void ConfigPidStationId::deleteThisObject()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPidStationId::deleteThisObject");
        TA_ASSERT(m_PidStationIdHelper != NULL,"The PidStationIdHelper pointer is null.");

        // Delete the PidStationId from the database
        m_PidStationIdHelper->deletePidStationId();
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete PidStationIdHelper pointer m_PidStationIdHelper");

        delete m_PidStationIdHelper;
        m_PidStationIdHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPidStationId::deleteThisObject");
    }


    void ConfigPidStationId::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPidStationId::applyChanges");
        TA_ASSERT(m_PidStationIdHelper != NULL, "applyChanges() has been called after PidStationId was deleted");

        m_PidStationIdHelper->writePidStationIdData();

        m_PidStationIdChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPidStationId::applyChanges");
    }


    void ConfigPidStationId::updatePidStationIdChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updatePidStationIdChanges");

		std::string oldValueString;
		std::string newValueString;
		
		// Search for this name in the map
		TA_Base_Core::ItemChanges::iterator matching = m_PidStationIdChanges.find(name);

        if (matching != m_PidStationIdChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_PidStationIdChanges.erase(matching);
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
            m_PidStationIdChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    
} // closes TA_IRS_Core


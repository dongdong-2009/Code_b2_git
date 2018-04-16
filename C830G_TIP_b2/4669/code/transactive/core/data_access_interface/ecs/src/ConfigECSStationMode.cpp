/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ecs/src/ConfigECSStationMode.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ConfigECSStationMode is a concrete implementation of IConfigECSStationMode, which is in turn an implementation
  * of IECSStationMode. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for ECSStationModes.
  */
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/ecs/src/ConfigECSStationMode.h"
#include "core/data_access_interface/ecs/src/ECSStationModeHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
    long ConfigECSStationMode::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string STATIONMODE        = "Station Mode";
    static const std::string LOCATIONKEY        = "Location Key";
    static const std::string OPERATION          = "Operation";
    static const std::string TRACK              = "Track";
    static const std::string CONDITION          = "Condition";
    static const std::string CONGESTIONLOGIC    = "Congestion Logic";


    ConfigECSStationMode::ConfigECSStationMode()
     : m_ECSStationModeHelper( new ECSStationModeHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigECSStationMode::ConfigECSStationMode(const unsigned long key)
     : m_ECSStationModeHelper( new ECSStationModeHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigECSStationMode::ConfigECSStationMode( const ConfigECSStationMode& theConfigECSStationMode)
     : m_ECSStationModeHelper( new ECSStationModeHelper( *(theConfigECSStationMode.m_ECSStationModeHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigECSStationMode::ConfigECSStationMode(unsigned long row, TA_Base_Core::IData& data)
     : m_ECSStationModeHelper( new ECSStationModeHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigECSStationMode::~ConfigECSStationMode()
    {
        try
        {
            delete m_ECSStationModeHelper;
            m_ECSStationModeHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigECSStationMode::isNew()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");

        return m_ECSStationModeHelper->isNew();
    }

    void ConfigECSStationMode::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }


    void ConfigECSStationMode::setStationMode(unsigned long mode)
    {
        FUNCTION_ENTRY("setStationMode");
        
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
        
        char buf1[64] = {0}, buf2[64] = {0};
        sprintf(buf1,"%ul", m_ECSStationModeHelper->getStationMode());
        sprintf(buf2,"%ul", mode);
        updateECSStationModeChanges(STATIONMODE, buf1, buf2);

        m_ECSStationModeHelper->setStationMode(mode);

        FUNCTION_EXIT;
    }

    void ConfigECSStationMode::setLocationKey(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("setLocationKey");
        
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
    
        char buf1[64] = {0}, buf2[64] = {0};
        sprintf(buf1,"%ul", m_ECSStationModeHelper->getLocationKey());
        sprintf(buf2,"%ul", locationKey);
        updateECSStationModeChanges(LOCATIONKEY, buf1, buf2);

        m_ECSStationModeHelper->setLocationKey(locationKey);

        FUNCTION_EXIT;
    }

    
    void ConfigECSStationMode::setOperation(const std::string& operation)
    {
        FUNCTION_ENTRY("setOperation");
        
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
    
        updateECSStationModeChanges(OPERATION, m_ECSStationModeHelper->getOperation(), operation);

        m_ECSStationModeHelper->setOperation(operation);

        FUNCTION_EXIT;
    }


    void ConfigECSStationMode::setTrack(const std::string& track)
    {
        FUNCTION_ENTRY("setTrack");
        
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
    
        updateECSStationModeChanges(TRACK, m_ECSStationModeHelper->getTrack(), track);

        m_ECSStationModeHelper->setTrack(track);

        FUNCTION_EXIT;
    }


    void ConfigECSStationMode::setCondition(const std::string& condition)
    {
        FUNCTION_ENTRY("setCondition");
        
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
    
        updateECSStationModeChanges(CONDITION, m_ECSStationModeHelper->getCondition(), condition);

        m_ECSStationModeHelper->setCondition(condition);

        FUNCTION_EXIT;
    }

    void ConfigECSStationMode::setCongestionLogic(const std::string& congestionLogic)
    {
        FUNCTION_ENTRY("setCongestionLogic");
        
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
    
        updateECSStationModeChanges(CONGESTIONLOGIC, m_ECSStationModeHelper->getCongestionLogic(), congestionLogic);

        m_ECSStationModeHelper->setCongestionLogic(congestionLogic);

        FUNCTION_EXIT;
    }


    std::string ConfigECSStationMode::getName()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The m_ECSStationModeHelper pointer is null.");

        std::ostringstream name;
		name << "ECS Station Mode " << m_ECSStationModeHelper->getKey();

        return name.str();
    }

    unsigned long ConfigECSStationMode::getKey()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
        return m_ECSStationModeHelper->getKey();
    }

    unsigned long ConfigECSStationMode::getStationMode()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
        return m_ECSStationModeHelper->getStationMode();
    }

    unsigned long ConfigECSStationMode::getLocationKey()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
        return m_ECSStationModeHelper->getLocationKey();
    }

    std::string ConfigECSStationMode::getOperation()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
        return m_ECSStationModeHelper->getOperation();
    }    
    
    std::string ConfigECSStationMode::getTrack()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
        return m_ECSStationModeHelper->getTrack();
    }    
    
    std::string ConfigECSStationMode::getCondition()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
        return m_ECSStationModeHelper->getCondition();
    }
    
    std::string ConfigECSStationMode::getCongestionLogic()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
        return m_ECSStationModeHelper->getCongestionLogic();
    }

    time_t ConfigECSStationMode::getDateCreated()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
		return m_ECSStationModeHelper->getDateCreated();
    }


    time_t ConfigECSStationMode::getDateModified()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
		return m_ECSStationModeHelper->getDateModified();
    }
 
	
    void ConfigECSStationMode::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigECSStationMode::invalidate");
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");

        m_ECSStationModeHelper->invalidate();
        m_ECSStationModeChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigECSStationMode::invalidate");
    }


    void ConfigECSStationMode::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigECSStationMode::deleteThisObject");
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");

        // Delete the ConfigECSStationMode from the database
        m_ECSStationModeHelper->deleteECSStationMode(cascade);
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete ECSStationModeHelper pointer m_ECSStationModeHelper");

        delete m_ECSStationModeHelper;
        m_ECSStationModeHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigECSStationMode::deleteThisObject");
    }


    void ConfigECSStationMode::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigECSStationMode::applyChanges");
        TA_ASSERT(m_ECSStationModeHelper != NULL, "applyChanges() has been called after ConfigECSStationMode was deleted");

        m_ECSStationModeHelper->writeECSStationModeData();

        m_ECSStationModeChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigECSStationMode::applyChanges");
    }


    void ConfigECSStationMode::updateECSStationModeChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateECSStationModeChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_ECSStationModeChanges.find(name);

        if (matching != m_ECSStationModeChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_ECSStationModeChanges.erase(matching);
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
            m_ECSStationModeChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    
} // closes TA_Core


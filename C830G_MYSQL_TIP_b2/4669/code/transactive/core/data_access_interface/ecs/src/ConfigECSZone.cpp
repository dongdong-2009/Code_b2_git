/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ecs/src/ConfigECSZone.cpp $
  * @author Nick Jardine
  * @version $Revision: #4 $
  * Last modification: $DateTime: 2013/10/07 08:59:52 $
  * Last modified by: $Author: huangjian $
  * 
  * ConfigECSZone is a concrete implementation of IConfigECSZone, which is in turn an implementation
  * of IECSZone. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for ECSZones.
  */
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/ecs/src/ConfigECSZone.h"
#include "core/data_access_interface/ecs/src/ECSZoneHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/TransactiveException.h"

#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string ECSZONEID          = "ECS Zone Id";
    static const std::string ECSLOCATION        = "ECS Location";
    static const std::string TRACK              = "Track";
    static const std::string OCCENTITYKEY       = "OCC Equipment Entity Key";
    static const std::string STNENTITYKEY       = "STN Equipment Entity Key";
    static const std::string ISIGNORINGCONGESTION = "Is Ignoring Congestion Flag";

    long ConfigECSZone::s_nextAvailableIdentifier = 0;


    ConfigECSZone::ConfigECSZone()
     : m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
		m_ECSZoneHelper = new ECSZoneHelper();
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			TA_THROW( TA_Base_Core::TransactiveException("The ECSZoneHelper pointer is null." ));
		}
    }


    ConfigECSZone::ConfigECSZone(const unsigned long idKey)
     : m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
		m_ECSZoneHelper = new ECSZoneHelper(idKey);
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			TA_THROW( TA_Base_Core::TransactiveException("The ECSZoneHelper pointer is null." ));
		}
    }

    ConfigECSZone::ConfigECSZone( const ConfigECSZone& theConfigECSZone)
     : m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
		m_ECSZoneHelper = new ECSZoneHelper( *(theConfigECSZone.m_ECSZoneHelper));
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			TA_THROW( TA_Base_Core::TransactiveException("The ECSZoneHelper pointer is null." ));
		}
    }

    ConfigECSZone::ConfigECSZone(unsigned long row, TA_Base_Core::IData& data)
     : m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
		m_ECSZoneHelper = new ECSZoneHelper(row, data);
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			TA_THROW( TA_Base_Core::TransactiveException("The ECSZoneHelper pointer is null." ));
		}
    }

    ConfigECSZone::~ConfigECSZone()
    {
        try
        {
            delete m_ECSZoneHelper;
            m_ECSZoneHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigECSZone::isNew()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return false;
		}

        return m_ECSZoneHelper->isNew();
    }

    void ConfigECSZone::setName(const std::string& name)
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "setName() should not be called." );
    }

    void ConfigECSZone::setECSZoneId(int id)
    {
        FUNCTION_ENTRY("setECSZoneId");
        
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return;
		}
        
        char buf1[64], buf2[64];
        sprintf(buf1,"%d", m_ECSZoneHelper->getECSZoneId());
        sprintf(buf2,"%d", id);
        updateECSZoneChanges(ECSZONEID, buf1, buf2);

        m_ECSZoneHelper->setECSZoneId(id);

        FUNCTION_EXIT;
    }

    void ConfigECSZone::setECSLocation(const std::string& ecsLocation)
    {
        FUNCTION_ENTRY("setECSLocation");
        
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return;
		}
    
        updateECSZoneChanges(ECSLOCATION, m_ECSZoneHelper->getECSLocation(), ecsLocation);

        m_ECSZoneHelper->setECSLocation(ecsLocation);

        FUNCTION_EXIT;
    }

    
    void ConfigECSZone::setTrack(const std::string& track)
    {
        FUNCTION_ENTRY("setTrack");
        
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return;
		}
    
        updateECSZoneChanges(TRACK, m_ECSZoneHelper->getTrack(), track);

        m_ECSZoneHelper->setTrack(track);

        FUNCTION_EXIT;
    }

    void ConfigECSZone::setOccEquipmentEntityKey(unsigned long entityKey)
    {
        FUNCTION_ENTRY("setOccEquipmentEntityKey");
        
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return;
		}
    
        char buf1[64], buf2[64];
        sprintf(buf1,"%ul", m_ECSZoneHelper->getOccEquipmentEntityKey());
        sprintf(buf2,"%ul", entityKey);
        updateECSZoneChanges(OCCENTITYKEY, buf1, buf2);

        m_ECSZoneHelper->setOccEquipmentEntityKey(entityKey);

        FUNCTION_EXIT;
    }

    void ConfigECSZone::setStnEquipmentEntityKey(unsigned long entityKey)
    {
        FUNCTION_ENTRY("setStnEquipmentEntityKey");
        
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return;
		}
    
        char buf1[64], buf2[64];
        sprintf(buf1,"%ul", m_ECSZoneHelper->getStnEquipmentEntityKey());
        sprintf(buf2,"%ul", entityKey);
        updateECSZoneChanges(STNENTITYKEY, buf1, buf2);

        m_ECSZoneHelper->setStnEquipmentEntityKey(entityKey);

        FUNCTION_EXIT;
    }

    void ConfigECSZone::setIsIgnoringCongestion(bool isIgnoringCongestion)
    {
        FUNCTION_ENTRY("setIsIgnoringCongestion");
        
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return;
		}
    
        char buf1[64], buf2[64];
        sprintf(buf1,"%s", m_ECSZoneHelper->getIsIgnoringCongestion()?"true":"false");
        sprintf(buf2,"%s", isIgnoringCongestion?"true":"false");
        updateECSZoneChanges(ISIGNORINGCONGESTION, buf1, buf2);

        m_ECSZoneHelper->setIsIgnoringCongestion(isIgnoringCongestion);

        FUNCTION_EXIT;
    }

    int ConfigECSZone::getECSZoneId()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
        return m_ECSZoneHelper->getECSZoneId();
    }

    std::string ConfigECSZone::getECSLocation()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return "";
		}
        return m_ECSZoneHelper->getECSLocation();
    }

    std::string ConfigECSZone::getTrack()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return "";
		}
        return m_ECSZoneHelper->getTrack();
    }

    unsigned long ConfigECSZone::getOccEquipmentEntityKey()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
        return m_ECSZoneHelper->getOccEquipmentEntityKey();
    }

    unsigned long ConfigECSZone::getStnEquipmentEntityKey()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
        return m_ECSZoneHelper->getStnEquipmentEntityKey();
    }

    bool ConfigECSZone::getIsIgnoringCongestion()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return false;
		}
        return m_ECSZoneHelper->getIsIgnoringCongestion();
    }

    std::string ConfigECSZone::getName()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return "";
		}

        std::string name = m_ECSZoneHelper->getECSLocation();
        name += " - " + m_ECSZoneHelper->getTrack();

        return name;
    }

    time_t ConfigECSZone::getDateCreated()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
		return m_ECSZoneHelper->getDateCreated();
    }


    time_t ConfigECSZone::getDateModified()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
		return m_ECSZoneHelper->getDateModified();
    }
 
	
    void ConfigECSZone::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigECSZone::invalidate");
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return;
		}

        m_ECSZoneHelper->invalidate();
        m_ECSZoneChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigECSZone::invalidate");
    }


    void ConfigECSZone::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigECSZone::deleteThisObject");
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return;
		}

        // Delete the ECSZone from the database
        m_ECSZoneHelper->deleteECSZone(cascade);
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete ECSZoneHelper pointer m_ECSZoneHelper");

        delete m_ECSZoneHelper;
        m_ECSZoneHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigECSZone::deleteThisObject");
    }


    void ConfigECSZone::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigECSZone::applyChanges");
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "applyChanges() has been called after ECSZone was deleted" );
			return;
		}

        m_ECSZoneHelper->writeECSZoneData();

        m_ECSZoneChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigECSZone::applyChanges");
    }


    void ConfigECSZone::updateECSZoneChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateECSZoneChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_ECSZoneChanges.find(name);

        if (matching != m_ECSZoneChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_ECSZoneChanges.erase(matching);
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
            m_ECSZoneChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    
} // closes TA_Core


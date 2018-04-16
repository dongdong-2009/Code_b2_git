/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/ConfigAtsMssZone.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * ConfigAtsMssZone is a concrete implementation of IConfigAtsMssZone, which is in turn an implementation
  * of IAtsMssZone. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for AtsMssZones.
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/ats/src/ConfigAtsMssZone.h"
#include "core/data_access_interface/ats/src/AtsMssZoneHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string STATIONID         = "Station Id";
    static const std::string MSSZONE           = "MSS Zone";
    
    long ConfigAtsMssZone::s_nextAvailableIdentifier = 0;


    ConfigAtsMssZone::ConfigAtsMssZone()
     : m_atsMssZoneHelper( new AtsMssZoneHelper() ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }


    ConfigAtsMssZone::ConfigAtsMssZone(const unsigned long idKey)
     : m_atsMssZoneHelper( new AtsMssZoneHelper(idKey) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsMssZone::ConfigAtsMssZone( const ConfigAtsMssZone& theConfigAtsMssZone)
     : m_atsMssZoneHelper( new AtsMssZoneHelper( *(theConfigAtsMssZone.m_atsMssZoneHelper)) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsMssZone::ConfigAtsMssZone(unsigned long row, TA_Base_Core::IData& data)
     : m_atsMssZoneHelper( new AtsMssZoneHelper(row, data)),
       m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
    }

    ConfigAtsMssZone::~ConfigAtsMssZone()
    {
        try
        {
            delete m_atsMssZoneHelper;
            m_atsMssZoneHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigAtsMssZone::isNew()
    {
        TA_ASSERT(m_atsMssZoneHelper != NULL,"The AtsMssZoneHelper pointer is null.");

        return m_atsMssZoneHelper->isNew();
    }

    void ConfigAtsMssZone::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }

    void ConfigAtsMssZone::setStationId(unsigned long id)
    {
        FUNCTION_ENTRY("setStationId");
        
        TA_ASSERT(m_atsMssZoneHelper != NULL,"The AtsMssZoneHelper pointer is null.");
        
		std::ostringstream oldStation, newStation;
		oldStation << m_atsMssZoneHelper->getStationId();
		newStation << id;

        updateAtsMssZoneChanges(STATIONID, oldStation.str().c_str(), newStation.str().c_str());

        m_atsMssZoneHelper->setStationId(id);

        FUNCTION_EXIT;
    }
    
    void ConfigAtsMssZone::setMssZone(const std::string& mssZone)
    {
        FUNCTION_ENTRY("setMssZone");
        
        TA_ASSERT(m_atsMssZoneHelper != NULL,"The AtsMssZoneHelper pointer is null.");
    
        updateAtsMssZoneChanges(MSSZONE, m_atsMssZoneHelper->getMssZone(), mssZone);

        m_atsMssZoneHelper->setMssZone(mssZone);

        FUNCTION_EXIT;
    }

	unsigned long ConfigAtsMssZone::getKey()
	{
        TA_ASSERT(m_atsMssZoneHelper != NULL,"The AtsMssZoneHelper pointer is null.");
        return m_atsMssZoneHelper->getKey();
    }

    unsigned long ConfigAtsMssZone::getStationId()
    {
        TA_ASSERT(m_atsMssZoneHelper != NULL,"The AtsMssZoneHelper pointer is null.");
        return m_atsMssZoneHelper->getStationId();
    }


    std::string ConfigAtsMssZone::getMssZone()
    {
        TA_ASSERT(m_atsMssZoneHelper != NULL,"The AtsMssZoneHelper pointer is null.");
        return m_atsMssZoneHelper->getMssZone();
    }

    std::string ConfigAtsMssZone::getName()
    {
        TA_ASSERT(m_atsMssZoneHelper != NULL,"The AtsMssZoneHelper pointer is null.");

        std::ostringstream name;
		name << m_atsMssZoneHelper->getStationId();
        name << " - ";
		name << m_atsMssZoneHelper->getMssZone();

        return name.str();
    }
 
	
    void ConfigAtsMssZone::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsMssZone::invalidate");
        TA_ASSERT(m_atsMssZoneHelper != NULL,"The AtsMssZoneHelper pointer is null.");

        m_atsMssZoneHelper->invalidate();
        m_atsMssZoneChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsMssZone::invalidate");
    }


    void ConfigAtsMssZone::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsMssZone::deleteThisObject");
        TA_ASSERT(m_atsMssZoneHelper != NULL,"The AtsMssZoneHelper pointer is null.");

        // Delete the AtsMssZone from the database
        m_atsMssZoneHelper->deleteAtsMssZone(cascade);
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete AtsMssZoneHelper pointer m_atsMssZoneHelper");

        delete m_atsMssZoneHelper;
        m_atsMssZoneHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsMssZone::deleteThisObject");
    }


    void ConfigAtsMssZone::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsMssZone::applyChanges");
        TA_ASSERT(m_atsMssZoneHelper != NULL, "applyChanges() has been called after AtsMssZone was deleted");

        m_atsMssZoneHelper->writeAtsMssZoneData();

        m_atsMssZoneChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsMssZone::applyChanges");
    }


    void ConfigAtsMssZone::updateAtsMssZoneChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateAtsMssZoneChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_atsMssZoneChanges.find(name);

        if (matching != m_atsMssZoneChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_atsMssZoneChanges.erase(matching);
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
            m_atsMssZoneChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    
} // closes TA_Core


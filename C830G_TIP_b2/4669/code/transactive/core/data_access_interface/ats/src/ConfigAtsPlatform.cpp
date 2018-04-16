/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/ConfigAtsPlatform.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ConfigAtsPlatform is a concrete implementation of IConfigAtsPlatform, which is in turn an implementation
  * of IAtsPlatform. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for AtsPlatforms.
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/ats/src/ConfigAtsPlatform.h"
#include "core/data_access_interface/ats/src/AtsPlatformHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
	static const std::string PLATFORMID        = "Platform Id";
    static const std::string STATIONID         = "Station Id";
    static const std::string TISPLATFORMID     = "Tis Platform Id";
    static const std::string NAME		       = "Platform Name";
    
    long ConfigAtsPlatform::s_nextAvailableIdentifier = 0;

    ConfigAtsPlatform::ConfigAtsPlatform()
     : m_atsPlatformHelper( new AtsPlatformHelper() ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }


    ConfigAtsPlatform::ConfigAtsPlatform(const unsigned long idKey)
     : m_atsPlatformHelper( new AtsPlatformHelper(idKey) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsPlatform::ConfigAtsPlatform( const ConfigAtsPlatform& theConfigAtsPlatform)
     : m_atsPlatformHelper( new AtsPlatformHelper( *(theConfigAtsPlatform.m_atsPlatformHelper)) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsPlatform::ConfigAtsPlatform(unsigned long row, TA_Base_Core::IData& data)
     : m_atsPlatformHelper( new AtsPlatformHelper(row, data)),
       m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
    }

    ConfigAtsPlatform::~ConfigAtsPlatform()
    {
        try
        {
            delete m_atsPlatformHelper;
            m_atsPlatformHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigAtsPlatform::isNew()
    {
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");

        return m_atsPlatformHelper->isNew();
    }

    void ConfigAtsPlatform::setName(const std::string& name)
    {
        FUNCTION_ENTRY("setName");
        
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");

        updateAtsPlatformChanges(NAME, m_atsPlatformHelper->getName().c_str(), name.c_str());

        m_atsPlatformHelper->setName(name);

        FUNCTION_EXIT;
    }

    void ConfigAtsPlatform::setStationId(unsigned long id)
    {
        FUNCTION_ENTRY("setStationId");
        
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");
        
		std::ostringstream oldStation, newStation;
		oldStation << m_atsPlatformHelper->getStationId();
		newStation << id;

        updateAtsPlatformChanges(STATIONID, oldStation.str().c_str(), newStation.str().c_str());

        m_atsPlatformHelper->setStationId(id);

        FUNCTION_EXIT;
    }
    
    void ConfigAtsPlatform::setPlatformId(unsigned long id)
    {
        FUNCTION_ENTRY("setPlatformId");
        
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");
    
		std::ostringstream oldPlatform, newPlatform;
		oldPlatform << m_atsPlatformHelper->getPlatformId();
		newPlatform << id;

        updateAtsPlatformChanges(PLATFORMID, oldPlatform.str().c_str(), newPlatform.str().c_str());

        m_atsPlatformHelper->setPlatformId(id);

        FUNCTION_EXIT;
    }


    void ConfigAtsPlatform::setTisPlatformId(unsigned char id)
    {
        FUNCTION_ENTRY("setTisPlatformId");
        
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");
    
		std::ostringstream oldPlatform, newPlatform;
		oldPlatform << m_atsPlatformHelper->getPlatformId();
		newPlatform << id;

        updateAtsPlatformChanges(TISPLATFORMID, oldPlatform.str().c_str(), newPlatform.str().c_str());

        m_atsPlatformHelper->setTisPlatformId(id);

        FUNCTION_EXIT;
    }


	unsigned long ConfigAtsPlatform::getKey()
	{
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");
        return m_atsPlatformHelper->getKey();
    }

    unsigned long ConfigAtsPlatform::getStationId()
    {
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");
        return m_atsPlatformHelper->getStationId();
    }


    unsigned long ConfigAtsPlatform::getPlatformId()
    {
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");
        return m_atsPlatformHelper->getPlatformId();
    }

    unsigned char ConfigAtsPlatform::getTisPlatformId()
    {
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");
        return m_atsPlatformHelper->getTisPlatformId();
    }


    std::string ConfigAtsPlatform::getName()
    {
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");
        return m_atsPlatformHelper->getName();
    }
 
	
    void ConfigAtsPlatform::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsPlatform::invalidate");
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");

        m_atsPlatformHelper->invalidate();
        m_atsPlatformChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsPlatform::invalidate");
    }


    void ConfigAtsPlatform::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsPlatform::deleteThisObject");
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");

        // Delete the AtsPlatform from the database
        m_atsPlatformHelper->deleteAtsPlatform(cascade);
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete AtsPlatformHelper pointer m_atsPlatformHelper");

        delete m_atsPlatformHelper;
        m_atsPlatformHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsPlatform::deleteThisObject");
    }


    void ConfigAtsPlatform::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsPlatform::applyChanges");
        TA_ASSERT(m_atsPlatformHelper != NULL, "applyChanges() has been called after AtsPlatform was deleted");

        m_atsPlatformHelper->writeAtsPlatformData();

        m_atsPlatformChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsPlatform::applyChanges");
    }


    void ConfigAtsPlatform::updateAtsPlatformChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateAtsPlatformChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_atsPlatformChanges.find(name);

        if (matching != m_atsPlatformChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_atsPlatformChanges.erase(matching);
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
            m_atsPlatformChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    
} // closes TA_Core


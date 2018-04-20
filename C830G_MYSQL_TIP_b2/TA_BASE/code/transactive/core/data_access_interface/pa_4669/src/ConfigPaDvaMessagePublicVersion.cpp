/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/ConfigPaDvaMessagePublicVersion.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ConfigPaDvaMessagePublicVersion is a concrete implementation of IConfigPaDvaMessagePublicVersion, 
  * which is in turn an implementation
  * of IPaDvaMessagePublicVersion. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaDvaMessagePublicVersions.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/pa_4669/src/ConfigPaDvaMessagePublicVersion.h"
// #include "core/data_access_interface/pa/src/PaDvaMessagePublicVersionHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    long ConfigPaDvaMessagePublicVersion::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string LOCATIONKEY              = "Label";
    static const std::string VERSIONCOLUMN_STRINGS[] =
    {
        "PUBLIC_ADHOC1",
        "PUBLIC_ADHOC2",
        "PUBLIC_ADHOC3",
        "PUBLIC_ADHOC4",
        "PUBLIC_ADHOC5",
        "PUBLIC_ADHOC6",
        "PUBLIC_PRERECORDED",
    };
    
    
    ConfigPaDvaMessagePublicVersion::ConfigPaDvaMessagePublicVersion()
     : m_paDvaMessagePublicVersionHelper( new PaDvaMessagePublicVersionHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaDvaMessagePublicVersion::ConfigPaDvaMessagePublicVersion(unsigned long row, TA_Base_Core::IData& data)
     : m_paDvaMessagePublicVersionHelper(new PaDvaMessagePublicVersionHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaDvaMessagePublicVersion::ConfigPaDvaMessagePublicVersion(const unsigned long key)
     : m_paDvaMessagePublicVersionHelper( new PaDvaMessagePublicVersionHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaDvaMessagePublicVersion::ConfigPaDvaMessagePublicVersion( const ConfigPaDvaMessagePublicVersion& theConfigPaDvaMessagePublicVersion)
     : m_paDvaMessagePublicVersionHelper( new PaDvaMessagePublicVersionHelper( *(theConfigPaDvaMessagePublicVersion.m_paDvaMessagePublicVersionHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {      
    }



    ConfigPaDvaMessagePublicVersion::~ConfigPaDvaMessagePublicVersion()
    {
        try
        {
            delete m_paDvaMessagePublicVersionHelper;
            m_paDvaMessagePublicVersionHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigPaDvaMessagePublicVersion::isNew()
    {
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The PaDvaMessagePublicVersionHelper pointer is null.");

        return m_paDvaMessagePublicVersionHelper->isNew();
    }


    void ConfigPaDvaMessagePublicVersion::setLocationKey(unsigned long key)
    {
        FUNCTION_ENTRY("setLocationKey");
        
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The PaDvaMessagePublicVersionHelper pointer is null.");
    
        updatePaDvaMessagePublicVersionChanges(LOCATIONKEY,
                                isNew() ? 0 : m_paDvaMessagePublicVersionHelper->getLocationKey(), 
                                key);

        m_paDvaMessagePublicVersionHelper->setLocationKey(key);

        FUNCTION_EXIT;
    }


    void ConfigPaDvaMessagePublicVersion::setVersionData(const std::string& newVersion,
                                        IPaDvaMessagePublicVersion::EVersionColumn column)
    {
        FUNCTION_ENTRY("setVersionData");

        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The PaDvaMessagePublicVersionHelper pointer is null.");
    
        updatePaDvaMessagePublicVersionChanges(VERSIONCOLUMN_STRINGS[column],
                                isNew() ? "" : m_paDvaMessagePublicVersionHelper->getVersionData(column), 
                                newVersion);

        m_paDvaMessagePublicVersionHelper->setVersionData(newVersion, column);

        FUNCTION_EXIT;
    }


    unsigned long ConfigPaDvaMessagePublicVersion::getKey()
    {
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The PaDvaMessagePublicVersionHelper pointer is null.");
        return m_paDvaMessagePublicVersionHelper->getKey();
    }


    unsigned long ConfigPaDvaMessagePublicVersion::getLocationKey()
    {
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The PaDvaMessagePublicVersionHelper pointer is null.");
        return m_paDvaMessagePublicVersionHelper->getLocationKey();
    }    
    

    std::string ConfigPaDvaMessagePublicVersion::getMessageVersion(
                                            IPaDvaMessagePublicVersion::ELocation location,
                                            unsigned long stationDvaMessageId)
    {
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The PaDvaMessagePublicVersionHelper pointer is null.");
        return m_paDvaMessagePublicVersionHelper->getMessageVersion(location, stationDvaMessageId);
    }


    std::string ConfigPaDvaMessagePublicVersion::getVersionData(EVersionColumn column)
    {
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The PaDvaMessagePublicVersionHelper pointer is null.");
        return m_paDvaMessagePublicVersionHelper->getVersionData(column);
    }


    time_t ConfigPaDvaMessagePublicVersion::getDateCreated()
    {
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The PaDvaMessagePublicVersionHelper pointer is null.");
		return m_paDvaMessagePublicVersionHelper->getDateCreated();
    }


    time_t ConfigPaDvaMessagePublicVersion::getDateModified()
    {
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The PaDvaMessagePublicVersionHelper pointer is null.");
		return m_paDvaMessagePublicVersionHelper->getDateModified();
    }
 
	
    void ConfigPaDvaMessagePublicVersion::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaDvaMessagePublicVersion::invalidate");
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The PaDvaMessagePublicVersionHelper pointer is null.");

        m_paDvaMessagePublicVersionHelper->invalidate();
        m_paDvaMessagePublicVersionChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaDvaMessagePublicVersion::invalidate");
    }


    void ConfigPaDvaMessagePublicVersion::deleteThisObject()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaDvaMessagePublicVersion::deleteThisObject");
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The PaDvaMessagePublicVersionHelper pointer is null.");

        // Delete the ConfigPaDvaMessagePublicVersion from the database
        m_paDvaMessagePublicVersionHelper->deletePaDvaMessagePublicVersion();
        
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "About to delete PaDvaMessagePublicVersionHelper pointer m_paDvaMessagePublicVersionHelper");

        delete m_paDvaMessagePublicVersionHelper;
        m_paDvaMessagePublicVersionHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaDvaMessagePublicVersion::deleteThisObject");
    }


    void ConfigPaDvaMessagePublicVersion::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaDvaMessagePublicVersion::applyChanges");
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL, "applyChanges() has been called after ConfigPaDvaMessagePublicVersion was deleted");

        m_paDvaMessagePublicVersionHelper->writePaDvaMessagePublicVersionData();

        m_paDvaMessagePublicVersionChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaDvaMessagePublicVersion::applyChanges");
    }


    void ConfigPaDvaMessagePublicVersion::updatePaDvaMessagePublicVersionChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updatePaDvaMessagePublicVersionChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_paDvaMessagePublicVersionChanges.find(name);

        if (matching != m_paDvaMessagePublicVersionChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_paDvaMessagePublicVersionChanges.erase(matching);
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
            m_paDvaMessagePublicVersionChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    void ConfigPaDvaMessagePublicVersion::updatePaDvaMessagePublicVersionChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue)
    {
        FUNCTION_ENTRY("updatePaDvaMessagePublicVersionChanges_ul");
        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updatePaDvaMessagePublicVersionChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
    }
    
    void ConfigPaDvaMessagePublicVersion::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }

    std::string ConfigPaDvaMessagePublicVersion::getName()
    {
        TA_ASSERT(m_paDvaMessagePublicVersionHelper != NULL,"The m_paDvaMessagePublicVersionHelper pointer is null.");

        std::ostringstream name;
		name << "PA DVA Message Version " << m_paDvaMessagePublicVersionHelper->getKey();

        return name.str();
    }     

} // closes TA_Base_Core


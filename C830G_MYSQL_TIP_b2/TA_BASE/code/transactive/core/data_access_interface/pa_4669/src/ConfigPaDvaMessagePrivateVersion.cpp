/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/ConfigPaDvaMessagePrivateVersion.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ConfigPaDvaMessagePrivateVersion is a concrete implementation of IConfigPaDvaMessagePrivateVersion, 
  * which is in turn an implementation
  * of IPaDvaMessagePrivateVersion. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaDvaMessagePrivateVersions.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/pa_4669/src/ConfigPaDvaMessagePrivateVersion.h"
#include "core/data_access_interface/pa_4669/src/PaDvaMessagePrivateVersionHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    long ConfigPaDvaMessagePrivateVersion::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string LOCATIONKEY              = "Label";
    static const std::string VERSIONCOLUMN_STRINGS[] =
    {
        "PRIVATE_ADHOC1",
        "PRIVATE_ADHOC2",
        "PRIVATE_ADHOC3",
        "PRIVATE_ADHOC4",
        "PRIVATE_ADHOC5",
        "PRIVATE_ADHOC6",
        "PRIVATE_PRERECORDED"
    };
    
    
    ConfigPaDvaMessagePrivateVersion::ConfigPaDvaMessagePrivateVersion()
     : m_paDvaMessagePrivateVersionHelper( new PaDvaMessagePrivateVersionHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaDvaMessagePrivateVersion::ConfigPaDvaMessagePrivateVersion(unsigned long row, TA_Base_Core::IData& data)
     : m_paDvaMessagePrivateVersionHelper(new PaDvaMessagePrivateVersionHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaDvaMessagePrivateVersion::ConfigPaDvaMessagePrivateVersion(const unsigned long key)
     : m_paDvaMessagePrivateVersionHelper( new PaDvaMessagePrivateVersionHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaDvaMessagePrivateVersion::ConfigPaDvaMessagePrivateVersion( const ConfigPaDvaMessagePrivateVersion& theConfigPaDvaMessagePrivateVersion)
     : m_paDvaMessagePrivateVersionHelper( new PaDvaMessagePrivateVersionHelper( *(theConfigPaDvaMessagePrivateVersion.m_paDvaMessagePrivateVersionHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {      
    }



    ConfigPaDvaMessagePrivateVersion::~ConfigPaDvaMessagePrivateVersion()
    {
        try
        {
            delete m_paDvaMessagePrivateVersionHelper;
            m_paDvaMessagePrivateVersionHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigPaDvaMessagePrivateVersion::isNew()
    {
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The PaDvaMessagePrivateVersionHelper pointer is null.");

        return m_paDvaMessagePrivateVersionHelper->isNew();
    }


    void ConfigPaDvaMessagePrivateVersion::setLocationKey(unsigned long key)
    {
        FUNCTION_ENTRY("setLocationKey");
        
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The PaDvaMessagePrivateVersionHelper pointer is null.");
    
        updatePaDvaMessagePrivateVersionChanges(LOCATIONKEY,
                                isNew() ? 0 : m_paDvaMessagePrivateVersionHelper->getLocationKey(), 
                                key);

        m_paDvaMessagePrivateVersionHelper->setLocationKey(key);

        FUNCTION_EXIT;
    }


    void ConfigPaDvaMessagePrivateVersion::setVersionData(const std::string& newVersion,
                                        IPaDvaMessagePrivateVersion::EVersionColumn column)
    {
        FUNCTION_ENTRY("setVersionData");

        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The PaDvaMessagePrivateVersionHelper pointer is null.");
    
        updatePaDvaMessagePrivateVersionChanges(VERSIONCOLUMN_STRINGS[column],
                                isNew() ? "" : m_paDvaMessagePrivateVersionHelper->getVersionData(column), 
                                newVersion);

        m_paDvaMessagePrivateVersionHelper->setVersionData(newVersion, column);

        FUNCTION_EXIT;
    }


    unsigned long ConfigPaDvaMessagePrivateVersion::getKey()
    {
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The PaDvaMessagePrivateVersionHelper pointer is null.");
        return m_paDvaMessagePrivateVersionHelper->getKey();
    }


    unsigned long ConfigPaDvaMessagePrivateVersion::getLocationKey()
    {
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The PaDvaMessagePrivateVersionHelper pointer is null.");
        return m_paDvaMessagePrivateVersionHelper->getLocationKey();
    }    
    

    std::string ConfigPaDvaMessagePrivateVersion::getMessageVersion(
                                            IPaDvaMessagePrivateVersion::ELocation location,
                                            unsigned long stationDvaMessageId)
    {
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The PaDvaMessagePrivateVersionHelper pointer is null.");
        return m_paDvaMessagePrivateVersionHelper->getMessageVersion(location, stationDvaMessageId);
    }


    std::string ConfigPaDvaMessagePrivateVersion::getVersionData(EVersionColumn column)
    {
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The PaDvaMessagePrivateVersionHelper pointer is null.");
        return m_paDvaMessagePrivateVersionHelper->getVersionData(column);
    }


    time_t ConfigPaDvaMessagePrivateVersion::getDateCreated()
    {
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The PaDvaMessagePrivateVersionHelper pointer is null.");
		return m_paDvaMessagePrivateVersionHelper->getDateCreated();
    }


    time_t ConfigPaDvaMessagePrivateVersion::getDateModified()
    {
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The PaDvaMessagePrivateVersionHelper pointer is null.");
		return m_paDvaMessagePrivateVersionHelper->getDateModified();
    }
 
	
    void ConfigPaDvaMessagePrivateVersion::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaDvaMessagePrivateVersion::invalidate");
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The PaDvaMessagePrivateVersionHelper pointer is null.");

        m_paDvaMessagePrivateVersionHelper->invalidate();
        m_paDvaMessagePrivateVersionChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaDvaMessagePrivateVersion::invalidate");
    }


    void ConfigPaDvaMessagePrivateVersion::deleteThisObject()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaDvaMessagePrivateVersion::deleteThisObject");
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The PaDvaMessagePrivateVersionHelper pointer is null.");

        // Delete the ConfigPaDvaMessagePrivateVersion from the database
        m_paDvaMessagePrivateVersionHelper->deletePaDvaMessagePrivateVersion();
        
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "About to delete PaDvaMessagePrivateVersionHelper pointer m_paDvaMessagePrivateVersionHelper");

        delete m_paDvaMessagePrivateVersionHelper;
        m_paDvaMessagePrivateVersionHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaDvaMessagePrivateVersion::deleteThisObject");
    }


    void ConfigPaDvaMessagePrivateVersion::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaDvaMessagePrivateVersion::applyChanges");
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL, "applyChanges() has been called after ConfigPaDvaMessagePrivateVersion was deleted");

        m_paDvaMessagePrivateVersionHelper->writePaDvaMessagePrivateVersionData();

        m_paDvaMessagePrivateVersionChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaDvaMessagePrivateVersion::applyChanges");
    }


    void ConfigPaDvaMessagePrivateVersion::updatePaDvaMessagePrivateVersionChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updatePaDvaMessagePrivateVersionChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_paDvaMessagePrivateVersionChanges.find(name);

        if (matching != m_paDvaMessagePrivateVersionChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_paDvaMessagePrivateVersionChanges.erase(matching);
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
            m_paDvaMessagePrivateVersionChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    void ConfigPaDvaMessagePrivateVersion::updatePaDvaMessagePrivateVersionChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue)
    {
        FUNCTION_ENTRY("updatePaDvaMessagePrivateVersionChanges_ul");
        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updatePaDvaMessagePrivateVersionChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
    }
    
    void ConfigPaDvaMessagePrivateVersion::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }

    std::string ConfigPaDvaMessagePrivateVersion::getName()
    {
        TA_ASSERT(m_paDvaMessagePrivateVersionHelper != NULL,"The m_paDvaMessagePrivateVersionHelper pointer is null.");

        std::ostringstream name;
		name << "PA DVA Message Version " << m_paDvaMessagePrivateVersionHelper->getKey();

        return name.str();
    }     

} // closes TA_Base_Core


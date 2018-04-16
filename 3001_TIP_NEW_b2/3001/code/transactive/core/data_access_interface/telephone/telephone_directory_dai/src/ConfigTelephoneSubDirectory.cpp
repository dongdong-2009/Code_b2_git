/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/ConfigTelephoneSubDirectory.cpp $
  * @author:  Wong Soh Sum
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * ConfigTelephoneSubDirectory is a concrete implementation of IConfigTelephoneSubDirectory, 
  * which is in turn an implementation of ITelephoneSubDirectory. 
  * It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for TE_SUBDIRECTORY.
  */
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <iterator>
#include <sstream>

#include "core/data_access_interface/telephone/telephone_directory_dai/src/ConfigTelephoneSubDirectory.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneSubDirectoryHelper.h"

using std::ostringstream;
using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
    long ConfigTelephoneSubDirectory::s_nextAvailableIdentifier = 0;


    ConfigTelephoneSubDirectory::ConfigTelephoneSubDirectory()
     : m_telephoneSubDirectoryHelper( new TelephoneSubDirectoryHelper() ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
        FUNCTION_ENTRY("Default Constructor");
        FUNCTION_EXIT;
    }


    ConfigTelephoneSubDirectory::ConfigTelephoneSubDirectory(const unsigned long idKey)
     : m_telephoneSubDirectoryHelper( new TelephoneSubDirectoryHelper(idKey) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
        FUNCTION_ENTRY("Constructor - idKey");
        FUNCTION_EXIT;
    }


    ConfigTelephoneSubDirectory::ConfigTelephoneSubDirectory( const ConfigTelephoneSubDirectory& theConfigTelephoneSubDirectory)
     : m_telephoneSubDirectoryHelper( new TelephoneSubDirectoryHelper( *(theConfigTelephoneSubDirectory.m_telephoneSubDirectoryHelper)) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
        FUNCTION_ENTRY("Copy Constructor");
        FUNCTION_EXIT;
    }


    ConfigTelephoneSubDirectory::ConfigTelephoneSubDirectory(unsigned long row, TA_Base_Core::IData& data)
     : m_telephoneSubDirectoryHelper( new TelephoneSubDirectoryHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
        FUNCTION_ENTRY("Constructor - IData");
        FUNCTION_EXIT;
    }


    ConfigTelephoneSubDirectory::~ConfigTelephoneSubDirectory()
    {
        FUNCTION_ENTRY("Destructor");
        try
        {
            delete m_telephoneSubDirectoryHelper;
            m_telephoneSubDirectoryHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
        FUNCTION_EXIT;
    }


    bool ConfigTelephoneSubDirectory::isNew()
    {
        FUNCTION_ENTRY("isNew");
        TA_ASSERT(m_telephoneSubDirectoryHelper != NULL,"The TelephoneSubDirectoryHelper pointer is null.");

        FUNCTION_EXIT;
        return m_telephoneSubDirectoryHelper->isNew();
    }


    void ConfigTelephoneSubDirectory::setName(const std::string& name)
    {
        FUNCTION_ENTRY("setName");
        TA_ASSERT(false ,"setName() should not be called.");
        FUNCTION_EXIT;
    }
   
    void ConfigTelephoneSubDirectory::setSubDirName(const std::string& name)
    {
        FUNCTION_ENTRY("setSubDirName");
        TA_ASSERT(m_telephoneSubDirectoryHelper != NULL,"The TelephoneSubDirectoryHelper pointer is null.");
    
        updateSubdirectoryChanges(TelephoneSubDirectoryHelper::SUB_DIR_NAME, m_telephoneSubDirectoryHelper->getSubDirName(), name);
        m_telephoneSubDirectoryHelper->setSubDirName(name);

        FUNCTION_EXIT;
    }

    
    void ConfigTelephoneSubDirectory::setStatus(const int status)
    {
        FUNCTION_ENTRY("setStatus");
        
        TA_ASSERT(m_telephoneSubDirectoryHelper != NULL,"The TelephoneSubDirectoryHelper pointer is null.");
    
        ostringstream oldStatus, newStatus;
        oldStatus << m_telephoneSubDirectoryHelper->getStatus();
        newStatus << status;

        updateSubdirectoryChanges(TelephoneSubDirectoryHelper::SUB_DIR_STATUS, oldStatus.str(), newStatus.str());
        m_telephoneSubDirectoryHelper->setStatus(status);

        FUNCTION_EXIT;
    }


    long ConfigTelephoneSubDirectory::getPrimaryKey()
    {
        FUNCTION_ENTRY("getPrimaryKey");
        TA_ASSERT(m_telephoneSubDirectoryHelper != NULL,"The TelephoneSubDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneSubDirectoryHelper->getPrimaryKey();
    }


    std::string ConfigTelephoneSubDirectory::getSubDirName()
    {
        FUNCTION_ENTRY("string ConfigTelephoneSubDirectory::getSubDirName");
        TA_ASSERT(m_telephoneSubDirectoryHelper != NULL,"The TelephoneSubDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneSubDirectoryHelper->getSubDirName();
    }


    int ConfigTelephoneSubDirectory::getStatus()
    {
        FUNCTION_ENTRY("getStatus");
        TA_ASSERT(m_telephoneSubDirectoryHelper != NULL,"The TelephoneSubDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneSubDirectoryHelper->getStatus();
    }

    std::string ConfigTelephoneSubDirectory::getName()
    {
        FUNCTION_ENTRY("string ConfigTelephoneSubDirectory::getName");
        TA_ASSERT(m_telephoneSubDirectoryHelper != NULL,"The TelephoneSubDirectoryHelper pointer is null.");

        std::ostringstream name;
        name << "TE_SUBDIRECTORY - " << m_telephoneSubDirectoryHelper->getPrimaryKey();
    
        FUNCTION_EXIT;
        return name.str();
    }

    void ConfigTelephoneSubDirectory::invalidate()
    {
        FUNCTION_ENTRY("invalidate");
        TA_ASSERT(m_telephoneSubDirectoryHelper != NULL,"The TelephoneSubDirectoryHelper pointer is null.");

        m_telephoneSubDirectoryHelper->invalidate();
        m_SubDirectoryChanges.clear();

        FUNCTION_EXIT;
    }


    void ConfigTelephoneSubDirectory::deleteThisObject(bool cascade)
    {
        FUNCTION_ENTRY("deleteThisObject");
        TA_ASSERT(m_telephoneSubDirectoryHelper != NULL,"The TelephoneSubDirectoryHelper pointer is null.");

        // Delete the ECSZone from the database
        m_telephoneSubDirectoryHelper->deleteTelephoneSubDir(cascade);
        
        delete m_telephoneSubDirectoryHelper;
        m_telephoneSubDirectoryHelper = NULL;

        FUNCTION_EXIT;
    }


    void ConfigTelephoneSubDirectory::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");
        TA_ASSERT(m_telephoneSubDirectoryHelper != NULL, "applyChanges() has been called after ECSZone was deleted");

        m_telephoneSubDirectoryHelper->writeTelephoneSubDirData();
        m_SubDirectoryChanges.clear();
        
        FUNCTION_EXIT;
    }


    void ConfigTelephoneSubDirectory::updateSubdirectoryChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateSubdirectoryChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_SubDirectoryChanges.find(name);

        if (matching != m_SubDirectoryChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_SubDirectoryChanges.erase(matching);
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
            m_SubDirectoryChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }   
} // closes TA_Core

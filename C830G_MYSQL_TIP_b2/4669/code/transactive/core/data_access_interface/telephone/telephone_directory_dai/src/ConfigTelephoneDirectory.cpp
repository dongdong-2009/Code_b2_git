/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/ConfigTelephoneDirectory.cpp $
  * @author:  Wong Soh Sum
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * ConfigTelephoneDirectory is a concrete implementation of IConfigTelephoneDirectory, 
  * which is in turn an implementation of ITelephoneDirectory. 
  * It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for TelephoneDirectorys.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <iterator>
#include <sstream>

#include "core/data_access_interface/telephone/telephone_directory_dai/src/ConfigTelephoneDirectory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectoryHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using std::ostringstream;
using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
    long ConfigTelephoneDirectory::s_nextAvailableIdentifier = 0;

    ConfigTelephoneDirectory::ConfigTelephoneDirectory()
        : m_telephoneDirectoryHelper( new TelephoneDirectoryHelper() ),
          m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
        FUNCTION_ENTRY("Default Constructor");
        FUNCTION_EXIT;
    }


    ConfigTelephoneDirectory::ConfigTelephoneDirectory(const unsigned long idKey)
     : m_telephoneDirectoryHelper( new TelephoneDirectoryHelper(idKey) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
        FUNCTION_ENTRY("Constructor - idKey");
        FUNCTION_EXIT;
    }

    ConfigTelephoneDirectory::ConfigTelephoneDirectory( const ConfigTelephoneDirectory& theConfigTelephoneDirectory)
     : m_telephoneDirectoryHelper( new TelephoneDirectoryHelper( *(theConfigTelephoneDirectory.m_telephoneDirectoryHelper)) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
        FUNCTION_ENTRY("Copy Constructor");
        FUNCTION_EXIT;
    }

    ConfigTelephoneDirectory::ConfigTelephoneDirectory(unsigned long row, TA_Base_Core::IData& data)
     : m_telephoneDirectoryHelper( new TelephoneDirectoryHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
        FUNCTION_ENTRY("Constructor - IData");
        FUNCTION_EXIT;
    }

    ConfigTelephoneDirectory::~ConfigTelephoneDirectory()
    {
        FUNCTION_ENTRY("Destructor");
        try
        {
            delete m_telephoneDirectoryHelper;
            m_telephoneDirectoryHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
        FUNCTION_EXIT;
    }


    bool ConfigTelephoneDirectory::isNew()
    {
        FUNCTION_ENTRY("isNew");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");

        FUNCTION_EXIT;
        return m_telephoneDirectoryHelper->isNew();
    }

    void ConfigTelephoneDirectory::setName(const std::string& name)
    {
        FUNCTION_ENTRY("setName");
        TA_ASSERT(false ,"setName() should not be called.");
        FUNCTION_EXIT;
    }

//    void ConfigTelephoneDirectory::setPrimaryKey(long id)
//    {
//        FUNCTION_ENTRY("setPrimaryKey");
//        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
//        
//        ostringstream oldId, newId;
//        oldId << m_telephoneDirectoryHelper->getPrimaryKey();
//        newId << id;
//        
//        updateTelephoneDirectoryChanges(TelephoneDirectoryHelper::DIR_PRI_KEY, oldId.str(), newId.str());
//        m_telephoneDirectoryHelper->setPrimaryKey(id);
//
//        FUNCTION_EXIT;
//    }

    void ConfigTelephoneDirectory::setSubDirKey(long id)
    {
        FUNCTION_ENTRY("setSubDirKey");
        
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
        
        ostringstream oldId, newId;
        oldId << m_telephoneDirectoryHelper->getSubDirKey();
        newId << id;

        updateTelephoneDirectoryChanges(TelephoneDirectoryHelper::DIR_SUB_DIR_PRI_KEY, oldId.str(), newId.str());
        m_telephoneDirectoryHelper->setSubDirKey(id);

        FUNCTION_EXIT;
    }

    void ConfigTelephoneDirectory::setTelephoneNum(const std::string& number)
    {
        FUNCTION_ENTRY("setTelephoneNum");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
    
        updateTelephoneDirectoryChanges(TelephoneDirectoryHelper::DIR_TEL_NUM, m_telephoneDirectoryHelper->getTelephoneNum(), number);
        m_telephoneDirectoryHelper->setTelephoneNum(number);

        FUNCTION_EXIT;
    }

    
    void ConfigTelephoneDirectory::setFullName(const std::string& fullName)
    {
        FUNCTION_ENTRY("setFullName");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
    
        updateTelephoneDirectoryChanges(TelephoneDirectoryHelper::DIR_FULL_NAME, m_telephoneDirectoryHelper->getFullName(), fullName);
        m_telephoneDirectoryHelper->setFullName(fullName);

        FUNCTION_EXIT;
    }


    void ConfigTelephoneDirectory::setLocation(const std::string& location)
    {
        FUNCTION_ENTRY("setLocation");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
    
        updateTelephoneDirectoryChanges(TelephoneDirectoryHelper::DIR_LOCATION, m_telephoneDirectoryHelper->getLocation(), location);
        m_telephoneDirectoryHelper->setLocation(location);

        FUNCTION_EXIT;
    }


    void ConfigTelephoneDirectory::setStatus(int status)
    {
        FUNCTION_ENTRY("setStatus");
        
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
    
        ostringstream oldStatus, newStatus;
        oldStatus << m_telephoneDirectoryHelper->getStatus();
        newStatus << status;

        updateTelephoneDirectoryChanges(TelephoneDirectoryHelper::DIR_STATUS, oldStatus.str(), newStatus.str());
        m_telephoneDirectoryHelper->setStatus(status);

        FUNCTION_EXIT;
    }

    unsigned long ConfigTelephoneDirectory::getPrimaryKey()
    {
        FUNCTION_ENTRY("getPrimaryKey");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneDirectoryHelper->getPrimaryKey();
    }

    unsigned long ConfigTelephoneDirectory::getSubDirKey()
    {
        FUNCTION_ENTRY("getSubDirKey");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneDirectoryHelper->getSubDirKey();
    }
    std::string ConfigTelephoneDirectory::getTelephoneNum()
    {
        FUNCTION_ENTRY("string ConfigTelephoneDirectory::getTelephoneNum");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneDirectoryHelper->getTelephoneNum();
    }

    std::string ConfigTelephoneDirectory::getFullName()
    {
        FUNCTION_ENTRY("string ConfigTelephoneDirectory::getFullName");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneDirectoryHelper->getFullName();
    }

    std::string ConfigTelephoneDirectory::getLocation()
    {
        FUNCTION_ENTRY("string ConfigTelephoneDirectory::getLocation");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneDirectoryHelper->getLocation();
    }

    int ConfigTelephoneDirectory::getStatus()
    {
        FUNCTION_ENTRY("getStatus");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneDirectoryHelper->getStatus();
    }

    std::string ConfigTelephoneDirectory::getName()
    {
        FUNCTION_ENTRY("string ConfigTelephoneDirectory::getName");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");

        std::string name = "TE_DIRECTORY_ENTRY - " + m_telephoneDirectoryHelper->getPrimaryKey();

        FUNCTION_EXIT;
        return name;
    }
	
    void ConfigTelephoneDirectory::invalidate()
    {
        FUNCTION_ENTRY("invalidate");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");

        m_telephoneDirectoryHelper->invalidate();
        m_telephoneDirectoryChanges.clear();

        FUNCTION_EXIT;
    }

    ITelephoneSubDirectory* ConfigTelephoneDirectory::getAssociatedSubdirectory()
    {
        FUNCTION_ENTRY("getAssociatedSubdirectory");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneDirectoryHelper->getAssociatedSubdirectory();
    }

    void ConfigTelephoneDirectory::deleteThisObject(bool cascade)
    {
        FUNCTION_ENTRY("deleteThisObject");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");

        // Delete the TelephoneDirectory from the database
        m_telephoneDirectoryHelper->deleteTelephoneDirectoryData();
        
        delete m_telephoneDirectoryHelper;
        m_telephoneDirectoryHelper = NULL;
        FUNCTION_EXIT;
    }


    void ConfigTelephoneDirectory::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");    
        TA_ASSERT(m_telephoneDirectoryHelper != NULL, "applyChanges() has been called after TelephoneDirectory was deleted");

        m_telephoneDirectoryHelper->writeTelephoneDirectoryData();
        m_telephoneDirectoryChanges.clear();

        FUNCTION_EXIT;
    }


    void ConfigTelephoneDirectory::updateTelephoneDirectoryChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateTelephoneDirectoryChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_telephoneDirectoryChanges.find(name);

        if (matching != m_telephoneDirectoryChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_telephoneDirectoryChanges.erase(matching);
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
            m_telephoneDirectoryChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }    
} // closes TA_Core

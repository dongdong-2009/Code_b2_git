/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectory.cpp $
  * @author:  Wong Soh Sum
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
  * 
  * TelephoneDirectory is an implementation of ITelephoneDirectory. It holds the data specific to a
  * TelephoneDirectory entry in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectory.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

    TelephoneDirectory::TelephoneDirectory(const unsigned long key)
    {
        FUNCTION_ENTRY("Constructor - key");
        m_telephoneDirectoryHelper = new TelephoneDirectoryHelper(key);
        FUNCTION_EXIT;
    }

    TelephoneDirectory::TelephoneDirectory(unsigned long row, TA_Base_Core::IData& data)
     : m_telephoneDirectoryHelper( new TelephoneDirectoryHelper(row, data))
    {
        FUNCTION_ENTRY("Constructor - IData");
        FUNCTION_EXIT;
    }


    TelephoneDirectory::~TelephoneDirectory()
	{
        FUNCTION_ENTRY("Destructor");

        delete m_telephoneDirectoryHelper;
        m_telephoneDirectoryHelper=NULL;

        FUNCTION_EXIT;
    }


    unsigned long TelephoneDirectory::getPrimaryKey()
    {
        FUNCTION_ENTRY("getPrimaryKey");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneDirectoryHelper->getPrimaryKey();
    }

    unsigned long TelephoneDirectory::getSubDirKey()
    {
        FUNCTION_ENTRY("getSubDirKey");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneDirectoryHelper->getSubDirKey();
    }


    std::string TelephoneDirectory::getTelephoneNum()
    {
        FUNCTION_ENTRY("string TelephoneDirectory::getTelephoneNum");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneDirectoryHelper->getTelephoneNum();
    }


    std::string TelephoneDirectory::getFullName()
    {
        FUNCTION_ENTRY("string TelephoneDirectory::getFullName");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneDirectoryHelper->getFullName();
    }


    std::string TelephoneDirectory::getLocation()
    {
        FUNCTION_ENTRY("string TelephoneDirectory::getLocation");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneDirectoryHelper->getLocation();
    }


    int TelephoneDirectory::getStatus()
    {
        FUNCTION_ENTRY("getStatus");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneDirectoryHelper->getStatus();
    }


    std::string TelephoneDirectory::getName()
    {
        FUNCTION_ENTRY("string TelephoneDirectory::getName");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");

        std::ostringstream name;
        name << "TE_DIRECTORY_ENTRY - " << m_telephoneDirectoryHelper->getPrimaryKey();

        FUNCTION_EXIT;
        return name.str();
    }
    

    void TelephoneDirectory::invalidate()
    {
        FUNCTION_ENTRY("invalidate");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
    
        m_telephoneDirectoryHelper->invalidate();
        
        FUNCTION_EXIT;
    }

    ITelephoneSubDirectory* TelephoneDirectory::getAssociatedSubdirectory()
    {
        FUNCTION_ENTRY("getAssociatedSubdirectory");
        TA_ASSERT(m_telephoneDirectoryHelper != NULL,"The TelephoneDirectoryHelper pointer is null.");
        FUNCTION_EXIT;
        return m_telephoneDirectoryHelper->getAssociatedSubdirectory();
    }
} // closes TA_Core

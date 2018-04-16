/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneSubDirectory.cpp $
  * @author:  Wong Soh Sum
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * TelephoneSubDirectory is an implementation of ITelephoneSubDirectory. It holds the data specific to an TelephoneSubDirectory entry
  * in the database, and allows read-only access to that data.
  */

#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneSubDirectory.h"

namespace TA_IRS_Core
{

    TelephoneSubDirectory::TelephoneSubDirectory(const unsigned long key)
    {
        FUNCTION_ENTRY("Constructor - key");
        m_TelephoneSubDirectoryHelper = new TelephoneSubDirectoryHelper(key);
        FUNCTION_EXIT;
    }

    TelephoneSubDirectory::TelephoneSubDirectory(unsigned long row, TA_Base_Core::IData& data)
     : m_TelephoneSubDirectoryHelper( new TelephoneSubDirectoryHelper(row, data))
    {
        FUNCTION_ENTRY("Constructor - IData");
        FUNCTION_EXIT;
    }


    TelephoneSubDirectory::~TelephoneSubDirectory() 
	{
        FUNCTION_ENTRY("Destructor");

        delete m_TelephoneSubDirectoryHelper;
        m_TelephoneSubDirectoryHelper=NULL;

        FUNCTION_EXIT;
    }


    long TelephoneSubDirectory::getPrimaryKey()
    {
        FUNCTION_ENTRY("getPrimaryKey");
        TA_ASSERT(m_TelephoneSubDirectoryHelper != NULL,"The TelephoneSubDirectoryHelper pointer is null.");
        
        FUNCTION_EXIT;
        return m_TelephoneSubDirectoryHelper->getPrimaryKey();
    }


    std::string TelephoneSubDirectory::getSubDirName()
    {
        FUNCTION_ENTRY("string TelephoneSubDirectory::getSubDirName");
        TA_ASSERT(m_TelephoneSubDirectoryHelper != NULL,"The TelephoneSubDirectoryHelper pointer is null.");

        FUNCTION_EXIT;
        return m_TelephoneSubDirectoryHelper->getSubDirName();
    }


    int TelephoneSubDirectory::getStatus()
    {
        FUNCTION_ENTRY("getStatus");
        TA_ASSERT(m_TelephoneSubDirectoryHelper != NULL,"The TelephoneSubDirectoryHelper pointer is null.");

        FUNCTION_EXIT;
        return m_TelephoneSubDirectoryHelper->getStatus();
    }


    std::string TelephoneSubDirectory::getName()
    {
        FUNCTION_ENTRY("string TelephoneSubDirectory::getName");
        TA_ASSERT(m_TelephoneSubDirectoryHelper != NULL,"The TelephoneSubDirectoryHelper pointer is null.");

        std::ostringstream name;
        name << "TE_SUBDIRECTORY - " << m_TelephoneSubDirectoryHelper->getPrimaryKey();

        FUNCTION_EXIT;
        return name.str();
    }

    
    void TelephoneSubDirectory::invalidate()
    {
        FUNCTION_ENTRY("invalidate");
        TA_ASSERT(m_TelephoneSubDirectoryHelper != NULL,"The TelephoneSubDirectoryHelper pointer is null.");
        
        m_TelephoneSubDirectoryHelper->invalidate();

        FUNCTION_EXIT;
    }
} // closes TA_Core

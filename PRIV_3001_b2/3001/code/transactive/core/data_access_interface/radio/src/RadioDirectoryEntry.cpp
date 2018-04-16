/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioDirectoryEntry.cpp $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Implementation of the RadioDirectoryEntry class.
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/radio/src/RadioDirectoryEntry.h"


namespace TA_IRS_Core
{
    RadioDirectoryEntry::RadioDirectoryEntry(const unsigned long key)
    :   m_radioDirectoryHelper(key)
    {
        FUNCTION_ENTRY("RadioDirectoryEntry ctor()");
        FUNCTION_EXIT;
    }
    
    RadioDirectoryEntry::RadioDirectoryEntry(const unsigned long row, TA_Base_Core::IData& data)
    :   m_radioDirectoryHelper(row, data)
    {
        FUNCTION_ENTRY("RadioDirectoryEntry ctor(const unsigned long, TA_Base_Core::IData&)");
        FUNCTION_EXIT;
    }

    RadioDirectoryEntry::~RadioDirectoryEntry()
    {
        FUNCTION_ENTRY("~RadioDirectoryEntry");
        FUNCTION_EXIT;
    }

    //
    // IRadioDirectoryEntry methods
    //
    unsigned long RadioDirectoryEntry::getOwner()
    {
        return m_radioDirectoryHelper.getOwner();
    }

    unsigned long RadioDirectoryEntry::getCategory()
    {
        return m_radioDirectoryHelper.getCategory();
    }


    unsigned long RadioDirectoryEntry::getKey()
    {
        return m_radioDirectoryHelper.getKey();
    }

    std::string RadioDirectoryEntry::getName()
    {
        return m_radioDirectoryHelper.getName();
    }

    void RadioDirectoryEntry::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        m_radioDirectoryHelper.invalidate();
        FUNCTION_EXIT;
    }
}

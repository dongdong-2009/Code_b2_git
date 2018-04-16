/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/ConfigRadioDirectoryEntry.cpp $
  * @author:  Anggiono
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#include "core/data_access_interface/radio/src/ConfigRadioDirectoryEntry.h"

namespace TA_IRS_Core
{

    ConfigRadioDirectoryEntry::ConfigRadioDirectoryEntry(const unsigned long key)
    :   m_radioDirectoryHelper(key)
    {
    }

    ConfigRadioDirectoryEntry::ConfigRadioDirectoryEntry(const unsigned long row, TA_Base_Core::IData& data)
    :   m_radioDirectoryHelper(row, data)
    {
    }

    ConfigRadioDirectoryEntry::ConfigRadioDirectoryEntry(unsigned long owner, unsigned long category, std::string name)
    :   m_radioDirectoryHelper(owner, category, name,true)
    {
    }

    ConfigRadioDirectoryEntry::~ConfigRadioDirectoryEntry()
    {
    }


    unsigned long ConfigRadioDirectoryEntry::getKey()
    {
        return m_radioDirectoryHelper.getKey();
    }

    unsigned long ConfigRadioDirectoryEntry::getOwner()
    {
        return m_radioDirectoryHelper.getOwner();
    }

    unsigned long ConfigRadioDirectoryEntry::getCategory()
    {
        return m_radioDirectoryHelper.getCategory();
    }

    std::string ConfigRadioDirectoryEntry::getName()
    {
        return m_radioDirectoryHelper.getName();
    }

    void ConfigRadioDirectoryEntry::invalidate()
    {
        m_radioDirectoryHelper.invalidate();
    }

    void ConfigRadioDirectoryEntry::save()
    {
        m_radioDirectoryHelper.writeDirectoryEntryData();
    }

}

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/ConfigRadioDirectoryMember.cpp $
  * @author:  WU Mintao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Implementation of the RadioDirectoryMember class.
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/radio/src/ConfigRadioDirectoryMember.h"


namespace TA_IRS_Core
{
    ConfigRadioDirectoryMember::ConfigRadioDirectoryMember(const unsigned long key)
    :   m_radioDirectoryHelper(key)
    {

    }
    
    ConfigRadioDirectoryMember::ConfigRadioDirectoryMember(const unsigned long row, TA_Base_Core::IData& data)
    :   m_radioDirectoryHelper(row, data)
    {

    }

    ConfigRadioDirectoryMember::ConfigRadioDirectoryMember(unsigned long parent, char resource_type, unsigned long resource_id)
        :   m_radioDirectoryHelper(parent, resource_type, resource_id)
    {
        
    }

    ConfigRadioDirectoryMember::~ConfigRadioDirectoryMember()
    {

    }

    //
    // IRadioDirectoryMember methods
    //
    unsigned long ConfigRadioDirectoryMember::getParent()
    {
        return m_radioDirectoryHelper.getParent();
    }

    char ConfigRadioDirectoryMember::getResourceType()
    {
        return m_radioDirectoryHelper.getResourceType();
    }

    unsigned long ConfigRadioDirectoryMember::getResourceId()
    {
        return m_radioDirectoryHelper.getResourceId();
    }


    unsigned long ConfigRadioDirectoryMember::getKey()
    {
        return m_radioDirectoryHelper.getKey();
    }


    void ConfigRadioDirectoryMember::invalidate()
    {
        m_radioDirectoryHelper.invalidate();
    }

    void ConfigRadioDirectoryMember::save()
    {
        m_radioDirectoryHelper.writeData();
    }

}

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioDirectoryMember.cpp $
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

#include "core/data_access_interface/radio/src/RadioDirectoryMember.h"


namespace TA_IRS_Core
{
    RadioDirectoryMember::RadioDirectoryMember(const unsigned long key)
    :   m_radioDirectoryHelper(key)
    {
        FUNCTION_ENTRY("RadioDirectoryMember ctor()");
        FUNCTION_EXIT;
    }
    
    RadioDirectoryMember::RadioDirectoryMember(const unsigned long row, TA_Base_Core::IData& data)
    :   m_radioDirectoryHelper(row, data)
    {
        FUNCTION_ENTRY("RadioDirectoryMember ctor(const unsigned long, TA_Base_Core::IData&)");
        FUNCTION_EXIT;
    }

    RadioDirectoryMember::~RadioDirectoryMember()
    {
        FUNCTION_ENTRY("~RadioDirectoryMember");
        FUNCTION_EXIT;
    }

    //
    // IRadioDirectoryMember methods
    //
    unsigned long RadioDirectoryMember::getParent()
    {
        FUNCTION_ENTRY("getParent");

        FUNCTION_EXIT;
        return m_radioDirectoryHelper.getParent();
    }

    char RadioDirectoryMember::getResourceType()
    {
        FUNCTION_ENTRY("getResourceType");

        FUNCTION_EXIT;
        return m_radioDirectoryHelper.getResourceType();
    }

    unsigned long RadioDirectoryMember::getResourceId()
    {
        FUNCTION_ENTRY("getResourceId");

        FUNCTION_EXIT;
        return m_radioDirectoryHelper.getResourceId();
    }


    unsigned long RadioDirectoryMember::getKey()
    {
        FUNCTION_ENTRY("getKey");

        FUNCTION_EXIT;
        return m_radioDirectoryHelper.getKey();
    }


    void RadioDirectoryMember::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        m_radioDirectoryHelper.invalidate();
        FUNCTION_EXIT;
    }
}

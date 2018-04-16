/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioBtnCfg.cpp $
  * @author:  Mark See
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/radio/src/RadioBtnCfg.h"

namespace TA_IRS_Core
{
    RadioBtnCfg::RadioBtnCfg(const unsigned long key)
        :m_helper(key)
    {
        FUNCTION_ENTRY("RadioBtnCfg Constructor");
        FUNCTION_EXIT;    
    }
        
    RadioBtnCfg::RadioBtnCfg(const unsigned long row, TA_Base_Core::IData& data)
        :m_helper(row, data)
    {
        FUNCTION_ENTRY("RadioBtnCfg(const unsigned long row, TA_Base_Core::IData& data)");
        FUNCTION_EXIT;
    }
        
    RadioBtnCfg::~RadioBtnCfg()
    {    
        FUNCTION_ENTRY("~RadioBtnCfg");
        FUNCTION_EXIT;
    }

    unsigned long RadioBtnCfg::getKey()
    {
        FUNCTION_ENTRY("getKey");
        FUNCTION_EXIT;    
        return m_helper.getKey();
    }

    unsigned long RadioBtnCfg::getOperatorProfile()
    {    
        FUNCTION_ENTRY("getOperatorProfile");
        FUNCTION_EXIT;
        return m_helper.getOperatorProfile();
    }

    bool RadioBtnCfg::isProfile()
    {
        return m_helper.isProfile();
    }

    unsigned long RadioBtnCfg::getButtonRef()
    {
        FUNCTION_ENTRY("getButtonRef");
        FUNCTION_EXIT;
        return m_helper.getButtonRef();    
    }

    char RadioBtnCfg::getResourceType()    
    {
        FUNCTION_ENTRY("getResourceType");
        FUNCTION_EXIT;
        return m_helper.getResourceType();    
    }

    unsigned long RadioBtnCfg::getResourceId()
    {
        FUNCTION_ENTRY("getResourceId");
        FUNCTION_EXIT;
        return m_helper.getResourceId();
    }


    void RadioBtnCfg::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        m_helper.invalidate();
        
        FUNCTION_EXIT;
    }
}

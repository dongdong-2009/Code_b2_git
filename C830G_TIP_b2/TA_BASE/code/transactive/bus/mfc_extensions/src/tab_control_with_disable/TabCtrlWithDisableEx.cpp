/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mfc_extensions/src/tab_control_with_disable/TabCtrlWithDisableEx.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * CTabCtrlWithDisableEx manages a tab control that allows the client code
  *     to disable specific tab indexes using the function EnableTabIndex
  * (extension of CTabCtrlWithDisable)
  *
  */

#pragma warning (disable : 4786)

#include "../StdAfx.h"
#include "bus/mfc_extensions/src/tab_control_with_disable/TabCtrlWithDisableEx.h"

namespace TA_Base_Bus
{

    BOOL CTabCtrlWithDisableEx::IsTabEnabled(int iTab)
    {
        std::map<int, bool>::iterator itr = m_enabledMap.find(iTab);
        if (itr == m_enabledMap.end())
        {
            // By default, any tab without an explicit disable call is enabled
            return true;
        }
        else
        {
            return itr->second;
        }
    }
    
    
    void CTabCtrlWithDisableEx::EnableTabIndex(int iTab, bool enable)
    {
        // Add in (or modify existing) map item
        m_enabledMap[iTab] = enable;
    }

} // namespace TA_Base_Bus

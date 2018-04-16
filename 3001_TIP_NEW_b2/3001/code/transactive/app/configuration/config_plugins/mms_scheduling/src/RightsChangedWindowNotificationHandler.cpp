/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/mms_scheduling/src/RightsChangedWindowNotificationHandler.cpp $
  * @author Jade Lee
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Helper class to handle posting of notifications (WM_PA_RIGHTS_CHANGE) to windows
  * when there is a change in rights
  *
  */

#pragma warning ( disable : 4786 )

#include "StdAfx.h"
#include "RightsChangedWindowNotificationHandler.h"
#include "CachedConfig.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{

    RightsChangedWindowNotificationHandler::RightsChangedWindowNotificationHandler(HWND hWnd)
    :
    m_notificationWnd(hWnd)
    {
        CachedConfig::getInstance().registerRightsChangeListener(*this);
    }
    
    
    RightsChangedWindowNotificationHandler::~RightsChangedWindowNotificationHandler()
    {
        // This is not a safe condition, we were still posting notifications to this window
        TA_ASSERT(::IsWindow(m_notificationWnd), "Window no longer valid while in destructor");
    
        CachedConfig::getInstance().deRegisterRightsChangeListener(*this);
    }
    
    
    void RightsChangedWindowNotificationHandler::onRightsChanged()
    {
        TA_ASSERT(::IsWindow(m_notificationWnd), "Window no longer valid");
    
        TA_Base_Core::ThreadGuard sync(m_lock);
    
        ::PostMessage(m_notificationWnd, WM_MMS_RIGHTS_CHANGE, 0, 0);
    }

} // end namespace TA_IRS_App
    

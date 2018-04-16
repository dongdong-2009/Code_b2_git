/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_monitor_gui/src/RightsChangedWindowNotificationHandler.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Helper class to handle posting of notifications to windows
  * when a cache map element changes
  * Links in with CachedMap class by registering this class
  * as an observer, it posts notifications (using key as lParam)
  * whenever an update received
  *
  */

#include "app/radio/radio_monitor_gui/src/StdAfx.h"
#include "app/radio/radio_monitor_gui/src/RightsChangedWindowNotificationHandler.h"
#include "core/synchronisation/src/ThreadGuard.h"

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
    
        ::PostMessage(m_notificationWnd, WM_RM_RIGHTS_CHANGE, 0, 0);
    }

} // end namespace TA_IRS_App
    

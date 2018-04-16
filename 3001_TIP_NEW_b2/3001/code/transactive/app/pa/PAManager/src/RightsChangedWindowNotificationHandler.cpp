/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/RightsChangedWindowNotificationHandler.cpp $
  * @author Jade Lee
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/04/19 10:24:26 $
  * Last modified by: $Author: qi.huang $
  * 
  * Helper class to handle posting of notifications to windows
  * when a cache map element changes
  * Links in with CachedMap class by registering this class
  * as an observer, it posts notifications (using key as lParam)
  * whenever an update received
  *
  */

#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/RightsChangedWindowNotificationHandler.h"
#include "app/pa/PAManager/src/PaRightsManager.h"
#include "core/synchronisation/src/ThreadGuard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

RightsChangedWindowNotificationHandler::RightsChangedWindowNotificationHandler(HWND hWnd)
:
m_notificationWnd(hWnd)
{
    PaRightsManager::getInstance().registerRightsChangeListener(*this);
}


RightsChangedWindowNotificationHandler::~RightsChangedWindowNotificationHandler()
{
    PaRightsManager::getInstance().deRegisterRightsChangeListener(*this);
}


void RightsChangedWindowNotificationHandler::onRightsChanged(const std::vector<unsigned long>& changedRightsGlobalIds)
{
    if ( !::IsWindow(m_notificationWnd) )
    {
        return;
    }

    TA_Base_Core::ThreadGuard sync(m_lock);

    ::PostMessage(m_notificationWnd, WM_PA_RIGHTS_CHANGE, 0, 0);
}

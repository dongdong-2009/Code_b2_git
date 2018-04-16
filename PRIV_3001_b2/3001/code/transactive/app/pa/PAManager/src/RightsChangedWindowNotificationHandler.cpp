/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/RightsChangedWindowNotificationHandler.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
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

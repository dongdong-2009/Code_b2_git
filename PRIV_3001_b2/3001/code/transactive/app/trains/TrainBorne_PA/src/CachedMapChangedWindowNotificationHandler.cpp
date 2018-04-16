/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/CachedMapChangedWindowNotificationHandler.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * Helper class to handle posting of notifications to windows
  * when a cache map element changes
  * Links in with CachedMap class by registering this class
  * as an observer, it posts notifications (using key as lParam)
  * whenever an update received
  *
  */

#include "app/trains/TrainBorne_PA/src/StdAfx.h"
#include "app/trains/TrainBorne_PA/src/CachedMapChangedWindowNotificationHandler.h"
#include "bus/pa/cachedmaps/src/ICachedMapObserverRegistry.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_IRS_Bus;


CachedMapChangedWindowNotificationHandler::CachedMapChangedWindowNotificationHandler
(
 HWND hWnd, 
 DWORD msgId,
 ICachedMapObserverRegistry& registry
)
:
m_notificationWnd(hWnd),
m_wmMessage(msgId),
m_observerRegistry(registry),
m_acknowledgeMentsRequired(0),
m_mustAcknowledgeMessagesBeforePosting(false)
{
    m_observerRegistry.registerObserver(this);
}


CachedMapChangedWindowNotificationHandler::~CachedMapChangedWindowNotificationHandler()
{
    // This is not a safe condition, we were still posting notifications to this window
    TA_ASSERT(::IsWindow(m_notificationWnd), "Window no longer valid while in destructor");

    m_observerRegistry.deRegisterObserver(this);
}


void CachedMapChangedWindowNotificationHandler::onCachedMapChanged(unsigned long pKey, bool changesMade)
{
    TA_ASSERT(::IsWindow(m_notificationWnd), "Window no longer valid");

    TA_Base_Core::ThreadGuard sync(m_lock);

    if (m_mustAcknowledgeMessagesBeforePosting)
    {
        if (m_acknowledgeMentsRequired > 0)
        {
            // No need to post out a message, still waiting for acknowledgement
            // of the last
            return;
        }
        // Otherwise increment counter before sending
        m_acknowledgeMentsRequired ++;
    }

    ::PostMessage(m_notificationWnd, m_wmMessage, pKey, (changesMade ? 1 : 0));
}


void CachedMapChangedWindowNotificationHandler::setMustAcknowledgeBeforeResendFlag(bool mustAcknowledge)
{
    TA_Base_Core::ThreadGuard sync(m_lock);

    m_mustAcknowledgeMessagesBeforePosting = mustAcknowledge;
}


void CachedMapChangedWindowNotificationHandler::acknowledgeMessage()
{
    TA_Base_Core::ThreadGuard sync(m_lock);

    TA_ASSERT(m_mustAcknowledgeMessagesBeforePosting, "Calling in invalid context");    

    TA_ASSERT(m_acknowledgeMentsRequired > 0, "No acknowledgement expected");

    m_acknowledgeMentsRequired --;
}

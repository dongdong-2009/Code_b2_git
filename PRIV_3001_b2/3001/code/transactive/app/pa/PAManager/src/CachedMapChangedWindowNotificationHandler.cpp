/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/CachedMapChangedWindowNotificationHandler.cpp $
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
#include "app/pa/PAManager/src/CachedMapChangedWindowNotificationHandler.h"
#include "bus/pa/cachedmaps/src/ICachedMapObserverRegistry.h"
#include "core/synchronisation/src/ThreadGuard.h"

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
    m_observerRegistry.deRegisterObserver(this);
}


void CachedMapChangedWindowNotificationHandler::onCachedMapChanged(unsigned long pKey, bool changesMade)
{
    if ( !::IsWindow(m_notificationWnd))
    {
        return;
    }

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

	if ( !m_mustAcknowledgeMessagesBeforePosting)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Calling in invalid context" );
		return ;
	}

	if ( m_acknowledgeMentsRequired <= 0)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No acknowledgement expected" );
		return ;
	}

    m_acknowledgeMentsRequired --;
}

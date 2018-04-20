/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/duty_monitor_framework/src/ISessionObserver.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class should be implemented by clients who want to be notified when  
  * SessionCache contents change due to operator login, logout and override events.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"

using namespace TA_Base_Bus;


ISessionObserver::ISessionObserver(SessionCache& sessionCache) :
    SeObserver<SessionCache>(sessionCache)
{
    FUNCTION_ENTRY("ISessionObserver(SessionCache& sessionCache)");
    FUNCTION_EXIT;
}


ISessionObserver::~ISessionObserver()
{
    FUNCTION_ENTRY("~ISessionObserver()");
    FUNCTION_EXIT;
}


void ISessionObserver::update(SessionCache& sessionCache)
{
    FUNCTION_ENTRY("update(SessionCache& sessionCache)");

    const SessionUpdate* sessionUpdate = sessionCache.getSessionUpdate();

    if (sessionUpdate != NULL)
    {
        const SessionUpdate sessionUpdateCopy(*sessionUpdate);

        sessionChanged(sessionUpdateCopy);
    }

    FUNCTION_EXIT;
}

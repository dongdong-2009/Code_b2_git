/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/PASEvent.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "app/pa/PAAgent/src/PASEvent.h"

namespace TA_IRS_App
{

    PASEvent::PASEvent( TA_IRS_Bus::Scheduler& socketScheduler,
                        TA_IRS_Bus::Scheduler& processScheduler,
                        IPasEventSource& eventSource )
                        : m_socketScheduler(socketScheduler),
                          m_processScheduler(processScheduler),
                          m_pasEventSource(eventSource)
    {
    }

    PASEvent::PASEvent( ACE_Time_Value timeToExpire,
                        TA_IRS_Bus::Scheduler& socketScheduler,
                        TA_IRS_Bus::Scheduler& processScheduler,
                        IPasEventSource& eventSource )
                        : IEvent(timeToExpire),
                          m_socketScheduler(socketScheduler),
                          m_processScheduler(processScheduler),
                          m_pasEventSource(eventSource)
    {
    }

    PASEvent::~PASEvent()
    {
    }

    void PASEvent::cancel()
    {
        delete this;
    }
}

/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/PASEvent.h $
 * @author  J. Dalin
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2008/11/28 16:26:01 $
 * Last modified by : $Author: builder $
 *
 * 
 */

#ifndef PASEVENT_H
#define PASEVENT_H

#include "bus/event/src/Scheduler.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_IRS_App
{
    class IPasEventSource;

    class PASEvent : public TA_IRS_Bus::IEvent
    {
    public:
        PASEvent( TA_IRS_Bus::Scheduler& socketScheduler,
                  TA_IRS_Bus::Scheduler& processScheduler,
                  IPasEventSource& eventSource );
        PASEvent( ACE_Time_Value timeToExpire,
                  TA_IRS_Bus::Scheduler& socketScheduler,
                  TA_IRS_Bus::Scheduler& processScheduler,
                  IPasEventSource& eventSource );

        virtual ~PASEvent();
        virtual void cancel();

    protected:
        /** The scheduler responsible for socket bound events. */
        TA_IRS_Bus::Scheduler& m_socketScheduler;
        /** The scheduler responsible for process events. */
        TA_IRS_Bus::Scheduler& m_processScheduler;
        /** The source of the ConnectToPAFEPC event instance. */
        IPasEventSource& m_pasEventSource;
    };

} // namespace TA_IRS_App

#endif

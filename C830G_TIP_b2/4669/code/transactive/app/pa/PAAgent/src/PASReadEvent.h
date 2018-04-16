/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/PASReadEvent.h $
 * @author  J. Dalin
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2008/11/28 16:26:01 $
 * Last modified by : $Author: builder $
 *
 * 
 */

#ifndef PASREADEVENT_H
#define PASREADEVENT_H

#include "app/pa/PAAgent/src/PASEvent.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_IRS_App
{
    class PASReadEvent : public PASEvent
    {
    public:
        PASReadEvent( TA_IRS_Bus::Scheduler& socketScheduler,
                      TA_IRS_Bus::Scheduler& processScheduler,
                      IPasEventSource& eventSource );
        PASReadEvent( ACE_Time_Value timeToExpire,
                      TA_IRS_Bus::Scheduler& socketScheduler,
                      TA_IRS_Bus::Scheduler& processScheduler,
                      IPasEventSource& eventSource );
        virtual ~PASReadEvent();
        virtual void consume();

        /** This function is called by the consume function and should
          * be overridden in all subclasses to perform a write to a table.
          * Subclasses are assured that they are the only ones reading from (or writing)
          * the PASConnection class at the point this function is executing.
          */
        virtual void readTable() = 0;
    };

} // namespace TA_IRS_App

#endif

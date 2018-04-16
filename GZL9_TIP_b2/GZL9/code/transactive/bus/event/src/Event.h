/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/GZL6_TIP/3002/transactive/bus/event/src/Event.h $
 * @author  K. Yip
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2012/06/12 13:35:44 $
 * Last modified by : $Author: builder $
 *
 * 
 */

#ifndef EVENT_H
#define EVENT_H

#include <ace/Time_Value.h> 
#include <ace/ACE.h> 

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_IRS_Bus
{
    class IEvent
    {
    public:
        IEvent();
        IEvent( ACE_Time_Value timeToExpire );
        virtual ~IEvent();
        virtual ACE_Time_Value getTimeToExpire() const;
        virtual void consume() = 0;
        virtual void cancel() = 0;
        bool operator < (IEvent& right);
        bool operator > (IEvent& right);
        const ACE_Time_Value m_timeToExpire;
    };

    struct IEventComparison
    {
        bool operator () ( IEvent* e1, IEvent* e2 );
    };
} // namespace TA_IRS_Bus

#endif

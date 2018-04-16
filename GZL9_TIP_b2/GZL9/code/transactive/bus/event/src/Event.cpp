/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/GZL6_TIP/3002/transactive/bus/event/src/Event.cpp $
 * @author  K. Yip
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2012/06/12 13:35:44 $
 * Last modified by : $Author: builder $
 *
 * 
 */

#include "Event.h"
#include <ace/OS.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_IRS_Bus
{
IEvent::IEvent()
    : m_timeToExpire(ACE_OS::gettimeofday())
{
};
IEvent::IEvent( ACE_Time_Value timeToExpire )
    : m_timeToExpire( timeToExpire )
{
};
IEvent::~IEvent()
{
};
ACE_Time_Value IEvent::getTimeToExpire() const
{
    return this->m_timeToExpire;
};
bool IEvent::operator < (IEvent& right)
{
    if( this->m_timeToExpire <= right.m_timeToExpire )
        return false;
    else
        return true;
};
bool IEvent::operator > (IEvent& right)
{
    if( this->m_timeToExpire >= right.m_timeToExpire )
        return true;
    else
        return false;
};

bool IEventComparison::operator () ( IEvent* e1, IEvent* e2 )
{
    return (*e1) > (*e2);
}
} // namespace TA_IRS_Bus


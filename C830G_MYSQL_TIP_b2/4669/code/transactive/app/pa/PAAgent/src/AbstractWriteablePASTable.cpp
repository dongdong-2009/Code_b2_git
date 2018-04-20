/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/AbstractWriteablePASTable.cpp $
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

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "app/pa/PAAgent/src/AbstractWriteablePASTable.h"
#include "app/pa/PAAgent/src/PASWriteEvent.h"
#include "app/pa/PAAgent/src/CachedConfig.h"

#include <string.h>

namespace TA_IRS_App
{
    AbstractWriteablePASTable::AbstractWriteablePASTable(unsigned long tableNumber,
                                                         unsigned int bufferSize)
    : AbstractPASTable(tableNumber, bufferSize)
    {
    }

    void AbstractWriteablePASTable::destroyCurrentWriteEvents()
    {
        TA_Base_Core::ThreadGuard guard(m_writeEventsLock);

        // Delete all the write events and clear the vector
        std::vector<PASWriteEvent*>::iterator itr;
        
        for (itr = m_currentWriteEvents.begin(); itr != m_currentWriteEvents.end(); itr++)
        {
            PASWriteEvent* event = *itr;
            delete event;
        }

        this->m_currentWriteEvents.clear();
    }

    void AbstractWriteablePASTable::repostCurrentWriteEvents(TA_IRS_Bus::Scheduler& socketScheduler)
    {
        TA_Base_Core::ThreadGuard guard(m_writeEventsLock);

        // Repost all the current write events with an new expiry time into the scheduler.
        // This means re-creating all the events as expiry time can only be set at
        // point of creation.
        std::vector<PASWriteEvent*>::iterator itr;
        std::vector<PASWriteEvent*> newEvents;
        
        ACE_Time_Value tv( 0, CachedConfig::getInstance()->getWorkerThreadsSleepTimeInMSecs() * 1000 );
        tv += ACE_OS::gettimeofday();

        for (itr = m_currentWriteEvents.begin(); itr != m_currentWriteEvents.end(); itr++)
        {
            PASWriteEvent* event = *itr;
            PASWriteEvent* newEvent = event->recreateEvent(tv);
            newEvents.push_back(newEvent);
            delete event;
            socketScheduler.post(newEvent);
        }

        m_currentWriteEvents = newEvents;
    }

    bool AbstractWriteablePASTable::addWriteEvent(PASWriteEvent& event)
    {
        TA_Base_Core::ThreadGuard guard(m_writeEventsLock);

        // add a write event to the list of current write events unless
        // it already exists in the list.
        std::vector<PASWriteEvent*>::iterator itr =
            std::find(this->m_currentWriteEvents.begin(), this->m_currentWriteEvents.end(), &event);

        if (itr == this->m_currentWriteEvents.end())
        {
            this->m_currentWriteEvents.push_back(&event);
            return true;
        }

        return false;
    }

    bool AbstractWriteablePASTable::removeWriteEvent(PASWriteEvent& event)
    {
        TA_Base_Core::ThreadGuard guard(m_writeEventsLock);

        // Remove the event from the current write event list unless it does
        // not exist in the list.
        std::vector<PASWriteEvent*>::iterator itr =
            std::find(this->m_currentWriteEvents.begin(), this->m_currentWriteEvents.end(), &event);

        if (itr != this->m_currentWriteEvents.end())
        {
            PASWriteEvent* event = *itr;
            this->m_currentWriteEvents.erase(itr);
            delete event;
            return true;
        }

        return false;
    }
}

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/GZL6_TIP/3002/transactive/bus/event/src/Scheduler.h $
  * @author  K. Yip
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2012/06/12 13:35:44 $
  * Last modified by : $Author: builder $
  *
  * 
  */

#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <vector>
#include <queue>
#include <deque>

#include "Event.h"

#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/TAAssert.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_IRS_Bus
{
    /**
     * A simple timed event loop that will consume events.
     *
     * An internal queue of IEvent pointers will be maintained.  If this is
     * empty, the Scheduler class will wait on a condition.  When it is
     * not empty, the Scheduler will consume them until the vector is
     * empty.
     *
     * This is meant to be a VERY simple event consumer.
     */
    class Scheduler : public TA_Base_Core::Thread
    {
    public:

        /** Constructs an instance of this class. */
        Scheduler();
        /** Destructs an instance of this class. */
        virtual ~Scheduler();
        /** Event loop. */
        virtual void run();
        /** Terminates the Scheduler. */
        virtual void terminate();
        /** Posts an event to this Scheduler. */
        virtual void post( IEvent* event );
        /** Removes an event from this Scheduler. */
        virtual void remove( IEvent* event );
        /** Clears all events from the Scheduler. */
        virtual void clear();
    private:
        /** The terminate flag. */
        volatile bool m_terminate;
        /** Thread lock to protect m_events from concurrent access. */
        mutable TA_Base_Core::ReEntrantThreadLockable m_eventsLock;
        /** Deque of IEvent pointers. */
        std::priority_queue<IEvent*,std::vector<IEvent*>, IEventComparison> m_events;
        /** The object to wait on. */
        TA_Base_Core::Condition m_waitObject;
        /** The current event being waited on. */
        IEvent* m_currentEvent;
        /**
         * Flag indicating whether to remove the current event.
         */
        volatile bool m_removeCurrentEvent;
    };

}

#endif

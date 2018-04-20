/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/bus/event/src/Scheduler.cpp $
  * @author  K. Yip
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * 
  */

#include "Scheduler.h"

#include "core/utilities/src/DebugUtil.h"
#include "ace/OS_NS_sys_time.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace TA_Base_Core;

namespace TA_IRS_Bus
{
/** Constructs an instance of this class. */
Scheduler::Scheduler()
    : TA_Base_Core::Thread()
    , m_terminate( false )
    , m_eventsLock()
    , m_events()
    , m_waitObject()
    , m_currentEvent( NULL )
    , m_removeCurrentEvent( false )
{
};
/** Destructs an instance of this class. */
Scheduler::~Scheduler()
{
};

/** Event loop. */
void Scheduler::run()
{
    while( !this->m_terminate )
    {
        unsigned int eventSize = 0;
        {
            TA_Base_Core::ThreadGuard guard( this->m_eventsLock );
            eventSize = this->m_events.size();
        }

        if( 0 == eventSize )
        {
            LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugDebug, "No events.");
            //Maochun Sun++
			//TD13408
            //this->m_waitObject.wait();
			this->m_waitObject.timedWait(2000);
			//++Maochun Sun
			//TD13408
            TA_Base_Core::ThreadGuard guard( this->m_eventsLock );
            continue;
        }

        {
            TA_Base_Core::ThreadGuard guard( this->m_eventsLock );

            if( 0 == this->m_events.size() )
                continue; 

            this->m_currentEvent = this->m_events.top();
            this->m_events.pop();
            this->m_removeCurrentEvent = false;
        }

        const ACE_Time_Value startTime = ACE_OS::gettimeofday();

        if( this->m_currentEvent->getTimeToExpire() <= startTime )
        {
            LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugDebug, "Consuming event.");
            try
            {
                this->m_currentEvent->consume();
            }
            catch( ... )
            {
                LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Uncaught exception.");

                // If we have an uncaught exception, the event will not have had
                // a chance to delete itself.
                delete this->m_currentEvent;
            }
            
            TA_Base_Core::ThreadGuard guard( this->m_eventsLock );
            this->m_currentEvent = NULL;
            continue;
        }

        const ACE_Time_Value waitPeriod =
            this->m_currentEvent->getTimeToExpire() - startTime;

        // Wait for the time period.
        // If the condition returns a 0, the time has elapsed
        // otherwise the condition has been signalled.
        if( 0 == this->m_waitObject.timedWait( waitPeriod.msec() ) )
        {
            LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugDebug, "Consuming event.");
            try
            {
                this->m_currentEvent->consume();
            }
            catch( ... )
            {
                LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Uncaught exception.");

                // If we have an uncaught exception, the event will not have had
                // a chance to delete itself.
                delete this->m_currentEvent;
            }
            
            TA_Base_Core::ThreadGuard guard( this->m_eventsLock );
            this->m_currentEvent = NULL;
        }
        else
        {
            // Re add it to the priority queue
            TA_Base_Core::ThreadGuard guard( this->m_eventsLock );
            if( !this->m_removeCurrentEvent )
            {
                this->m_events.push( this->m_currentEvent );
            }
            else
            {
                this->m_currentEvent->cancel();
            }
            this->m_currentEvent = NULL;
        }
    }

    // Any events that are left in the priority queue must be cancelled.
    unsigned int queueSize = this->m_events.size();
    for( unsigned int i = 0; i < queueSize; i++ )
    {
        IEvent* currentEvent = this->m_events.top();
        this->m_events.pop();
        currentEvent->cancel();
    }
};
/** Terminates the Scheduler. */
void Scheduler::terminate()
{
    TA_Base_Core::ThreadGuard guard( this->m_eventsLock );
    this->m_terminate = true;
    this->m_waitObject.signal();
};

/** Posts an event to this Scheduler. */
void Scheduler::post( IEvent* event )
{
    TA_Base_Core::ThreadGuard guard( this->m_eventsLock );
    this->m_events.push( event );
    this->m_waitObject.signal();
};
/** Removes an event from this Scheduler. */
void Scheduler::remove( IEvent* event )
{
    // This is ugly
    if( event == this->m_currentEvent )
    {
        TA_Base_Core::ThreadGuard guard( this->m_eventsLock );
        this->m_removeCurrentEvent = true;
        this->m_waitObject.signal();
    }
    else
    {
        TA_Base_Core::ThreadGuard guard( this->m_eventsLock );
        std::priority_queue<IEvent*,std::vector<IEvent*>, IEventComparison> events;
        for( unsigned int i = 0; i < this->m_events.size(); i++ )
        {
            IEvent* currentEvent = this->m_events.top();
            this->m_events.pop();
            if( event != currentEvent )
            {
                events.push( currentEvent );
            }
            else
            {
                event->cancel();
            }
        }
        this->m_events = events;
    }
};

/** Clears all events from the Scheduler. */
void Scheduler::clear()
{
    TA_Base_Core::ThreadGuard guard( this->m_eventsLock );
    if( NULL != this->m_currentEvent )
    {
        this->m_removeCurrentEvent = true;
    }
    // Any events that are left in the priority queue must be cancelled.
    unsigned int queueSize = this->m_events.size();
    for( unsigned int i = 0; i < queueSize; i++ )
    {
        IEvent* currentEvent = this->m_events.top();
        this->m_events.pop();
        currentEvent->cancel();
    }
    this->m_waitObject.signal();
}

}


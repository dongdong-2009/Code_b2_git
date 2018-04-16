/**
  * The source code in this file is the property of
  * MI Services Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/history/traffic_historian/src/EventConsumer.h $
  * @author Craig Grant
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: Katherine Thomson
  *
  * The EventConsumer accepts and processes events pushed from the 
  * CORBA event service.  It listens for any new Traffic Detector data and 
  * passes it to the appropriate TrafficDetector object.
  *
  */

#ifndef EVENT_CONSUMER_H
#define EVENT_CONSUMER_H

#include "CallbackUser.h"
#include "MiThreadLockable.h"
#include "SafeQueue.h"
#include "MiThread.h"

class CEventConsumer : public MiThread, public CallbackUser
{
public:
    // Override methods from MiThread.

    /** 
      * run
      * 
      * This method continuously checks the queue to see if there are
      * any events to be processed.  If there are events on the queue,
      * they are popped off the queue and processed.  Otherwise the 
      * thread will sleep momentarily and then try again.  This method
      * implements the virtual method from MiThread.
      *
      */
    virtual void run();

    /** 
      * terminate
      * 
      * This method tells the EventConsumer thread to stop processing events, so
      * that the thread can be terminated. It implements the virtual method 
      * from MiThread.
      *
      */
    virtual void terminate();

    /** 
      * callback
      *
      * This method handles events coming in from the Current event channel.
      * Events relating to traffic detector statistics are pushed onto a queue
      * for further processing. It implements a virtual method from CallbackUser.
      *
      * @param The received event information.
      * 
      * @exception A CORBA::SystemException is thrown if the CORBA related
      *            processing of the string fails.
      *
      */
    void callback(const CallbackSubmission* submission)
        throw( CORBA::SystemException);

private:
        
    /**
      * ProcessStatsEvent
      *
      * This method processes the first event on the queue if there
      * is one.  It then pops the event off the front of the queue.
      *
      * @return A bool which is true if an event was processed.
      *
      */
    bool ProcessStatsEvent();

    // Queue of received events.

    SafeQueue m_eventQueue;

    // Indicates when the thread is being terminated.
    
    bool m_done;

    // Stops events from being processed while the thread is 
    // trying to terminate and vice versa.
    
    MiThreadLockable m_securityGuard;
};

#endif  // EVENT_CONSUMER_H

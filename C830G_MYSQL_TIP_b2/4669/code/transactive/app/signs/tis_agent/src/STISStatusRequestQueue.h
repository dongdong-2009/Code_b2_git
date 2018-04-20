#if !defined(STISSTATUSREQUESTQUEUE_H)
#define STISSTATUSREQUESTQUEUE_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISStatusRequestQueue.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class maintains a queue of STIS status request messages.
  * It processes each of them in its own thread, passing them on
  * to the STISManager for handling.
  *
  */

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <queue>

namespace TA_IRS_App
{
    class STISStatusRequest; // Forward declaration


    class STISStatusRequestQueue : public TA_Base_Core::Thread
    {

    public:

        /**
          * Constructor
          */
        STISStatusRequestQueue();


        /**
          * Destructor
          */
        ~STISStatusRequestQueue();


        /** 
          * addMessageToQueue
          *
          * Adds a message to the queue of messages to be processed.
          *
          * @param message  The message to add to the queue.
          */
        virtual void addMessageToQueue(STISStatusRequest* message);
    

        /**
          * run
          *
          * Handles messages on the queue, as they are added.
          */
        virtual void run();


        /**
          * terminate
          *
          * Terminates the thread.
          */
        virtual void terminate();


        /**
          * handleMessage
          *
          * Handles either an STISOCCStatusRequest message, or an STISStationStatusRequest
          * message, by passing it onto the STISManager.
          *
          * @param message  The STISStatusRequest message.
          */
        virtual void handleMessage(STISStatusRequest* message);


    private:

        /**
          * The queue of messages
          */
        std::queue<STISStatusRequest*> m_messages;

        /**
          * Signalled when a message is added to the queue.
          */
        TA_Base_Core::Semaphore m_queueSemaphore;
        
        /**
          * Guards the message queue
          */
        TA_Base_Core::NonReEntrantThreadLockable m_messageLock;

        /**
          * True if the thread is running, false otherwise.
          */
        bool m_isRunning;

    };

} // namespace TA_IRS_App

#endif // STISSTATUSREQUESTQUEUE_H
#if !defined(STISSTATUSPROCESSQUEUE_H)
#define STISSTATUSPROCESSQUEUE_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/STISStatusProcessQueue.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2014/03/31 14:20:27 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * This class maintains a queue of STIS status request messages.
  * It processes each of them in its own thread, passing them on
  * to the STISManager for handling.
  *
  */

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <queue>

namespace TA_IRS_App
{
    class STISStatusRequest; // Forward declaration


    class STISStatusProcessQueue : public TA_Base_Core::Thread
    {

    public:

        /**
          * Constructor
          */
        STISStatusProcessQueue();


        /**
          * Destructor
          */
        ~STISStatusProcessQueue();


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

		void clearQueue();


    private:

		STISStatusRequest* take();
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
        TA_Base_Core::ReEntrantThreadLockable m_messageLock;

        /**
          * True if the thread is running, false otherwise.
          */
        bool m_isRunning;

    };

} // namespace TA_IRS_App

#endif // STISSTATUSREQUESTQUEUE_H
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSInputQueue.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Manages the receiption of SDS events. The rate of incoming events may affect 
  * the rate of sending events
  */

#ifndef _SDS_InputQueue2034920349943_H
#define _SDS_InputQueue2034920349943_H

#include <vector>

#include "ace/Task.h"
#include "ace/Singleton.h"
#include "ace/Activation_Queue.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSEventSubject.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSEvent.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


namespace TA_IRS_App 
{
    class SDSEventObserver;
    class SDSQueueManager;


	class SDSInputQueue: public virtual ACE_Task_Base, public virtual SDSEventSubject
	{
		public:
			SDSInputQueue(SDSQueueManager& queueManager); 

			virtual ~SDSInputQueue();
            
	        /**
	          * start the queue
	          */
			void start();

	        /**
	          * stop the queue
	          */
			void stop();

	        /**
	          * add event to the queue
	          */
			void enqueue(SDSEvent* rspMsg);

			virtual int svc (void);

			virtual void registerObserver(SDSEventObserver* evtObj);
			virtual void deregisterObserver(SDSEventObserver* evtObj);

			void clearQueue();

		private:
            // disabled methods
            SDSInputQueue(const SDSInputQueue&);
            void operator=(const SDSInputQueue&);

        private:

			void notifyObservers(SDSEvent* event);

		private:

			ACE_Activation_Queue m_queue;

	        /**
	          * flag to terminate the thread
	          */
			bool m_active;

	        /**
	          * lock to synchronize access to the observers
	          */
			TA_Base_Core::NonReEntrantThreadLockable m_lock;

			std::vector<SDSEventObserver*> m_observers;

            SDSQueueManager& m_queueManager;
	};
};
	
#endif 

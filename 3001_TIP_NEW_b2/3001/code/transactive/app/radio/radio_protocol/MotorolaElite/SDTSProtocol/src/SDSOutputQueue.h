/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSOutputQueue.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Manages the sending of SDDS requests. The internal scheduler is subject to
  * rate limiting. Rate limiting is based upon rate of sending and receiving
  * messages between the SDS Client and SDS Host.
  */


#ifndef _SDS_OutputQueue20349239042384_H
#define _SDS_OutputQueue20349239042384_H

#include "ace/Task.h"
#include "ace/Singleton.h"
#include "ace/Activation_Queue.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"


namespace TA_IRS_App 
{
    class SDSFunction;
    class SDSEventObserver;
    class SDSQueueManager;


	class SDSOutputQueue: public ACE_Task_Base
	{
		public:

			SDSOutputQueue(SDSQueueManager& queueManager); 

			virtual ~SDSOutputQueue();
			
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
			void enqueue(SDSFunction* sendMsg);
			
			virtual int svc (void);

			void clearQueue();
	

		private:
			ACE_Activation_Queue m_queue;

	        /**
	          * flag to terminate the thread
	          */
			bool m_active;
            SDSQueueManager& m_queueManager;
	};

};

	
#endif

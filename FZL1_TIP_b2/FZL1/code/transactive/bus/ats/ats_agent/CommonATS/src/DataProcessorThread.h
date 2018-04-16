/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\DataProcessorThread.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible handling the data processing thread
  */

#ifndef _DATA_PROCESSOR_THREAD_H_
#define _DATA_PROCESSOR_THREAD_H_

#include "core/threads/src/Thread.h"
#include "bus/ats/ats_agent/CommonATS/src/DataNotification.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include <queue>
#include <vector>

namespace TA_IRS_Bus
{
	class DataProcessorThread : public TA_Base_Core::Thread 
	{
	public:
		DataProcessorThread(TA_Base_Core::AtsAgentEntityDataPtr EntityData);
		virtual ~DataProcessorThread();

		typedef std::vector<DataNotificationPtr> DataNotificationVecPtr;
		typedef DataNotificationVecPtr::iterator DataNotificationVecPtrIter;

		/**
		 * This is the telegram processing thread, it waits on a timed semaphore
		 * (m_incomingMessageSemaphore) until either a new message is received or the
		 * semaphore times out.
		 */
		virtual void run();

		/**
		 * Ends the thread, only ever called on agent shutdown.
		 */
		virtual void terminate();

		virtual void processAtsData(DataNotificationPtr item) = 0;

	protected:
		/**
		 * Adds an item to the incoming queue, and signals the semaphore to wake it up.
		 * 
		 * @param telegram
		 * 
		 */
		void addItem(DataNotificationPtr dataNotif, bool latestRecord=false);

		/**
		 * Process the item, implemented by child classes to provide
		 * implementation specific processing.
		 * @param item
		 * 
		 */
		virtual void processItem(DataNotificationPtr item) = 0;

		TA_Base_Core::AtsAgentEntityDataPtr m_atsEntityData;
		bool m_isProcessSuccess;

	private:
		/**
		 * Disable copy and assignment constructor
		 */
		DataProcessorThread(const DataProcessorThread&);
		DataProcessorThread& operator= (const DataProcessorThread&);

		/**
		 * The list of Data Notification to process
		 */
		std::queue<DataNotificationPtr> m_messageQueue;

		TA_Base_Core::ReEntrantThreadLockable m_msgCacheLock;
		TA_Base_Core::Semaphore m_newDataNotif;
		DataNotificationVecPtr m_messageCache;
		bool m_keepRunning;

	};
}
#endif
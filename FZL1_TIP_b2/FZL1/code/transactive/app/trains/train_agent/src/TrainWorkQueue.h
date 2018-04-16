/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/trains/train_agent/src/TrainWorkQueue.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */



#if !defined(TrainWorkQueue__INCLUDED_)
#define TrainWorkQueue__INCLUDED_

#ifdef __WIN32__
#pragma warning(disable:4503)
#endif

#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/threads/src/IWorkItem.h"

#include "app/trains/train_agent/src/TrainManager.h"

namespace TA_IRS_App
{
	/**
	* This is a simple work item that simply has a call method
	*/
	class AbstractTrainWorkQueueItem : public TA_Base_Core::IWorkItem
	{
	public:
		AbstractTrainWorkQueueItem();
		virtual ~AbstractTrainWorkQueueItem();
		virtual int call() = 0;
		virtual std::string info();
		void executeWorkItem();
	};

	/**
	* Dispatch requests according to the following rules:
	* For TIMSNotifications...
	* - If no existing workers, dispatch
	* - If existing workers but not for this train, dispatch
	* - If existing workers for this train, wait until finish (consider allowing to dispatch, but worker waiting on a lock/barier/... )
	*/
	class TrainWorkQueueManager
	{

	public:
		TrainWorkQueueManager();
		virtual ~TrainWorkQueueManager();
		int perform(AbstractTrainWorkQueueItem * item);

	private:
		TA_Base_Core::ThreadPoolSingletonManager* m_threadPoolManager; 
	};

	/**
	 * Sends a TIMS notification to train via Radio agent
	 */
	class TrainTIMSNotification : public AbstractTrainWorkQueueItem
	{
	public:
		TrainTIMSNotification(TrainManager & trainManager, TrainEvent & trainEvent) 
			: m_trainManager(trainManager), m_event(trainEvent)
		{
		}

		virtual int call();
		virtual std::string info();

	private:
		TrainManager & m_trainManager;
		TrainEvent m_event;
	};

	/**
	 * Sends an ATS notification to the ATS agent
	 */
	class TrainATSNotification : public AbstractTrainWorkQueueItem
	{
	public:
		TrainATSNotification(TrainManager & trainManager, ATSTrainMessage & event)
			: m_trainManager(trainManager), m_event(event)
		{
		}

		virtual int call();
		virtual std::string info();

	private:
		TrainManager & m_trainManager;
		ATSTrainMessage m_event;
	};

	/**
	 * Updates the Operation Mode Manager with the duty state
	 */
	class TrainDutyStateUpdate : public AbstractTrainWorkQueueItem
	{
	public:
		TrainDutyStateUpdate(TrainManager & trainManager, DutyTrainMessage & event)
			: m_trainManager(trainManager), m_event(event)
		{
		}

		virtual int call();
		virtual std::string info();

	private:
		TrainManager & m_trainManager;
		DutyTrainMessage m_event;
	};

    class TrainLibraryDownloadUpdate : public AbstractTrainWorkQueueItem
	{
	public:
		TrainLibraryDownloadUpdate( TrainManager & trainManager,
                                    unsigned char trainId,
                                    bool success,
                                    const std::string& failReason )
			: m_trainManager(trainManager),
              m_trainId(trainId),
              m_success(success),
              m_failReason(failReason)
		{
		}

		virtual int call();
		virtual std::string info();

	private:
		TrainManager & m_trainManager;
		unsigned char m_trainId;
        bool m_success;
        std::string m_failReason;
	};

};
#endif // !defined(TrainWorkQueue__INCLUDED_)

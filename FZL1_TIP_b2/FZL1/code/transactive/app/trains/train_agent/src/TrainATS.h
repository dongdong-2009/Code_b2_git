/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/trains/train_agent/src/TrainATS.h $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  *
  */

#if !defined TRAIN_ATS_H
#define TRAIN_ATS_H

#include <string>
#include <vector>

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "app/trains/train_agent/src/TrainStatusManager.h"
#include "app/trains/train_agent/src/ITrainEventReaderDelegator.h"
#include "app/trains/train_agent/src/ITrainNotificationFacade.h"
#include "TrainAgent.h"

#include "TrainTypes.h"

namespace TA_IRS_App
{
	// fwd decls
	class TrainEventSubscriber;
	class OperationModeManager;
	class TrainActivityMonitor;

	/// @todo don't rely on statics
	static TA_Base_Core::NonReEntrantThreadLockable   atsSubscriptionLock;

	/**
	*
	* 
	* 1. Obtain AtsSubscriptionLock to prevent any notifications being processed until the bulk get completes
	* 2. Make an idl call to ATS to get the bulk list
	* 3. Process the list and release the lock
	* 4. Any queued notifications in TrainEventSubscriber thread can now proceed
	*
	* Note: until the bulk list is collected, no ats notifications can be received. Thus, it is 
	* critical that it does succeed at some point. 
	*
	* retry delay logic - decided quite arbitrarily:
	* - try 4 times on a 5 second delay (20 sec since start)
	* - subsequently try on a 10 sec delay (
	* - on 10th attempt keep trying on 30 sec interval until success
	*/
	class AtsTrainInfoListHelper : virtual public TA_Base_Core::Thread
	{
	public:
		AtsTrainInfoListHelper(
			const TrainEventSubscriber & callback, 
			TrainAgent & trainAgent, 
			const std::string &atsEntityName
			);
		virtual ~AtsTrainInfoListHelper();

		void run();
		void terminate();

	private:
		const TrainEventSubscriber& m_callback;
		TrainAgent & m_trainAgent;
		std::string m_atsEntityName;
		bool m_terminate;
	};

	/// @todo don't rely on statics
	static AtsTrainInfoListHelper * atsTrainInfoListHelper = 0;


	/**
	*  Receives and consumes an ATS message
	*/
	class TrainATS : public ITrainATSDelegator,
                     public ITrainEventListener
	{
	public:
		TrainATS(TrainAgent & trainAgent,
			ITrainManager& callback, 
			ITrainNotificationFacade& notifier, 
			TrainTypes::TrainAgentLocationType agentLocationType, 
			ITrainStatusUpdateMonitor& trainStatusUpdateMonitor,
			OperationModeManager& operationModeManager,
			TrainActivityMonitor& trainActivityMonitor,
            TrainWorkQueueManager& trainWorkQueueManager);

		virtual ~TrainATS();
		
		bool delegate(ATSTrainMessage& msg);
		void receive(TrainEvent& event);
		
		/// do the real work here
		void onATSEvent(ATSTrainMessage &event);
		
	private:
		TrainAgent & m_trainAgent;
		ITrainManager& m_trainManager;
		ITrainNotificationFacade& m_notifier;
		TrainTypes::TrainAgentLocationType m_agentLocationType;
		ITrainStatusUpdateMonitor& m_trainStatusUpdateMonitor;
		OperationModeManager& m_operationModeManager;
		TrainActivityMonitor& m_trainActivityMonitor;
        TrainWorkQueueManager& m_workQueueManager;

		/// Ensure that ats events are processed sequentially by workers
		TA_Base_Core::NonReEntrantThreadLockable   m_atsEventLock; /// @todo check the lock is fifo

	};

};  // Namespace TA_IRS_App


#endif

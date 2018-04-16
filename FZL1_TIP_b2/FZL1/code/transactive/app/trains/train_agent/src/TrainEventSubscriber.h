/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/trains/train_agent/src/TrainEventSubscriber.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

///////////////////////////////////////////////////////////
//  TrainEventSubscriber.h
//  Implementation of the Class TrainEventSubscriber
//  Created on:      26-Nov-2003 03:40:30 PM
///////////////////////////////////////////////////////////

#if !defined(TrainEventSubscriber_FFAFAF28_84E9_4a49_B8C4_19B9062E84AB__INCLUDED_)
#define TrainEventSubscriber_FFAFAF28_84E9_4a49_B8C4_19B9062E84AB__INCLUDED_

#ifdef __WIN32__
#pragma warning(disable:4503)
#endif

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"
#include "bus/trains/TrainAgentCorbaDef/idl/src/ITrainCorbaDef.h"

namespace TA_Base_Core 
{
	typedef unsigned int SubscriptionID; // ick
}

/*
 * See synchronisation strategy in TrainEventSender.h
 */

namespace TA_IRS_App
{
	class TrainAgent;
	class TrainManager;
	class TrainWorkQueueManager;
	class OperationModeManager;
	
	/**
	* This class will listen for messages receieved from the Radio Agent, Duty Agent,
	* ATS Agent and other Train Agents.
	* Both BroadcastComms and StateUpdate type messages are received.
	* Subscriptions are managed eg. subscribing, resubscribing and unsubscribing.
	* All Train Agents in control mode subscribe to Radio Agent, ATS Agent and
	* Duty Agent.
	* All Train Agents subscribe to BroadcastComms.
	* All Train Agents in monitor mode subscribe to StateUpdates.
	* Radio Agent subscription: Enables Radio messages to be receieved by the
	*     Train Agent.  SDS messages originating from the Train radio comms
	*     hardware, TIMS, are received, processed ansd sent to the Train Agent.
	* ATS Agent subscription:  Enables ATS messages to be receieved by the
	*     Train Agent.  The ATS Agent sends Train location information to the
	*     Train Agent.
	* Duty Agent subscription:  Enables Duty messages to be receieved by the
	*     Train Agent.  The only way a Train Agent can revert back to Normal
	*     mode is when it receives a duty change from the Duty agent.
	* Train Agent subscription:  Enables Train messages to be receieved by the
	*     Train Agent.  Train Agents send synchronisation messages to each
	*     other.
	* Managing control of trains:  A Train Agent should only send
	* synchronisation messages for its controlled trains and should only
	* receive synchronisation mesages for Trains controlled by other Train
	* Agents.  In the unfortunate event of a Train Agent receiving a
	* synchronisation message for one of its controlled Trains, the Train Agent
	* will take control of the train again.
	*/
	class TrainEventSubscriber 
	:   public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>,
		public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>
	{
//		friend class AtsTrainInfoListHelper;
		public:

			TrainEventSubscriber(TrainAgent& trainAgent, TrainManager& trainManager,
				TrainWorkQueueManager& workQueue, OperationModeManager& operationModeManager);
			virtual ~TrainEventSubscriber();

			/**
			* Subscribe to all agents including the train agent
			* Condition: 
			* @note logic applied within OperationModeManager
			*/
			void subscribe();

			/**
			* Resubscribe to all agents including the train agent
			* Subscribes to comms and stateupdates
			* Condition: 
			* @note logic applied within OperationModeManager
			*/
			void resubscribe();

			/**
			* Unsubscribe to all agents including the train agent
			* Unsubscribes comms and stateupdates
			* Condition: 
			* @note logic applied within OperationModeManager
			*/
			void unsubscribe();


			/**
			* CommsMessageCorbaDef interface methods
			*/
			virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

			/**
			* StateUpdateMessageCorbaDef interface methods
			*/
			virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef & message);

			unsigned long getRadioAgentEntityKey(unsigned long locationKey);
            unsigned long getAtsAgentEntityKey(unsigned long locationKey);
			unsigned long getDutyAgentEntityKey(unsigned long locationKey);


			TrainAgent&				m_trainAgent;
			TrainManager&			m_trainManager;
			OperationModeManager&	m_operationModeManager;

		private:

			TrainWorkQueueManager&	m_workQueueManager;
			unsigned long m_entityKey;
			unsigned long m_senderEntityKey;

			// chenlei++
			// these parameters are preloaded during construction to decrease db connection
			unsigned long m_radioEntityKey;
			unsigned long m_atsEntityKey;
			unsigned long m_dutyEntityKey;

			TrainEventSubscriber(const TrainEventSubscriber& theTrainEventSubscriber);


			/**
			* Subscribe to the train agent
			* subscribes to stateupdates only
			* Condition: 
			* @note logic applied within OperationModeManager
			*/
			void subscribeToTrainAgent();

			/**
			* Subscribe to ATS and Radio etc
			* subscribes to comms only
			* Condition: 
			* @note logic applied within OperationModeManager
			*/
			void subscribeToOtherAgents();

			void receiveCallDriverSpecialisedMessage(
				const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCallDriver& message );
			void receiveCctvSpecialisedMessage(
				const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCctv& message );
			void receiveLibVersSpecialisedMessage(
				const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateLibVers& message );
			void receivePASpecialisedMessage(
				const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePA& message );
			void receivePECSpecialisedMessage(
				const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePEC& message );
			void receiveTSISpecialisedMessage(
				const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateTSI& message );
			void receiveAlarmIDsSpecialisedMessage(
				const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateAlarmIDs& message );
			void receiveOperationModeSpecialisedMessage(
				const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateOperationMode& message );

			/**
			 * manageControlOfTrain
			 * We need to handle the situation where two locations control a train
			 * The symptom of this is; we receive a sync message from a train agent
			 * for a train this agent is in control of.  In this case we should take control
			 * of the train again.  ATS is the authority on train localisation, once it
			 * communicates the real location of the train, the right train agent will finally
			 * take control.
			 */
			void manageControlOfTrain(unsigned long trainID);

			/*
			 * Be careful here!
			 * This list of Call IDs is special.  Its the list of call IDs for
			 * the location only; its a subset of the full list of call IDs
			 */
			void receiveCallIDsSpecialisedMessage(
				const TA_Base_Bus::ITrainCorbaDef::TrainAgentStateUpdateCallIDs& message );

			/*
			 * Used when receiving synchronistaion messages for unknown trains
			 */
			void createTrain(unsigned long trainID);

	};
};
#endif // !defined(TrainEventSubscriber_FFAFAF28_84E9_4a49_B8C4_19B9062E84AB__INCLUDED_)

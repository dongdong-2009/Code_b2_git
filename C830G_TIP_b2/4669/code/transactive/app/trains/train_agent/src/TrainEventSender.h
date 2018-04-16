/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainEventSender.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#ifndef TrainEventSender_HEADER
#define TrainEventSender_HEADER

#include <list>

#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/src/CommsMessageSender.h"
#include "bus/trains/TrainAgentCorbaDef/idl/src/ITrainCorbaDef.h"

#include "TrainInformation.h"

/*
 * Synchronisation strategy:
 * All Controling agents send broadcast updates
 * All Control and Monitor agents subscribe to and receieve broadcast events
 * All Controling agents send StateUpdate updates
 * All Monitor agents subscribe to and receieve StateUpdate events
 *
 * Broadcasts: AlarmIDs, CallDriver, CCTV, PA, PEC, TSI
 * StateUpdates: OperationModes, CallIDs
 */

namespace TA_IRS_App
{

	class TrainManager;
	class TrainManager;
	class OperationModeManager;

	/**
	* This class is responsible for sending synchronisation messages to other
	* Train Agents.  Messages are sent as either StateUpdates or BroadcastComms
	* messages.  The types of synchronistation messages sent are: Call Driver,
	* CCTV, Library Versions, PA, PEC, TSI, Alarm ID, OperationMode.
	* StateUpdates: sent from control Train Agents to monitor Train Agents.
	* BroadcastComms: sent to all Train Agents.
	* A message sent using BroadcastComms will reach subscribing monitor Train
	* Agents (does not need to be sent again as a StateUpdate message).
	* Location Call IDs, PEC and Operation Mode states do not need to persist
	* across all locations, therefor they are sent as StateUpdate messages.
	*/
	class TrainEventSender
	{
	public:

		void send(const TrainStatus::TrainInformation& trainInformation);

		/*
		 * Be careful here!
		 * This list of Call IDs is special.  Its the list of call IDs for
		 * the location only; its a subset of the full list of call IDs
		 */
        void send(const std::list<unsigned long>& callList);

		void sendCallDriverSpecialisedMessage(
			const TrainStatus::TrainInformation& trainInformation);
		void sendCctvSpecialisedMessage(
			const TrainStatus::TrainInformation& trainInformation);
		void sendLibVersSpecialisedMessage(
			const TrainStatus::TrainInformation& trainInformation);
		void sendPASpecialisedMessage(
			const TrainStatus::TrainInformation& trainInformation);
		void sendPECSpecialisedMessage(
			const TrainStatus::TrainInformation& trainInformation);
		void sendTSISpecialisedMessage(
			const TrainStatus::TrainInformation& trainInformation);
		void sendAlarmIDsSpecialisedMessage(
			const TrainStatus::TrainInformation& trainInformation);
		void sendOperationMode(const TrainOperationMode& trainOperationMode);

		TrainEventSender(TrainManager& trainManager,
			OperationModeManager& operationModeManager,
			std::string entityName, unsigned long enitityKey);
		TrainEventSender(const TrainEventSender& TrainEventSender);
		virtual ~TrainEventSender();

	private:

		TrainManager& m_trainManager;
		OperationModeManager& m_operationModeManager;
	    TA_Base_Core::CommsMessageSender* m_commsMessageSender;
		TA_Base_Core::StateUpdateMessageSender* m_stateUpdateMessageSender;
		unsigned long m_entityKey;
		std::string m_entityName;

	};

};

#endif

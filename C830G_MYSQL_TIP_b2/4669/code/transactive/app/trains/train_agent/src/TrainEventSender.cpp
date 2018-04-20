/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainEventSender.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/TrainAgentBroadcastComms_MessageTypes.h"
#include "core/message/types/TrainAgentStateUpdate_MessageTypes.h"


#include "core/message/types/VisualStateUpdate_MessageTypes.h"



#include "OperationModeManager.h"
#include "TrainInformation.h"
#include "TrainEventSender.h"
#include "TrainCorbaTypes.h"

using namespace TA_IRS_App::TrainStatus;

namespace TA_IRS_App
{

	TrainEventSender::TrainEventSender(TrainManager& trainManager,
		OperationModeManager& operationModeManager,
		std::string entityName,
		unsigned long entityKey)
	:   m_trainManager(trainManager),
		m_operationModeManager(operationModeManager),
		m_entityKey(entityKey),
		m_commsMessageSender(0),
		m_stateUpdateMessageSender(0)
	{
		m_entityName = entityName;
	    m_commsMessageSender = TA_Base_Core::MessagePublicationManager
			::getInstance().getCommsMessageSender( TA_Base_Core::TrainAgentBroadcastComms
			::TrainAgentSyncComms );
	    m_stateUpdateMessageSender = TA_Base_Core::MessagePublicationManager
			::getInstance().getStateUpdateMessageSender( TA_Base_Core::TrainAgentStateUpdate
			::TrainAgentSyncStateUpdate );
		
	}

	TrainEventSender::~TrainEventSender()
	{
	}

	void TrainEventSender::send(const TrainStatus::TrainInformation& trainInformation)
	{
		switch(trainInformation.syncState)
		{
			case syncCallDriver:
			{
				sendCallDriverSpecialisedMessage(trainInformation);
				break;
			}
			case syncCctv:
			{
				sendCctvSpecialisedMessage(trainInformation);
				break;
			}
			case syncLibraryVersions:
			{
				sendLibVersSpecialisedMessage(trainInformation);
				break;
			}
			case syncPassengerEmergencyCall:
			{
				sendPECSpecialisedMessage(trainInformation);
				break;
			}
			case syncPublicAddress:
			{
				sendPASpecialisedMessage(trainInformation);
				break;
			}
			case syncTSI:
			{
				sendTSISpecialisedMessage(trainInformation);
				break;
			}
			case syncAlarmIDs:
			{
				sendAlarmIDsSpecialisedMessage(trainInformation);
				break;
			}
			case syncNOP:
			{
				break;
			}
			default:
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Invalid syncState");
				break;
			}
		}
	}

	/*
	 * send (StateUpdate)
	 * Be careful here!
	 * This list of Call IDs is special.  Its the list of call IDs for
	 * the location only; its a subset of the full list of call IDs
	 */
    void TrainEventSender::send(const std::list<unsigned long>& callList)
    {
        TA_Base_Bus::ITrainCorbaDef::TrainAgentStateUpdateCallIDs callMsg;
		std::ostringstream s;

        callMsg.callList.length( callList.size() );
    
		s << "sync [StateUpdate] message out from [Entity=" << m_entityKey << "] ";
		s << "[Sync=CallIDs] ";
		s << "[CallIDCount=" << callList.size() << "] ";
        int i = 0;
        for (std::list<unsigned long>::const_iterator iter = callList.begin();
        iter != callList.end(); ++iter, ++i)
        {
            callMsg.callList[i] = *iter;
			s << callMsg.callList[i] << " ";
        }

        CORBA::Any data;
	    data <<= callMsg;
		TA_ASSERT(m_stateUpdateMessageSender, "StateUpdateMessageSender has not been created");
		m_stateUpdateMessageSender->sendStateUpdateMessage(
			TA_Base_Core::TrainAgentStateUpdate::TrainAgentSyncStateUpdate,
			m_entityKey, m_entityName, data);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            s.str().c_str());
    }

	/*
	 * sendCallDriverSpecialisedMessage (Broadcast)
	 */
	void TrainEventSender::sendCallDriverSpecialisedMessage(
		const TrainStatus::TrainInformation& trainInformation)
	{
		CORBA::Any data;
		TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCallDriver event;
		std::ostringstream s;

		event = convertCallDriver(trainInformation);
		data <<= event;
		TA_ASSERT(m_commsMessageSender, "CommsMessageSender has not been created");
		m_commsMessageSender->sendBroadcastCommsMessage(
			TA_Base_Core::TrainAgentBroadcastComms::TrainAgentSyncComms, m_entityKey, data);
		s << "sync [BroadcastComms] message out from [Entity=" << m_entityKey << "] ";
		s << "[trainID="<< static_cast<int>(trainInformation.trainID) << "] ";
		s << "[Sync=CallDriver] ";
		s << "[trainStatus=" << trainInformation.trainStatus << "] ";
		s << "[SessionID=" << event.sessionID << "] ";
		s << "[CallID=" << event.callID << "] ";
		s << std::ends;
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
	}

	/*
	 * sendCctvSpecialisedMessage (Broadcast)
	 */
	void TrainEventSender::sendCctvSpecialisedMessage(
		const TrainStatus::TrainInformation& trainInformation)
	{
		CORBA::Any data;
		TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCctv event;
		std::ostringstream s;

		event = convertCctv(trainInformation);
		data <<= event;
		TA_ASSERT(m_commsMessageSender, "CommsMessageSender has not been created");
		m_commsMessageSender->sendBroadcastCommsMessage(
			TA_Base_Core::TrainAgentBroadcastComms::TrainAgentSyncComms, m_entityKey, data);
		s << "sync [BroadcastComms] message out from [Entity=" << m_entityKey << "] ";
		s << "[trainID="<< static_cast<int>(trainInformation.trainID) << "] ";
		s << "[Sync=CCTV] ";
		s << "[SessionID=" << event.sessionID << "] ";
		s << "[CallID=" << event.callID << "] ";
		s << std::ends;
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
	}

	/*
	 * sendLibVersSpecialisedMessage (Broadcast)
	 */
	void TrainEventSender::sendLibVersSpecialisedMessage(
		const TrainStatus::TrainInformation& trainInformation)
	{
		CORBA::Any data;
		TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateLibVers event;
		std::ostringstream s;

		event = convertLibVers(trainInformation);
		data <<= event;
		TA_ASSERT(m_commsMessageSender, "CommsMessageSender has not been created");
		m_commsMessageSender->sendBroadcastCommsMessage(
			TA_Base_Core::TrainAgentBroadcastComms::TrainAgentSyncComms, m_entityKey, data);
		s << "sync [BroadcastComms] message out from [Entity=" << m_entityKey << "] ";
		s << "[trainID="<< static_cast<int>(trainInformation.trainID) << "] ";
		s << "[Sync=LibVers] ";
		s << "[TTISLibraryVersion=" << trainInformation.TTISLibraryVersion << "] ";
		s << "[PALibraryVersion=" << trainInformation.PALibraryVersion << "] ";
		s << "[TTISScheduleVersion=" << trainInformation.TTISScheduleVersion << "] ";
		s << std::ends;
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
	}

	/*
	 * sendPASpecialisedMessage (Broadcast)
	 */
	void TrainEventSender::sendPASpecialisedMessage(
		const TrainStatus::TrainInformation& trainInformation)
	{
		CORBA::Any data;
		TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePA event;
		std::ostringstream s;

		event = convertPA(trainInformation);
		data <<= event;
		TA_ASSERT(m_commsMessageSender, "CommsMessageSender has not been created");
		m_commsMessageSender->sendBroadcastCommsMessage(
			TA_Base_Core::TrainAgentBroadcastComms::TrainAgentSyncComms, m_entityKey, data);
		s << "sync [BroadcastComms] message out from [Entity=" << m_entityKey << "] ";
		s << "[trainID="<< static_cast<int>(trainInformation.trainID) << "] ";
		s << "[Sync=PA] ";
		s << "[PAAnnouncementID=" << static_cast<int>(trainInformation.PAAnnouncementID) << "] ";
		s << "[SessionID=" << event.sessionID << "] ";
		s << "[CallID=" << event.callID << "] ";
		s << std::ends;
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
	}

	/*
	 * sendPECSpecialisedMessage (Broadcast)
	 */
	void TrainEventSender::sendPECSpecialisedMessage(
		const TrainStatus::TrainInformation& trainInformation)
	{
		CORBA::Any data;
		TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePEC event;
		std::ostringstream s;

		event = convertPEC(trainInformation);
		data <<= event;
		TA_ASSERT(m_stateUpdateMessageSender, "StateUpdateMessageSender has not been created");
		m_stateUpdateMessageSender->sendStateUpdateMessage(
			TA_Base_Core::TrainAgentStateUpdate::TrainAgentSyncStateUpdate,
			m_entityKey, m_entityName, data);
		s << "sync [BroadcastComms] message out from [Entity=" << m_entityKey << "] ";
		s << "[trainID="<< static_cast<int>(trainInformation.trainID) << "] ";
		s << "[Sync=PEC] ";
		s << "[SessionID=" << event.sessionID << "] ";
		s << "[CallID=" << event.callID << "] ";
		s << std::ends;
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
	}

	/*
	 * sendTSISpecialisedMessage (Broadcast)
	 */
	void TrainEventSender::sendTSISpecialisedMessage(
		const TrainStatus::TrainInformation& trainInformation)
	{
		CORBA::Any data;
		TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateTSI event;
		std::ostringstream s;

		event = convertTSI(trainInformation);
		data <<= event;
		TA_ASSERT(m_commsMessageSender, "CommsMessageSender has not been created");
		m_commsMessageSender->sendBroadcastCommsMessage(
			TA_Base_Core::TrainAgentBroadcastComms::TrainAgentSyncComms, m_entityKey, data);
		s << "sync [BroadcastComms] message out from [Entity=" << m_entityKey << "] ";
		s << "[trainID="<< static_cast<int>(trainInformation.trainID) << "] ";
		s << "[Sync=TSI] ";
		s << "[mpuTSI[0]=" << event.trainTSI[0] << "] ";
		s << "[mpuTSI[1]=" << event.trainTSI[1] << "] ";
		s << "[active mpuTSI=" << event.activeTrainTSI << "] ";
		s << std::ends;
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
	}

	/*
	 * sendAlarmIDsSpecialisedMessage (Broadcast)
	 */
	void TrainEventSender::sendAlarmIDsSpecialisedMessage(
			const TrainStatus::TrainInformation& trainInformation)
	{
		CORBA::Any data;
		TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateAlarmIDs event;
		std::ostringstream s;

		event = convertAlarmIDs(trainInformation);
		data <<= event;
		TA_ASSERT(m_stateUpdateMessageSender, "StateUpdateMessageSender has not been created");
		m_stateUpdateMessageSender->sendStateUpdateMessage(
			TA_Base_Core::TrainAgentStateUpdate::TrainAgentSyncStateUpdate,
			m_entityKey, m_entityName, data);
		s << "sync [StateUpdate] message out from [Entity=" << m_entityKey << "] ";
		s << "[trainID="<< static_cast<int>(trainInformation.trainID) << "] ";
		s << "[Sync=syncAlarmIDs] ";
		s << "[atcAlarmID=" << event.atcAlarmID << "] ";
		s << "[atcAlarmID=" << event.oa1AlarmID << "] ";
		s << "[radioModeAlarmID=" << event.radioModeAlarmID << "] ";
		s << std::ends;
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
	}

	/*
	 * sendOperationMode (StateUpdate)
	 */
	void TrainEventSender::sendOperationMode(const TrainOperationMode& trainOperationMode)
	{
		CORBA::Any data;
		TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateOperationMode event;
		std::ostringstream s;

		event = convertOperationMode(trainOperationMode);
		data <<= event;
		TA_ASSERT(m_stateUpdateMessageSender, "StateUpdateMessageSender has not been created");
		m_stateUpdateMessageSender->sendStateUpdateMessage(
			TA_Base_Core::TrainAgentStateUpdate::TrainAgentSyncStateUpdate,
			m_entityKey, m_entityName, data);
		s << "sync [StateUpdate] message out from [Entity=" << m_entityKey << "] ";
		s << "[Sync=OperationMode] ";
		s << trainOperationMode;
		s << std::ends;
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
	}

}

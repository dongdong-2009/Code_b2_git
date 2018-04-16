/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/trains/train_agent/src/TrainEventSubscriber.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/06/22 17:53:38 $
  * Last modified by:  $Author: peter.wong $
  *
  */

///////////////////////////////////////////////////////////
//  TrainEventSubscriber.cpp
//  Implementation of the Class TrainEventSubscriber
//  Created on:      26-Nov-2003 03:40:30 PM
///////////////////////////////////////////////////////////

#include "TrainEventSubscriber.h"
#include "app/trains/train_agent/src/TARethrow.h"
#include "app/trains/train_agent/src/TrainAgent.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEvent.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEventReader.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/utilities/src/TAAssert.h"
#include "core/message/src/MessageConstants.h"
#include "core/message/types/TrainAgentBroadcastComms_MessageTypes.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "core/message/types/TrainAgentStateUpdate_MessageTypes.h"
#include "core/message/types/DutyAgentBroadcastComms_MessageTypes.h"
#include "core/message/types/DutyStateUpdate_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/RadioEntityData.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/DutyAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"
#include "bus/ats/ats_agent/IDL/src/IAtsTrainCorbaDef.h"
#include "bus/security/duty_agent/IDL/src/DutyAgentTypeCorbaDef.h"
#include "TrainCorbaTypes.h"
#include "TrainException.h"
#include "TrainManager.h"
#include "TrainWorkQueue.h"
#include "TrainATS.h"
#include "OperationModeManager.h"

// ss++
// TD12082
// Internal patch for duty
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
// ++ss

using namespace TA_IRS_App;


/**
 * constructor  default constructor
 */
TrainEventSubscriber::TrainEventSubscriber(TrainAgent& trainAgent, TrainManager& trainManager,
		TrainWorkQueueManager& workQueue, OperationModeManager& operationModeManager)
:	m_trainAgent(trainAgent),
	m_trainManager(trainManager),
	m_workQueueManager(workQueue),
	m_operationModeManager(operationModeManager)
{
	m_entityKey = m_trainAgent.getEntityKey();
	// chenlei++
	// preload radio, ats and duty entity key to decrease db connection
	unsigned long locationKey = m_trainAgent.getAgentLocation();
	m_radioEntityKey = 0;//getRadioAgentEntityKey(locationKey);
	m_atsEntityKey = getAtsAgentEntityKey(locationKey);
	m_dutyEntityKey = getDutyAgentEntityKey(locationKey);

}

/**
 * destructor
 */
TrainEventSubscriber::~TrainEventSubscriber()
{
	// Stop listening for train messages
	unsubscribe();
}

/**
 * subscribe
 * Subscribe to the needed agents
 */
void TrainEventSubscriber::subscribe()
{
	FUNCTION_ENTRY("subscribe");

	subscribeToTrainAgent();
	subscribeToOtherAgents();

	FUNCTION_EXIT;
}

/**
 * resubscribe
 * Sesubscribe to the needed agents
 * Resubscribe when the agent mode changes
 */
void TrainEventSubscriber::resubscribe()
{
	FUNCTION_ENTRY("resubscribe");

	unsubscribe();
	subscribe();

	FUNCTION_EXIT;
}

/**
 * unsubscribe
 * Unsubscribe from all agents
 * Performed during resubscribe and shutting down
 */
void TrainEventSubscriber::unsubscribe()
{
	FUNCTION_ENTRY("unsubscribe");

	TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
		dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*>(this));
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo,
		"Unsubscribed from CommsMessageCorbaDef");

	TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
		dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this));
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo,
		"Unsubscribed from StateUpdateMessageCorbaDef");

	if(atsTrainInfoListHelper != 0)
	{
		delete atsTrainInfoListHelper; 
		atsTrainInfoListHelper = 0;
	}

	FUNCTION_EXIT;
}

/**
 * subscribeToTrainAgent
 * Subscribe to the local train agent for local synchronisation
 */
void TrainEventSubscriber::subscribeToTrainAgent()
{
 	FUNCTION_ENTRY("subscribeToTrainAgent");

	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage( 
		TA_Base_Core::TrainAgentBroadcastComms::TrainAgentSyncComms,
		dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*>(this));
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo,
		"Subscribed to TrainAgentBroadcastComms::TrainAgentSyncComms");

	if(m_operationModeManager.isRunningControl() == false)
	{
		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage( 
			TA_Base_Core::TrainAgentStateUpdate::TrainAgentSyncStateUpdate, 
			dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this),
			TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ).c_str() );
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo,
			"Subscribed to TrainAgentStateUpdate::TrainAgentSyncStateUpdate");
	}

	FUNCTION_EXIT;
}

/**
 * subscribeToOtherAgents
 * Subscribe to other agents; ATS, radio, duty etc
 */
void TrainEventSubscriber::subscribeToOtherAgents()
{
 	FUNCTION_ENTRY("subscribeToOtherAgents");

    unsigned long locationKey = m_trainAgent.getAgentLocation();
    
	if(m_operationModeManager.isRunningControl() == true)
	{
		// Get the train manager entity for the location
		// chenlei-- 
		// use the preloaded member
		//unsigned long radioEntityKey = getRadioAgentEntityKey(locationKey);

		// Start subscription to changes
		//TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( TA_Base_Core::RadioComms::TrainMessage,
		//	dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*>(this), 
		//	m_radioEntityKey, 0, locationKey, NULL );
		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo,
		//	"Subscribed to RadioComms::TrainMessage");

        // also subscribe to call ending
        //TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( TA_Base_Core::RadioComms::RadioCallStatus,
        //    dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*>(this),
        //    m_radioEntityKey, 0, locationKey, NULL );
		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo,
		//	"Subscribed to RadioComms::RadioCallStatus");

        // also subscribe to circuit mode notifications
        //TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( TA_Base_Core::RadioComms::RadioCircuitModeNotification,
        //    dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*>(this),
        //    m_radioEntityKey, 0, locationKey, NULL );
		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo,
		//	"Subscribed to RadioComms::RadioCircuitModeNotification");
	 }
	 
    // Listen to ATS notifications
    // regardless of whether in control or not
    // this isnt synced
	
	// chenlei-- 
	// use the preloaded member
    //unsigned long atsEntityKey = getAtsAgentEntityKey(locationKey);
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( TA_Base_Core::ATSAgentComms::AtsTrainUpdate,
        dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*>(this), m_atsEntityKey, 0, locationKey, NULL );
    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo,
        "Subscribed to ATSAgentComms::AtsTrainUpdate");

    // Load existing list in background
    if(atsTrainInfoListHelper != 0)
    {
        delete atsTrainInfoListHelper; 
        atsTrainInfoListHelper = 0;
    }
	// chenlei-+
	// use the preloaded member of TrainAgent instead of querying db
    //atsTrainInfoListHelper = new AtsTrainInfoListHelper((*this), (*this).m_trainAgent, m_trainAgent.getATSEntityName(m_trainAgent.getAgentLocation()));	
	atsTrainInfoListHelper = new AtsTrainInfoListHelper((*this), (*this).m_trainAgent, m_trainAgent.getATSEntityName());	
    atsTrainInfoListHelper->start();
		
	if(m_operationModeManager.isRunningControl() == true)
	{
		// chenlei-- 
		// this duty entity key is useless at all
		//unsigned long dutyEntityKey = getDutyAgentEntityKey(locationKey);
		
        // Register for subsystem duty add and remove messages
        // Note this is a hack
        // ss++
        // TD12082
	    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
            TA_Base_Core::DutyAgentBroadcastComms::AddSubsystemDuty,
            dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*>(this),
            NULL);
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo,
			"Subscribed to DutyAgentBroadcastComms::AddSubsystemDuty");

	    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
            TA_Base_Core::DutyAgentBroadcastComms::RemoveSubsystemDuty,
            dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*>(this),
            NULL);
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo,
			"Subscribed to DutyAgentBroadcastComms::RemoveSubsystemDuty");

		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage( 
			TA_Base_Core::DutyAgentBroadcastComms::SubsystemStateChangeNotification,
			dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*>(this),
			NULL);
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo,
			"Subscribed to DutyAgentBroadcastComms::SubsystemStateChangeNotification");
        // ++ss
	}

	FUNCTION_EXIT;
}

/**
 * receiveSpecialisedMessage (Comms and Broadcast)
 * Callback for the specialiase message subscriber for comms message interface
 */
void TrainEventSubscriber::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
 	FUNCTION_ENTRY("receiveSpecialisedMessage");

	// Ignore own messages but let them be consumed by monitor agent
	m_senderEntityKey = message.assocEntityKey;
	if((m_senderEntityKey == m_entityKey) && (m_operationModeManager.isRunningControl() == true))
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugDebug, "Ignoring own messge");
		return;
	}

	{
		std::stringstream s;
		s << "Received train comms message from entity [" << message.assocEntityKey << "]" 
			<< std::ends;
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
	}

	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCallDriver* trainStateUpdateCallDrive = 0;
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateLibVers* trainStateUpdateLibVers = 0;
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePA* trainStateUpdatePA = 0;
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePEC* trainStateUpdatePEC = 0;
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCctv* trainStateUpdateCctv = 0;
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateTSI* trainStateUpdateTSI = 0;


    TA_Base_Bus::IRadioCorbaDef::SDSNotificationEvent* sdsNotificationEvent = 0;
    TA_Base_Bus::IRadioCorbaDef::CallStatusEvent* callNotificationEvent = NULL;
    TA_Base_Bus::IRadioCorbaDef::SendDataNotification* dataNotificationEvent = 0;
    TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef* atsNotification = 0;
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCollection* trainStatusUpdateCollection = 0;
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateTISDownload* trainStatusUpdateTISDownload = 0;

    // ss++
    // TD12082
    // Temporarily changed cause duty doesnt do what we need it to
    TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence* subsystemStateSequence = 0;
    TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence* subsystemDutySequence = 0;
    // ++ss

    if ( ( (message.messageState) >>= sdsNotificationEvent ) != 0)
    {
		// sanity, ignore messages in monitor mode
		if ( m_operationModeManager.isRunningControl() == false) 
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugDebug, "In monitor mode, ignoring request [SDSNotification]");
			FUNCTION_EXIT;
			return;
		}
		
		// Process the train message
        std::string trainITSI = static_cast<const char*>(sdsNotificationEvent->sendingITSI);
        std::vector<unsigned char> trainMessageData;
        for (unsigned int index=0;index<sdsNotificationEvent->messageData.length();index++)
        {
            trainMessageData.push_back(sdsNotificationEvent->messageData[index]);
        }
        
// TIP5-PROTO handling
		// There are 2 main strategies that may be adopted
		// 1. Trap errors in receiving the event and consume the message. This has the 
		//    effect of preventing a synchronous SDS command from  receiving its reply event. 
		// 2. Allow the invalid event to propagate up to TrainManager to allow caller to 
		//    process the invalid message sooner. Note: To do this, need to inform TrainManager
		//    that we have received an invalid event for a train. We cannot do this directly through
		//    onTrainEvent(), as we haven't successfully constructed a valid message.
		// 3. Trap the error, and replace the event with a manually constructed BadMessage event. 
		//    The TrainManager class should already correctly handle BadMessage, so this could be 
		//    an appropriate solution.
		// Note: consider if appropriate for policy/strategy pattern for implementation of bad message handler.
		try
		{
			TrainEvent trainEvent(trainITSI,trainMessageData);
			// TIP5-PROTO TA_ASSERT(trainEvent.isValid(),"Invalid train message received from the radio system.");
			
			{
				/// TIP5-PROTO todo: try/catch ProtocolExc, log, ignore, continue
				TrainEventReader*  reader = trainEvent.getReader();
				std::stringstream s;
				s << "Received train message [" << typeid(*reader).name() << "]" 
					<< " from train [" << static_cast<int>(trainEvent.getTrainID()) << "]" << std::ends;
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
			}

			{
				std::ostringstream s;
				s << "SDS message  IN: [recv] " << trainEvent;
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
			}

			// don't suck exception before letting train manager handle it, if we do, then callers will get a timeout instead of a fast-fail.
			// need this here due to variable scope

			// send this to the TrainWorkQueueManager for processing in another thread
			TrainTIMSNotification * x = new TrainTIMSNotification(m_trainManager, trainEvent);
			m_workQueueManager.perform(x);
		} 
		catch (TrainProtocolException & e)
		{
			std::ostringstream s;
			s << "SDS message IN: [recv] SDS protocol error: " << e.what() << std::ends;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, s.str().c_str());
		}
    }
    else if ( ( message.messageState >>= callNotificationEvent ) != 0)
    {
        // only process a call disconnection
        // and only process it if it is in the watch list
        if ( (callNotificationEvent->callStatus == TA_Base_Bus::IRadioCorbaDef::Deleted) &&
             m_trainManager.getTrainStatusManager().isCallInWatchList(callNotificationEvent->callID) )
        {
            m_trainManager.onCallEndEvent(callNotificationEvent->callID);
        }
    }
    else if ( ( message.messageState >>= dataNotificationEvent ) != 0)
    {
        std::string itsi( dataNotificationEvent->ITSI.in() );
        bool isSuccessful = dataNotificationEvent->isSuccessful;
        std::string failReason( dataNotificationEvent->failReason.in() );
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Received RadioComms CircuitModeNotification, TSI=%s successful=%d failReason=\"%s\"", itsi.c_str(), isSuccessful, failReason.c_str());

        unsigned char trainID = m_trainManager.getTrainStatusManager().trainTSI2ID(itsi);

        TrainLibraryDownloadUpdate * x = new TrainLibraryDownloadUpdate( m_trainManager,
                                                                         trainID,
                                                                         isSuccessful,
                                                                         failReason );
		m_workQueueManager.perform(x);
    }
    else if ( ( (message.messageState) >>= atsNotification ) != 0)
    {
		// sanity, ignore messages in monitor mode
		if ( m_operationModeManager.isRunningControl() == false) 
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugDebug, "In monitor mode, ignoring request [ATSNotification]");
			FUNCTION_EXIT;
			return;
		}
		
		TA_THREADGUARD(atsSubscriptionLock); // to prevent interuption of background loading of current train list

		ATSTrainMessage atsMsg = convertATSEvent(atsNotification);
		{
			std::stringstream s;
			s << "Received ATS message: " << atsMsg << std::ends;
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
		}
		// send this to the TrainWorkQueueManager for processing in another thread
		TrainATSNotification * x = new TrainATSNotification(m_trainManager, atsMsg);
		m_workQueueManager.perform(x);
	}
    else if ( ( (message.messageState) >>= trainStatusUpdateCollection ) != 0)
    {
		/// @todo add handling of TrainStateUpdate messages
//        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Received TrainStateUpdate, but don't handle this message yet.");

		// sanity, ignore messages in control mode
		if ( m_operationModeManager.isRunningControl() == true) 
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugDebug, "In control mode, ignoring request [TrainStateUpdate]");
			FUNCTION_EXIT;
			return;
		}

		std::vector<TrainStatus::TrainInformation> stateUpdate = convertTrainStateUpdate(*trainStatusUpdateCollection);
		m_trainManager.onTrainStateUpdate(stateUpdate);
	}
    else if ( ( (message.messageState) >>= trainStatusUpdateTISDownload ) != 0)
    {
		/// @todo add handling of TrainStateUpdateTISDownload messages
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Received TrainStateUpdateTISDownload, but don't handle this message yet.");
	}

    // ss++
    // TD12082
    else if ( ( (message.messageState) >>= subsystemStateSequence ) != 0)
    {
        if ( m_operationModeManager.isRunningControl() == false) 
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugDebug, "In monitor mode, ignoring request [SubsystemStateSequence]");
			FUNCTION_EXIT;
			return;
		}

		for(unsigned long index = 0; index < subsystemStateSequence->length(); ++index)
		{
            unsigned long ourSubsystemKey = m_trainAgent.getDutySubsystemKey();

			if((*subsystemStateSequence)[index].subsystemKey == ourSubsystemKey)
			{

                DutyTrainMessage dutyMsg = convertDutyEvent(&((*subsystemStateSequence)[index]));

                // ss++
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Duty changed for our subsystem for location %d from mode %d to mode %d [Normal - 0/Delegate - 1/Degraded - 2]", 
                    dutyMsg.locationKey, dutyMsg.remState, dutyMsg.addState);
                // ++ss

				TrainDutyStateUpdate* x = new TrainDutyStateUpdate(m_trainManager, dutyMsg);
				m_workQueueManager.perform(x);
			}
		}
	}
    // ++ss

    // ss++ Commented for now until duty agent is fixed
    // TD12082
    // Added temporarily until duty gets its shit together
//    else if ( ( (message.messageState) >>= subsystemDutySequence ) != 0)
//    {
//		if ( m_operationModeManager.isRunningControl() == false) 
//		{
//			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugDebug, "In monitor mode, ignoring request [SubsystemStateSequence]");
//			FUNCTION_EXIT;
//			return;
//		}
//
//        unsigned long ourSubsystemKey = m_trainAgent.getDutySubsystemKey();
//
//		for(unsigned long index = 0; index < subsystemDutySequence->length(); ++index)
//		{
//            TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDuty& subDuty = (*subsystemDutySequence)[index];
//            // check the subsystem key to see if it is our subsystem
//            if (ourSubsystemKey == subDuty.subsystemKey)
//            {
//                // this is an update for our subsystem
//                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
//                    "Duty changed for our subsystem");
//
//                // build an incomplete update
//                // the handler needs to query for some information
//                // did I mention duty blows?
//                DutyTrainMessage dutyMsg = convertDutyEventIncomplete(&((*subsystemDutySequence)[index]));
//
//                TrainDutyStateUpdate* x = new TrainDutyStateUpdate(m_trainManager, dutyMsg);
//				m_workQueueManager.perform(x);
//            }
//		}
//	}

    // internal patch - to be removed when duty design has been fixed
    else if ( ( (message.messageState) >>= subsystemDutySequence ) != 0)
    {
		if ( m_operationModeManager.isRunningControl() == false) 
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugDebug, "In monitor mode, ignoring request [SubsystemStateSequence]");
			FUNCTION_EXIT;
			return;
		}

        unsigned long ourSubsystemKey = m_trainAgent.getDutySubsystemKey();

		for(unsigned long index = 0; index < subsystemDutySequence->length(); ++index)
		{
            TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDuty& subDuty = (*subsystemDutySequence)[index];
            // check the subsystem key to see if it is our subsystem
            if (ourSubsystemKey == subDuty.subsystemKey)
            {
                // this is an update for our subsystem
//                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
//                    "Duty changed for our subsystem");

                bool toAdd = false;
                std::string messageTypeKey(message.messageTypeKey);
                if(messageTypeKey == TA_Base_Core::DutyAgentBroadcastComms::AddSubsystemDuty.getTypeKey())
                {
                    toAdd = true;
                }

                DutyTrainMessage dutyMsg;
                dutyMsg.locationKey = subDuty.regionKey; 
                dutyMsg.subsystemKey = subDuty.subsystemKey;

                std::string session(subDuty.session);
                TA_Base_Core::IConsole* console = NULL;
                try
                {
	                console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(session);
                }
                catch(...)
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "Exception caught on getting Console from ConsoleAccessFactory");
                    continue;
                }

                unsigned long occLocKey = 0;
                try
                {
                    occLocKey = TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey();
                }
                catch(...)
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "Exception caught on getting OCC Location Key from LocationAccessFactory");
                    continue;
                }

                // OCC
                switch(m_trainAgent.getAgentLocationType())
                {
                    case TrainTypes::LOC_OCC:
                        if(console->getLocation() != occLocKey)
                        {
                            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                "console->getLocation() != occLocKey but we are OCC agent");
                            continue;
                        }

                        if(toAdd)
                        {
                            dutyMsg.addState = TrainTypes::SS_NORMAL;
                            dutyMsg.remState = TrainTypes::SS_NORMAL;
                        }
                        else
                        {
                            dutyMsg.addState = TrainTypes::SS_DELEGATED;
                            dutyMsg.remState = TrainTypes::SS_DELEGATED;
                        }

                        break;

                    case TrainTypes::LOC_STATION:
                        if(console->getLocation() == occLocKey)
                        {
                            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                "console->getLocation() == occLocKey but we are STATION agent");
                            continue;
                        }
                        if(toAdd)
                        {
                            dutyMsg.addState = TrainTypes::SS_DELEGATED;
                            dutyMsg.remState = TrainTypes::SS_DELEGATED;
                        }
                        else
                        {
                            dutyMsg.addState = TrainTypes::SS_NORMAL;
                            dutyMsg.remState = TrainTypes::SS_NORMAL;
                        }

                        break;
                };


	            delete console;

//                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
//                    "AddState [%d] RemState [%d]", dutyMsg.addState, dutyMsg.remState); 
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Duty changed for our subsystem and agent");
                
                TrainDutyStateUpdate* x = new TrainDutyStateUpdate(m_trainManager, dutyMsg);
				m_workQueueManager.perform(x);
            }
		}
	}
    // ++ss

	else if ( 0 != ( message.messageState >>= trainStateUpdateLibVers ) )   // Is this a Library Versions state update message?
	{
		receiveLibVersSpecialisedMessage(*trainStateUpdateLibVers);
	}
	else if ( 0 != ( message.messageState >>= trainStateUpdatePA ) )   // Is this a public announcement state update message?
	{
		receivePASpecialisedMessage(*trainStateUpdatePA);
	}
	else if ( 0 != ( message.messageState >>= trainStateUpdatePEC ) )   // Is this a passenger emergency call state update message?
	{
		receivePECSpecialisedMessage(*trainStateUpdatePEC);
	}
	else if ( 0 != ( message.messageState >>= trainStateUpdateCallDrive ) )   // Is this a call drive state update message?
	{
		receiveCallDriverSpecialisedMessage(*trainStateUpdateCallDrive);
	}
	else if ( 0 != ( message.messageState >>= trainStateUpdateCctv ) )   // Is this a call drive state update message?
	{
		receiveCctvSpecialisedMessage(*trainStateUpdateCctv);
	}
	else if ( 0 != ( message.messageState >>= trainStateUpdateTSI ) )   // Is this a TSI state update message?
	{
		receiveTSISpecialisedMessage(*trainStateUpdateTSI);
	}
	else
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Notification does not contain anything train agent is interested in");
    }

	FUNCTION_EXIT;
}

/**
 * receiveSpecialisedMessage (StateUpdate)
 * Callback for the specialiase message subscriber for stateupdate message interface
 */
void TrainEventSubscriber::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef & message)
{
	FUNCTION_ENTRY("receiveSpecialisedMessage");

	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCallDriver* trainStateUpdateCallDrive = 0;
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateLibVers* trainStateUpdateLibVers = 0;
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePA* trainStateUpdatePA = 0;
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePEC* trainStateUpdatePEC = 0;
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCctv* trainStateUpdateCctv = 0;
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateTSI* trainStateUpdateTSI = 0;
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateOperationMode* trainStateUpdateOperationMode = 0;
	TA_Base_Bus::ITrainCorbaDef::TrainAgentStateUpdateCallIDs* trainStateUpdateCallIDs = 0;
	TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateAlarmIDs* trainStateUpdateAlarmIDs = 0;

	if ( 0 != ( message.stateUpdateInfo >>= trainStateUpdateLibVers ) )   // Is this a Library Versions state update message?
	{
		receiveLibVersSpecialisedMessage(*trainStateUpdateLibVers);
	}
	else if ( 0 != ( message.stateUpdateInfo >>= trainStateUpdatePA ) )   // Is this a public announcement state update message?
	{
		receivePASpecialisedMessage(*trainStateUpdatePA);
	}
	else if ( 0 != ( message.stateUpdateInfo >>= trainStateUpdatePEC ) )   // Is this a passenger emergency call state update message?
	{
		receivePECSpecialisedMessage(*trainStateUpdatePEC);
	}
	else if ( 0 != ( message.stateUpdateInfo >>= trainStateUpdateCallDrive ) )   // Is this a call drive state update message?
	{
		receiveCallDriverSpecialisedMessage(*trainStateUpdateCallDrive);
	}
	else if ( 0 != ( message.stateUpdateInfo >>= trainStateUpdateCctv ) )   // Is this a call drive state update message?
	{
		receiveCctvSpecialisedMessage(*trainStateUpdateCctv);
	}
	else if ( 0 != ( message.stateUpdateInfo >>= trainStateUpdateTSI ) )   // Is this a TSI state update message?
	{
		receiveTSISpecialisedMessage(*trainStateUpdateTSI);
	}
	else if ( 0 != ( message.stateUpdateInfo >>= trainStateUpdateAlarmIDs ) )   // Is this a TSI state update message?
	{
		receiveAlarmIDsSpecialisedMessage(*trainStateUpdateAlarmIDs);
	}
	else if ( 0 != ( message.stateUpdateInfo >>= trainStateUpdateOperationMode ) )   // Is this an OperationMode state update message?
	{
		receiveOperationModeSpecialisedMessage(*trainStateUpdateOperationMode);
	}
	else if ( 0 != ( message.stateUpdateInfo >>= trainStateUpdateCallIDs ) )   // Is this an OperationMode state update message?
	{
		receiveCallIDsSpecialisedMessage(*trainStateUpdateCallIDs);
	}
	else
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, "Uknown sychronisation message receieved"); 
	}

	FUNCTION_EXIT;
}

/**
 * receiveCallDriverSpecialisedMessage (Broadcast)
 * Receive and consume a Call Driver message (synchrosnisation)
 */
void TrainEventSubscriber::receiveCallDriverSpecialisedMessage(
	const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCallDriver& message )
{
	FUNCTION_ENTRY("receiveCallDriverSpecialisedMessage");

	TA_IRS_App::TrainStatus::TrainInformation trainInformation;
	std::vector<TA_IRS_App::TrainStatus::TrainInformation> stateUpdate;
	std::ostringstream s;

	trainInformation = convertCallDriver(message);
	stateUpdate.push_back(trainInformation);
	m_trainManager.onTrainStateUpdate(stateUpdate);
	manageControlOfTrain(message.trainID);

	s << "sync [BroadcastComms] messsage in from [Entity=" << m_senderEntityKey << "] ";
	s << "[TrainID=" << static_cast<unsigned int>(message.trainID) << "] ";
	s << "[Sync=syncCallDriver] ";
	s << "[trainStatus=" << trainInformation.trainStatus << "] ";
	s << "[SessionID=" << message.sessionID << "] ";
	s << "[CallID=" << message.callID << "] ";
	s << std::ends;
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());

	FUNCTION_EXIT;
}

/**
 * receiveCctvSpecialisedMessage (Broadcast)
 * Receive and consume a CCTV message (synchrosnisation)
 */
void TrainEventSubscriber::receiveCctvSpecialisedMessage(
	const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateCctv& message )
{
	FUNCTION_ENTRY("receiveCctvSpecialisedMessage");

	TA_IRS_App::TrainStatus::TrainInformation trainInformation;
	std::vector<TA_IRS_App::TrainStatus::TrainInformation> stateUpdate;
	std::ostringstream s;

	trainInformation = convertCctv(message);
	stateUpdate.push_back(trainInformation);
	m_trainManager.onTrainStateUpdate(stateUpdate);
	manageControlOfTrain(message.trainID);

	s << "sync [BroadcastComms] messsage in from [Entity=" << m_senderEntityKey << "] ";
	s << "[TrainID=" << static_cast<unsigned int>(message.trainID) << "] ";
	s << "[Sync=CCTV] ";
	s << "[SessionID=" << message.sessionID << "] ";
	s << "[CallID=" << message.callID << "] ";
	s << std::ends;
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());

	FUNCTION_EXIT;
}

/**
 * receiveLibVersSpecialisedMessage (Broadcast)
 * Receive and consume a Library Versions message (synchrosnisation)
 */
void TrainEventSubscriber::receiveLibVersSpecialisedMessage(
	const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateLibVers& message )
{
	FUNCTION_ENTRY("receiveLibVersSpecialisedMessage");

	TA_IRS_App::TrainStatus::TrainInformation trainInformation;
	std::vector<TA_IRS_App::TrainStatus::TrainInformation> stateUpdate;
	std::ostringstream s;

	trainInformation = convertLibVers(message);
	stateUpdate.push_back(trainInformation);
	m_trainManager.onTrainStateUpdate(stateUpdate);
	manageControlOfTrain(message.trainID);

	s << "sync [BroadcastComms] messsage in from [Entity=" << m_senderEntityKey << "] ";
	s << "[TrainID=" << static_cast<unsigned int>(message.trainID) << "] ";
	s << "[Sync=LibVers] ";
	s << "[TTISLibraryVersion=" << trainInformation.TTISLibraryVersion << "] ";
	s << "[PALibraryVersion=" << trainInformation.PALibraryVersion << "] ";
	s << "[TTISScheduleVersion=" << trainInformation.TTISScheduleVersion << "] ";
	s << std::ends;
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());

	FUNCTION_EXIT;
}

/**
 * receivePASpecialisedMessage (Broadcast)
 * Receive and consume a PA message (synchrosnisation)
 */
void TrainEventSubscriber::receivePASpecialisedMessage(
	const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePA& message )
{
	FUNCTION_ENTRY("receivePASpecialisedMessage");

	TA_IRS_App::TrainStatus::TrainInformation trainInformation;
	std::vector<TA_IRS_App::TrainStatus::TrainInformation> stateUpdate;
	std::ostringstream s;

	trainInformation = convertPA(message);
	stateUpdate.push_back(trainInformation);
	m_trainManager.onTrainStateUpdate(stateUpdate);
	manageControlOfTrain(message.trainID);

	s << "sync [BroadcastComms] messsage in from [Entity=" << m_senderEntityKey << "] ";
	s << "[TrainID=" << static_cast<unsigned int>(message.trainID) << "] ";
	s << "[Sync=PA] ";
	s << "[PAAnnouncementID=" << static_cast<int>(trainInformation.PAAnnouncementID) << "] ";
	s << "[SessionID=" << message.sessionID << "] ";
	s << "[CallID=" << message.callID << "] ";
	s << std::ends;
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());

	FUNCTION_EXIT;
}

/**
 * receivePECSpecialisedMessage (Broadcast)
 * Receive and consume a PEC message (synchrosnisation)
 */
void TrainEventSubscriber::receivePECSpecialisedMessage(
	const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdatePEC& message )
{
	FUNCTION_ENTRY("receivePECSpecialisedMessage");

	TA_IRS_App::TrainStatus::TrainInformation trainInformation;
	std::vector<TA_IRS_App::TrainStatus::TrainInformation> stateUpdate;
	std::ostringstream s;

	trainInformation = convertPEC(message);
	stateUpdate.push_back(trainInformation);
	m_trainManager.onTrainStateUpdate(stateUpdate);
	manageControlOfTrain(message.trainID);

	s << "sync [BroadcastComms] messsage in from [Entity=" << m_senderEntityKey << "] ";
	s << "[TrainID=" << static_cast<unsigned int>(message.trainID) << "] ";
	s << "[Sync=PEC] ";
	s << "[SessionID=" << message.sessionID << "] ";
	s << "[CallID=" << message.callID << "] ";
	s << std::ends;
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());

	FUNCTION_EXIT;
}

/**
 * receiveTSISpecialisedMessage (Broadcast)
 * Receive and consume a TSI message (synchrosnisation)
 */
void TrainEventSubscriber::receiveTSISpecialisedMessage(
	const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateTSI& message )
{
	FUNCTION_ENTRY("receiveTSISpecialisedMessage");

	TA_IRS_App::TrainStatus::TrainInformation trainInformation;
	std::vector<TA_IRS_App::TrainStatus::TrainInformation> stateUpdate;
	std::ostringstream s;

	trainInformation = convertTSI(message);
	stateUpdate.push_back(trainInformation);
	m_trainManager.onTrainStateUpdate(stateUpdate);
	manageControlOfTrain(message.trainID);

	s << "sync [BroadcastComms] messsage in from [Entity=" << m_senderEntityKey << "] ";
	s << "[TrainID=" << static_cast<unsigned int>(message.trainID) << "] ";
	s << "[Sync=TSI] ";
	s << "[mpuTSI[0]=" << trainInformation.mpuTSI[0].c_str() << "] ";
	s << "[mpuTSI[1]=" << trainInformation.mpuTSI[1].c_str() << "] ";
	s << "[activeMPU=" << trainInformation.activeMPU << "] ";
	s << std::ends;
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());

	FUNCTION_EXIT;
}

/**
 * receiveAlarmIDsSpecialisedMessage (Broadcast)
 * Receive and consume a AlarmIDs message (synchrosnisation)
 */
void TrainEventSubscriber::receiveAlarmIDsSpecialisedMessage(
	const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateAlarmIDs& message )
{
	FUNCTION_ENTRY("receiveAlarmIDsSpecialisedMessage");

	TA_IRS_App::TrainStatus::TrainInformation trainInformation;
	std::vector<TA_IRS_App::TrainStatus::TrainInformation> stateUpdate;
	std::ostringstream s;

	trainInformation = convertAlarmIDs(message);
	stateUpdate.push_back(trainInformation);
	m_trainManager.onTrainStateUpdate(stateUpdate);
	manageControlOfTrain(message.trainID);

	s << "sync [BroadcastComms] messsage in from [Entity=" << m_senderEntityKey << "] ";
	s << "[TrainID=" << static_cast<unsigned int>(message.trainID) << "] ";
	s << "[Sync=AlarmIDs] ";
	s << "[AtcAlarmId=" << trainInformation.atcAlarmId.c_str() << "] ";
	s << "[Oa1AlarmId=" << trainInformation.oa1AlarmId.c_str() << "] ";
	s << "[RadioModeAlarmID=" << trainInformation.radioModeAlarmID.c_str() << "] ";
	s << std::ends;
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());

	FUNCTION_EXIT;
}

/**
 * receiveCallIDsSpecialisedMessage (StateUpdate)
 * Receive and consume a CallIDs message (synchrosnisation)
 * Be careful here!
 * This list of Call IDs is special.  Its the list of call IDs for
 * the location only; its a subset of the full list of call IDs
 */
void TrainEventSubscriber::receiveCallIDsSpecialisedMessage(
	const TA_Base_Bus::ITrainCorbaDef::TrainAgentStateUpdateCallIDs& message )
{
	FUNCTION_ENTRY("receiveCallIDsSpecialisedMessage");
	std::ostringstream s;

	s << "sync [StateUpdate] messsage in from [Entity=" << m_senderEntityKey << "] ";
	s << "[Sync=CallIDs] ";
	s << "[CallIDCount=" << message.callList.length() << "] ";
	std::list<unsigned long> newCallIdWatchList;
	for (unsigned int i = 0; i < message.callList.length(); ++i)
	{
		newCallIdWatchList.push_back( message.callList[i] );
		s << "[CallID=" << message.callList[i] << "] ";
	}
	m_trainManager.getTrainStatusManager().synchroniseWatchList( newCallIdWatchList );

	s << std::ends;
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
	
	FUNCTION_EXIT;
}

/**
 * receiveOperationModeSpecialisedMessage (StateUpdate)
 * Receive and consume a OperationMode message (synchrosnisation)
 */
void TrainEventSubscriber::receiveOperationModeSpecialisedMessage(
	const TA_Base_Bus::ITrainCorbaDef::TrainStateUpdateOperationMode& message )
{
 	FUNCTION_ENTRY("receiveOperationModeSpecialisedMessage");
	
	if(m_operationModeManager.isRunningControl() == false)
	{
		TrainOperationMode trainOperationMode;
		std::ostringstream s;

		trainOperationMode = convertOperationMode(message);
		m_operationModeManager.synchroniseOperationMode(trainOperationMode);

		s << "sync [StateUpdate] messsage in from [Entity=" << m_senderEntityKey << "] ";
		s << "[Sync=OperationMode] ";
		s << trainOperationMode;
		s << std::ends;
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
	}

	FUNCTION_EXIT;
}

/**
 * getRadioAgentEntityKey
 */
unsigned long TrainEventSubscriber::getRadioAgentEntityKey(unsigned long locationKey)
{
 	FUNCTION_ENTRY("getRadioAgentEntityKey");
	std::vector<TA_Base_Core::IEntityData*> entityList;
	TA_Base_Core::IEntityData*  entity;
	unsigned long  radioEntityKey;

	// Should be just one radio entity at each location
	entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::RadioEntityData::getStaticType(),locationKey);
	TA_ASSERT(entityList.size()>0,"Radio entity not found for this location.");
	TA_ASSERT(entityList.size()<2,"More than one radio entity defined for this location.");
	entity = entityList[0];
	radioEntityKey = entity->getKey();
	delete entity;

	FUNCTION_EXIT;
	return radioEntityKey;
}

/**
 * getAtsAgentEntityKey
 */
unsigned long TrainEventSubscriber::getAtsAgentEntityKey(unsigned long locationKey)
{
 	FUNCTION_ENTRY("getAtsAgentEntityKey");
	std::vector<TA_Base_Core::IEntityData*> entityList;
	TA_Base_Core::IEntityData*  entity;
	unsigned long  entityKey;

	// Should be just one ats entity at each location
	entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::AtsAgentEntityData::getStaticType(),locationKey);
	TA_ASSERT(entityList.size()>0,"ATS entity not found for this location.");
	TA_ASSERT(entityList.size()<2,"More than one ATS entity defined for this location.");
	entity = entityList[0];
	entityKey = entity->getKey();
	delete entity;

	FUNCTION_EXIT;
	return entityKey;
}

/**
 * getDutyAgentEntityKey
 */
unsigned long TrainEventSubscriber::getDutyAgentEntityKey(unsigned long locationKey)
{
 	FUNCTION_ENTRY("getDutyAgentEntityKey");

	std::vector<TA_Base_Core::IEntityData*> entityList;
	TA_Base_Core::IEntityData*  entity;
	unsigned long  dutyEntityKey;

	// Should be just one radio entity at each location
	entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::DutyAgentEntityData::getStaticType(), locationKey);
	TA_ASSERT(entityList.size()>0,"Duty agent entity not found for this location.");
	TA_ASSERT(entityList.size()<2,"More than one duty entity defined for this location.");
	entity = entityList[0];
	dutyEntityKey = entity->getKey();
	delete entity;

	FUNCTION_EXIT;

	return dutyEntityKey;
}

/**
 * createTrain
 */
void TrainEventSubscriber::createTrain(unsigned long trainID)
{
	m_trainManager.getTrainStatusManager().setTrainDetails(
		trainID,
		0, // location
		0, // cctvZone
		"", // serviceNumber
		TrainTypes::Track_Unknown, // track
		false, // isMute
		TrainTypes::AtsValid_Unknown // isLocalisationValid;
		);
}

/**
 * manageControlOfTrain
 * We need to handle the situation where two locations control a train
 * The symptom of this is; we receive a sync message from a train agent
 * for a train this agent is in control of.  In this case we should take control
 * of the train again.  ATS is the authority on train localisation, once it
 * communicates the real location of the train, the right train agent will finally
 * take control.
 */
void TrainEventSubscriber::manageControlOfTrain(unsigned long trainID)
{
	unsigned char ucTrainID;

	// Dont mamange control of trains in monitor mode
	if(m_operationModeManager.isRunningControl() == false)
	{
		return;
	}
	
	unsigned char radioMode = TrainMisc::Radio_Degraded;
    
    if (m_trainAgent.getAgentLocationType() == TrainTypes::LOC_OCC)
    {
        radioMode = TrainMisc::Radio_Normal;
    }

	ucTrainID = static_cast<unsigned char>(trainID);
	if(m_operationModeManager.isInControlOfTrain(ucTrainID) == true)
	{
		if(m_trainManager.getTrainTransactionManager().hasTrainsactions(ucTrainID) == false)
		{
            // take control of the train
            TrainMiscChangeModeCommand * x = new TrainMiscChangeModeCommand(
                m_trainAgent.getTrainMisc(), ucTrainID, radioMode, "");

			m_workQueueManager.perform(x);

            // monitor the train for OA1/ATC activity
            m_trainAgent.getTrainActivityMonitor().addTrain(ucTrainID);
			{
				std::ostringstream s;

				s << "Taking control of train on sync message in [trainID=" << trainID << "] " << std::ends;
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
			}
		}
	}
}

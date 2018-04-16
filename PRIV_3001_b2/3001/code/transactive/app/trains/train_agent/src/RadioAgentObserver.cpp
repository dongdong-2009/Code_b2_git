/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "app/trains/train_agent/src/RadioAgentObserver.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/IRadioMessageProcessor.h"
#include "app/trains/train_agent/src/IRadioAgentInterface.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

//#include "bus/managed_agent/src/AgentManager.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"

#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

namespace TA_IRS_App
{

    RadioAgentObserver::RadioAgentObserver( ITrainAgentConfiguration& configuration,
                                            IRadioAgentInterface& radioAgentInterface,
                                            IRadioMessageProcessor& radioMessageProcessor )
            : m_radioAgentInterface( radioAgentInterface ),
              m_radioMessageProcessor( radioMessageProcessor )
    {
        FUNCTION_ENTRY( "RadioAgentObserver" );

        // subscribes to comms messages using subscribeToMessages
        // this object is active as soon as it is created
        subscribeToMessages( configuration );

        FUNCTION_EXIT;
    }


    RadioAgentObserver::~RadioAgentObserver()
    {
        FUNCTION_ENTRY( "~RadioAgentObserver" );

        unsubscribeToMessages();

        FUNCTION_EXIT;
    }


    void RadioAgentObserver::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        // Check the agent operation mode using AgentManager, if this agent is not in the control state, return.

        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // extract the message into one of the following:
        TA_Base_Bus::IRadioCorbaDef::SDSNotificationEvent* sdsEvent = NULL;
        TA_Base_Bus::IRadioCorbaDef::CallStatusEvent* callStatusEvent = NULL;
        TA_Base_Bus::IRadioCorbaDef::SendDataNotification* sendDataEvent = NULL;

        if ( message.messageState >>= sdsEvent )
        {
            // convert the c++ type
            TA_IRS_Bus::TrainMessageData cppData;
            unsigned int dataLength = sdsEvent->messageData.length();

            for ( unsigned int index = 0; index < dataLength; ++index )
            {
                cppData.push_back( sdsEvent->messageData[ index ] );
            }

            // pass the message onto m_radioMessageProcessor
            m_radioMessageProcessor.processIncomingSds( cppData,
                                                        std::string( sdsEvent->sendingITSI.in() ),
                                                        "" ); // note the receiver is not used in this agent
                                                              // (the radio agent needs modification to include this information if necessary)
        }
        else if ( message.messageState >>= callStatusEvent )
        {
            // if the callStatus is IRadioCorbaDef::Deleted
            if ( TA_Base_Bus::IRadioCorbaDef::Deleted == callStatusEvent->callStatus )
            {
                // pass the message onto m_radioMessageProcessor
                m_radioMessageProcessor.processCallEndEvent( callStatusEvent->consoleKey,
                                                             callStatusEvent->callID );
            }

            // otherwise ignore the message
        }
        else if ( message.messageState >>= sendDataEvent )
        {
            // pass the message onto m_radioMessageProcessor
            m_radioMessageProcessor.processDataCallEndEvent( std::string( sendDataEvent->ITSI.in() ),
                                                             sendDataEvent->isSuccessful,
                                                             std::string( sendDataEvent->failReason.in() ) );
        }

        FUNCTION_EXIT;
    }


    void RadioAgentObserver::subscribeToMessages( ITrainAgentConfiguration& configuration )
    {
        FUNCTION_ENTRY( "subscribeToMessages" );

        // subscribe to the following message types:
        // RadioComms::TrainMessage
        // RadioComms::RadioCallStatus
        // RadioComms::RadioCircuitModeNotification

        unsigned long senderEntity = m_radioAgentInterface.getRadioAgentEntityKey();


		unsigned long locationKey = atol( TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
		std::vector<TA_Base_Core::IEntityData*> entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation("RadioServerCommand" , locationKey , true);
		TA_ASSERT(entityList.size()>0,"RadioServer entity not found for this location.");
		TA_ASSERT(entityList.size()<2,"More than one radio entity defined for this location.");


		unsigned long m_serverAgentKey = entityList[0]->getKey();

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"locationKey: %u, m_serverAgentKey: %u",locationKey, m_serverAgentKey  );


        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
            TA_Base_Core::RadioComms::TrainMessage,
            this,
            senderEntity,
            0,
            configuration.getLocationKey() );

        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
            TA_Base_Core::RadioComms::RadioCallStatus,
            this,
            senderEntity,
            0,
            configuration.getLocationKey() );

        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
            TA_Base_Core::RadioComms::RadioCircuitModeNotification,
            this,
            /*senderEntity,*/
			m_serverAgentKey,
            0,
            configuration.getLocationKey() );

        FUNCTION_EXIT;
    }


    void RadioAgentObserver::unsubscribeToMessages()
    {
        FUNCTION_ENTRY( "unsubscribeToMessages" );

        // unsubscribe from comms messages
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );

        FUNCTION_EXIT;
    }

} // TA_IRS_App

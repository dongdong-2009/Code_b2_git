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

#include "app/trains/train_agent/src/MessageSender.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

#include "bus/trains/TrainCorbaProxy/src/CallTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/CctvTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/CommonTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/PecTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/PaTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/RadioGroupTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/StateUpdateTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/TisTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/TrainInformationConversion.h"

//#include "bus/managed_agent/src/AgentManager.h"

#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/TrainAgentBroadcastComms_MessageTypes.h"
#include "core/message/types/TrainAgentComms_MessageTypes.h"
#include "core/message/types/TrainAgentStateUpdate_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

    MessageSender::MessageSender( ITrainAgentConfiguration& trainAgentConfiguration )
            : m_configuration( trainAgentConfiguration ),
              m_commsMessageSender( NULL ),
              m_broadcastCommsMessageSender( NULL ),
              m_stateUpdateMessageSender( NULL )
    {
        FUNCTION_ENTRY( "MessageSender" );

        using TA_Base_Core::MessagePublicationManager;
        MessagePublicationManager& messagePublicationManager = MessagePublicationManager::getInstance();

        // Using MessagePublicationManager, create the 3 message sender objects that are needed.
        m_commsMessageSender = messagePublicationManager.getCommsMessageSender( TA_Base_Core::TrainAgentComms::Context );
        m_broadcastCommsMessageSender = messagePublicationManager.getCommsMessageSender( TA_Base_Core::TrainAgentBroadcastComms::Context );
        m_stateUpdateMessageSender = messagePublicationManager.getStateUpdateMessageSender( TA_Base_Core::TrainAgentStateUpdate::Context );

        FUNCTION_EXIT;
    }


    MessageSender::~MessageSender()
    {
        FUNCTION_ENTRY( "~MessageSender" );

        // clean up the message sender objects
        delete m_commsMessageSender;
        m_commsMessageSender = NULL;

        delete m_broadcastCommsMessageSender;
        m_broadcastCommsMessageSender = NULL;

        delete m_stateUpdateMessageSender;
        m_stateUpdateMessageSender = NULL;

        FUNCTION_EXIT;
    }


    void MessageSender::sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainCommsSyncState& state )
    {
        FUNCTION_ENTRY( "sendStateUpdate( TrainCommsSyncState )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::StateUpdateTypesConversion::convertToCorba( state );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_broadcastCommsMessageSender->sendBroadcastCommsMessage(
            TA_Base_Core::TrainAgentBroadcastComms::TrainAgentSyncState,
            m_configuration.getEntityKey(),
            data );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", state.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainAlarmSyncState& state )
    {
        FUNCTION_ENTRY( "sendStateUpdate( TrainAlarmSyncState )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::StateUpdateTypesConversion::convertToCorba( state );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_stateUpdateMessageSender->sendStateUpdateMessage(
            TA_Base_Core::TrainAgentStateUpdate::TrainAgentSyncStateUpdate,
            m_configuration.getEntityKey(),
            m_configuration.getEntityName(),
            data );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", state.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainCallSyncState& state )
    {
        FUNCTION_ENTRY( "sendStateUpdate( TrainCallSyncState )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::StateUpdateTypesConversion::convertToCorba( state );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_stateUpdateMessageSender->sendStateUpdateMessage(
            TA_Base_Core::TrainAgentStateUpdate::TrainAgentSyncStateUpdate,
            m_configuration.getEntityKey(),
            m_configuration.getEntityName(),
            data );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", state.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainCctvSyncState& state )
    {
        FUNCTION_ENTRY( "sendStateUpdate( TrainCctvSyncState )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::StateUpdateTypesConversion::convertToCorba( state );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_broadcastCommsMessageSender->sendBroadcastCommsMessage(
            TA_Base_Core::TrainAgentBroadcastComms::TrainAgentSyncState,
            m_configuration.getEntityKey(),
            data );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", state.getLogString().c_str() );


        FUNCTION_EXIT;
    }


    void MessageSender::sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainPaSyncState& state )
    {
        FUNCTION_ENTRY( "sendStateUpdate( TrainPaSyncState )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::StateUpdateTypesConversion::convertToCorba( state );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_stateUpdateMessageSender->sendStateUpdateMessage(
            TA_Base_Core::TrainAgentStateUpdate::TrainAgentSyncStateUpdate,
            m_configuration.getEntityKey(),
            m_configuration.getEntityName(),
            data );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", state.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainPecSyncState& state )
    {
        FUNCTION_ENTRY( "sendStateUpdate( TrainPecSyncState )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::StateUpdateTypesConversion::convertToCorba( state );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_stateUpdateMessageSender->sendStateUpdateMessage(
            TA_Base_Core::TrainAgentStateUpdate::TrainAgentSyncStateUpdate,
            m_configuration.getEntityKey(),
            m_configuration.getEntityName(),
            data );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", state.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainTisSyncState& state )
    {
        FUNCTION_ENTRY( "sendStateUpdate( TrainTisSyncState )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::StateUpdateTypesConversion::convertToCorba( state );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_stateUpdateMessageSender->sendStateUpdateMessage(
            TA_Base_Core::TrainAgentStateUpdate::TrainAgentSyncStateUpdate,
            m_configuration.getEntityKey(),
            m_configuration.getEntityName(),
            data );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", state.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainSelectionMap& state )
    {
        FUNCTION_ENTRY( "sendStateUpdate( TrainSelectionMap )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::StateUpdateTypesConversion::convertToCorba( state );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_stateUpdateMessageSender->sendStateUpdateMessage(
            TA_Base_Core::TrainAgentStateUpdate::TrainAgentSyncStateUpdate,
            m_configuration.getEntityKey(),
            m_configuration.getEntityName(),
            data );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s",
                     TA_IRS_Bus::StateUpdateTypes::trainSelectionMapToString( state ).c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetailsSet& state )
    {
        FUNCTION_ENTRY( "sendStateUpdate( RadioCallDetailsSet )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::StateUpdateTypesConversion::convertToCorba( state );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_stateUpdateMessageSender->sendStateUpdateMessage(
            TA_Base_Core::TrainAgentStateUpdate::TrainAgentSyncStateUpdate,
            m_configuration.getEntityKey(),
            m_configuration.getEntityName(),
            data );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s",
                     TA_IRS_Bus::StateUpdateTypes::radioCallDetailsSetToString( state ).c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendStateUpdate( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& state )
    {
        FUNCTION_ENTRY( "sendStateUpdate( AgentOperationMode )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::TrainInformationConversion::convertToCorba( state );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_stateUpdateMessageSender->sendStateUpdateMessage(
            TA_Base_Core::TrainAgentStateUpdate::TrainAgentSyncStateUpdate,
            m_configuration.getEntityKey(),
            m_configuration.getEntityName(),
            data );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, //limin, CL-20691
                     "Sent %s", state.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::NonDutyTrainTSI& state )
    {
        FUNCTION_ENTRY( "sendStateUpdate( NonDutyTrainTSI )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::StateUpdateTypesConversion::convertToCorba( state );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_broadcastCommsMessageSender->sendBroadcastCommsMessage(
            TA_Base_Core::TrainAgentBroadcastComms::TrainCommunicationError,
            m_configuration.getEntityKey(),
            data );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", state.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendStateUpdate( const TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup& state )
    {
        FUNCTION_ENTRY( "sendStateUpdate( TrainRadioGroup )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::RadioGroupTypesConversion::convertToCorba( state );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_broadcastCommsMessageSender->sendBroadcastCommsMessage(
            TA_Base_Core::TrainAgentBroadcastComms::TrainAgentSyncState,
            m_configuration.getEntityKey(),
            data );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", state.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendClientUpdate( const TA_IRS_Bus::CctvTypes::CctvSwitchState& update )
    {
        FUNCTION_ENTRY( "sendClientUpdate( CctvSwitchState )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::CctvTypesConversion::convertToCorba( update );

        m_commsMessageSender->sendCommsMessage(
            TA_Base_Core::TrainAgentComms::CctvUpdate,
            m_configuration.getEntityKey(),
            data,
            0, // not used
            m_configuration.getLocationKey() );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", update.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendClientUpdate( const TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup& update )
    {
        FUNCTION_ENTRY( "sendClientUpdate( TrainRadioGroup )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::RadioGroupTypesConversion::convertToCorba( update );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_broadcastCommsMessageSender->sendBroadcastCommsMessage(
            TA_Base_Core::TrainAgentBroadcastComms::RadioGroupUpdate,
            m_configuration.getEntityKey(),
            data );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", update.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendClientUpdate( const TA_IRS_Bus::PaTypes::TrainLibraryInfo& update )
    {
        FUNCTION_ENTRY( "sendClientUpdate( TrainLibraryInfo )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::PaTypesConversion::convertToCorba( update );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_broadcastCommsMessageSender->sendBroadcastCommsMessage(
            TA_Base_Core::TrainAgentBroadcastComms::TrainPaLibraryUpdate,
            m_configuration.getEntityKey(),
            data );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", update.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendClientUpdate( const TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate& update )
    {
        FUNCTION_ENTRY( "sendClientUpdate( TisLibraryVersionUpdate )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::TisTypesConversion::convertToCorba( update );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_broadcastCommsMessageSender->sendBroadcastCommsMessage(
            TA_Base_Core::TrainAgentBroadcastComms::TrainTisLibraryUpdate,
            m_configuration.getEntityKey(),
            data );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", update.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendClientUpdate( const TA_IRS_Bus::TtisTypes::TisLibraryUpdate& update )
    {
        FUNCTION_ENTRY( "sendClientUpdate( TisLibraryUpdate )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::TisTypesConversion::convertToCorba( update );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_broadcastCommsMessageSender->sendBroadcastCommsMessage(
            TA_Base_Core::TrainAgentBroadcastComms::TrainTisLibraryUpdate,
            m_configuration.getEntityKey(),
            data );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", update.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendClientUpdate( const TA_IRS_Bus::TrainInformationTypes::TrainDetails& update )
    {
        FUNCTION_ENTRY( "sendClientUpdate( TrainDetails )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::TrainInformationConversion::convertToCorba( update );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_commsMessageSender->sendCommsMessage(
            TA_Base_Core::TrainAgentComms::TrainDetailsUpdate,
            m_configuration.getEntityKey(),
            data,
            0, // not used
            m_configuration.getLocationKey() );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", update.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendClientUpdate( const TA_IRS_Bus::TrainInformationTypes::TrainSelectionUpdate& update )
    {
        FUNCTION_ENTRY( "sendClientUpdate( TrainSelectionUpdate )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::TrainInformationConversion::convertToCorba( update );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_commsMessageSender->sendCommsMessage(
            TA_Base_Core::TrainAgentComms::TrainSelectionUpdate,
            m_configuration.getEntityKey(),
            data,
            0, // not used
            m_configuration.getLocationKey() );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", update.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendClientUpdate( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& update )
    {
        FUNCTION_ENTRY( "sendClientUpdate( AgentOperationMode )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::TrainInformationConversion::convertToCorba( update );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_commsMessageSender->sendCommsMessage(
            TA_Base_Core::TrainAgentComms::OperationModeUpdate,
            m_configuration.getEntityKey(),
            data,
            0, // not used
            m_configuration.getLocationKey() );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", update.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendClientUpdate( const TA_IRS_Bus::CallTypes::OccCallReset& update )
    {
        FUNCTION_ENTRY( "sendClientUpdate( OccCallReset )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::CallTypesConversion::convertToCorba( update );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_commsMessageSender->sendCommsMessage(
            TA_Base_Core::TrainAgentComms::CallDriverMessage,
            m_configuration.getEntityKey(),
            data,
            0, // not used
            m_configuration.getLocationKey() );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", update.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendClientUpdate( const TA_IRS_Bus::CallTypes::OccCallRequest& update )
    {
        FUNCTION_ENTRY( "sendClientUpdate( OccCallRequest )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::CallTypesConversion::convertToCorba( update );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_commsMessageSender->sendCommsMessage(
            TA_Base_Core::TrainAgentComms::CallDriverMessage,
            m_configuration.getEntityKey(),
            data,
            0, // not used
            m_configuration.getLocationKey() );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", update.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendClientUpdate( const TA_IRS_Bus::PecTypes::PecUpdate& update )
    {
        FUNCTION_ENTRY( "sendClientUpdate( PecUpdate )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::PecTypesConversion::convertToCorba( update );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_commsMessageSender->sendCommsMessage(
            TA_Base_Core::TrainAgentComms::PECMessage,
            m_configuration.getEntityKey(),
            data,
            0, // not used
            m_configuration.getLocationKey() );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", update.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendClientUpdate( const TA_IRS_Bus::PaTypes::PaUpdateEvent& update )
    {
        FUNCTION_ENTRY( "sendClientUpdate( PaUpdateEvent )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::PaTypesConversion::convertToCorba( update );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_commsMessageSender->sendCommsMessage(
            TA_Base_Core::TrainAgentComms::PAMessage,
            m_configuration.getEntityKey(),
            data,
            0, // not used
            m_configuration.getLocationKey() );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", update.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendClientUpdate( const TA_IRS_Bus::PaTypes::PaAnnouncementUpdate& update )
    {
        FUNCTION_ENTRY( "sendClientUpdate( PaAnnouncementUpdate )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::PaTypesConversion::convertToCorba( update );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_commsMessageSender->sendCommsMessage(
            TA_Base_Core::TrainAgentComms::LiveAnnouncementUpdate,
            m_configuration.getEntityKey(),
            data,
            0, // not used
            m_configuration.getLocationKey() );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", update.getLogString().c_str() );

        FUNCTION_EXIT;
    }


    void MessageSender::sendClientUpdate( const TA_IRS_Bus::TtisTypes::TisReturnStatus& update )
    {
        FUNCTION_ENTRY( "sendClientUpdate( TisReturnStatus )" );

        // Checks AgentManager to ensure the agent is in control mode, if not in control mode simply return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // uses the corba conversion methods from the train agent proxy library to convert the c++ to corba
        CORBA::Any data;
        data <<= TA_IRS_Bus::TisTypesConversion::convertToCorba( update );

        // sends the appropriate type of message as per the Train Agent Message Types diagrams
        m_commsMessageSender->sendCommsMessage(
            TA_Base_Core::TrainAgentComms::TISMessage,
            m_configuration.getEntityKey(),
            data,
            0, // not used
            m_configuration.getLocationKey() );


        // Also logs the message that is sent out (full message details)
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Sent %s", update.getLogString().c_str() );

        FUNCTION_EXIT;
    }


} // TA_IRS_App

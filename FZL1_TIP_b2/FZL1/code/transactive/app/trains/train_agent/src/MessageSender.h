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
#if !defined(MessageSender_H)
#define MessageSender_H

#include "app/trains/train_agent/src/IStateUpdateMessageSender.h"
#include "app/trains/train_agent/src/IMessageSender.h"

// forward declarations
namespace TA_IRS_App
{
    class ITrainAgentConfiguration;
}

namespace TA_Base_Core
{
    class CommsMessageSender;
    class StateUpdateMessageSender;
}

namespace TA_IRS_App
{

    /**
     * This is the implementation of both train message senders.
     * It will convert C++ types to CORBA, and send the messages.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:53 PM
     */
    class MessageSender : public IStateUpdateMessageSender,
                          public IMessageSender
    {

    public:

        /**
         * Constructor
         *
         * @param trainAgentConfiguration    This holds all the agent configuration
         */
        MessageSender( ITrainAgentConfiguration& trainAgentConfiguration );


        /**
         * Destructor
         */
        virtual ~MessageSender();


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainCommsSyncState& state );


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainAlarmSyncState& state );


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainCallSyncState& state );


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainCctvSyncState& state );


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainPaSyncState& state );


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainPecSyncState& state );


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainTisSyncState& state );


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::TrainSelectionMap& state );


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetailsSet& state );


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& state );


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::StateUpdateTypes::NonDutyTrainTSI& state );


        /**
         * Sends a state update
         *
         * @param state    The state to send
         */
        virtual void sendStateUpdate( const TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup& state );


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::CctvTypes::CctvSwitchState& update );


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup& update );


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::PaTypes::TrainLibraryInfo& update );


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate& update );


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::TtisTypes::TisLibraryUpdate& update );


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::TrainInformationTypes::TrainDetails& update );


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::TrainInformationTypes::TrainSelectionUpdate& update );


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& update );


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::CallTypes::OccCallReset& update );


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::CallTypes::OccCallRequest& update );


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::PecTypes::PecUpdate& update );


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::PaTypes::PaUpdateEvent& update );


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::PaTypes::PaAnnouncementUpdate& update );


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::TtisTypes::TisReturnStatus& update );


    private:

        /**
         * Constructor
         */
        MessageSender();


        /**
         * This holds the agent configuration
         */
        ITrainAgentConfiguration& m_configuration;


        /**
         * This is used to send comms messages
         */
        TA_Base_Core::CommsMessageSender* m_commsMessageSender;


        /**
         * This is used to send broadcast comms messages
         */
        TA_Base_Core::CommsMessageSender* m_broadcastCommsMessageSender;


        /**
         * This is used to send state update messages
         */
        TA_Base_Core::StateUpdateMessageSender* m_stateUpdateMessageSender;

    };

} // TA_IRS_App

#endif // !defined(MessageSender_H)

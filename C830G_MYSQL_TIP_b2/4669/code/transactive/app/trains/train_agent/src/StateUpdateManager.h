/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(StateUpdateManager_H)
#define StateUpdateManager_H

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "bus/trains/TrainTransactionModel/src/ITrainStateUpdateListener.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainStateUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/TrainStateUpdateCorbaProxy.h"

// forward declarations

namespace TA_IRS_App
{
    class ITrainAgentConfiguration;
    class IStateUpdateMessageSender;
    class IMessageSender;
    class TrainAgentStatusManager;
    class OperationModeManager;
    class RadioGroupCoordinator;
    class ITimsModeManagerInternal;
}

namespace TA_IRS_App
{

    /**
     * This listens for internal state changes, and broadcasts the messages out to inform peer and other agents of changes.
     *
     * Also it receives the updates from external agents, and sets the state locally internally.
     *
     * It is not created at agent startup, rather only at second stage initialisation.
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:03 PM
     */
    class StateUpdateManager : public TA_IRS_Bus::ITrainStateUpdateListener,
                               public TA_IRS_Bus::ITrainStateUpdateObserver
    {

    public:

        /**
         *
         * @param configuration    Agent configuration
         * @param statusManager    Used to get states, update states, and register for state changes
         * @param operationModeManager    Used to get operation mode, update operation mode, and register for operation mode changes
         * @param radioGroupCoordinator    Used to manage radio group resources
         * @param timsModeManager    Used to handle non duty train TSI updates
         * @param stateUpdateSender    Used to actually send state updates
         * @param messageSender    Used to rebroadcast global state updates to local clients
         */
        StateUpdateManager( ITrainAgentConfiguration& configuration,
                            TrainAgentStatusManager& statusManager,
                            OperationModeManager& operationModeManager,
                            RadioGroupCoordinator& radioGroupCoordinator,
                            ITimsModeManagerInternal& timsModeManager,
                            IStateUpdateMessageSender& stateUpdateSender,
                            IMessageSender& messageSender );


        /**
         * Destructor
         */
        virtual ~StateUpdateManager();


        /**
         * This will perform a full agent synchronisation, taking into account state from the peer, OCC, and Depot agents.
         */
        void performFullSync();


        /**
         * This will sync global states when the agent is first going to control mode.
         * This is for the case of the agent starting up in control mode.
         * It must get at least the global states before it can go to control mode.
         */
        void performFirstControlModeSync();


        /**
         * This will get the full global state of this agent
         */
        TA_IRS_Bus::StateUpdateTypes::FullGlobalState getGlobalState();


        /**
         * This will get the full local state of this agent
         */
        TA_IRS_Bus::StateUpdateTypes::FullLocalState getLocalState();


        /**
         * This is called to notify a station (or depot) it is isolated from the OCC.
         */
        void stationIsIsolated();


        /**
         * This is called to notify a station (or depot) it is no longer isolated from the OCC.
         * It will trigger a full resync and state broadcast.
         */
        void stationIsNoLongerIsolated();


        /**
         * This handles a train state changing, and sends the required state update message.
         *
         * @param state
         */
        virtual void processTrainStateChanged( TA_IRS_Bus::TrainStatePtr state );


        /**
         * This will receive a NonDutyTrainTSI message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processNonDutyTrainTSI( const TA_IRS_Bus::StateUpdateTypes::NonDutyTrainTSI& event,
                                             unsigned long senderEntity );


        /**
         * This will receive a AgentOperationMode message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processAgentOperationMode( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& event,
                                                unsigned long senderEntity );


        /**
         * This will receive a RadioCallDetailsSet message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processRadioCallDetailsSet( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetailsSet& event,
                                                 unsigned long senderEntity );


        /**
         * This will receive a TrainSelectionMap message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainSelectionMap( const TA_IRS_Bus::StateUpdateTypes::TrainSelectionMap& event,
                                               unsigned long senderEntity );


        /**
         * This will receive a TrainCommsSyncState message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainCommsSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainCommsSyncState& event,
                                                 unsigned long senderEntity );


        /**
         * This will receive a TrainCallSyncState message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainCallSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainCallSyncState& event,
                                                unsigned long senderEntity );


        /**
         * This will receive a TrainPaSyncState message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainPaSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainPaSyncState& event,
                                              unsigned long senderEntity );


        /**
         * This will receive a TrainPecSyncState message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainPecSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainPecSyncState& event,
                                               unsigned long senderEntity );


        /**
         * This will receive a TrainTisSyncState message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainTisSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainTisSyncState& event,
                                               unsigned long senderEntity );


        /**
         * This will receive a TrainAlarmSyncState message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainAlarmSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainAlarmSyncState& event,
                                                 unsigned long senderEntity );


        /**
         * This will receive a TrainRadioGroup message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainRadioGroup( const TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup& event,
                                             unsigned long senderEntity );


        /**
         * This will receive a TrainCctvSyncState message from the local train agent.
         *
         * @param event    The event to process
         * @param senderEntity    the entity key of the entity that sent this message
         */
        virtual void processTrainCctvSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainCctvSyncState& event,
                                                unsigned long senderEntity );


        /**
         * The operation mode of the agent has changed.
         *
         * @param operationState    The new operation mode
         */
        virtual void notifyOperationStateChange( TA_Base_Core::EOperationMode operationState );//limin++, CL-29779


    private:

        /**
         * Private default cosntructor
         */
        StateUpdateManager();


        /**
         * This will ensure the system gets a copy of the global train states, applicable system wide.
         */
        void syncGlobalStates();


        /**
         * This will ensure the system gets a copy of the local train states, from the control mode peer if present.
         */
        void syncLocalStates();


        /**
         * This will take full global state received from another agent, and merge it with this agent's own state.
         *
         * @param fullGlobalState    The full state to set
         */
        void processFullGlobalState( const TA_IRS_Bus::StateUpdateTypes::FullGlobalState& fullGlobalState );


        /**
         * This will take full local state received from another agent, and merge it with this agent's own state.
         *
         * @param fullLocalState    The full state to set
         */
        void processFullLocalState( const TA_IRS_Bus::StateUpdateTypes::FullLocalState& fullLocalState );


        /**
         * This will broadcast the agents entire state.
         * It will update any peer agents, and also agents system wide.
         */
        void broadcastCurrentState();


        /**
         * Holds agent configuration items
         */
        ITrainAgentConfiguration& m_configuration;


        /**
         * Holds state, and notifies of state changes
         */
        TrainAgentStatusManager& m_statusManager;


        /**
         * This is used to notify this class of operation mode changes, and aso sync state is sent to it
         */
        OperationModeManager& m_operationModeManager;


        /**
         * Used to re-broadcast global states when received from other agents.
         */
        IMessageSender& m_messageSender;


        /**
         * Used to actually send the state update messages
         */
        IStateUpdateMessageSender& m_stateUpdateSender;


        /**
         * The object managing train radio groups
         */
        RadioGroupCoordinator& m_radioGroupCoordinator;


        /**
         * This is used to handle the non duty train TSI messages.
         */
        ITimsModeManagerInternal& m_timsModeManager;


        /**
         * This is a corba proxy to the local train agent
         */
        TA_IRS_Bus::TrainStateUpdateCorbaProxyPtr m_localPeer;


        /**
         * This is a corba proxy to the occ train agent
         */
        TA_IRS_Bus::TrainStateUpdateCorbaProxyPtr m_occPeer;


        /**
         * This is a corba proxy to the depot train agent
         */
        TA_IRS_Bus::TrainStateUpdateCorbaProxyPtr m_depotPeer;

    };

} // TA_IRS_App

#endif // !defined(StateUpdateManager_H)

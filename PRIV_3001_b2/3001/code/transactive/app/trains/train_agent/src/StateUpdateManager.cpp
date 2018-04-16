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

#include "app/trains/train_agent/src/StateUpdateManager.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/IStateUpdateMessageSender.h"
#include "app/trains/train_agent/src/IMessageSender.h"
#include "app/trains/train_agent/src/ITimsModeManagerInternal.h"
#include "app/trains/train_agent/src/TrainAgentStatusManager.h"
#include "app/trains/train_agent/src/OperationModeManager.h"
#include "app/trains/train_agent/src/RadioGroupCoordinator.h"

#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
//#include "bus/managed_agent/src/AgentManager.h"

#include "core/data_access_interface/TrainStateDai/src/PersistedTrainStateAccessFactory.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

    StateUpdateManager::StateUpdateManager( ITrainAgentConfiguration& configuration,
                                            TrainAgentStatusManager& statusManager,
                                            OperationModeManager& operationModeManager,
                                            RadioGroupCoordinator& radioGroupCoordinator,
                                            ITimsModeManagerInternal& timsModeManager,
                                            IStateUpdateMessageSender& stateUpdateSender,
                                            IMessageSender& messageSender )
            : m_configuration( configuration ),
              m_statusManager( statusManager ),
              m_operationModeManager( operationModeManager ),
              m_messageSender( messageSender ),
              m_stateUpdateSender( stateUpdateSender ),
              m_radioGroupCoordinator( radioGroupCoordinator ),
              m_timsModeManager( timsModeManager ),
              m_localPeer( TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainStateUpdateCorbaProxy() ),
              m_occPeer( TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createOccTrainStateUpdateCorbaProxy() ),
              m_depotPeer( TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createDepotTrainStateUpdateCorbaProxy() )
    {
        FUNCTION_ENTRY( "StateUpdateManager" );

        // register for state changes for all train states, using m_statusManager
        m_statusManager.registerForStateChanges( TA_IRS_Bus::CommonTypes::CoreResource, *this );
        m_statusManager.registerForStateChanges( TA_IRS_Bus::CommonTypes::TrainAlarmResource, *this );
        m_statusManager.registerForStateChanges( TA_IRS_Bus::CommonTypes::TrainCctvResource, *this );
        m_statusManager.registerForStateChanges( TA_IRS_Bus::CommonTypes::TrainTisResource, *this );
        m_statusManager.registerForStateChanges( TA_IRS_Bus::CommonTypes::TrainPaResource, *this );
        m_statusManager.registerForStateChanges( TA_IRS_Bus::CommonTypes::TrainCallResource, *this );
        m_statusManager.registerForStateChanges( TA_IRS_Bus::CommonTypes::TrainPecResource, *this );

        // using m_localPeer, register this object for updates
        // there is no need to register using the other peers,
        // the local peer registers for everything that this agent needs.
        m_localPeer->addObserver( this );

        FUNCTION_EXIT;
    }


    StateUpdateManager::~StateUpdateManager()
    {
        FUNCTION_ENTRY( "~StateUpdateManager" );

        // unsubscribe for updates from m_localPeer
        m_localPeer->removeObserver( this );

        // deregister for train state updates from m_statusManager
        m_statusManager.deregisterForStateChanges( TA_IRS_Bus::CommonTypes::CoreResource, *this );
        m_statusManager.deregisterForStateChanges( TA_IRS_Bus::CommonTypes::TrainAlarmResource, *this );
        m_statusManager.deregisterForStateChanges( TA_IRS_Bus::CommonTypes::TrainCctvResource, *this );
        m_statusManager.deregisterForStateChanges( TA_IRS_Bus::CommonTypes::TrainTisResource, *this );
        m_statusManager.deregisterForStateChanges( TA_IRS_Bus::CommonTypes::TrainPaResource, *this );
        m_statusManager.deregisterForStateChanges( TA_IRS_Bus::CommonTypes::TrainCallResource, *this );
        m_statusManager.deregisterForStateChanges( TA_IRS_Bus::CommonTypes::TrainPecResource, *this );

        FUNCTION_EXIT;
    }


    void StateUpdateManager::performFullSync()
    {
        FUNCTION_ENTRY( "performFullSync" );

        // this will perform a total resynchronisation,
        // it will first sync the global states, then sync the local states
		// DTL-1771 : No need to synch global train data
        //syncGlobalStates();
        syncLocalStates();

        FUNCTION_EXIT;
    }

    
    void StateUpdateManager::performFirstControlModeSync()
    {
        FUNCTION_ENTRY( "performFullSync" );

        // just sync the global states
		// DTL-1771 : No need to synch global train data
        //syncGlobalStates();

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::StateUpdateTypes::FullGlobalState StateUpdateManager::getGlobalState()
    {
        FUNCTION_ENTRY( "getGlobalState" );

        // Create a FullGlobalState struct
        TA_IRS_Bus::StateUpdateTypes::FullGlobalState fullState;

        // from m_statusManager, get all train comms states
        TA_IRS_Bus::TrainIdToStateMap commsStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::CoreResource );

        for ( TA_IRS_Bus::TrainIdToStateMap::iterator commsStateIter = commsStates.begin();
              commsStates.end() != commsStateIter; ++commsStateIter )
        {
            // cast to a TrainCommsStatePtr
            TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( commsStateIter->second );
            TA_ASSERT( NULL != commsState.get(), "Cannot cast to correct state" );

            TA_IRS_Bus::TrainStateLock lock( commsState );

            // get the sync state, and add it to the list of TrainCommsSyncState
            fullState.commsStates.push_back( commsState->getSyncState() );
        }

        // from m_statusManager, get all train cctv states
        TA_IRS_Bus::TrainIdToStateMap cctvStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainCctvResource );

        // for each state
        for ( TA_IRS_Bus::TrainIdToStateMap::iterator cctvStateIter = cctvStates.begin();
              cctvStates.end() != cctvStateIter; ++cctvStateIter )
        {
            // cast to a TrainCctvStatePtr
            TrainCctvStatePtr cctvState = boost::dynamic_pointer_cast< TrainCctvState >( cctvStateIter->second );
            TA_ASSERT( NULL != cctvState.get(), "Cannot cast to correct state" );

            TA_IRS_Bus::TrainStateLock lock( cctvState );

            // get the sync state, and add it to the list of TrainCctvSyncState
            fullState.cctvStates.push_back( cctvState->getSyncState() );
        }

        // from m_radioGroupCoordinator getRadioGroups
        fullState.radioGroupMap = m_radioGroupCoordinator.getRadioGroups();

        FUNCTION_EXIT;
        return fullState;
    }


    TA_IRS_Bus::StateUpdateTypes::FullLocalState StateUpdateManager::getLocalState()
    {
        FUNCTION_ENTRY( "getLocalState" );

        // Create a FullLocalState struct
        TA_IRS_Bus::StateUpdateTypes::FullLocalState fullState;

        // get the AgentOperationMode from m_operationModeManager
        fullState.operationMode = m_operationModeManager.getCurrentOperationMode();

        // get the call details from m_statusManager
        fullState.ongoingCalls = m_statusManager.getRadioCallDetails();

        // get the train selection map from m_statusManager
        fullState.selectionLists = m_statusManager.getTrainSelectionMap();

        // from m_statusManager, get all train alarm states
        TA_IRS_Bus::TrainIdToStateMap alarmStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainAlarmResource );

        for ( TA_IRS_Bus::TrainIdToStateMap::iterator alarmStateIter = alarmStates.begin();
              alarmStates.end() != alarmStateIter; ++alarmStateIter )
        {
            // cast to a TrainAlarmStatePtr
            TrainAlarmStatePtr alarmState = boost::dynamic_pointer_cast< TrainAlarmState >( alarmStateIter->second );
            TA_ASSERT( NULL != alarmState.get(), "Cannot cast to correct state" );

            TA_IRS_Bus::TrainStateLock lock( alarmState );

            // get the sync state, and add it to the list of TrainAlarmSyncState
            fullState.alarmStates.push_back( alarmState->getSyncState() );
        }

        // from m_statusManager, get all train call states
        TA_IRS_Bus::TrainIdToStateMap callStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainCallResource );

        // for each state
        for ( TA_IRS_Bus::TrainIdToStateMap::iterator callStateIter = callStates.begin();
              callStates.end() != callStateIter; ++callStateIter )
        {
            // cast to a TrainCallStatePtr
            TrainCallStatePtr callState = boost::dynamic_pointer_cast< TrainCallState >( callStateIter->second );
            TA_ASSERT( NULL != callState.get(), "Cannot cast to correct state" );

            TA_IRS_Bus::TrainStateLock lock( callState );

            // get the sync state, and add it to the list of TrainCallSyncState
            fullState.callStates.push_back( callState->getSyncState() );
        }

        // from m_statusManager, get all train pa states
        TA_IRS_Bus::TrainIdToStateMap paStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainPaResource );

        // for each state
        for ( TA_IRS_Bus::TrainIdToStateMap::iterator paStateIter = paStates.begin();
              paStates.end() != paStateIter; ++paStateIter )
        {
            // cast to a TrainPaStatePtr
            TrainPaStatePtr paState = boost::dynamic_pointer_cast< TrainPaState >( paStateIter->second );
            TA_ASSERT( NULL != paState.get(), "Cannot cast to correct state" );

            TA_IRS_Bus::TrainStateLock lock( paState );

            // get the sync state, and add it to the list of TrainPaSyncState
            fullState.paStates.push_back( paState->getSyncState() );
        }

        // from m_statusManager, get all train pec states
        TA_IRS_Bus::TrainIdToStateMap pecStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainPecResource );

        // for each state
        for ( TA_IRS_Bus::TrainIdToStateMap::iterator pecStateIter = pecStates.begin();
              pecStates.end() != pecStateIter; ++pecStateIter )
        {
            // cast to a TrainPecStatePtr
            TrainPecStatePtr pecState = boost::dynamic_pointer_cast< TrainPecState >( pecStateIter->second );
            TA_ASSERT( NULL != pecState.get(), "Cannot cast to correct state" );

            TA_IRS_Bus::TrainStateLock lock( pecState );

            // get the sync state, and add it to the list of TrainPecSyncState
            fullState.pecStates.push_back( pecState->getSyncState() );
        }

        // from m_statusManager, get all train tis states
        TA_IRS_Bus::TrainIdToStateMap tisStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainTisResource );

        // for each state
        for ( TA_IRS_Bus::TrainIdToStateMap::iterator tisStateIter = tisStates.begin();
              tisStates.end() != tisStateIter; ++tisStateIter )
        {
            // cast to a TrainTisStatePtr
            TrainTisStatePtr tisState = boost::dynamic_pointer_cast< TrainTisState >( tisStateIter->second );
            TA_ASSERT( NULL != tisState.get(), "Cannot cast to correct state" );

            TA_IRS_Bus::TrainStateLock lock( tisState );

            // get the sync state, and add it to the list of TrainTisSyncState
            fullState.tisStates.push_back( tisState->getSyncState() );
        }

        FUNCTION_EXIT;

        return fullState;
    }


    void StateUpdateManager::stationIsIsolated()
    {
        FUNCTION_ENTRY( "stationIsIsolated" );

        // This can only occur at a station or a depot.
        // An isolated station will have no effect, this is only here for completeness

        FUNCTION_EXIT;
    }


    void StateUpdateManager::stationIsNoLongerIsolated()
    {
        FUNCTION_ENTRY( "stationIsNoLongerIsolated" );

        // This can only occur at a station or a depot.
        // Check the agent location type from m_configuration, if it is the OCC, return.

        if ( TA_Base_Core::ILocation::OCC == m_configuration.getAgentLocationType() )
        {
            FUNCTION_EXIT;
            return;
        }

        // Otherwise, coming out of isolation, perform a full agent synchronisation:
        performFullSync();

        // then broadcast out all the current known states
        // (in case any state was updated in this agent during isolation)
        broadcastCurrentState();

        FUNCTION_EXIT;
    }


    void StateUpdateManager::processTrainStateChanged( TA_IRS_Bus::TrainStatePtr state )
    {
        FUNCTION_ENTRY( "processTrainStateChanged" );

        // from the state object, check isSynchronisationRequired
        // if not, return

        if ( false == state->isSynchronisationRequired() )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the type of state received
        TA_IRS_Bus::CommonTypes::ETrainResource resource = state->getResourceCategory();

        // for each state type
        // acquire the state lock
        // down cast the state
        // get the sync state
        // Send the appropriate state update using m_stateUpdateSender
        // setSynchronisationComplete

        TA_IRS_Bus::TrainStateLock lock( state );

        switch ( resource )
        {

            case TA_IRS_Bus::CommonTypes::CoreResource:
            {
                TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( state );
                TA_ASSERT( NULL != commsState.get(), "Cannot cast to correct state" );
                //rollback for processing OA1,ActiveMPU status 
				 m_stateUpdateSender.sendStateUpdate( commsState->getSyncState() );
                // now sync the persisted state if required

                if ( true == commsState->hasPeristedStateChanged() )
                {
                    // persist the state to the db
                    try
                    {

                        TA_IRS_Core::PersistedTrainStateAccessFactory::saveTrainState( commsState->getTrainId(),
                                                                                       commsState->getPrimaryTsi(),
                                                                                       commsState->getSecondaryTsi(),
                                                                                       commsState->getPaLibraryVersion(),
                                                                                       commsState->getTtisLibraryVersion(),
                                                                                       commsState->getTtisScheduleVersion(),
                                                                                       commsState->getTimestamp() );
                    }
                    catch ( TA_Base_Core::TransactiveException& e )
                    {
                        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
                    }

                    // reset the flag
                    commsState->resetPersistedStateChanged();
                }
            }

            break;

            case TA_IRS_Bus::CommonTypes::TrainAlarmResource:
            {
                TrainAlarmStatePtr alarmState = boost::dynamic_pointer_cast< TrainAlarmState >( state );
                TA_ASSERT( NULL != alarmState.get(), "Cannot cast to correct state" );

                m_stateUpdateSender.sendStateUpdate( alarmState->getSyncState() );
            }

            break;

            case TA_IRS_Bus::CommonTypes::TrainCctvResource:
            {
                TrainCctvStatePtr cctvState = boost::dynamic_pointer_cast< TrainCctvState >( state );
                TA_ASSERT( NULL != cctvState.get(), "Cannot cast to correct state" );

                m_stateUpdateSender.sendStateUpdate( cctvState->getSyncState() );
            }

            break;

            case TA_IRS_Bus::CommonTypes::TrainTisResource:
            {
                TrainTisStatePtr tisState = boost::dynamic_pointer_cast< TrainTisState >( state );
                TA_ASSERT( NULL != tisState.get(), "Cannot cast to correct state" );

                m_stateUpdateSender.sendStateUpdate( tisState->getSyncState() );
            }

            break;

            case TA_IRS_Bus::CommonTypes::TrainPaResource:
            {
                TrainPaStatePtr paState = boost::dynamic_pointer_cast< TrainPaState >( state );
                TA_ASSERT( NULL != paState.get(), "Cannot cast to correct state" );

                m_stateUpdateSender.sendStateUpdate( paState->getSyncState() );
            }

            break;

            case TA_IRS_Bus::CommonTypes::TrainCallResource:
            {
                TrainCallStatePtr callState = boost::dynamic_pointer_cast< TrainCallState >( state );
                TA_ASSERT( NULL != callState.get(), "Cannot cast to correct state" );

                m_stateUpdateSender.sendStateUpdate( callState->getSyncState() );
            }

            break;

            case TA_IRS_Bus::CommonTypes::TrainPecResource:
            {
                TrainPecStatePtr pecState = boost::dynamic_pointer_cast< TrainPecState >( state );
                TA_ASSERT( NULL != pecState.get(), "Cannot cast to correct state" );

                m_stateUpdateSender.sendStateUpdate( pecState->getSyncState() );
            }

            break;

            default:
                TA_ASSERT( false, "Invalid state type" );
                break;
        }

        state->setSynchronisationComplete();

        FUNCTION_EXIT;
    }


    void StateUpdateManager::processNonDutyTrainTSI( const TA_IRS_Bus::StateUpdateTypes::NonDutyTrainTSI& event,
                                                     unsigned long senderEntity )
    {
        FUNCTION_ENTRY( "processNonDutyTrainTSI" );

        // if the sender entity is the same as this entity
        // (check senderEntity against the entity key from m_configuration) then return

        if ( senderEntity == m_configuration.getEntityKey() )
        {
            FUNCTION_EXIT;
            return;
        }

        // pass the message onto m_timsModeManager
        m_timsModeManager.processNonDutyTrainTSI( event );

        FUNCTION_EXIT;
    }


    void StateUpdateManager::processAgentOperationMode( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& event,
                                                        unsigned long senderEntity )
    {
        FUNCTION_ENTRY( "processAgentOperationMode" );

        // if the sender entity is the same as this entity (check senderEntity against the entity key from m_configuration)
        // and this agent is in control mode (check AgentManager) then return

        if ( ( senderEntity == m_configuration.getEntityKey() ) &&
             ( true == OperationModeManager::isRunningControl() ) )
        {
            FUNCTION_EXIT;
            return;
        }

        // Pass the data onto m_operationModeManager
        m_operationModeManager.syncCurrentOperationMode( event );

        FUNCTION_EXIT;
    }


    void StateUpdateManager::processRadioCallDetailsSet( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetailsSet& event,
                                                         unsigned long senderEntity )
    {
        FUNCTION_ENTRY( "processRadioCallDetailsSet" );

        // if the sender entity is the same as this entity (check senderEntity against the entity key from m_configuration)
        // and this agent is in control mode (check AgentManager) then return

        if ( ( senderEntity == m_configuration.getEntityKey() ) &&
             ( true == OperationModeManager::isRunningControl() ) )
        {
            FUNCTION_EXIT;
            return;
        }

        // pass the data onto m_statusManager
        m_statusManager.syncRadioCallDetails( event );

        FUNCTION_EXIT;
    }


    void StateUpdateManager::processTrainSelectionMap( const TA_IRS_Bus::StateUpdateTypes::TrainSelectionMap& event,
                                                       unsigned long senderEntity )
    {
        FUNCTION_ENTRY( "processTrainSelectionMap" );

        // if the sender entity is the same as this entity (check senderEntity against the entity key from m_configuration)
        // and this agent is in control mode (check AgentManager) then return

        if ( ( senderEntity == m_configuration.getEntityKey() ) &&
             ( true == OperationModeManager::isRunningControl() ) )
        {
            FUNCTION_EXIT;
            return;
        }

        // pass the data onto m_statusManager
        m_statusManager.syncTrainSelection( event );

        FUNCTION_EXIT;
    }


    void StateUpdateManager::processTrainCommsSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainCommsSyncState& event,
                                                         unsigned long senderEntity )
    {
        FUNCTION_ENTRY( "processTrainCommsSyncState" );

        // if the sender entity is the same as this entity (check senderEntity against the entity key from m_configuration)
        // and this agent is in control mode (check AgentManager) then return

        if ( ( senderEntity == m_configuration.getEntityKey() ) &&
             ( true == OperationModeManager::isRunningControl() ) )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the comms state for the given train from m_statusManager
        TrainCommsStatePtr state = m_statusManager.getTrainCommsState( event.trainId );

        // get the original TIS versions, and PA versions from the state object
        //unsigned short paLibraryVersion = state->getPaLibraryVersion();
        unsigned long paLibraryVersion = state->getPaLibraryVersion();
        unsigned long ttisLibraryVersion = state->getTtisLibraryVersion();
        unsigned long ttisScheduleVersion = state->getTtisScheduleVersion();

        // set the sync state on the state object
        state->setSyncState( event );

        // get the latest train details, TIS versions, and PA versions from the state object.

        // send the train details to local clients using m_messageSender
        m_messageSender.sendClientUpdate( state->getTrainDetails() );

        // if the PA versions have changed, send the PA library updates to local clients using m_messageSender
        if ( state->getPaLibraryVersion() != paLibraryVersion )
        {
            TA_IRS_Bus::PaTypes::TrainLibraryInfo paLibraryInfo;
            paLibraryInfo.trainId = event.trainId;
            paLibraryInfo.libraryVersion = state->getPaLibraryVersion();

            m_messageSender.sendClientUpdate( paLibraryInfo );
        }

        // if the TTIS versions have changed, send the TTIS library updates to local clients using m_messageSender
        if ( ( state->getTtisLibraryVersion() != ttisLibraryVersion ) ||
             ( state->getTtisScheduleVersion() != ttisScheduleVersion ) )
        {
            TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate tisLibraryInfo;
            tisLibraryInfo.trainId = event.trainId;
            tisLibraryInfo.libraryVersion = state->getTtisLibraryVersion();
            tisLibraryInfo.scheduleVersion = state->getTtisScheduleVersion();

            m_messageSender.sendClientUpdate( tisLibraryInfo );
        }

        FUNCTION_EXIT;
    }


    void StateUpdateManager::processTrainCallSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainCallSyncState& event,
                                                        unsigned long senderEntity )
    {
        FUNCTION_ENTRY( "processTrainCallSyncState" );

        // if the sender entity is the same as this entity (check senderEntity against the entity key from m_configuration)
        // and this agent is in control mode (check AgentManager) then return

        if ( ( senderEntity == m_configuration.getEntityKey() ) &&
             ( true == OperationModeManager::isRunningControl() ) )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the call state for the given train from m_statusManager
        TrainCallStatePtr state = m_statusManager.getTrainCallState( event.trainId );

        // set the sync state on the state object
        state->setSyncState( event );

        FUNCTION_EXIT;
    }


    void StateUpdateManager::processTrainPaSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainPaSyncState& event,
                                                      unsigned long senderEntity )
    {
        FUNCTION_ENTRY( "processTrainPaSyncState" );

        // if the sender entity is the same as this entity (check senderEntity against the entity key from m_configuration)
        // and this agent is in control mode (check AgentManager) then return

        if ( ( senderEntity == m_configuration.getEntityKey() ) &&
             ( true == OperationModeManager::isRunningControl() ) )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the pa state for the given train from m_statusManager
        TrainPaStatePtr state = m_statusManager.getTrainPaState( event.trainId );

        // set the sync state on the state object
        state->setSyncState( event );

        FUNCTION_EXIT;
    }


    void StateUpdateManager::processTrainPecSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainPecSyncState& event,
                                                       unsigned long senderEntity )
    {
        FUNCTION_ENTRY( "processTrainPecSyncState" );

        // if the sender entity is the same as this entity (check senderEntity against the entity key from m_configuration)
        // and this agent is in control mode (check AgentManager) then return

        if ( ( senderEntity == m_configuration.getEntityKey() ) &&
             ( true == OperationModeManager::isRunningControl() ) )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the pec state for the given train from m_statusManager
        TrainPecStatePtr state = m_statusManager.getTrainPecState( event.trainId );

        // set the sync state on the state object
        state->setSyncState( event );

        FUNCTION_EXIT;
    }


    void StateUpdateManager::processTrainTisSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainTisSyncState& event,
                                                       unsigned long senderEntity )
    {
        FUNCTION_ENTRY( "processTrainTisSyncState" );

        // if the sender entity is the same as this entity (check senderEntity against the entity key from m_configuration)
        // and this agent is in control mode (check AgentManager) then return

        if ( ( senderEntity == m_configuration.getEntityKey() ) &&
             ( true == OperationModeManager::isRunningControl() ) )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the tis state for the given train from m_statusManager
        TrainTisStatePtr state = m_statusManager.getTrainTisState( event.trainId );

        // set the sync state on the state object
        state->setSyncState( event );

        FUNCTION_EXIT;
    }


    void StateUpdateManager::processTrainAlarmSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainAlarmSyncState& event,
                                                         unsigned long senderEntity )
    {
        FUNCTION_ENTRY( "processTrainAlarmSyncState" );

        // if the sender entity is the same as this entity (check senderEntity against the entity key from m_configuration)
        // and this agent is in control mode (check AgentManager) then return

        if ( ( senderEntity == m_configuration.getEntityKey() ) &&
             ( true == OperationModeManager::isRunningControl() ) )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the alarm state for the given train from m_statusManager
        TrainAlarmStatePtr state = m_statusManager.getTrainAlarmState( event.trainId );

        // set the sync state on the state object
        state->setSyncState( event );

        FUNCTION_EXIT;
    }


    void StateUpdateManager::processTrainRadioGroup( const TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup& event,
                                                     unsigned long senderEntity )
    {
        FUNCTION_ENTRY( "processTrainRadioGroup" );

        // if the sender entity is the same as this entity (check senderEntity against the entity key from m_configuration)
        // and this agent is in control mode (check AgentManager) then return

        if ( ( senderEntity == m_configuration.getEntityKey() ) &&
             ( true == OperationModeManager::isRunningControl() ) )
        {
            FUNCTION_EXIT;
            return;
        }

        // pass the sync state onto m_radioGroupCoordinator
        m_radioGroupCoordinator.syncRadioGroup( event );

        // re-get the latest state for the group from m_radioGroupCoordinator, and send updates to local clients using m_messageSender
        m_messageSender.sendClientUpdate( m_radioGroupCoordinator.getRadioGroup( event.groupTsi ) );

        FUNCTION_EXIT;
    }


    void StateUpdateManager::processTrainCctvSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainCctvSyncState& event,
                                                        unsigned long senderEntity )
    {
        FUNCTION_ENTRY( "processTrainCctvSyncState" );

        // if the sender entity is the same as this entity (check senderEntity against the entity key from m_configuration)
        // and this agent is in control mode (check AgentManager) then return

        if ( ( senderEntity == m_configuration.getEntityKey() ) &&
             ( true == OperationModeManager::isRunningControl() ) )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the cctv state for the given train from m_statusManager
        TrainCctvStatePtr state = m_statusManager.getTrainCctvState( event.trainId );

        // set the sync state on the state object
        state->setSyncState( event );

        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( event.trainId );

        // get the latest CCTV switch state for the train from the state object, and send updates to local clients using m_messageSender
        TA_IRS_Bus::CctvTypes::CctvSwitchState clientUpdate = state->getCctvSwitchState();

        clientUpdate.stageId = commsState->getCurrentCctvZone().getCurrentStage();

        m_messageSender.sendClientUpdate( clientUpdate );

        FUNCTION_EXIT;
    }


    void StateUpdateManager::syncGlobalStates()
    {
        FUNCTION_ENTRY( "syncGlobalStates" );

        // This will first attempt the local peer (if this agent isnt in control mode)
        // Then it will try the occ, then the depot.

        bool success = false;

        // if the agent is not in control mode,
        // use m_localPeer to get the full global state

        if ( false == OperationModeManager::isRunningControl() )
        {
            try
            {
                TA_IRS_Bus::StateUpdateTypes::FullGlobalState fullState = m_localPeer->getGlobalState();

                // if successful, process the state and set the flag
                processFullGlobalState( fullState );
                success = true;
            }
            catch ( TA_IRS_Bus::AgentCommunicationException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
            }
        }

        // if success is false and the agent is not the occ agent
        // use m_occPeer to get the full global state
        if ( ( false == success ) &&
             ( TA_Base_Core::ILocation::OCC != m_configuration.getAgentLocationType() ) )
        {
            try
            {
                TA_IRS_Bus::StateUpdateTypes::FullGlobalState fullState = m_occPeer->getGlobalState();

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Performing Train State Synchronization from OccTrainAgent..");
                // if successful, process the state and set the flag
                processFullGlobalState( fullState );
                success = true;
            }
            catch ( TA_IRS_Bus::AgentCommunicationException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Unable to perform Train State Synchronization from OccTrainAgent..");
            }
        }

        // if success is false and the agent is not the depot agent
        // use m_depotPeer to get the full global state
        if ( ( false == success ) &&
             ( TA_Base_Core::ILocation::DPT != m_configuration.getAgentLocationType() ) )
        {
			typedef std::map<unsigned long, TA_IRS_Bus::TrainStateUpdateCorbaProxyPtr> DepotPeerMap;
			typedef std::vector<unsigned long> DepotLocationData;
			bool corbaFail = false;

			//Synchronize the Train State Information from DepotTrainAgent based on the High priority Depot configuration.
			//If the high priority DepotTrainAgent fails due to Agent Communication failure, then it will go the next priority Depot.
			DepotLocationData depotLocations = m_configuration.getStateSyncLocPriority();
			DepotLocationData::iterator iterHighPriorityDepot = depotLocations.begin();

			//If there is a problem on the first high priority Depot location, then
			//the next priority depot location will be used.
			do 
			{
				try
				{
					//Find the High Priority Depot TrainAgent
					DepotPeerMap::iterator iterDepotPeer = m_depotPeer.find(*iterHighPriorityDepot);
					TA_IRS_Bus::StateUpdateTypes::FullGlobalState fullState;

					//If found in the CORBA Proxy m_depotPeer cache
					if ( iterDepotPeer != m_depotPeer.end() )
					{
						//Synchronize Train State information from High priority DepotTrainAgent
						//This will throw a AgentCommnicationException if the DepotTrainAgent cannot be contacted.
						fullState = iterDepotPeer->second->getGlobalState();
						
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Performing Train State Synchronization from DepotTrainAgent location key : %d",
							         (*iterDepotPeer).first );

						// if successful, process the state and set the flag
						processFullGlobalState( fullState );
						success = true;
						corbaFail = false;
					}
			
				}
				catch ( TA_IRS_Bus::AgentCommunicationException& e )
				{
					LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
					
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Unable to perform Train State Synchronization due to Agent Communication problem on DepotTrainAgent location key : %d",
						*iterHighPriorityDepot);

					//Set the corbafail flag to true if there is a DepotTraingent Communication exception
					corbaFail = true;

					//Then use the next high priority DepotTrainAgent if the previous has failed.
					iterHighPriorityDepot++;
				}

			} while (corbaFail && (iterHighPriorityDepot != depotLocations.end()));
           
        }

        if ( false == success )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "Failed to get full global state from any peers" );
        }

        FUNCTION_EXIT;
    }


    void StateUpdateManager::syncLocalStates()
    {
        FUNCTION_ENTRY( "syncLocalStates" );

        // This will only attempt to sync with the local control mode peer
        // (if this agent isnt in control mode)

        // if the agent is not in control mode,

        if ( false == OperationModeManager::isRunningControl() )
        {
            try
            {
                TA_IRS_Bus::StateUpdateTypes::FullLocalState fullState = m_localPeer->getLocalState();

                // if successful, process the state
                processFullLocalState( fullState );
            }
            catch ( TA_IRS_Bus::AgentCommunicationException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
            }
        }

        FUNCTION_EXIT;
    }


    void StateUpdateManager::processFullGlobalState( const TA_IRS_Bus::StateUpdateTypes::FullGlobalState& fullGlobalState )
    {
        FUNCTION_ENTRY( "processFullGlobalState" );

        // for each Comms state in the comms state list, call processTrainCommsSyncState
        using TA_IRS_Bus::StateUpdateTypes::CommsStateList;

        for ( CommsStateList::const_iterator commsStateIter = fullGlobalState.commsStates.begin();
              fullGlobalState.commsStates.end() != commsStateIter; ++commsStateIter )
        {
            processTrainCommsSyncState( *commsStateIter, 0 );
        }

        // for each cctv state in the comms state list, call processTrainCctvSyncState
        using TA_IRS_Bus::StateUpdateTypes::CctvStateList;

        for ( CctvStateList::const_iterator cctvStateIter = fullGlobalState.cctvStates.begin();
              fullGlobalState.cctvStates.end() != cctvStateIter; ++cctvStateIter )
        {
            processTrainCctvSyncState( *cctvStateIter, 0 );
        }

        // for the radio group list, pass the entire list onto m_radioGroupCoordinator
        m_radioGroupCoordinator.syncRadioGroupList( fullGlobalState.radioGroupMap );

        FUNCTION_EXIT;
    }


    void StateUpdateManager::processFullLocalState( const TA_IRS_Bus::StateUpdateTypes::FullLocalState& fullLocalState )
    {
        FUNCTION_ENTRY( "processFullLocalState" );

        // process all the singular states by calling handlers
        processAgentOperationMode( fullLocalState.operationMode, 0 );

        processRadioCallDetailsSet( fullLocalState.ongoingCalls, 0 );

        processTrainSelectionMap( fullLocalState.selectionLists, 0 );

        // for each train state list (Alarm, Call, Pa, Pec, Tis)
        // for each item in the list, call processTrainXXXSyncState

        using TA_IRS_Bus::StateUpdateTypes::AlarmStateList;

        for ( AlarmStateList::const_iterator alarmStateIter = fullLocalState.alarmStates.begin();
              fullLocalState.alarmStates.end() != alarmStateIter; ++alarmStateIter )
        {
            processTrainAlarmSyncState( *alarmStateIter, 0 );
        }

        using TA_IRS_Bus::StateUpdateTypes::CallStateList;

        for ( CallStateList::const_iterator callStateIter = fullLocalState.callStates.begin();
              fullLocalState.callStates.end() != callStateIter; ++callStateIter )
        {
            processTrainCallSyncState( *callStateIter, 0 );
        }

        using TA_IRS_Bus::StateUpdateTypes::PaStateList;

        for ( PaStateList::const_iterator paStateIter = fullLocalState.paStates.begin();
              fullLocalState.paStates.end() != paStateIter; ++paStateIter )
        {
            processTrainPaSyncState( *paStateIter, 0 );
        }

        using TA_IRS_Bus::StateUpdateTypes::PecStateList;

        for ( PecStateList::const_iterator pecStateIter = fullLocalState.pecStates.begin();
              fullLocalState.pecStates.end() != pecStateIter; ++pecStateIter )
        {
            processTrainPecSyncState( *pecStateIter, 0 );
        }

        using TA_IRS_Bus::StateUpdateTypes::TisStateList;

        for ( TisStateList::const_iterator tisStateIter = fullLocalState.tisStates.begin();
              fullLocalState.tisStates.end() != tisStateIter; ++tisStateIter )
        {
            processTrainTisSyncState( *tisStateIter, 0 );
        }

        FUNCTION_EXIT;
    }


    void StateUpdateManager::broadcastCurrentState()
    {
        FUNCTION_ENTRY( "broadcastCurrentState" );

        // get the operation mode from m_operationModeManager, and send it using m_stateUpdateSender
        m_stateUpdateSender.sendStateUpdate( m_operationModeManager.getCurrentOperationMode() );

        // get the radio group list from m_radioGroupCoordinator, and send it using m_stateUpdateSender
        TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap radioGroups = m_radioGroupCoordinator.getRadioGroups();
        
        for ( TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap::iterator radioGroupIter = radioGroups.begin();
              radioGroups.end() != radioGroupIter; ++radioGroupIter )
        {
            m_stateUpdateSender.sendStateUpdate( radioGroupIter->second );
        }

        // get the train selection map from m_statusManager, and send it using m_stateUpdateSender
        m_stateUpdateSender.sendStateUpdate( m_statusManager.getTrainSelectionMap() );

        // get the radio call details list from m_statusManager, and send it using m_stateUpdateSender
        m_stateUpdateSender.sendStateUpdate( m_statusManager.getRadioCallDetails() );

        TA_IRS_Bus::TrainIdToStateMap::iterator stateIter;

        TA_IRS_Bus::TrainIdToStateMap commsStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::CoreResource );

        for ( stateIter = commsStates.begin(); commsStates.end() != stateIter; ++stateIter )
        {
            TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( stateIter->second );
            TA_ASSERT( NULL != commsState.get(), "Cannot cast to correct state" );

            TA_IRS_Bus::TrainStateLock lock( commsState );

            // get the sync state, and add it to the list of TrainCommsSyncState
            m_stateUpdateSender.sendStateUpdate( commsState->getSyncState() );
        }

        TA_IRS_Bus::TrainIdToStateMap cctvStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainCctvResource );

        for ( stateIter = cctvStates.begin(); cctvStates.end() != stateIter; ++stateIter )
        {
            TrainCctvStatePtr cctvState = boost::dynamic_pointer_cast< TrainCctvState >( stateIter->second );
            TA_ASSERT( NULL != cctvState.get(), "Cannot cast to correct state" );

            TA_IRS_Bus::TrainStateLock lock( cctvState );

            // get the sync state, and add it to the list of TrainCctvSyncState
            m_stateUpdateSender.sendStateUpdate( cctvState->getSyncState() );
        }

        TA_IRS_Bus::TrainIdToStateMap alarmStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainAlarmResource );

        for ( stateIter = alarmStates.begin(); alarmStates.end() != stateIter; ++stateIter )
        {
            TrainAlarmStatePtr alarmState = boost::dynamic_pointer_cast< TrainAlarmState >( stateIter->second );
            TA_ASSERT( NULL != alarmState.get(), "Cannot cast to correct state" );

            TA_IRS_Bus::TrainStateLock lock( alarmState );

            // get the sync state, and add it to the list of TrainAlarmSyncState
            m_stateUpdateSender.sendStateUpdate( alarmState->getSyncState() );
        }

        TA_IRS_Bus::TrainIdToStateMap tisStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainTisResource );

        for ( stateIter = tisStates.begin(); tisStates.end() != stateIter; ++stateIter )
        {
            TrainTisStatePtr tisState = boost::dynamic_pointer_cast< TrainTisState >( stateIter->second );
            TA_ASSERT( NULL != tisState.get(), "Cannot cast to correct state" );

            TA_IRS_Bus::TrainStateLock lock( tisState );

            // get the sync state, and add it to the list of TrainTisSyncState
            m_stateUpdateSender.sendStateUpdate( tisState->getSyncState() );
        }

        TA_IRS_Bus::TrainIdToStateMap paStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainPaResource );

        for ( stateIter = paStates.begin(); paStates.end() != stateIter; ++stateIter )
        {
            TrainPaStatePtr paState = boost::dynamic_pointer_cast< TrainPaState >( stateIter->second );
            TA_ASSERT( NULL != paState.get(), "Cannot cast to correct state" );

            TA_IRS_Bus::TrainStateLock lock( paState );

            // get the sync state, and add it to the list of TrainPaSyncState
            m_stateUpdateSender.sendStateUpdate( paState->getSyncState() );
        }

        TA_IRS_Bus::TrainIdToStateMap callStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainCallResource );

        for ( stateIter = callStates.begin(); callStates.end() != stateIter; ++stateIter )
        {
            TrainCallStatePtr callState = boost::dynamic_pointer_cast< TrainCallState >( stateIter->second );
            TA_ASSERT( NULL != callState.get(), "Cannot cast to correct state" );

            TA_IRS_Bus::TrainStateLock lock( callState );

            // get the sync state, and add it to the list of TrainCallSyncState
            m_stateUpdateSender.sendStateUpdate( callState->getSyncState() );
        }

        TA_IRS_Bus::TrainIdToStateMap pecStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainPecResource );

        for ( stateIter = pecStates.begin(); pecStates.end() != stateIter; ++stateIter )
        {
            TrainPecStatePtr pecState = boost::dynamic_pointer_cast< TrainPecState >( stateIter->second );
            TA_ASSERT( NULL != pecState.get(), "Cannot cast to correct state" );

            TA_IRS_Bus::TrainStateLock lock( pecState );

            // get the sync state, and add it to the list of TrainPecSyncState
            m_stateUpdateSender.sendStateUpdate( pecState->getSyncState() );
        }

        FUNCTION_EXIT;
    }

}  // TA_IRS_App

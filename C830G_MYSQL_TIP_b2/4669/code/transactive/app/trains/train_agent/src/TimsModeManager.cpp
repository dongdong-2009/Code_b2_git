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

#include "app/trains/train_agent/src/TimsModeManager.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"
#include "app/trains/train_agent/src/IOperationModeManager.h"
#include "app/trains/train_agent/src/IAlarmUtility.h"
#include "app/trains/train_agent/src/IRightsChecker.h"
#include "app/trains/train_agent/src/IAuditSender.h"
#include "app/trains/train_agent/src/AtsMessageProcessor.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

#include "bus/trains/TrainCommonLibrary/src/TrainAccessDeniedException.h"
#include "bus/trains/TrainCommonLibrary/src/BadCommandException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTransmissionFailureException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTimeoutException.h"
#include "bus/trains/TrainCommonLibrary/src/TransactionCancelledException.h"
#include "bus/trains/TrainCommonLibrary/src/MpuChangeoverException.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"
#include "bus/trains/TrainTransactionModel/src/ITrainTransactionManager.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"
#include "bus/trains/TrainProtocolLibrary/src/ChangeModeCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/ChangeAreaCommand.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{

    TimsModeManager::TimsModeManager( ITrainAgentConfiguration& configuration,
                                      ITrainStatusManager& statusManager,
                                      TA_IRS_Bus::ITrainTransactionManager& transactionManager,
                                      IOperationModeManager& operationModeManager,
                                      IAlarmUtility& alarms,
                                      IRightsChecker& rightsChecker,
                                      IAuditSender& auditSender,
                                      AtsMessageProcessor& atsMessageProcessor )
            : m_configuration( configuration ),
            m_statusManager( statusManager ),
            m_transactionManager( transactionManager ),
            m_operationModeManager( operationModeManager ),
            m_alarms( alarms ),
            m_rightsChecker( rightsChecker ),
            m_auditSender( auditSender ),
            m_atsMessageProcessor( atsMessageProcessor ),
            m_threadPool( *TA_Base_Core::ThreadPoolSingletonManager::getInstance() )
    {
        FUNCTION_ENTRY( "TimsModeManager" );

        // register for operaton mode changes
        m_operationModeManager.registerForOperationModeChanges( this );

        // register for ats changes
        m_atsMessageProcessor.addObserver( this );

        // register for the following train events
        // AtcEvent
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::AtcEvent::getStaticType(), this );
        // Oa1Event
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::Oa1Event::getStaticType(), this );

        // register for operation state callbacks
        //TA_Base_Bus::AgentManager::registerForStateChangeCallbacks( this );

        FUNCTION_EXIT;
    }


    TimsModeManager::~TimsModeManager()
    {
        FUNCTION_ENTRY( "~TimsModeManager" );
        
        // deregister for everything registered for in the constructor

        // deregister for operation state callbacks
        //TA_Base_Bus::AgentManager::deregisterForStateChangeCallbacks( this );

        // deregister for the following train events
        // AtcEvent
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::AtcEvent::getStaticType(), this );
        // Oa1Event
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::Oa1Event::getStaticType(), this );

        // deregister for ats changes
        m_atsMessageProcessor.removeObserver( this );

        // deregister for operaton mode changes
        m_operationModeManager.deregisterForOperationModeChanges( this );
        
        // remove the thread pool instance
        TA_Base_Core::ThreadPoolSingletonManager::removeInstance( &m_threadPool );

        FUNCTION_EXIT;
    }


    void TimsModeManager::manuallyTakeTrainControl( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                    const std::string& sessionId )
    {
        FUNCTION_ENTRY( "manuallyTakeTrainControl" );
        
        // no need to check control mode - this has been done already as this is a client call

        // check the operator has the required access to perform this task
        // if not, throw an access denied exception
        if ( false == m_rightsChecker.canForceTrainControl( sessionId ) )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not force train control" ) );
        }

        // Create a ChangeModeCommandObserver
        ChangeModeCommandObserver observer;

        // schedule the change mode task using scheduleChangeModeTask, passing the observer
        scheduleChangeModeTask( trainId, true, false, &observer );

        // wait on the ChangeModeCommandObserver
        // when the observer unblocks, get the result of the command
        EChangeModeCommandResult result = observer.waitForCompletion();

        switch ( result )
        {
            case Success:
                // if successful, audit the operation using m_auditSender
                m_auditSender.auditOperatorTakeTrainControl( sessionId, trainId, time( NULL ) );
                break;

            // based on the result, throw the corresponding exception

            case BadCommandBadChecksum:
                TA_THROW( TA_IRS_Bus::BadCommandException( observer.getExtraStatusInformation(),
                                                           TA_IRS_Bus::ProtocolCommonTypes::BadChecksum ) );
                break;

            case BadCommandUnknownCommand:
                TA_THROW( TA_IRS_Bus::BadCommandException( observer.getExtraStatusInformation(),
                                                           TA_IRS_Bus::ProtocolCommonTypes::UnknownCommand ) );
                break;

            case BadCommandBadParameter:
                TA_THROW( TA_IRS_Bus::BadCommandException( observer.getExtraStatusInformation(),
                                                           TA_IRS_Bus::ProtocolCommonTypes::BadParameter ) );
                break;

            case BadCommandUnexpectedCommand:
                TA_THROW( TA_IRS_Bus::BadCommandException( observer.getExtraStatusInformation(),
                                                           TA_IRS_Bus::ProtocolCommonTypes::UnexpectedCommand ) );
                break;

            case InvalidTrainError:
                TA_THROW( TA_IRS_Bus::InvalidTrainException( observer.getExtraStatusInformation(), trainId ) );
                break;

            case TransmissionFailureError:
                TA_THROW( TA_IRS_Bus::TrainTransmissionFailureException( observer.getExtraStatusInformation() ) );
                break;

            case TransactionTimeoutError:
                TA_THROW( TA_IRS_Bus::TrainTimeoutException( observer.getExtraStatusInformation() ) );
                break;

            case TransactionCancelledError:
                TA_THROW( TA_IRS_Bus::TransactionCancelledException( observer.getExtraStatusInformation() ) );
                break;

            case MpuChangeoverError:
                TA_THROW( TA_IRS_Bus::MpuChangeoverException( observer.getExtraStatusInformation() ) );
                break;

            default:
                TA_ASSERT( false, "Invalid EChangeModeCommandResult" );
                break;
        }

        FUNCTION_EXIT;
    }


    void TimsModeManager::signalChangeMode( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "signalChangeMode" );

        // check control mode using AgentManager, if in control return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }
        
        // take control of it
        scheduleChangeModeTask( trainId, true, false );

        FUNCTION_EXIT;
    }


    void TimsModeManager::processAtsUpdate( const AtsTrainInformation& oldInformation,
                                            const AtsTrainInformation& newInformation )
    {
        FUNCTION_ENTRY( "processAtsUpdate" );

        // if ( the new ATS information is not valid )
        // OR ( the new location is not controlled by this agent AND the old location is (ignore localisation validity) )
        // this means the train was (or may have been) controlled by this agent, and it is now no longer controlled by this agent. Any action to take control of it must be stopped.
        if ( ( false == newInformation.trainValid ) ||
             ( ( false == m_operationModeManager.isInControlOfLocation( newInformation.localisation.currentLocation ) ) &&
               ( true == m_operationModeManager.isInControlOfLocation( oldInformation.localisation.currentLocation ) ) ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Train %d moved out of control area, releasing control",
                         newInformation.physicalTrainNumber );

            // cancel taking control of the train
            cancelTakeControlOfTrain( newInformation.physicalTrainNumber );
            
            FUNCTION_EXIT;
            return;
        }

        // check control mode using AgentManager, if in control return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // if the new location is controlled by this agent
        if ( ( true == newInformation.localisation.localisationValid ) &&
             ( true == m_operationModeManager.isInControlOfLocation( newInformation.localisation.currentLocation ) ) )
        {
            // This means the train should be controlled by this agent,
            // this agent must take control of it if it is not already communicating with this agent.

            // get the train comms state
            TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( newInformation.physicalTrainNumber );
            bool communicationValid = false;
            unsigned long communicatingLocation = 0;

            {
                // acquire the lock
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                // get the comms validity
                communicationValid = commsState->isTrainCommunicationValid();
            
                // get the communicating location
                communicatingLocation = commsState->getCommunicatingLocation();

                // release the lock
            }

            // if the train is already communicating with this agent, return
            if ( ( true == communicationValid ) &&
                 ( m_configuration.getLocationKey() == communicatingLocation ) )
            {
                FUNCTION_EXIT;
                return;
            }

            // now need to check if the train has changed area
            // it changed area if:
            // 1. this is the depot OR
            // 2. the train is communicating with the depot (and this is not the depot) OR
            // 3. ATS information is new OR (old ats area != new ats area) OR
            // 4. ATS information is new AND the train is next to the depot

            bool changedArea = false;
            
            if ( TA_Base_Core::ILocation::DPT == m_configuration.getAgentLocationType() )
            {
                // this is the depot (1)
                changedArea = true;
            }
            else
            {
                // not the depot

                // (2)
                bool communicatingWithDepot = ( 0 != communicatingLocation ) &&
                                              ( TA_Base_Core::ILocation::DPT == m_configuration.getLocationType( communicatingLocation ) );

                // (3)
                bool previousAreaUnknown = ( false == oldInformation.trainValid );

                // (3)
                bool areaIsDifferent = ( true == previousAreaUnknown ) ||
                                       ( m_configuration.getLocationType( newInformation.localisation.currentLocation ) !=
                                         m_configuration.getLocationType( oldInformation.localisation.currentLocation ) );
                // (4)
                bool appearedNextToDepot = ( true == previousAreaUnknown ) &&
                                           ( true == m_configuration.isNextToDepot( newInformation.localisation.currentLocation ) );
                changedArea = ( true == communicatingWithDepot ) ||
                              ( true == areaIsDifferent ) ||
                              ( true == appearedNextToDepot );

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                             "Change area calculation [Train: %d][communicatingWithDepot: %s][previousAreaUnknown: %s][areaIsDifferent: %s][appearedNextToDepot: %s]",
                             newInformation.physicalTrainNumber,
                             communicatingWithDepot ? "true" : "false",
                             previousAreaUnknown ? "true" : "false",
                             areaIsDifferent ? "true" : "false",
                             appearedNextToDepot ? "true" : "false" );
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Train moved into control area, scheduling a change mode task [Train: %d][communicationValid: %s][communicatingLocation: %d]",
                         newInformation.physicalTrainNumber,
                         communicationValid ? "true" : "false",
                         communicatingLocation );

            // take control of the train
            scheduleChangeModeTask( newInformation.physicalTrainNumber, true, changedArea );
        }

        FUNCTION_EXIT;
    }


    void TimsModeManager::locationControlAcquired( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "locationControlAcquired" );

        // check control mode using AgentManager, if in control return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Acquired control of location %d, sending change mode to trains at that location",
                     locationKey );

        // take control of all trains at the given location
        takeControlOfTrainsAtLocation( locationKey );

        FUNCTION_EXIT;
    }


    void TimsModeManager::locationControlReleased( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "locationControlReleased" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Released control of location %d, cancelling change mode to trains at that location",
                     locationKey );
        
        // cancel any taking control of all trains at the given location
        cancelTakeControlOfTrainsAtLocation( locationKey );

        FUNCTION_EXIT;
    }


    void TimsModeManager::processNonDutyTrainTSI( const TA_IRS_Bus::StateUpdateTypes::NonDutyTrainTSI& event )
    {
        FUNCTION_ENTRY( "processNonDutyTrainTSI" );

        // check control mode using AgentManager, if in control return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // if this agent should be in control of the given train 
        if ( true == m_operationModeManager.isInControlOfTrain( event.trainId, true, false ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Train %d is communicating with the wrong agent, sending change mode",
                         event.trainId );

            takeControlOfTrain( event.trainId );
        }

        FUNCTION_EXIT;
    }


    void TimsModeManager::notifyOperationStateChange( TA_Base_Core::EOperationMode operationState )
    {
        FUNCTION_ENTRY( "notifyOperationStateChange" );
        
        // if the operation state is control
        if ( TA_Base_Core::Control == operationState )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Agent changed to control state, ensuring control of all trains in controlled locations" );

            takeControlOfTrainsAtAllOwnedLocations();
        }
        // if the operation state is any other
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Agent changed to non-control state, releasing control of all trains" );

            cancelTakeControlOfTrains();
        }

        FUNCTION_EXIT;
    }


    void TimsModeManager::processEvent( TA_IRS_Bus::TrainEventPtr event )
    {
        FUNCTION_ENTRY( "processEvent" );

        // return if this agent isnt in the control state
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // close any radiomode alarms for the train,
        closeRadioModeAlarm( event->getTrainId() );

        FUNCTION_EXIT;
    }


    void TimsModeManager::takeControlOfTrainsAtAllOwnedLocations()
    {
        FUNCTION_ENTRY( "takeControlOfTrainsAtAllOwnedLocations" );
        
        // get the set of controlled locations
        std::set< unsigned long > controlledLocations = m_operationModeManager.getControlledLocations();
        
        // take control of trains at those locations
        takeControlOfTrainsAtLocations( controlledLocations );

        FUNCTION_EXIT;
    }


    void TimsModeManager::takeControlOfTrainsAtLocation( unsigned long location )
    {
        FUNCTION_ENTRY( "takeControlOfTrainsAtLocation" );
        
        // create a set of locations, with the given location
        // as the single entry and use the common function

        std::set< unsigned long > locations;
        locations.insert( location );
        
        // take control of trains at those locations
        takeControlOfTrainsAtLocations( locations );

        FUNCTION_EXIT;
    }


    void TimsModeManager::takeControlOfTrainsAtLocations( const std::set< unsigned long >& locations )
    {
        FUNCTION_ENTRY( "takeControlOfTrainsAtLocations" );
        
        // get all train comms states
        TA_IRS_Bus::TrainIdToStateMap stateMap = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::CoreResource );

        for ( TA_IRS_Bus::TrainIdToStateMap::iterator stateIter = stateMap.begin();
              stateMap.end() != stateIter; ++stateIter )
        {
            // down cast to the specific type
            TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( stateIter->second );
            TA_ASSERT( NULL != commsState.get(), "Failed to cast to the correct type" );

            bool atsValid = false;
            unsigned long trainLocation = 0;
            bool communicationValid = false;
            unsigned long communicatingLocation = 0;

            {
                // acquire the lock for the train state
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                // get the train's ATS validity
                atsValid = commsState->isAtsValid();

                // get the train's location
                trainLocation = commsState->getLocation();

                // get the comms validity
                communicationValid = commsState->isTrainCommunicationValid();
                
                // get the communicating location
                communicatingLocation = commsState->getCommunicatingLocation();

                // release the lock for the train state
            }

            // if the ats is valid, and the location is in the set of locations
            // and if the comms validity is false, or the communicating location is not this agent location
            if ( ( ( true == atsValid ) &&
                   ( locations.find( trainLocation ) != locations.end() ) ) &&
                 ( ( false == communicationValid ) ||
                   ( m_configuration.getLocationKey() != communicatingLocation ) ) )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "Scheduling a change mode task for [Train: %d][communicationValid: %s][communicatingLocation: %d]",
                             stateIter->first,
                             communicationValid ? "true" : "false",
                             communicatingLocation );

                // take control of the train
                scheduleChangeModeTask( stateIter->first, true, false );
            }
        }

        FUNCTION_EXIT;
    }


    void TimsModeManager::takeControlOfTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "takeControlOfTrain" );
        
        // get the train comms state
        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );
        bool communicationValid = false;
        unsigned long communicatingLocation = 0;

        {
            // acquire the lock
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // get the comms validity
            communicationValid = commsState->isTrainCommunicationValid();
            
            // get the communicating location
            communicatingLocation = commsState->getCommunicatingLocation();

            // release the lock
        }

        // if the comms validity is false, or the communicating location is not this agent location
        if ( ( false == communicationValid ) ||
             ( m_configuration.getLocationKey() != communicatingLocation ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Scheduling a change mode task for [Train: %d][communicationValid: %s][communicatingLocation: %d]",
                         trainId,
                         communicationValid ? "true" : "false",
                         communicatingLocation );

            // take control of the train
            scheduleChangeModeTask( trainId, true, false );
        }

        FUNCTION_EXIT;
    }


    void TimsModeManager::scheduleChangeModeTask( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                  bool changeMode,
                                                  bool changeArea,
                                                  ChangeModeCommandObserver* observer )
    {
        FUNCTION_ENTRY( "scheduleChangeModeTask" );

        // first determine what to send...
        // the depot cant send change mode, so always send change area in its place
        if ( TA_Base_Core::ILocation::DPT == m_configuration.getAgentLocationType() )
        {
            changeMode = false;
            changeArea = true;
        }


        if ( true == changeArea )
        {
            // aqcuire m_changeAreaTaskLock
            TA_THREADGUARD( m_changeAreaTaskLock );
        
            // find the entry for the given train in m_changeAreaTasks
            ChangeModeChangeAreaTaskMap::iterator findIter = m_changeAreaTasks.find( trainId );

            if ( m_changeAreaTasks.end() != findIter )
            {
                // add the given observer if one was given,
                // and a change mode is not to be scheduled (the observer will be attached to the change mode)
                if ( ( NULL != observer ) &&
                     ( false == changeMode ) )
                {
                    findIter->second->addObserver( observer );
                }
            }
            else
            {
                // create a new change area task
                ChangeModeChangeAreaTaskPtr newTask( new ChangeModeChangeAreaTask( *this,
                                                                                   trainId,
                                                                                   ChangeModeChangeAreaTask::ChangeAreaTask ) );

                if ( ( NULL != observer ) &&
                     ( false == changeMode ) )
                {
                    newTask->addObserver( observer );
                }

                // add the task to m_changeAreaTasks
                m_changeAreaTasks.insert( ChangeModeChangeAreaTaskMap::value_type( trainId, newTask ) );

                // schedule the new change area task using the thread pool
                m_threadPool.queueWorkItem( newTask );
            }
        }

        
        if ( true == changeMode )
        {
            // aqcuire m_changeModeTaskLock
            TA_THREADGUARD( m_changeModeTaskLock );
            if ( true == changeArea)
            {
				TA_Base_Core::Thread::sleep(5000);//CL-20591
				//lichao++   set 5 sec interval between the sending of message 75 and message 73. 
            }
            // find the entry for the given train in m_changeModeTasks
            ChangeModeChangeAreaTaskMap::iterator findIter = m_changeModeTasks.find( trainId );

            // if it exists
            if ( m_changeModeTasks.end() != findIter )
            {
                // add the given observer if one was given
                if ( NULL != observer )
                {
                    findIter->second->addObserver( observer );
                }
            }
            // if it doesnt exist
            else
            {
                // create a new change mode task
                ChangeModeChangeAreaTaskPtr newTask( new ChangeModeChangeAreaTask( *this,
                                                                                   trainId,
                                                                                   ChangeModeChangeAreaTask::ChangeModeTask ) );
            
                // add the given observer if one was given
                if ( NULL != observer )
                {
                    newTask->addObserver( observer );
                }

                // add the task to m_changeModeTasks
                m_changeModeTasks.insert( ChangeModeChangeAreaTaskMap::value_type( trainId, newTask ) );

                // schedule the task using the thread pool
                m_threadPool.queueWorkItem( newTask );
            }
        }

        FUNCTION_EXIT;
    }


    void TimsModeManager::cancelTakeControlOfTrains()
    {
        FUNCTION_ENTRY( "cancelTakeControlOfTrains" );
        
        TA_IRS_Bus::CommonTypes::TrainIdList emptyTrainList;

        // all tasks must be cancelled, including operator triggered tasks,
        // however those with observers must still notify their observers of cancellation to avoid deadlocks

        {
            // aqcuire m_changeModeTaskLock
            TA_THREADGUARD( m_changeModeTaskLock );

            // for each entry in m_changeModeTasks
            ChangeModeChangeAreaTaskMap::iterator changeModeIter = m_changeModeTasks.begin();
            while ( m_changeModeTasks.end() != changeModeIter )
            {
                // set the task to cancelled
                changeModeIter->second->setCancelled();
            
                // DO NOT remove the task from the threadpool, as the observers must still be notified of the cancellation

                // remove the entry in the map (causing the task to be deleted once executed)
                //changeModeIter=m_changeModeTasks.erase( changeModeIter ); // Using this will work fine on VC compilers. But not in solaris Compilers. So, no need to change.
		m_changeModeTasks.erase( changeModeIter++ );
            }

            // cancel all change mode transactions in the transaction manager (this caters for all change mode tasks currently executing)
            TA_IRS_Bus::TrainMessageTypeList commandList;
            commandList.insert( TA_IRS_Bus::ChangeModeCommand::getStaticMessageType() );

            m_transactionManager.cancelTransactions( emptyTrainList, commandList );

            // release m_changeModeTaskLock
        }

        {
            // aqcuire m_changeAreaTaskLock
            TA_THREADGUARD( m_changeAreaTaskLock );

            // for each entry in m_changeAreaTasks
            ChangeModeChangeAreaTaskMap::iterator changeAreaIter = m_changeAreaTasks.begin();
            while ( m_changeAreaTasks.end() != changeAreaIter )
            {
                // DO NOT remove the task from the threadpool, as the observers must still be notified of the cancellation
                
                // remove the entry in the map (causing the task to be deleted)
                m_changeAreaTasks.erase( changeAreaIter++ );
            }

            //cancel all change area transactions in the transaction manager (this caters for all change area tasks currently executing)
            TA_IRS_Bus::TrainMessageTypeList commandList;
            commandList.insert( TA_IRS_Bus::ChangeAreaCommand::getStaticMessageType() );

            m_transactionManager.cancelTransactions( emptyTrainList, commandList );

            // release m_changeAreaTaskLock
        }

        FUNCTION_EXIT;
    }


    void TimsModeManager::cancelTakeControlOfTrainsAtLocation( unsigned long location )
    {
        FUNCTION_ENTRY( "cancelTakeControlOfTrainsAtLocation" );
        
        // get all train comms states
        TA_IRS_Bus::TrainIdToStateMap stateMap = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::CoreResource );

        for ( TA_IRS_Bus::TrainIdToStateMap::iterator stateIter = stateMap.begin();
              stateMap.end() != stateIter; ++stateIter )
        {
            // down cast to the specific type
            TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( stateIter->second );
            TA_ASSERT( NULL != commsState.get(), "Failed to cast to the correct type" );

            bool atsValid = false;
            unsigned long trainLocation = 0;

            {
                // acquire the lock for the train state
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                // get the train's ATS validity
                atsValid = commsState->isAtsValid();

                // get the train's location
                trainLocation = commsState->getLocation();

                // release the lock for the train state
            }

            // if the ats is valid, and the location matches the given location
            if ( ( true == atsValid ) &&
                 ( trainLocation == location ) )
            {
                // cancel the take control of train
                cancelTakeControlOfTrain( stateIter->first );
            }
        }

        FUNCTION_EXIT;
    }


    void TimsModeManager::cancelTakeControlOfTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "cancelTakeControlOfTrain" );

        TA_IRS_Bus::CommonTypes::TrainIdList trainList;
        trainList.insert( trainId );
        
        {
            // aqcuire m_changeModeTaskLock
            TA_THREADGUARD( m_changeModeTaskLock );

            // find the entry for the given train in m_changeModeTasks
            ChangeModeChangeAreaTaskMap::iterator findIter = m_changeModeTasks.find( trainId );

            // if it exists
            if ( m_changeModeTasks.end() != findIter )
            {
                // if there are observers for this task (this means it was a manually triggered task by the operator and should not be cancelled)
                if ( true == findIter->second->hasObservers() )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                 "Change mode task for train %d not cancelled as it was operator triggered",
                                 trainId );
                }
                // otherwise if there are no observers
                else
                {
                    // remove the item from the threadpool (this caters for the case the work item is still queued waiting for execution)
                    m_threadPool.removeWorkItem( findIter->second );

                    // cancel the transaction in the transaction manager (this caters for the case the task is currently executing)
                    TA_IRS_Bus::TrainMessageTypeList commandList;
                    commandList.insert( TA_IRS_Bus::ChangeModeCommand::getStaticMessageType() );

                    m_transactionManager.cancelTransactions( trainList, commandList );

                    // remove the entry in the map (causing the task to be deleted)
                    m_changeModeTasks.erase( findIter );
                }
            }

            // release m_changeModeTaskLock
        }

        {
            // acquire m_changeAreaTaskLock
            TA_THREADGUARD( m_changeAreaTaskLock );

            // find the entry for the given train in m_changeAreaTasks
            ChangeModeChangeAreaTaskMap::iterator findIter = m_changeAreaTasks.find( trainId );

            // if it exists
            if ( m_changeAreaTasks.end() != findIter )
            {
                // if there are observers for this task (this means it was a manually triggered task by the operator and should not be cancelled)
                if ( true == findIter->second->hasObservers() )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                 "Change area task for train %d not cancelled as it was operator triggered",
                                 trainId );
                }
                // otherwise if there are no observers
                else
                {
                    // remove the item from the threadpool (this caters for the case the work item is still queued waiting for execution)
                    m_threadPool.removeWorkItem( findIter->second );

                    // cancel the transaction in the transaction manager (this caters for the case the task is currently executing)
                    TA_IRS_Bus::TrainMessageTypeList commandList;
                    commandList.insert( TA_IRS_Bus::ChangeAreaCommand::getStaticMessageType() );

                    m_transactionManager.cancelTransactions( trainList, commandList );

                    // remove the entry in the map (causing the task to be deleted)
                    m_changeAreaTasks.erase( findIter );
                }
            }
        }

        // close any radio mode alarm
        closeRadioModeAlarm( trainId );

        FUNCTION_EXIT;
    }


    void TimsModeManager::raiseRadioModeAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "raiseRadioModeAlarm" );

        // get the train alarm state for this train
        TrainAlarmStatePtr alarmState = m_statusManager.getTrainAlarmState( trainId );

        // acquire the lock for the train state
        TA_IRS_Bus::TrainStateLock stateLock( alarmState );

        //if there is not a radio mode alarm (the alarm ID is empty)
        if ( true == alarmState->getRadioModeAlarmId().empty() )
        {
            // raise the alarm
            // set the radio mode alarm to the returned alarm ID in the alarm state
            alarmState->setRadioModeAlarmId( m_alarms.raiseRadioModeTimerAlarm( trainId, time( NULL ) ) );
        }

        // release the state lock

        FUNCTION_EXIT;
    }


    void TimsModeManager::closeRadioModeAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "closeRadioModeAlarm" );

        // get the train alarm state for this train
        TrainAlarmStatePtr alarmState = m_statusManager.getTrainAlarmState( trainId );

        // acquire the lock for the train state
        TA_IRS_Bus::TrainStateLock stateLock( alarmState );

        // if there is a radio mode alarm (not "")
        std::string alarmId = alarmState->getRadioModeAlarmId();
        if ( false == alarmId.empty() )
        {
            // close the alarm using m_alarms
            m_alarms.closeAlarm( alarmId );

            // set the radio mode alarm to "" in the alarm state
            alarmState->setRadioModeAlarmId();
        }

        // release the state lock

        FUNCTION_EXIT;
    }


    EChangeModeCommandResult TimsModeManager::performChangeMode( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                                 std::string& errorDetails )
    {
        FUNCTION_ENTRY( "performChangeMode" );
        
        EChangeModeCommandResult result = Success;
        errorDetails = "";

        try
        {
            // create a ChangeModeCommand
            // use the agent origin
            // get the agent location type
            // if it is the depot or the occ, set the mode to Normal
            // if it is a station, set the mode to Degraded
            TA_IRS_Bus::ETrainMode trainMode = TA_IRS_Bus::TrainNormal;
        
            if ( TA_Base_Core::ILocation::STATION == m_configuration.getAgentLocationType() )
            {
                trainMode = TA_IRS_Bus::TrainDegraded;
            }

            TA_IRS_Bus::ChangeModeCommandPtr command(
                new TA_IRS_Bus::ChangeModeCommand( trainId,
                                                   m_configuration.getAgentOrigin(),
                                                   trainMode ) );

            // get the train comms state
            TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );
            std::string standbyTsi;

            {
                // acquire the lock
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                // get the standby tsi
                standbyTsi = commsState->getStandbyTsi();

                // release the lock
            }


            // if the standby TSI is not set
            if ( true == standbyTsi.empty() )
            {
                // send the command as normal, using the retries from m_configuration, and using the TimerISCS2 from m_configuration.
                // Ignore validity as the train may not exist properly yet
                //m_transactionManager.sendCommand( command, m_configuration.getTimerISCS2(), m_configuration.getTimsRetryLimit(), true );
				//hongzhi, continuely send it till receive response, interval is 20 seconds
                m_transactionManager.sendCommand( command, 20000, 4320, true );

                // exceptions will be thrown on failure
            }
            // if the standby tsi is set
            else
            {
                // send the command in parallel to both TSIs, using the retries from m_configuration, and using the TimerISCS2 from m_configuration

                TA_IRS_Bus::TrainCommandContainer primaryContainer;
                primaryContainer.command = command;

                TA_IRS_Bus::TrainCommandContainer secondaryContainer;
                secondaryContainer.command = TA_IRS_Bus::ChangeModeCommandPtr(
                    new TA_IRS_Bus::ChangeModeCommand( trainId,
                                                       m_configuration.getAgentOrigin(),
                                                       trainMode ) );
                secondaryContainer.radioId = standbyTsi;


                TA_IRS_Bus::TrainCommandList commands;
                commands.push_back( primaryContainer );
                commands.push_back( secondaryContainer );

/*                std::vector<TA_IRS_Bus::ETrainError> resultList = m_transactionManager.sendCommands( commands,
                                                                                                     m_configuration.getTimerISCS2(),
                                                                                                     m_configuration.getTimsRetryLimit(),
                                                                                                     true );
*/
				//hongzhi
                std::vector<TA_IRS_Bus::ETrainError> resultList = m_transactionManager.sendCommands( commands,
					20000,
					4320,
					true );

                // the result for both commands will be the same, because any response from the active MPU will actually
                // be accepted by both commands. It is not necessary for both MPUs to respond.

                TA_IRS_Bus::ETrainError commandResult = resultList[0];

                switch ( commandResult )
                {
                    case TA_IRS_Bus::GeneralFailure:
                        TA_THROW( TA_IRS_Bus::TrainException( "Unknown Error" ) );
                        break;

                    case TA_IRS_Bus::InvalidTrain:
                        TA_THROW( TA_IRS_Bus::InvalidTrainException( "Invalid Train", trainId ) );
                        break;

                    case TA_IRS_Bus::ProtocolError:
                        TA_THROW( TA_IRS_Bus::ProtocolException( "Invalid Protocol", "", "" ) );
                        break;

                    case TA_IRS_Bus::TrainBusy:
                        TA_THROW( TA_IRS_Bus::TrainException( "Unknown Error" ) );
                        break;

                    case TA_IRS_Bus::TrainTimeout:
                        TA_THROW( TA_IRS_Bus::TrainTimeoutException( "Train Timeout" ) );
                        break;

                    case TA_IRS_Bus::TransmissionFailure:
                        TA_THROW( TA_IRS_Bus::TrainTransmissionFailureException( "Transmission Failure" ) );
                        break;

                    case TA_IRS_Bus::AccessDeniedException:
                        TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Access Denied" ) );
                        break;

                    case TA_IRS_Bus::TransactionCancelled:
                        TA_THROW( TA_IRS_Bus::TransactionCancelledException( "Cancelled" ) );
                        break;

                    case TA_IRS_Bus::NoError:
                        // fall through

                    default:
                        break;
                }
            }

            // get the Bad Command Response, and the MPU changeover response
            TA_IRS_Bus::BadCommandEventPtr badCommand = command->getBadCommandResponse();
            TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = command->getChangeoverStatusEventResponse();

            // if the bad command event is returned
            if ( badCommand )
            {
                TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError errorCode = badCommand->getErrorCode();

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "Change mode command for train %d returned an error: %s",
                             trainId,
                             TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode ).c_str() );

                // set the appropriate error status in the result variable depending on the error code in the bad command event
                // set the errorDetails string to a string depending on the error code in the bad command event, eg "Bad Command - Bad Checksum"

                switch ( errorCode )
                {
                    case TA_IRS_Bus::ProtocolCommonTypes::BadChecksum:
                        result = BadCommandBadChecksum;
                        errorDetails = "Bad Command - Bad Checksum";
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::UnknownCommand:
                        result = BadCommandUnknownCommand;
                        errorDetails = "Bad Command - Bad Unknown Command";
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::BadParameter:
                        result = BadCommandBadParameter;
                        errorDetails = "Bad Command - Bad Parameter";
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::UnexpectedCommand:
                        result = BadCommandUnexpectedCommand;
                        errorDetails = "Bad Command - Unexpected Command";
                        break;
                    
                    default:
                        TA_ASSERT( false, "This value must be validated by the protocol library" );
                        break;
                }
            }

            // if the mpu changeover event is returned
            else if ( mpuChangeover )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "Change area command for train %d returned an MPU changeover",
                             trainId );

                result = MpuChangeoverError;
                errorDetails = "MPU Changeover";
            }

            // otherwise some other event was received - it was ok
        }
        catch( TA_IRS_Bus::TrainAccessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );
            
            // set the appropriate error status in the result
            result = InvalidTrainError;
            
            // set the errorDetails string to the exception string
            errorDetails = e.what();
        }
        catch( TA_IRS_Bus::TrainTimeoutException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimeoutException", e.what() );
            
            // set the appropriate error status in the result
            result = TransactionTimeoutError;
            
            // set the errorDetails string to the exception string
            errorDetails = e.what();
        }
        catch( TA_IRS_Bus::TrainTransmissionFailureException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );
            
            // set the appropriate error status in the result
            result = TransmissionFailureError;
            
            // set the errorDetails string to the exception string
            errorDetails = e.what();
        }
        catch( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );
            
            // set the appropriate error status in the result
            result = InvalidTrainError;
            
            // set the errorDetails string to the exception string
            errorDetails = e.what();
        }
        catch( TA_IRS_Bus::TransactionCancelledException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactionCancelledException", e.what() );
            
            // set the appropriate error status in the result
            result = TransactionCancelledError;
            
            // set the errorDetails string to the exception string
            errorDetails = e.what();
        }
        catch( TA_IRS_Bus::ProtocolException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::ProtocolException", e.what() );
            
            // set the appropriate error status in the result
            result = InvalidTrainError;
            
            // set the errorDetails string to the exception string
            errorDetails = e.what();
        }
        catch( TA_IRS_Bus::TrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainException", e.what() );
            
            // set the appropriate error status in the result
            result = InvalidTrainError;
            
            // set the errorDetails string to the exception string
            errorDetails = e.what();
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Uncaught exception, only specified exceptions should get here" );
			result = BadCommandUnknownCommand;
        }
        
        // if result is not Success or TransactionCancelledError
        if ( ( result != Success ) && 
             ( result != TransactionCancelledError ) )
        {
            // raise the radio mode timer alarm
            raiseRadioModeAlarm( trainId );
        }


        // aqcuire m_changeModeTaskLock
        TA_THREADGUARD( m_changeModeTaskLock );

        // find the entry for the given train in m_changeModeTasks
        ChangeModeChangeAreaTaskMap::iterator findIter = m_changeModeTasks.find( trainId );

        // if it exists
        if ( m_changeModeTasks.end() != findIter )
        {
            // if it exists, remove the entry in the map (causing the task to be deleted once execution is complete)
            m_changeModeTasks.erase( findIter );
        }

        // release m_changeModeTaskLock

        FUNCTION_EXIT;
        return result;
    }


    EChangeModeCommandResult TimsModeManager::performChangeArea( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                                 std::string& errorDetails )
    {
        FUNCTION_ENTRY( "performChangeArea" );

        EChangeModeCommandResult result = Success;
        errorDetails = "";
        
        try
        {
            // create a ChangeAreaCommand
            // use the agent origin from m_configuration
            // get the agent location type
            // if it is the depot, set the depot area
            // if it is the occ or station, set the mainline area
            TA_IRS_Bus::ETrainArea trainArea = TA_IRS_Bus::MainlineArea;
        
            if ( TA_Base_Core::ILocation::DPT == m_configuration.getAgentLocationType() )
            {
                trainArea = TA_IRS_Bus::DepotArea;
            }

            TA_IRS_Bus::ChangeAreaCommandPtr command(
                new TA_IRS_Bus::ChangeAreaCommand( trainId,
                                                   m_configuration.getAgentOrigin(),
                                                   trainArea ) );

            // get the train comms state
            TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );
            std::string standbyTsi;

            {
                // acquire the lock
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                // get the standby tsi
                standbyTsi = commsState->getStandbyTsi();

                // release the lock
            }


            // if the standby TSI is not set
            if ( true == standbyTsi.empty() )
            {
                // send the command as normal, using the retries from m_configuration, and using the TimerISCS2 from m_configuration.
                // Ignore validity as the train may not exist properly yet
//                m_transactionManager.sendCommand( command, m_configuration.getTimerISCS1(), m_configuration.getTimsRetryLimit(), true );
				//hongzhi
                m_transactionManager.sendCommand( command, 20000, 4320, true );

                // exceptions will be thrown on failure
            }
            // if the standby tsi is set
            else
            {
                // send the command in parallel to both TSIs, using the retries from m_configuration, and using the TimerISCS2 from m_configuration

                TA_IRS_Bus::TrainCommandContainer primaryContainer;
                primaryContainer.command = command;

                TA_IRS_Bus::TrainCommandContainer secondaryContainer;
                secondaryContainer.command = TA_IRS_Bus::ChangeAreaCommandPtr( 
                    new TA_IRS_Bus::ChangeAreaCommand( trainId,
                                                       m_configuration.getAgentOrigin(),
                                                       trainArea ) );
                secondaryContainer.radioId = standbyTsi;


                TA_IRS_Bus::TrainCommandList commands;
                commands.push_back( primaryContainer );
                commands.push_back( secondaryContainer );

/*                std::vector<TA_IRS_Bus::ETrainError> resultList = m_transactionManager.sendCommands( commands,
                                                                                         m_configuration.getTimerISCS1(),
                                                                                         m_configuration.getTimsRetryLimit(),
                                                                                         true );
*/
                std::vector<TA_IRS_Bus::ETrainError> resultList = m_transactionManager.sendCommands( commands,
					20000,
					4320,
					true );

                // the result for both commands will be the same, because any response from the active MPU will actually
                // be accepted by both commands. It is not necessary for both MPUs to respond.

                TA_IRS_Bus::ETrainError result = resultList[0];

                switch ( result )
                {
                    case TA_IRS_Bus::GeneralFailure:
                        TA_THROW( TA_IRS_Bus::TrainException( "Unknown Error" ) );
                        break;

                    case TA_IRS_Bus::InvalidTrain:
                        TA_THROW( TA_IRS_Bus::InvalidTrainException( "Invalid Train", trainId ) );
                        break;

                    case TA_IRS_Bus::ProtocolError:
                        TA_THROW( TA_IRS_Bus::ProtocolException( "Invalid Protocol", "", "" ) );
                        break;

                    case TA_IRS_Bus::TrainBusy:
                        TA_THROW( TA_IRS_Bus::TrainException( "Unknown Error" ) );
                        break;

                    case TA_IRS_Bus::TrainTimeout:
                        TA_THROW( TA_IRS_Bus::TrainTimeoutException( "Train Timeout" ) );
                        break;

                    case TA_IRS_Bus::TransmissionFailure:
                        TA_THROW( TA_IRS_Bus::TrainTransmissionFailureException( "Transmission Failure" ) );
                        break;

                    case TA_IRS_Bus::AccessDeniedException:
                        TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Access Denied" ) );
                        break;

                    case TA_IRS_Bus::TransactionCancelled:
                        TA_THROW( TA_IRS_Bus::TransactionCancelledException( "Cancelled" ) );
                        break;

                    case TA_IRS_Bus::NoError:
                        // fall through

                    default:
                        break;
                }
            }

            // get the Command Received Response, the Bad Command Response, and the MPU changeover response
            TA_IRS_Bus::ChangeAreaCommandReceivedEventPtr commandReceived = command->getCommandReceivedResponse();
            TA_IRS_Bus::BadCommandEventPtr badCommand = command->getBadCommandResponse();
            TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = command->getChangeoverStatusEventResponse();

            // if the command received event is returned
            if ( commandReceived )
            {
                // check the area from the event, and ensure it matches the area that was sent
                // if it does not match, log an error
                if ( commandReceived->getArea() != trainArea )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                                 "Train %d responded to a change area command with area %d when area %d was sent",
                                 trainId, commandReceived->getArea(), trainArea );
                }
            }
            
            // if the bad command event is returned
            else if ( badCommand )
            {
                TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError errorCode = badCommand->getErrorCode();

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "Change mode command for train %d returned an error: %s",
                             trainId,
                             TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode ).c_str() );

                // set the appropriate error status in the result variable depending on the error code in the bad command event
                // set the errorDetails string to a string depending on the error code in the bad command event, eg "Bad Command - Bad Checksum"

                switch ( errorCode )
                {
                    case TA_IRS_Bus::ProtocolCommonTypes::BadChecksum:
                        result = BadCommandBadChecksum;
                        errorDetails = "Bad Command - Bad Checksum";
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::UnknownCommand:
                        result = BadCommandUnknownCommand;
                        errorDetails = "Bad Command - Bad Unknown Command";
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::BadParameter:
                        result = BadCommandBadParameter;
                        errorDetails = "Bad Command - Bad Parameter";
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::UnexpectedCommand:
                        result = BadCommandUnexpectedCommand;
                        errorDetails = "Bad Command - Unexpected Command";
                        break;
                    
                    default:
                        //TA_ASSERT( false, "This value must be validated by the protocol library" );
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "This value must be validated by the protocol library" );
						result = BadCommandUnexpectedCommand;
                        errorDetails = "Bad Command - Unexpected Command";
                        break;
                }
            }

            // if the mpu changeover event is returned
            else if ( mpuChangeover )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "Change area command for train %d returned an MPU changeover",
                             trainId );

                result = MpuChangeoverError;
                errorDetails = "MPU Changeover";
            }
            else
            {
                // if none of those 3 events were returned,
                // assert that the command completed without a valid response
//                TA_ASSERT( false, "This command can only exit via exception or a valid response" );
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "This command can only exit via exception or a valid response" );
				result = BadCommandUnknownCommand;
            }

        }
        catch( TA_IRS_Bus::TrainAccessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );
            
            // set the appropriate error status in the result
            result = InvalidTrainError;
            
            // set the errorDetails string to the exception string
            errorDetails = e.what();
        }
        catch( TA_IRS_Bus::TrainTimeoutException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimeoutException", e.what() );
            
            // set the appropriate error status in the result
            result = TransactionTimeoutError;
            
            // set the errorDetails string to the exception string
            errorDetails = e.what();
        }
        catch( TA_IRS_Bus::TrainTransmissionFailureException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );
            
            // set the appropriate error status in the result
            result = TransmissionFailureError;
            
            // set the errorDetails string to the exception string
            errorDetails = e.what();
        }
        catch( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );
            
            // set the appropriate error status in the result
            result = InvalidTrainError;
            
            // set the errorDetails string to the exception string
            errorDetails = e.what();
        }
        catch( TA_IRS_Bus::TransactionCancelledException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactionCancelledException", e.what() );
            
            // set the appropriate error status in the result
            result = TransactionCancelledError;
            
            // set the errorDetails string to the exception string
            errorDetails = e.what();
        }
        catch( TA_IRS_Bus::ProtocolException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::ProtocolException", e.what() );
            
            // set the appropriate error status in the result
            result = InvalidTrainError;
            
            // set the errorDetails string to the exception string
            errorDetails = e.what();
        }
        catch( TA_IRS_Bus::TrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainException", e.what() );
            
            // set the appropriate error status in the result
            result = InvalidTrainError;
            
            // set the errorDetails string to the exception string
            errorDetails = e.what();
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Uncaught exception, only specified exceptions should get here" );
			result = BadCommandUnknownCommand;
        }

        // aqcuire m_changeAreaTaskLock
        TA_THREADGUARD( m_changeAreaTaskLock );

        // find the entry for the given train in m_changeModeTasks
        ChangeModeChangeAreaTaskMap::iterator findIter = m_changeAreaTasks.find( trainId );

        // if it exists
        if ( m_changeAreaTasks.end() != findIter )
        {
            // if it exists, remove the entry in the map (causing the task to be deleted once execution is complete)
            m_changeAreaTasks.erase( findIter );
        }

        // release m_changeAreaTaskLock

        FUNCTION_EXIT;
        return result;
    }

} // TA_IRS_App

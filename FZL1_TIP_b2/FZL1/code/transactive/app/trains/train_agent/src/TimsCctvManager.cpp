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

#include "app/trains/train_agent/src/TimsCctvManager.h"

#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"
#include "app/trains/train_agent/src/IOperationModeManager.h"
#include "app/trains/train_agent/src/IAlarmUtility.h"
#include "app/trains/train_agent/src/IRightsChecker.h"
#include "app/trains/train_agent/src/IMessageSender.h"
#include "app/trains/train_agent/src/IAuditSender.h"
#include "app/trains/train_agent/src/AtsMessageProcessor.h"
#include "app/trains/train_agent/src/ICctvAgentInterface.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

#include "bus/trains/TrainCommonLibrary/src/TrainAccessDeniedException.h"
#include "bus/trains/TrainCommonLibrary/src/BadCommandException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTransmissionFailureException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTimeoutException.h"
#include "bus/trains/TrainCommonLibrary/src/TransactionCancelledException.h"
#include "bus/trains/TrainCommonLibrary/src/MpuChangeoverException.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"
#include "bus/trains/TrainCommonLibrary/src/CctvException.h"
#include "bus/trains/TrainCommonLibrary/src/CctvConflictException.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus/trains/TrainTransactionModel/src/ITrainTransactionManager.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"

#include "bus/trains/TrainProtocolLibrary/src/CctvCarrierOffCommand.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/utilities/src/DebugUtil.h"
#include <list>


namespace TA_IRS_App
{
    TimsCctvManager::TimsCctvManager( ITrainAgentConfiguration& configuration,
                                      ITrainStatusManager& statusManager,
                                      TA_IRS_Bus::ITrainTransactionManager& transactionManager,
                                      IOperationModeManager& operationModeManager,
                                      IAlarmUtility& alarms,
                                      IRightsChecker& rightsChecker,
                                      IMessageSender& messageSender,
                                      IAuditSender& auditSender,
                                      AtsMessageProcessor& atsMessageProcessor,
                                      ICctvAgentInterface& switchAgent )
            : m_configuration( configuration ),
              m_statusManager( statusManager ),
              m_operationModeManager( operationModeManager ),
              m_messageSender( messageSender ),
              m_firstRun( true ),
              m_transactionManager( transactionManager ),
              m_alarms( alarms ),
              m_rightsChecker( rightsChecker ),
              m_auditSender( auditSender ),
              m_atsMessageProcessor( atsMessageProcessor ),
              m_switchAgent( switchAgent ),
              m_threadPool( *TA_Base_Core::ThreadPoolSingletonManager::getInstance() )
    {
        FUNCTION_ENTRY( "TimsCctvManager" );

        // register for operaton mode changes
        m_operationModeManager.registerForOperationModeChanges( this );

        // register for ats changes
        m_atsMessageProcessor.addObserver( this );

        // register for operation state callbacks
        //TA_Base_Bus::AgentManager::registerForStateChangeCallbacks( this );

        // register for state changes for TrainCctvResource, and for CoreResource
        m_statusManager.registerForStateChanges( TA_IRS_Bus::CommonTypes::CoreResource, *this );
        m_statusManager.registerForStateChanges( TA_IRS_Bus::CommonTypes::TrainCctvResource, *this );

        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::ChangeoverStatusEvent::getStaticType(), this );

        FUNCTION_EXIT;
    }


    TimsCctvManager::~TimsCctvManager()
    {
        FUNCTION_ENTRY( "~TimsCctvManager" );

        //
        // deregister for everything registered for in the constructor
        //

        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::ChangeoverStatusEvent::getStaticType(), this );

        // deregister for train events
        m_statusManager.deregisterForStateChanges( TA_IRS_Bus::CommonTypes::CoreResource, *this );
        m_statusManager.deregisterForStateChanges( TA_IRS_Bus::CommonTypes::TrainCctvResource, *this );

        // deregister for operation state callbacks
        //TA_Base_Bus::AgentManager::deregisterForStateChangeCallbacks( this );

        // deregister for ats changes
        m_atsMessageProcessor.removeObserver( this );

        // deregister for operaton mode changes
        m_operationModeManager.deregisterForOperationModeChanges( this );

        // cancel any ongoing execution
        cancelAllCarrierOffTasks();

        // remove the thread pool instance
        TA_Base_Core::ThreadPoolSingletonManager::removeInstance( &m_threadPool );

        FUNCTION_EXIT;
    }


    unsigned short TimsCctvManager::getFlexibleTimeOutValue()
    {
        FUNCTION_ENTRY( "getFlexibleTimeOutValue" );

        // from m_configuration, get the number of retries, and timer cctv1
        unsigned long timeout = ( ( ( m_configuration.getTimsRetryLimit() + 1 ) * m_configuration.getTimerCCTV1() ) / 1000 ) + 5; // +5 for processing overhead

        FUNCTION_EXIT;
        return static_cast< unsigned short>( timeout );
    }


    void TimsCctvManager::setCarrierOn( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                        const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setCarrierOn" );

        // call setCarrierOnInternal with no specified train location
        setCarrierOnInternal( trainId, sessionId );

        FUNCTION_EXIT;
    }


    void TimsCctvManager::setCarrierOnDelocalisedTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                        unsigned long trainLocation,
                                                        const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setCarrierOnDelocalisedTrain" );

        //
        // First ensure this train is delocalised
        //

        {
            // get the train comms state
            TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

            // acquire the state lock
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // if ATS is valid, and localisation is valid,
            // throw an InvalidParameterException( location can only be specified on delocalised or invalid trains)
            if ( ( true == commsState->isAtsValid() ) &&
                 ( true == commsState->isLocalisationValid() ) )
            {
                TA_THROW( TA_IRS_Bus::InvalidParameterException( "Location can only be specified on delocalised or invalid trains",
                                                                 "Location",
                                                                 TA_IRS_Bus::LocationCache::getInstance().getLocationByKey( trainLocation ) ) );
            }

            // release the state lock
        }


        // call setCarrierOnInternal with the specified train location
        setCarrierOnInternal( trainId, sessionId, trainLocation );

        FUNCTION_EXIT;
    }


    void TimsCctvManager::setCarrierOff( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                         const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setCarrierOff" );

        // check the operator has the required priveliges to deactivate cctv using m_rightsChecker.canControlCctv
        // if the operator doesnt have rights, throw an AccessDeniedException

        if ( false == m_rightsChecker.canControlCctv( sessionId ) )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not control CCTV for this train" ) );
        }

        if ( false == m_operationModeManager.isInControlOfTrain( trainId, true, true ) )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Train is not controlled from this location" ) );
        }

        setCarrierOffInternal( trainId, sessionId );

        // audit carrier off
        m_auditSender.auditCctvCarrier( sessionId, trainId, time( NULL ), false );

        FUNCTION_EXIT;
    }


    void TimsCctvManager::setSingleScreen( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                           TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera,
                                           const std::string& sessionId,
                                           unsigned long trainLocation )
    {
        FUNCTION_ENTRY( "setSingleScreen" );

        // create a CctvSingleCommand
        // get the operator's console from m_configuration (getConsoleFromSession)
        // then get the origin for the console from m_configuration (getConsoleOrigin)

        TA_IRS_Bus::CctvSingleCommandPtr command(
            new TA_IRS_Bus::CctvSingleCommand( trainId,
                                               m_configuration.getOperatorOrigin( sessionId ),
                                               camera ) );

        // call the common, templatised implementation
        genericSetCctvMode( trainId,
                            sessionId,
                            command,
                            "CCTV Single Screen",
                            trainLocation );

        FUNCTION_EXIT;
    }


    void TimsCctvManager::setQuadScreen( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                         TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraW,
                                         TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraX,
                                         TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraY,
                                         TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraZ,
                                         const std::string& sessionId,
                                         unsigned long trainLocation )
    {
        FUNCTION_ENTRY( "setQuadScreen" );

        // create a CctvSingleCommand
        // get the operator's console from m_configuration (getConsoleFromSession)
        // then get the origin for the console from m_configuration (getConsoleOrigin)

        TA_IRS_Bus::CctvQuadCommandPtr command(
            new TA_IRS_Bus::CctvQuadCommand( trainId,
                                             m_configuration.getOperatorOrigin( sessionId ),
                                             cameraW, cameraX, cameraY, cameraZ ) );

        // call the common, templatised implementation
        genericSetCctvMode( trainId,
                            sessionId,
                            command,
                            "CCTV Quad Screen",
                            trainLocation );

        FUNCTION_EXIT;
    }


    void TimsCctvManager::setSequence( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                       TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceNumber sequenceNo,
                                       TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceMode sequenceMode,
                                       const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setSequence" );

        // create a CctvSingleCommand
        // get the operator's console from m_configuration (getConsoleFromSession)
        // then get the origin for the console from m_configuration (getConsoleOrigin)

        TA_IRS_Bus::CctvSequenceCommandPtr command(
            new TA_IRS_Bus::CctvSequenceCommand( trainId,
                                                 m_configuration.getOperatorOrigin( sessionId ),
                                                 sequenceNo, sequenceMode ) );

        // call the common, templatised implementation
        genericSetCctvMode( trainId,
                            sessionId,
                            command,
                            "CCTV Sequence" );

        FUNCTION_EXIT;
    }


    void TimsCctvManager::addAlarmImage( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                         const TA_IRS_Bus::CctvTypes::TrainCameraSequence& cameraIds,
                                         const std::string& sessionId )
    {
        FUNCTION_ENTRY( "addAlarmImage" );

        // call addAlarmImageInternal with no specified train location
        addAlarmImageInternal( trainId, cameraIds, sessionId );

        FUNCTION_EXIT;
    }


    void TimsCctvManager::addAlarmImageDelocalisedTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                         const TA_IRS_Bus::CctvTypes::TrainCameraSequence& cameraIds,
                                                         unsigned long trainLocation,
                                                         const std::string& sessionId )
    {
        FUNCTION_ENTRY( "addAlarmImageDelocalisedTrain" );

        //
        // First ensure this train is delocalised
        //

        {
            // get the train comms state
            TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

            // acquire the state lock
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // if ATS is valid, and localisation is valid,
            // throw an InvalidParameterException( location can only be specified on delocalised or invalid trains)

            if ( ( true == commsState->isAtsValid() ) &&
                 ( true == commsState->isLocalisationValid() ) )
            {
                TA_THROW( TA_IRS_Bus::InvalidParameterException( "Location can only be specified on delocalised or invalid trains",
                                                                 "Location",
                                                                 TA_IRS_Bus::LocationCache::getInstance().getLocationByKey( trainLocation ) ) );
            }

            // release the state lock
        }

        // call addAlarmImageInternal with the specified train location
        addAlarmImageInternal( trainId, cameraIds, sessionId, trainLocation );

        FUNCTION_EXIT;
    }


    void TimsCctvManager::removeAlarmImage( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                            TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraId,
                                            const std::string& sessionId )
    {
        FUNCTION_ENTRY( "removeAlarmImage" );

        // get the train cctv state
        TrainCctvStatePtr cctvState = m_statusManager.getTrainCctvState( trainId );
        TA_IRS_Bus::CctvTypes::CctvSwitchState cctvSwitchState;
        {
            // acquire the train cctv state lock
            TA_IRS_Bus::TrainStateLock stateLock( cctvState );

            // get the train's cctv switch state
            cctvSwitchState = cctvState->getCctvSwitchState();

            // release the train cctv state lock
        }

        // if the state is not a quad or camera
        // throw a CctvException( this train is not in alarm mode )

        if ( ( TA_IRS_Bus::CctvTypes::CctvInactive == cctvSwitchState.cctvState ) ||
             ( TA_IRS_Bus::CctvTypes::CctvSequence == cctvSwitchState.cctvState ) )
        {
            TA_THROW( TA_IRS_Bus::CctvException( "Train is not in alarm mode", TA_IRS_Bus::ProtocolCctvTypes::CctvFailed ) );
        }


        // create a list for the alarm stack items
        TA_IRS_Bus::CctvTypes::TrainCameraSequence alarmStack;

        //
        // for each item in the switch state
        // if the item is not 0, push it onto the back of the alarm stack list
        if ( 0 != cctvSwitchState.item1 )
        {
            alarmStack.push_back( static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>( cctvSwitchState.item1 ) );
        }

        if ( 0 != cctvSwitchState.item2 )
        {
            alarmStack.push_back( static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>( cctvSwitchState.item2 ) );
        }

        if ( 0 != cctvSwitchState.item3 )
        {
            alarmStack.push_back( static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>( cctvSwitchState.item3 ) );
        }

        if ( 0 != cctvSwitchState.item4 )
        {
            alarmStack.push_back( static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>( cctvSwitchState.item4 ) );
        }


        // find the given camera ID in alarmStack
        TA_IRS_Bus::CctvTypes::TrainCameraSequence::iterator findIter = std::find( alarmStack.begin(),
                                                                                   alarmStack.end(),
                                                                                   cameraId );

        // if it is not found, return, as this operation has no effect
        if ( alarmStack.end() == findIter )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "Camera %d not found in alarm stack on train %d to remove - possible logic error",
                         cameraId, trainId );

            FUNCTION_EXIT;
            return;
        }

        // if it is found, remove it
        alarmStack.erase( findIter );

        // if the new size of alarmStack is 0
        if ( true == alarmStack.empty() )
        {
            // then the train image can be deactivated
            setCarrierOff( trainId, sessionId );
        }

        // if the new size of alarmStack is 1
        else if ( 1 == alarmStack.size() )
        {
            // then this train can be set to single screen mode
            setSingleScreen( trainId, *alarmStack.begin(), sessionId );
        }

        // otherwise a quad must be set
        else
        {
            // if the alarmStack size is less than 4, pad it out to length 4 using CameraNone as the padding value
            if ( alarmStack.size() < 4 )
            {
                alarmStack.insert( alarmStack.end(),
                                   4 - alarmStack.size(),
                                   TA_IRS_Bus::ProtocolCctvTypes::CameraNone );
            }

            // set the train to quad mode using
            setQuadScreen( trainId, alarmStack[0], alarmStack[1], alarmStack[2], alarmStack[3], sessionId );
        }

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::CctvTypes::CctvSwitchState TimsCctvManager::getActiveTrainForStage( unsigned long stageId )
    {
        FUNCTION_ENTRY( "getActiveTrainForStage" );

        // get all train comms states
        TA_IRS_Bus::TrainIdToStateMap commsStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::CoreResource );

        // get all train cctv states
        TA_IRS_Bus::TrainIdToStateMap cctvStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainCctvResource );

        // both maps can be iterated through at the same time, they both contain the same trains
        // and are both sorted. we can assert to prove this fact.
        TA_ASSERT( commsStates.size() == cctvStates.size(),
                   "Both state maps should contain the same trains" );

        TA_IRS_Bus::TrainIdToStateMap::iterator commsStateIter = commsStates.begin();
        TA_IRS_Bus::TrainIdToStateMap::iterator cctvStateIter = cctvStates.begin();

        // Note: only need to check 1 iterator bound due to the assert above

        for ( ; commsStates.end() != commsStateIter ; ++commsStateIter, ++cctvStateIter )
        {
            // downcast to train cctv state
            TrainCctvStatePtr cctvState = boost::dynamic_pointer_cast< TrainCctvState >( cctvStateIter->second );
            TA_ASSERT( NULL != cctvState.get(), "Cast failed" );

            TA_IRS_Bus::CctvTypes::CctvSwitchState trainCctvState;

            {
                // get the cctv state lock
                TA_IRS_Bus::TrainStateLock stateLock( cctvState );

                // if the train is not active, continue (skip this train)

                if ( false == cctvState->isCctvActivated() )
                {
                    continue;
                }

                // get the train cctv details
                trainCctvState = cctvState->getCctvSwitchState();

                // release the cctv state lock
            }

            //    if here, the train is active

            // downcast to train comms state
            TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( commsStateIter->second );
            TA_ASSERT( NULL != commsState.get(), "Cast failed" );

            {
                // get the comms state lock
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                // if the train is not valid, continue  (skip this train)
                // ignore ats validity - as this may have been manually activated

                if ( false == commsState->isValid() )
                {
                    continue;
                }

                // get the train cctv zone
                trainCctvState.stageId = commsState->getCurrentCctvZone().getCurrentStage();

                // release the comms state lock
            }

            // if the cctv zones current stage matches the given stage, return the train cctv details

            if ( trainCctvState.stageId == stageId )
            {
                FUNCTION_EXIT;
                return trainCctvState;
            }
        }

        // return empty cctv details, with the train id set to 0
        TA_IRS_Bus::CctvTypes::CctvSwitchState emptyState;
        emptyState.trainId = 0;
        emptyState.preConflictTrain = 0;
        emptyState.stageId = 0;
        emptyState.cctvState = TA_IRS_Bus::CctvTypes::CctvInactive;
        emptyState.item1 = 0;
        emptyState.item2 = 0;
        emptyState.item3 = 0;
        emptyState.item4 = 0;

        FUNCTION_EXIT;
        return emptyState;
    }


    void TimsCctvManager::notifyOperationStateChange( TA_Base_Core::EOperationMode operationState )
    {
        FUNCTION_ENTRY( "notifyOperationStateChange" );

        // if the operation state is control
        if ( TA_Base_Core::Control == operationState )
        {
            // then re-evaluate conflict state so that any carrier off
            // tasks scheduled in the previous control agent can be rescheduled in this agent
            reEvaluateConflicts();
        }

        // if the operation state is any other state
        else
        {
            // cancel all carrier off tasks
            cancelAllCarrierOffTasks();
        }


        // now if this is the first control mode, broadcast the current state
        if ( true == m_firstRun )
        {
            // if the mode is control
            if ( TA_Base_Core::Control == operationState )
            {
                m_firstRun = false;

                // get all train comms states
                TA_IRS_Bus::TrainIdToStateMap commsStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::CoreResource );

                // get all train cctv states
                TA_IRS_Bus::TrainIdToStateMap cctvStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainCctvResource );

                // both maps can be iterated through at the same time, they both contain the same trains
                // and are both sorted. we can assert to prove this fact.
                TA_ASSERT( commsStates.size() == cctvStates.size(),
                           "Both state maps should contain the same trains" );

                TA_IRS_Bus::TrainIdToStateMap::iterator commsStateIter = commsStates.begin();
                TA_IRS_Bus::TrainIdToStateMap::iterator cctvStateIter = cctvStates.begin();

                // Note: only need to check 1 iterator bound due to the assert above

                for ( ; commsStates.end() != commsStateIter ; ++commsStateIter, ++cctvStateIter )
                {
                    // downcast to train cctv state
                    TrainCctvStatePtr cctvState = boost::dynamic_pointer_cast< TrainCctvState >( cctvStateIter->second );
                    TA_ASSERT( NULL != cctvState.get(), "Cast failed" );

                    // downcast to train comms state
                    TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( commsStateIter->second );
                    TA_ASSERT( NULL != commsState.get(), "Cast failed" );

                    TA_IRS_Bus::CctvTypes::CctvSwitchState update;
                    {
                        TA_IRS_Bus::TrainStateLock stateLock( cctvState );

                        update = cctvState->getCctvSwitchState();
                    }

                    // populate the stage
                    {
                        TA_IRS_Bus::TrainStateLock stateLock( commsState );

                        update.stageId = commsState->getCurrentCctvZone().getCurrentStage();
                    }

                    // send a client update
                    m_messageSender.sendClientUpdate( update );
                }
            }
            // otherwise if its monitor
            else if ( TA_Base_Core::Monitor == operationState )
            {
                m_firstRun = false;
            }
        }

        FUNCTION_EXIT;
    }


    void TimsCctvManager::locationControlAcquired( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "locationControlAcquired" );

        // Check the agent is in control mode using AgentManager, if not return.

        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // This agent has gained control of a station, if there are any trains in this area in pre conflict, the alarm must be raised
        // also, if there are any that should be deactivated, the task should be scheduled

        reEvaluateConflicts();

        FUNCTION_EXIT;
    }


    void TimsCctvManager::locationControlReleased( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "locationControlReleased" );

        // Check the agent is in control mode using AgentManager, if not return

        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // This agent has lost control of a station, if there are any trains in this area in pre conflict,
        // the alarm must be closed as this agent can no longer resolve the conflict

        // Pre conflict is only possible for trains actively broadcasting,
        // so get the active train for the location using getActiveTrainForStation
        TA_IRS_Bus::CctvTypes::CctvSwitchState activeTrainState = getActiveTrainForStation( locationKey );

        // if it returns a train ID other than 0
        if ( 0 != activeTrainState.trainId )
        {
            // close any open pre conflict alarm using closePreConflictAlarm
            closePreConflictAlarm( activeTrainState.trainId );
        }

        // also, any carrier off tasks for the location should be cancelled
        cancelCarrierOffTasksForLocation( locationKey );

        FUNCTION_EXIT;
    }


    void TimsCctvManager::processAtsUpdate( const AtsTrainInformation& oldInformation,
                                            const AtsTrainInformation& newInformation )
    {
        FUNCTION_ENTRY( "processAtsUpdate" );

        // Check the agent is in control mode using AgentManager, if not return

        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        TA_IRS_Bus::CommonTypes::TrainIdType trainId = newInformation.physicalTrainNumber;

        // Get the CCTV state for this train
        TrainCctvStatePtr cctvState = m_statusManager.getTrainCctvState( trainId );
        {
            // acquire the train cctv state lock
            TA_IRS_Bus::TrainStateLock stateLock( cctvState );

            // if the cctv is not active, return

            if ( false == cctvState->isCctvActivated() )
            {
                FUNCTION_EXIT;
                return;
            }

            // release the train cctv state lock
        }

        // wasInControlArea = ( old ATS valid ) AND ( agent is in control of old location )
        bool wasInControlArea = ( true == oldInformation.trainValid ) &&
                                ( m_operationModeManager.isInControlOfLocation( oldInformation.localisation.currentLocation ) );

        // isInControlArea = ( new ATS valid ) AND ( agent is in control of new location )
        bool isInControlArea = ( true == newInformation.trainValid ) &&
                               ( m_operationModeManager.isInControlOfLocation( newInformation.localisation.currentLocation ) );

        // if this agent is in degraded mode

        if ( true == m_operationModeManager.isIscsDegradedMode() )
        {
            // This means this is a station agent in control of its location.

            // if the the train was in our control area and now isnt in our control area
            if ( ( true == wasInControlArea ) &&
                 ( false == isInControlArea ) )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "Train %d moved out of this agents area of control in degraded mode. Deactivating train CCTV.",
                             trainId );

                // then we must carrier off immediately
                scheduleCarrierOffTask( trainId, 0, false );

                // the train has been deactivated, there is no more processing so return
                FUNCTION_EXIT;
                return;
            }

            // create a CctvZone object for both the old and new cctv zones
            CctvZone oldZone( oldInformation.localisation.currentCctvZone );
            CctvZone newZone( newInformation.localisation.currentCctvZone );

            // if the old cctv zone type is not conflict, and the new zone type is
            if ( ( CctvZone::Conflict != oldZone.getZoneType() ) &&
                 ( CctvZone::Conflict == newZone.getZoneType() ) )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "Train %d moved into a conflict zone in degraded mode. Deactivating train CCTV.",
                             trainId );

                // then we must carrier off immediately
                scheduleCarrierOffTask( trainId, 0, false );

                // the train has been deactivated, there is no more processing so return
                FUNCTION_EXIT;
                return;
            }

            // if the train was not in our control area, and now is
            if ( ( false == wasInControlArea ) &&
                 ( true == isInControlArea ) )
            {
                // we must assume the train is no longer active
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                             "Train %d moved into control area in degraded mode. Assuming CCTV inactive.",
                             trainId );

                // acquire the train cctv state lock
                TA_IRS_Bus::TrainStateLock stateLock( cctvState );

                // set the train to deactivated
                cctvState->setActivated( false, time( NULL ) );

                // release the train cctv state lock
            }
        }
        else
        {
            // this is a station in control but not degraded, or the OCC in normal mode

            // first check if this agent is not in control of the train's current location
            if ( false == isInControlArea )
            {
                // check if it was in the control area
                if ( true == wasInControlArea )
                {
                    // then any pre conflict alarms must be closed
                    closePreConflictAlarm( trainId );
                }

                // cancel any queued carrier off tasks
                cancelCarrierOffTask( trainId );

                // nothing can be done to this train if it is not controlled by this agent, return
                FUNCTION_EXIT;
                return;
            }

            // second check if the train changed from OCC->Depot, or Depot->OCC
            if ( ( false == oldInformation.trainValid ) ||
                 ( m_configuration.getLocationType( oldInformation.localisation.currentLocation ) !=
                   m_configuration.getLocationType( newInformation.localisation.currentLocation ) ) )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "Train %d changed areas. Deactivating train CCTV.",
                             trainId );

                // this means the area changed, schedule a carrier off task, but wait for any change modes to complete first
                scheduleCarrierOffTask( trainId, 0, true );
            }

            // third, check if this train has moved cctv zones
            // create a CctvZone object for both the old and new cctv zones
            CctvZone oldZone( oldInformation.localisation.currentCctvZone );
            CctvZone newZone( newInformation.localisation.currentCctvZone );

            // if the new zone type is normal, and the old zone type is not normal
            if ( ( CctvZone::NoConflict != oldZone.getZoneType() ) &&
                 ( CctvZone::NoConflict == newZone.getZoneType() ) )
            {
                bool wasInPreConflict = false;

                {
                    // acquire the state lock
                    TA_IRS_Bus::TrainStateLock stateLock( cctvState );

                    wasInPreConflict = cctvState->isPreConflict();

                    // reset cctv conflict
                    cctvState->clearPreConflict();

                    // release the state lock
                }

                // if the train was in pre conflict, then any pre conflict alarms must be closed

                if ( true == wasInPreConflict )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                 "Train %d moved out of pre conflict. Clearing pre conflict state.",
                                 trainId );

                    closePreConflictAlarm( trainId );
                }
            }

            // else if (the new zone type is conflict or pre conflict) and (the old zone type is not the same as the new zone type)
            else if ( ( CctvZone::NoConflict != newZone.getZoneType() ) &&
                      ( oldZone.getZoneType() != newZone.getZoneType() ) )
            {
                // check if there is a train broadcasting in the next stage using getActiveTrainForStage
                TA_IRS_Bus::CctvTypes::CctvSwitchState activeTrainState = getActiveTrainForStage( newZone.getNextStage() );

                // if there is

                if ( 0 != activeTrainState.trainId )
                {
                    // then this train is either in pre conflict or conflict
                    if ( CctvZone::PreConflict == newZone.getZoneType() )
                    {
                        bool wasInPreConflict = false;
                        {
                            // acquire the state lock
                            TA_IRS_Bus::TrainStateLock stateLock( cctvState );

                            wasInPreConflict = cctvState->isPreConflict();

                            // set cctv pre conflict
                            cctvState->setPreConflict( activeTrainState.trainId );

                            // release the state lock
                        }

                        // if the train was not in pre conflict, then a pre conflict alarms must be raised

                        if ( false == wasInPreConflict )
                        {
                            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                         "Train %d moved into pre conflict with train %d",
                                         trainId, activeTrainState.trainId );

                            raisePreConflictAlarm( trainId, activeTrainState.trainId );
                        }
                    }
                    else if ( CctvZone::Conflict == newZone.getZoneType() )
                    {
                        bool isOccAgent = ( TA_Base_Core::ILocation::OCC == m_configuration.getAgentLocationType() );

                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                     "Train %d moved into conflict with train %d. Deactivating train CCTV. %s",
                                     trainId, activeTrainState.trainId,
                                     ( true == isOccAgent ) ? "Disallowing stage switch" : "" );

                        // send a carrier off to this train by scheduling a carrier off task
                        scheduleCarrierOffTask( trainId, activeTrainState.trainId, false );

                        // then if this is the OCC Agent
                        // disallow switching from the current zone to the next zone

                        if ( true == isOccAgent )
                        {
                            try
                            {
                                m_switchAgent.setStageSwitchMode( newZone.getCurrentStage(),
                                                                  newZone.getNextStage(),
                                                                  false );
                            }
                            catch ( TA_IRS_Bus::TrainException& e )
                            {
                                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainException", e.what() );
                            }
                        }
                    }
                }
                else
                {
                    // if the current zone type is a conflict zone, and this is the OCC agent
                    // then if this is the OCC Agent allow switching from the current zone to the next zone

                    if ( CctvZone::Conflict == newZone.getZoneType() )
                    {
                        bool isOccAgent = ( TA_Base_Core::ILocation::OCC == m_configuration.getAgentLocationType() );

                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                     "Train %d moved into conflict zone with no conflicting train. %s",
                                     trainId,
                                     ( true == isOccAgent ) ? "Allowing stage switch" : "" );

                        if ( true == isOccAgent )
                        {
                            try
                            {
                                m_switchAgent.setStageSwitchMode( newZone.getCurrentStage(),
                                                                  newZone.getNextStage(),
                                                                  true );
                            }
                            catch ( TA_IRS_Bus::TrainException& e )
                            {
                                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainException", e.what() );
                            }
                        }

                        bool wasInPreConflict = false;

                        {
                            // acquire the state lock
                            TA_IRS_Bus::TrainStateLock stateLock( cctvState );

                            wasInPreConflict = cctvState->isPreConflict();

                            // reset cctv conflict
                            cctvState->clearPreConflict();

                            // release the state lock
                        }

                        // if the train was in pre conflict, then any pre conflict alarms must be closed

                        if ( true == wasInPreConflict )
                        {
                            closePreConflictAlarm( trainId );
                        }
                    }
                }
            }
        }

        FUNCTION_EXIT;
    }


    void TimsCctvManager::processTrainStateChanged( TA_IRS_Bus::TrainStatePtr state )
    {
        FUNCTION_ENTRY( "processTrainStateChanged" );

        // Checks the agent is in control state and returns if not
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        TA_IRS_Bus::CommonTypes::ETrainResource trainResource = state->getResourceCategory();

        // downcast the state to either a TrainCommsStatePtr or TrainCctvStatePtr and call the appropriate method

        switch ( trainResource )
        {

            case TA_IRS_Bus::CommonTypes::CoreResource:
            {
                TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( state );
                TA_ASSERT( NULL != commsState.get(), "downcast to TrainCommsState failed" );

                processTrainCommsStateChanged( commsState );
            }

            break;

            case TA_IRS_Bus::CommonTypes::TrainCctvResource:
            {
                TrainCctvStatePtr cctvState = boost::dynamic_pointer_cast< TrainCctvState >( state );
                TA_ASSERT( NULL != cctvState.get(), "downcast to TrainCctvState failed" );

                processTrainCctvStateChanged( cctvState );
            }

            break;

            default:
                TA_ASSERT( false, "Received state change for unsubscribed state" );
                break;
        }

        FUNCTION_EXIT;
    }


    void TimsCctvManager::processEvent( TA_IRS_Bus::TrainEventPtr event )
    {
        FUNCTION_ENTRY( "processEvent" );
        
        // return if this agent isnt in the control state
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }
                
        if ( event->getEventType() == TA_IRS_Bus::ChangeoverStatusEvent::getStaticType() )
        {
            TA_IRS_Bus::ChangeoverStatusEventPtr changeoverStatusEvent = boost::dynamic_pointer_cast< TA_IRS_Bus::ChangeoverStatusEvent >( event );
            TA_ASSERT( NULL != changeoverStatusEvent.get(), "Event received identified as a changeover status event but did not cast to it" );
            
            processChangeoverStatusEvent( changeoverStatusEvent );
        }
        else
        {
            TA_ASSERT( false, "The event type received was not subscribed for" );
        }
        
        FUNCTION_EXIT;
    }
    void TimsCctvManager::setCarrierOffInternal( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                 const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setCarrierOffInternal" );

        unsigned char origin = 0;

        // if the session id is given
        if ( false == sessionId.empty() )
        {
            // this is an operator command
            origin = m_configuration.getOperatorOrigin( sessionId );
        }
        else
        {
            // this is an automagic command
            origin = m_configuration.getAgentOrigin();
        }

        // create a CctvCarrierOffCommand
        TA_IRS_Bus::CctvCarrierOffCommandPtr command(
            new TA_IRS_Bus::CctvCarrierOffCommand( trainId,
                                                   m_configuration.getOperatorOrigin( sessionId ) ) );

        // get the CCTV state
        TrainCctvStatePtr cctvState = m_statusManager.getTrainCctvState( trainId );

        try
        {

            {
                // get the command lock and acquire it using a threadguard
                // for some reason it doesnt work all in one line
                TA_Base_Core::IThreadLockable& commandLock = cctvState->getCommandLock();
                TA_THREADGUARD( commandLock );

                // send the command
                m_transactionManager.sendCommand( command,
                                                  m_configuration.getTimerCCTV1(),
                                                  m_configuration.getTimsRetryLimit() );

                // release the command lock
            }

            // get the CctvCommandReceived response, the bad command response, and the changeover status response from the command
            // if none of these are returned, ASSERT - the command should not exit normally without a valid resposne
            TA_IRS_Bus::CctvCommandReceivedEventPtr commandReceived = command->getCommandReceivedResponse();
            TA_IRS_Bus::BadCommandEventPtr badCommand = command->getBadCommandResponse();
            TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = command->getChangeoverStatusEventResponse();

            // if it is a CctvCommandReceived event
            if ( commandReceived )
            {
                // get the result of the command
                // if it is a cctv failed or CctvInUse, throw a CctvException
                if ( TA_IRS_Bus::ProtocolCctvTypes::CctvProceeding != commandReceived->getResult() )
                {
                    TA_THROW( TA_IRS_Bus::CctvException( "Failed to deactivate CCTV", commandReceived->getResult() ) );
                }

                // train deactivation was successful, perform post deactivation steps
                onTrainDeactivation( cctvState );
            }

            // if the bad command event is returned
            else if ( badCommand )
            {
                // get the error code from the event
                TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError errorCode = badCommand->getErrorCode();

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "CCTV Carrier Off command for train %d returned an error: %s",
                             trainId,
                             TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode ).c_str() );

                // if it is an UnexpectedCommand, log the error and consider this operation passed
                // if it is any of the other error codes, throw a BadCommandException

                if ( TA_IRS_Bus::ProtocolCommonTypes::UnexpectedCommand != errorCode )
                {
                    TA_THROW( TA_IRS_Bus::BadCommandException( "Error deactivating CCTV", errorCode ) );
                }
            }
            else if ( mpuChangeover )
            {
                // if the mpu changeover event is returned
                // throw an MpuChangeoverException
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "CCTV Carrier Off command for train %d returned an MPU changeover",
                             trainId );

                TA_THROW( TA_IRS_Bus::MpuChangeoverException( "MPU changeover while deactivating CCTV" ) );
            }
            else
            {
                // if no valid events were returned,
                // assert that the command completed without a valid response
                TA_ASSERT( false, "This command can only exit via exception or a valid response" );
            }
        }

        // These exceptions will be treated as success, they will all call:
        // onTrainDeactivation() then rethrow the exception using throw
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );

            onTrainDeactivation( cctvState );

            throw;
        }
        catch ( TA_IRS_Bus::TrainTransmissionFailureException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );

            onTrainDeactivation( cctvState );

            throw;
        }
        catch ( TA_IRS_Bus::TrainTimeoutException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimeoutException", e.what() );

            onTrainDeactivation( cctvState );

            throw;
        }
        catch ( TA_IRS_Bus::TransactionCancelledException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactionCancelledException", e.what() );

            onTrainDeactivation( cctvState );

            throw;
        }

        // any other exceptions mean the train responded with a bad status
        // and the operation should be retried by the operator

        FUNCTION_EXIT;
    }


    void TimsCctvManager::scheduleCarrierOffTask( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                  TA_IRS_Bus::CommonTypes::TrainIdType owningTrainId,
                                                  bool waitForCommunication )
    {
        FUNCTION_ENTRY( "scheduleCarrierOffTask" );

        // acquire the m_carrierOffTaskLock
        TA_THREADGUARD( m_carrierOffTaskLock );

        // find the entry for the given train
        CarrierOffTaskMap::iterator existingTask = m_carrierOffTasks.find( trainId );

        CarrierOffTaskPtr task;

        // if a task doesnt exist
        if ( m_carrierOffTasks.end() == existingTask )
        {
            // create a new CarrierOffTask
            task.reset( new CarrierOffTask( *this, trainId, owningTrainId ) );

            // insert it into m_carrierOffTasks
            m_carrierOffTasks.insert( CarrierOffTaskMap::value_type( trainId, task ) );
        }
        else
        {
            task = existingTask->second;
        }

        // if the task is not scheduled
        if ( false == task->isScheduled() )
        {
            bool canBeScheduled = true;

            if ( true == waitForCommunication )
            {
                // get the comms state
                TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

                // acquire the state lock
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                canBeScheduled = ( true == commsState->isTrainCommunicationValid() ) &&
                                 ( commsState->getCommunicatingLocation() == m_configuration.getLocationKey() );

                // release the train state lock
            }

            // if the task can be scheduled
            if ( true == canBeScheduled )
            {
                // set it to scheduled
                task->setScheduled();

                // queue it in m_threadPool
                m_threadPool.queueWorkItem( task );
            }
        }

        FUNCTION_EXIT;
    }


    void TimsCctvManager::cancelAllCarrierOffTasks()
    {
        FUNCTION_ENTRY( "cancelAllCarrierOffTasks" );

        // acquire the m_carrierOffTaskLock
        TA_THREADGUARD( m_carrierOffTaskLock );

        // for each entry in m_carrierOffTasks

        for ( CarrierOffTaskMap::iterator taskIter = m_carrierOffTasks.begin();
              m_carrierOffTasks.end() != taskIter; /* incremented in loop */ )
        {
            // remove it from m_threadPool (in case it hasnt executed yet)
            m_threadPool.removeWorkItem( taskIter->second );

            // remove it from the map
            m_carrierOffTasks.erase( taskIter++ );
        }

        FUNCTION_EXIT;
    }


    void TimsCctvManager::cancelCarrierOffTask( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "cancelCarrierOffTask" );

        // acquire the m_carrierOffTaskLock
        TA_THREADGUARD( m_carrierOffTaskLock );

        // find the entry for the given train
        CarrierOffTaskMap::iterator existingTask = m_carrierOffTasks.find( trainId );

        // if it exists
        if ( m_carrierOffTasks.end() != existingTask )
        {
            // remove it from m_threadPool (in case it hasnt executed yet)
            m_threadPool.removeWorkItem( existingTask->second );

            // remove it from the map
            m_carrierOffTasks.erase( existingTask );
        }

        FUNCTION_EXIT;
    }


    void TimsCctvManager::cancelCarrierOffTasksForLocation( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "cancelCarrierOffTasksForLocation" );

        // acquire the m_carrierOffTaskLock
        TA_THREADGUARD( m_carrierOffTaskLock );

        // for each entry in m_carrierOffTasks
        for ( CarrierOffTaskMap::iterator taskIter = m_carrierOffTasks.begin();
              m_carrierOffTasks.end() != taskIter; /* incremented in loop */ )
        {
            unsigned long trainLocation = 0;
            {
                // get the train comms state for the train
                TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( taskIter->first );

                // acquire the state lock
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                if ( true == commsState->isAtsValid() )
                {
                    // get the location (if ats is valid)
                    trainLocation = commsState->getLocation();
                }

                // release the train state lock
            }

            // if the location matches the given location
            if ( trainLocation == locationKey )
            {
                // remove it from m_threadPool (in case it hasnt executed yet)
                m_threadPool.removeWorkItem( taskIter->second );

                // remove it from the map
                m_carrierOffTasks.erase( taskIter++ );
            }
            else
            {
                ++taskIter;
            }
        }

        FUNCTION_EXIT;
    }

    
    void TimsCctvManager::processChangeoverStatusEvent( TA_IRS_Bus::ChangeoverStatusEventPtr changeoverEvent )
    {
        FUNCTION_ENTRY( "processChangeoverStatusEvent" );
        
        //
        // set the cctv to inactive for this train
        //

        // get its CCTV state
        TrainCctvStatePtr cctvState = m_statusManager.getTrainCctvState( changeoverEvent->getTrainId() );
        
        // acquire the lock
        TA_IRS_Bus::TrainStateLock stateLock( cctvState );
        
        // set cctv inactive
        cctvState->setActivated( false, changeoverEvent->getTime() );
        
        FUNCTION_EXIT;
    }
    
    

    void TimsCctvManager::performCarrierOff( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                             TA_IRS_Bus::CommonTypes::TrainIdType owningTrainId )
    {
        FUNCTION_ENTRY( "performCarrierOff" );

        bool success = false;

        try
        {
            // send the carrier off command
            setCarrierOffInternal( trainId );

            success = true;
        }
        // catch any exceptions and log them
        // these exceptions are considered a pass
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );
            success = true;
        }
        catch ( TA_IRS_Bus::TrainTransmissionFailureException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );
            success = true;
        }
        catch ( TA_IRS_Bus::TrainTimeoutException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimeoutException", e.what() );
            success = true;
        }
        catch ( TA_IRS_Bus::TransactionCancelledException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactionCancelledException", e.what() );
            success = true;
        }
        // any other exceptions a failure
        catch ( TA_IRS_Bus::TrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainException", e.what() );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
        }
        catch ( ... )
        {
            TA_ASSERT( false, "FAIL - exception leak" );
        }


        if ( true == success )
        {
            if ( 0 != owningTrainId )
            {
                // carrier off due to conflict
                m_auditSender.auditCctvConflict( trainId,
                                                 owningTrainId,
                                                 time( NULL ) );
            }
            else
            {
                // carrier off due to some other reason (moving area most likely)
                m_auditSender.auditCctvCarrier( m_configuration.getAgentSessionId(),
                                                trainId,
                                                time( NULL ),
                                                false );
            }
        }

        TA_THREADGUARD( m_carrierOffTaskLock );

        // find the entry for the given train
        CarrierOffTaskMap::iterator existingTask = m_carrierOffTasks.find( trainId );

        // if it exists
        if ( m_carrierOffTasks.end() != existingTask )
        {
            // remove it from the map
            m_carrierOffTasks.erase( existingTask );
        }

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::CctvTypes::CctvSwitchState TimsCctvManager::getActiveTrainForStation( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "getActiveTrainForStation" );

        // get all train comms states
        TA_IRS_Bus::TrainIdToStateMap commsStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::CoreResource );

        // get all train cctv states
        TA_IRS_Bus::TrainIdToStateMap cctvStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainCctvResource );

        // both maps can be iterated through at the same time, they both contain the same trains
        // and are both sorted. we can assert to prove this fact.
        TA_ASSERT( commsStates.size() == cctvStates.size(),
                   "Both state maps should contain the same trains" );

        TA_IRS_Bus::TrainIdToStateMap::iterator commsStateIter = commsStates.begin();
        TA_IRS_Bus::TrainIdToStateMap::iterator cctvStateIter = cctvStates.begin();

        // Note: only need to check 1 iterator bound due to the assert above

        for ( ; commsStates.end() != commsStateIter ; ++commsStateIter, ++cctvStateIter )
        {
            // downcast to train cctv state
            TrainCctvStatePtr cctvState = boost::dynamic_pointer_cast< TrainCctvState >( cctvStateIter->second );
            TA_ASSERT( NULL != cctvState.get(), "Cast failed" );

            TA_IRS_Bus::CctvTypes::CctvSwitchState trainCctvState;

            {
                // get the cctv state lock
                TA_IRS_Bus::TrainStateLock stateLock( cctvState );

                // if the train is not active, continue (skip this train)

                if ( false == cctvState->isCctvActivated() )
                {
                    continue;
                }

                // get the train cctv details
                trainCctvState = cctvState->getCctvSwitchState();

                // release the cctv state lock
            }

            // if here, the train is active

            // downcast to train comms state
            TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( commsStateIter->second );
            TA_ASSERT( NULL != commsState.get(), "Cast failed" );

            unsigned long trainLocation = 0;
            {
                // get the comms state lock
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                // if the train is not valid, continue  (skip this train)
                // ignore ats validity - as this may have been manually activated
                if ( false == commsState->isValid() )
                {
                    continue;
                }

                // get the train location
                trainLocation = commsState->getLocation();

                // get the train cctv zone
                trainCctvState.stageId = commsState->getCurrentCctvZone().getCurrentStage();

                // release the comms state lock
            }

            // if the location matches the given location, return the train cctv details
            if ( trainLocation == locationKey )
            {
                FUNCTION_EXIT;
                return trainCctvState;
            }
        }

        // return empty cctv details, with the train id set to 0
        TA_IRS_Bus::CctvTypes::CctvSwitchState emptyState;
        emptyState.trainId = 0;
        emptyState.preConflictTrain = 0;
        emptyState.stageId = 0;
        emptyState.cctvState = TA_IRS_Bus::CctvTypes::CctvInactive;
        emptyState.item1 = 0;
        emptyState.item2 = 0;
        emptyState.item3 = 0;
        emptyState.item4 = 0;

        FUNCTION_EXIT;
        return emptyState;
    }


    void TimsCctvManager::setCarrierOnInternal( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                const std::string& sessionId,
                                                unsigned long delocalisedTrainLocation )
    {
        FUNCTION_ENTRY( "setCarrierOnInternal" );

        // create a CctvCarrierOnCommand
        TA_IRS_Bus::CctvCarrierOnCommandPtr command(
            new TA_IRS_Bus::CctvCarrierOnCommand( trainId, m_configuration.getOperatorOrigin( sessionId ) ) );


        // call the common, templatised version
        genericSetCctvMode( trainId,
                            sessionId,
                            command,
                            "CCTV Carrier On",
                            delocalisedTrainLocation );

        FUNCTION_EXIT;
    }


    void TimsCctvManager::addAlarmImageInternal( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                 const TA_IRS_Bus::CctvTypes::TrainCameraSequence& cameraIds,
                                                 const std::string& sessionId,
                                                 unsigned long trainLocation )
    {
        FUNCTION_ENTRY( "addAlarmImageInternal" );

        // get the train cctv state
        TrainCctvStatePtr cctvState = m_statusManager.getTrainCctvState( trainId );
        TA_IRS_Bus::CctvTypes::CctvSwitchState cctvSwitchState;
        {
            // acquire the train cctv state lock
            TA_IRS_Bus::TrainStateLock stateLock( cctvState );

            // get the train's cctv switch state
            cctvSwitchState = cctvState->getCctvSwitchState();

            // release the train cctv state lock
        }

        // use addCamerasToAlarmStack to get the new CCTV switch state
        TA_IRS_Bus::CctvTypes::TrainCameraSequence alarmItems = addCamerasToAlarmStack( cctvSwitchState, cameraIds );

        // assert the new alarm stack has 4 entries
        // TA_ASSERT( alarmItems.size() == 4, "Must have 4 items" );

        // if item 2 (index 1) of the new alarm stack is CameraNone
        if ( TA_IRS_Bus::ProtocolCctvTypes::CameraNone == alarmItems[1] )
        {
            // this is a single image
            setSingleScreen( trainId, alarmItems[0], sessionId, trainLocation );
        }
        else
        {
            // more than one image, a quad must be used
            setQuadScreen( trainId, alarmItems[0], alarmItems[1], alarmItems[2], alarmItems[3], sessionId, trainLocation );
        }

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::CctvTypes::TrainCameraSequence TimsCctvManager::addCamerasToAlarmStack( const TA_IRS_Bus::CctvTypes::CctvSwitchState& switchState,
            const TA_IRS_Bus::CctvTypes::TrainCameraSequence& cameraIds )
    {
        FUNCTION_ENTRY( "addCamerasToAlarmStack" );

        // create a list for the alarm stack items
        std::list< TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera > alarmStack;

        if ( TA_IRS_Bus::CctvTypes::CctvCamera == switchState.cctvState )
        {
            // push back item 1 from the switch state
            if ( 0 != switchState.item1 )
            {
                alarmStack.push_back( static_cast< TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera > ( switchState.item1 ) );
            }
        }
        else if ( TA_IRS_Bus::CctvTypes::CctvQuad == switchState.cctvState )
        {
            // for each item in the switch state
            // if the item is not 0, push it onto the back of the alarm stack list
            if ( 0 != switchState.item1 )
            {
                alarmStack.push_back( static_cast< TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera >( switchState.item1 ) );
            }

            if ( 0 != switchState.item2 )
            {
                alarmStack.push_back( static_cast< TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera >( switchState.item2 ) );
            }

            if ( 0 != switchState.item3 )
            {
                alarmStack.push_back( static_cast< TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera >( switchState.item3 ) );
            }

            if ( 0 != switchState.item4 )
            {
                alarmStack.push_back( static_cast< TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera >( switchState.item4 ) );
            }
        }

        // otherwise the train may be in the sequence or inactive CCTV mode

        // now the new camera IDs can be added to the list:
        // for each item in the cameraIds list in reverse order
        for ( TA_IRS_Bus::CctvTypes::TrainCameraSequence::const_reverse_iterator revIter = cameraIds.rbegin();
              revIter != cameraIds.rend(); ++revIter )
        {
            // push it on the FRONT of the alarmStack list
            if ( TA_IRS_Bus::ProtocolCctvTypes::CameraNone != *revIter )
            {
                alarmStack.push_front( *revIter );
            }
        }

        // now the first 4 unique items of the alarmStack list will build the quad state
        // ordered list (maintains original sorting order)
        TA_IRS_Bus::CctvTypes::TrainCameraSequence uniqueAlarmStack;

        // sorted list (for efficient uniqueness checking)
        std::set< TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera > uniqueCheck;

        // for each item in alarmStack
        for ( std::list< TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera >::iterator iter = alarmStack.begin();
              alarmStack.end() != iter; ++iter )
        {
            // if it does not yet exist in uniqueAlarmStack, push it onto the back
            if ( uniqueCheck.find( *iter ) == uniqueCheck.end() )
            {
                uniqueAlarmStack.push_back( *iter );
                uniqueCheck.insert( *iter );
            }
        }

        // if the uniqueAlarmStack size is less than 4, padd it out to length 4 using CameraNone as the padding value
        if ( uniqueAlarmStack.size() < 4 )
        {
            uniqueAlarmStack.insert( uniqueAlarmStack.end(),
                                     4 - uniqueAlarmStack.size(),
                                     TA_IRS_Bus::ProtocolCctvTypes::CameraNone );
        }

        FUNCTION_EXIT;

        return uniqueAlarmStack;
    }


    template <typename CommandTypePtr>
    void TimsCctvManager::genericSetCctvMode( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                              const std::string& sessionId,
                                              CommandTypePtr command,
                                              const std::string& operationDescription,
                                              unsigned long trainLocation )
    {
        FUNCTION_ENTRY( "genericSetCctvMode" );

        // perform all pre activation checks, and get the train conflcit state
        TrainConflict trainConflict = TimsCctvManager::preActivateTrain( trainId, sessionId, trainLocation );

        // get the CCTV state
        TrainCctvStatePtr cctvState = m_statusManager.getTrainCctvState( trainId );
        {
            // get the command lock and acquire it using a threadguard
            // for some reason it doesnt work all in one line
            TA_Base_Core::IThreadLockable& commandLock = cctvState->getCommandLock();
            TA_THREADGUARD( commandLock );

            // send the command
            m_transactionManager.sendCommand( command,
                                              m_configuration.getTimerCCTV1(),
                                              m_configuration.getTimsRetryLimit() );

            // release the command lock
        }

        // get the CctvCommandReceived response, the bad command response, and the changeover status response from the command
        // if none of these are returned, ASSERT - the command should not exit normally without a valid resposne
        TA_IRS_Bus::CctvCommandReceivedEventPtr commandReceived = command->getCommandReceivedResponse();
        TA_IRS_Bus::BadCommandEventPtr badCommand = command->getBadCommandResponse();
        TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = command->getChangeoverStatusEventResponse();

        // if it is a CctvCommandReceived event

        if ( commandReceived )
        {
            // call the specific result handler for this command
            handleCommandResult( trainId,
                                 sessionId,
                                 cctvState,
                                 trainConflict,
                                 command,
                                 commandReceived );
        }

        // if the bad command event is returned
        else if ( badCommand )
        {
            // get the error code from the event
            TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError errorCode = badCommand->getErrorCode();

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "%s command for train %d returned an error: %s",
                         operationDescription.c_str(), trainId,
                         TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode ).c_str() );

            std::ostringstream error;
            error << "Error setting " << operationDescription;

            TA_THROW( TA_IRS_Bus::BadCommandException( error.str(), errorCode ) );
        }
        else if ( mpuChangeover )
        {
            // if the mpu changeover event is returned
            //    throw an MpuChangeoverException
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "%s command for train %d returned an MPU changeover",
                         operationDescription.c_str(), trainId );

            std::ostringstream error;
            error << "MPU changeover while setting " << operationDescription;

            TA_THROW( TA_IRS_Bus::MpuChangeoverException( error.str() ) );
        }
        else
        {
            // if no valid events were returned,
            // assert that the command completed without a valid response
            TA_ASSERT( false, "This command can only exit via exception or a valid response" );
        }

        FUNCTION_EXIT;
    }


    void TimsCctvManager::handleCommandResult( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                               const std::string& sessionId,
                                               TrainCctvStatePtr cctvState,
                                               const TrainConflict& trainConflict,
                                               TA_IRS_Bus::CctvCarrierOnCommandPtr command,
                                               TA_IRS_Bus::CctvCommandReceivedEventPtr commandReceived )
    {
        FUNCTION_ENTRY( "handleCommandResult( CctvCarrierOnCommandPtr )" );

        // get the result of the command
        // if it is a cctv failed, throw a CctvException
        // (CctvProceeding means the command succeeded, CctvInUse means the train was already active, so can be considered success)
        if ( TA_IRS_Bus::ProtocolCctvTypes::CctvFailed == commandReceived->getResult() )
        {
            TA_THROW( TA_IRS_Bus::CctvException( "Failed to set CCTV Carrier On", commandReceived->getResult() ) );
        }

        {
            // acquire the train cctv state lock
            TA_IRS_Bus::TrainStateLock stateLock( cctvState );

            // set the train to activated
            cctvState->setActivated( true, commandReceived->getTime() );

            // perform post activation steps
            postActivateTrain( cctvState, trainConflict );

            // release the train cctv state lock
        }

        // audit the operator action
        m_auditSender.auditCctvCarrier( sessionId, trainId, commandReceived->getTime(), true );

        FUNCTION_EXIT;
    }
    
    
    void TimsCctvManager::handleCommandResult( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                               const std::string& sessionId,
                                               TrainCctvStatePtr cctvState,
                                               const TrainConflict& trainConflict,
                                               TA_IRS_Bus::CctvSingleCommandPtr command,
                                               TA_IRS_Bus::CctvCommandReceivedEventPtr commandReceived )
    {
        FUNCTION_ENTRY( "handleCommandResult( CctvSingleCommandPtr )" );

        // get the result of the command
        // if it is a cctv failed or CctvInUse, throw a CctvException
        if ( TA_IRS_Bus::ProtocolCctvTypes::CctvProceeding != commandReceived->getResult() )
        {
            TA_THROW( TA_IRS_Bus::CctvException( "Failed to set CCTV Single Screen", commandReceived->getResult() ) );
        }

        {
            // acquire the train cctv state lock
            TA_IRS_Bus::TrainStateLock stateLock( cctvState );

            // set the train to single camera
            cctvState->setSingleCamera( command->getCamera(),
                                        commandReceived->getTime() );

            // perform post activation steps
            postActivateTrain( cctvState, trainConflict );

            // release the train cctv state lock
        }

        // audit the operator action
        m_auditSender.auditCctvSingleScreen( sessionId,
                                             trainId,
                                             commandReceived->getTime(),
                                             command->getCamera() );

        FUNCTION_EXIT;
    }


    void TimsCctvManager::handleCommandResult( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                               const std::string& sessionId,
                                               TrainCctvStatePtr cctvState,
                                               const TrainConflict& trainConflict,
                                               TA_IRS_Bus::CctvQuadCommandPtr command,
                                               TA_IRS_Bus::CctvCommandReceivedEventPtr commandReceived )
    {
        FUNCTION_ENTRY( "handleCommandResult( CctvQuadCommandPtr )" );

        // get the result of the command
        // if it is a cctv failed or CctvInUse, throw a CctvException
        if ( TA_IRS_Bus::ProtocolCctvTypes::CctvProceeding != commandReceived->getResult() )
        {
            TA_THROW( TA_IRS_Bus::CctvException( "Failed to set CCTV Quad Screen", commandReceived->getResult() ) );
        }

        {
            // acquire the train cctv state lock
            TA_IRS_Bus::TrainStateLock stateLock( cctvState );

            // set the train to quad
            cctvState->setQuad( command->getCameraW(),
                                command->getCameraX(),
                                command->getCameraY(),
                                command->getCameraZ(),
                                commandReceived->getTime() );

            // perform post activation steps
            postActivateTrain( cctvState, trainConflict );

            // release the train cctv state lock
        }

        // audit the operator action
        m_auditSender.auditCctvQuad( sessionId,
                                     trainId,
                                     commandReceived->getTime(),
                                     command->getCameraW(),
                                     command->getCameraX(),
                                     command->getCameraY(),
                                     command->getCameraZ() );

        FUNCTION_EXIT;
    }


    void TimsCctvManager::handleCommandResult( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                               const std::string& sessionId,
                                               TrainCctvStatePtr cctvState,
                                               const TrainConflict& trainConflict,
                                               TA_IRS_Bus::CctvSequenceCommandPtr command,
                                               TA_IRS_Bus::CctvCommandReceivedEventPtr commandReceived )
    {
        FUNCTION_ENTRY( "handleCommandResult( CctvSequenceCommandPtr )" );

        // get the result of the command
        // if it is a cctv failed or CctvInUse, throw a CctvException
        if ( TA_IRS_Bus::ProtocolCctvTypes::CctvProceeding != commandReceived->getResult() )
        {
            TA_THROW( TA_IRS_Bus::CctvException( "Failed to set CCTV Sequence", commandReceived->getResult() ) );
        }

        {
            // acquire the train cctv state lock
            TA_IRS_Bus::TrainStateLock stateLock( cctvState );

            // set the train to sequence
            cctvState->setSequence( command->getSequenceNumber(),
                                    commandReceived->getTime() );

            // perform post activation steps
            postActivateTrain( cctvState, trainConflict );

            // release the train cctv state lock
        }

        // audit the operator action
        m_auditSender.auditCctvSequence( sessionId,
                                         trainId,
                                         commandReceived->getTime(),
                                         command->getSequenceNumber(),
                                         command->getSequenceMode() );

        FUNCTION_EXIT;
    }


    TimsCctvManager::TrainConflict TimsCctvManager::preActivateTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                                      const std::string& operatorSession,
                                                                      unsigned long delocalisedLocation )
    {
        FUNCTION_ENTRY( "preActivateTrain" );

        // if the delocalised location is not given, then check if this agent is in control of this train normally
        if ( 0 == delocalisedLocation )
        {
            // check the train is controlled by this agent
            if ( false == m_operationModeManager.isInControlOfTrain( trainId, true, true ) )
            {
                TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Train is not controlled from this location" ) );
            }
        }

        // if the delocalised location is given, check the agent is in control of the delocalised location
        else
        {
            // check the location is controlled by this agent
            if ( false == m_operationModeManager.isInControlOfLocation( delocalisedLocation ) )
            {
                TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Specified train location is not controlled from this location" ) );
            }
        }

        // check the operator has the required priveliges to activate cctv
        // if the operator doesnt have rights, throw an AccessDeniedException
        if ( false == m_rightsChecker.canControlCctv( operatorSession ) )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not control CCTV" ) );
        }

        // create a TrainConflict structure
        TrainConflict trainConflict;
        trainConflict.conflictType = CctvZone::NoConflict;
        trainConflict.owningTrain = 0;

        // check if the train is already active:

        // get the cctv state for the train
        TrainCctvStatePtr cctvState = m_statusManager.getTrainCctvState( trainId );

        bool trainActive = false;

        {
            // acquire the train cctv state lock
            TA_IRS_Bus::TrainStateLock stateLock( cctvState );

            // get whether the train is activated already
            // get whether the train is in pre conflict, and the pre conflict train
            // (store these into the TrainConflict structure: trainConflict.conflictType == NoConflict or PreConflict, trainConflict.owningTrain)

            trainActive = cctvState->isCctvActivated();

            // if the train is active
            if ( true == trainActive )
            {
                // check for pre conflict
                trainConflict.conflictType = cctvState->isPreConflict() ? CctvZone::PreConflict : CctvZone::NoConflict;

                // check for the pre conflict train
                trainConflict.owningTrain = cctvState->getPreConflictTrain();
            }

            // release the train cctv state lock
        }

        // if the train is not already activated
        if ( false == trainActive )
        {
            // perform the localisation check and possible override:

            // get the train comms state for the train
            TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

            {
                // acquire the state lock
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                // get the ats validity and the localisation validity
                // if either one is false
                if ( ( false == commsState->isAtsValid() ) ||
                     ( false == commsState->isLocalisationValid() ) )
                {
                    // if delocalisedLocation is not specified
                    if ( 0 == delocalisedLocation )
                    {
                        // throw a CctvException (the train is not localised)
                        TA_THROW( TA_IRS_Bus::CctvException( "Train is not localised",
                                                             TA_IRS_Bus::ProtocolCctvTypes::CctvFailed ) );
                    }
                    else
                    {
                        // get the cctv zone for the delocalisedLocation
                        CctvZone trainZone = m_configuration.getCctvZoneFromLocation( delocalisedLocation );

                        // if the given location returns a 0 zone, then throw a InvalidParameterException (the given location is not valid)

                        if ( 0 == trainZone.getCurrentStage() )
                        {
                            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Given location is not valid for CCTV",
                                                                             "Location",
                                                                             TA_IRS_Bus::LocationCache::getInstance().getLocationByKey( delocalisedLocation ) ) );
                        }

                        // override the train's current location with the delocalisedLocation and the cctv zone
                        commsState->overrideCurrentLocation( delocalisedLocation, trainZone.getCurrentStage() );
                    }
                }

                // release the train state lock
            }

            // check the train has no conflicts:
            trainConflict = checkConflicts( commsState );

            // if the conflict type is "Conflict", throw a CctvConflictException

            if ( CctvZone::Conflict == trainConflict.conflictType )
            {
                TA_THROW( TA_IRS_Bus::CctvConflictException( "Cannot activate cctv due to conflict with another train",
                                                             trainConflict.owningTrain ) );
            }
        }

        FUNCTION_EXIT;
        return trainConflict;
    }


    void TimsCctvManager::postActivateTrain( TrainCctvStatePtr trainCctvState,
                                             const TrainConflict& trainConflict )
    {
        FUNCTION_ENTRY( "postActivateTrain" );

        // if the conflict type is pre conflict
        if ( CctvZone::PreConflict == trainConflict.conflictType )
        {
            {
                // acquire the state lock
                TA_IRS_Bus::TrainStateLock stateLock( trainCctvState );

                // set the pre conflict state in the train state
                trainCctvState->setPreConflict( trainConflict.owningTrain );

                // release the state lock
            }

            // raise a pre conflict alarm for the train
            raisePreConflictAlarm( trainCctvState->getTrainId(), trainConflict.owningTrain );
        }
        else
        {
            {
                // acquire the state lock
                TA_IRS_Bus::TrainStateLock stateLock( trainCctvState );

                // cancel pre conflict for the train
                trainCctvState->clearPreConflict();

                // release the state lock
            }

            // close the pre conflict alarm for the train
            closePreConflictAlarm( trainCctvState->getTrainId() );
        }

        FUNCTION_EXIT;
    }


    void TimsCctvManager::onTrainDeactivation( TrainCctvStatePtr trainCctvState,
                                               time_t timeStamp )
    {
        FUNCTION_ENTRY( "onTrainDeactivation" );

        // if the timestamp is 0, get the current time.

        if ( 0 == timeStamp )
        {
            timeStamp = time( NULL );
        }

        {
            // acquire the state lock
            TA_IRS_Bus::TrainStateLock stateLock( trainCctvState );

            // set the train to deactivated
            trainCctvState->setActivated( false, timeStamp );

            // release the state lock
        }

        // close any pre conflict alarms for this train
        closePreConflictAlarm( trainCctvState->getTrainId() );

        FUNCTION_EXIT;
    }


    void TimsCctvManager::processTrainCommsStateChanged( TrainCommsStatePtr state )
    {
        FUNCTION_ENTRY( "processTrainCommsStateChanged" );

        // if a train became comms valid, and there is a scheduled task waiting, we must start it
        // if the communication is valid

        if ( state->isTrainCommunicationValid() )
        {
            // acquire the m_carrierOffTaskLock
            TA_THREADGUARD( m_carrierOffTaskLock );

            // find the entry for the given train
            CarrierOffTaskMap::iterator existingTask = m_carrierOffTasks.find( state->getTrainId() );

            // if it exists, and is not scheduled

            if ( ( m_carrierOffTasks.end() != existingTask ) &&
                 ( false == existingTask->second->isScheduled() ) )
            {
                if ( state->getCommunicatingLocation() == m_configuration.getLocationKey() )
                {
                    // set it to scheduled
                    existingTask->second->setScheduled();

                    // queue it in m_threadPool
                    m_threadPool.queueWorkItem( existingTask->second );
                }
                else
                {
                    // this train is communicating with another agent
                    m_carrierOffTasks.erase( existingTask );
                }
            }
        }

        FUNCTION_EXIT;
    }


    void TimsCctvManager::processTrainCctvStateChanged( TrainCctvStatePtr state )
    {
        FUNCTION_ENTRY( "processTrainCctvStateChanged" );

        //
        // Update the clients:
        //

        // get the switch state
        TA_IRS_Bus::CctvTypes::CctvSwitchState update;
        {
            TA_IRS_Bus::TrainStateLock stateLock( state );

            update = state->getCctvSwitchState();
        }

        // populate the stage
        {
            TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( state->getTrainId() );

            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            update.stageId = commsState->getCurrentCctvZone().getCurrentStage();
        }

        // send a client update
        m_messageSender.sendClientUpdate( update );

        //
        // Update other trains:
        //

        // When a train is activated or deactivated, other trains may be affected by this
        // all the active trains this agent is responsible for should have their cctv state re-evaluated

        reEvaluateConflicts();

        FUNCTION_EXIT;
    }


    void TimsCctvManager::reEvaluateConflicts()
    {
        FUNCTION_ENTRY( "reEvaluateConflicts" );

        // get all train comms states
        TA_IRS_Bus::TrainIdToStateMap commsStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::CoreResource );

        // get all train cctv states
        TA_IRS_Bus::TrainIdToStateMap cctvStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainCctvResource );

        // both maps can be iterated through at the same time, they both contain the same trains
        // and are both sorted. we can assert to prove this fact.
        TA_ASSERT( commsStates.size() == cctvStates.size(),
                   "Both state maps should contain the same trains" );

        TA_IRS_Bus::TrainIdToStateMap::iterator commsStateIter = commsStates.begin();
        TA_IRS_Bus::TrainIdToStateMap::iterator cctvStateIter = cctvStates.begin();

        // Note: only need to check 1 iterator bound due to the assert above

        for ( ; commsStates.end() != commsStateIter ; ++commsStateIter, ++cctvStateIter )
        {
            // downcast to train cctv state
            TrainCctvStatePtr cctvState = boost::dynamic_pointer_cast< TrainCctvState >( cctvStateIter->second );
            TA_ASSERT( NULL != cctvState.get(), "Cast failed" );

            TA_IRS_Bus::CommonTypes::TrainIdType preConflictTrain = 0;

            {
                // get the cctv state lock
                TA_IRS_Bus::TrainStateLock stateLock( cctvState );

                // if the train is not active, continue (skip this train)

                if ( false == cctvState->isCctvActivated() )
                {
                    continue;
                }

                // get the pre-conflict train ID
                preConflictTrain = cctvState->getPreConflictTrain();

                // release the cctv state lock
            }

            // downcast to train comms state
            TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( commsStateIter->second );
            TA_ASSERT( NULL != commsState.get(), "Cast failed" );

            CctvZone currentTrainZone;

            {
                // get the comms state lock
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                // if the train is not valid, continue  (skip this train)
                // ignore ats validity - as this may have been manually activated

                if ( false == commsState->isValid() )
                {
                    continue;
                }

                // get the train location
                // if not in control of train location, continue (skip this train)
                if ( false == m_operationModeManager.isInControlOfLocation( commsState->getLocation() ) )
                {
                    continue;
                }

                // get the cctv zone
                currentTrainZone = commsState->getCurrentCctvZone();

                // release the comms state lock
            }

            // at this point, this is an active train controlled by this agent, we must check its conflicts

            TrainConflict conflict = checkConflicts( commsState );

            TA_IRS_Bus::CommonTypes::TrainIdType trainId = commsStateIter->first;

            switch ( conflict.conflictType )
            {

                case CctvZone::PreConflict:

                    // if the pre-conflict train ID is not the owningTrain
                    if ( preConflictTrain != conflict.owningTrain )
                    {
                        {
                            // acquire the train cctv state lock
                            TA_IRS_Bus::TrainStateLock stateLock( cctvState );

                            // set the pre conflict train in the state to the owningTrain
                            cctvState->setPreConflict( conflict.owningTrain );

                            // release the train cctv state lock
                        }

                        // re-raise the pre conflict alarm with the new train ID
                        closePreConflictAlarm( trainId );
                        raisePreConflictAlarm( trainId, conflict.owningTrain );
                    }

                    // cancel any carrier off tasks for this train
                    cancelCarrierOffTask( trainId );

                    break;

                case CctvZone::NoConflict:
                    // if the pre-conflict train ID is not 0
                    if ( 0 != preConflictTrain )
                    {
                        {
                            // acquire the train cctv state lock
                            TA_IRS_Bus::TrainStateLock stateLock( cctvState );

                            // clear the pre conflict in this train state
                            cctvState->clearPreConflict();

                            // release the train cctv state lock
                        }

                        closePreConflictAlarm( trainId );
                    }

                    break;

                case CctvZone::Conflict:
                    // this train must be deactivated
                    scheduleCarrierOffTask( trainId, conflict.owningTrain, true );

                    // also, if this is conflict due to a conflict zone, and this is the occ agent
                    // then the switching must be disallowed

                    if ( ( CctvZone::Conflict == currentTrainZone.getZoneType() ) &&
                         ( TA_Base_Core::ILocation::OCC == m_configuration.getAgentLocationType() ) )
                    {
                        try
                        {
                            m_switchAgent.setStageSwitchMode( currentTrainZone.getCurrentStage(),
                                                              currentTrainZone.getNextStage(),
                                                              false );
                        }
                        catch ( TA_IRS_Bus::TrainException& e )
                        {
                            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainException", e.what() );
                        }
                    }

                    break;
            }

        }

        FUNCTION_EXIT;
    }


    TimsCctvManager::TrainConflict TimsCctvManager::checkConflicts( TrainCommsStatePtr commsState )
    {
        FUNCTION_ENTRY( "checkConflicts" );

        TrainConflict trainConflict;
        trainConflict.conflictType = CctvZone::NoConflict;
        trainConflict.owningTrain = 0;


        CctvZone currentTrainZone;
        unsigned long trainLocation = 0;

        {
            // get the comms state lock
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // get the CCTV Zone for the given train
            currentTrainZone = commsState->getCurrentCctvZone();

            // get the train's current location
            trainLocation = commsState->getLocation();

            // release the comms state lock
        }

        // if this agent is not in degraded mode
        if ( false == m_operationModeManager.isIscsDegradedMode() )
        {
            //
            // check for local conflicts within the current stage:
            //

            // get the active train for the current stage
            TA_IRS_Bus::CctvTypes::CctvSwitchState activeTrain = getActiveTrainForStage( currentTrainZone.getCurrentStage() );

            // if there is one, and the train ID is not the given train

            if ( ( activeTrain.trainId != 0 ) &&
                 ( activeTrain.trainId != commsState->getTrainId() ) )
            {
                // there is a conflict!
                trainConflict.conflictType = CctvZone::Conflict;
                trainConflict.owningTrain = activeTrain.trainId;

                FUNCTION_EXIT;
                return trainConflict;
            }

            // if the current zone type is a conflict or pre conflict zone
            if ( CctvZone::NoConflict != currentTrainZone.getZoneType() )
            {
                // check the next stage for train conflicts:
                activeTrain = getActiveTrainForStage( currentTrainZone.getNextStage() );

                // if there is one, and the train ID is not the given train

                if ( ( activeTrain.trainId != 0 ) &&
                     ( activeTrain.trainId != commsState->getTrainId() ) )
                {
                    // there is a conflict!
                    trainConflict.conflictType = currentTrainZone.getZoneType();
                    trainConflict.owningTrain = activeTrain.trainId;

                    FUNCTION_EXIT;
                    return trainConflict;
                }
            }
        }
        else
        {
            // check for local conflicts within the current station:

            // get the active train for the station
            TA_IRS_Bus::CctvTypes::CctvSwitchState activeTrain = getActiveTrainForStation( trainLocation );

            // if there is one, and the train ID is not the given train

            if ( ( activeTrain.trainId != 0 ) &&
                 ( activeTrain.trainId != commsState->getTrainId() ) )
            {
                // there is a conflict!
                trainConflict.conflictType = CctvZone::Conflict;
                trainConflict.owningTrain = activeTrain.trainId;

                FUNCTION_EXIT;
                return trainConflict;
            }
        }

        FUNCTION_EXIT;

        return trainConflict;
    }


    void TimsCctvManager::raisePreConflictAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                 TA_IRS_Bus::CommonTypes::TrainIdType conflictingTrainId )
    {
        FUNCTION_ENTRY( "raisePreConflictAlarm" );

        // get the train alarm state for this train
        TrainAlarmStatePtr alarmState = m_statusManager.getTrainAlarmState( trainId );

        // acquire the lock for the train state
        TA_IRS_Bus::TrainStateLock stateLock( alarmState );

        // if there is not a pre conflict alarm (the alarm ID is empty)

        if ( true == alarmState->getCctvPreConflictAlarmId().empty() )
        {
            // raise the alarm and
            // set the pre conflict alarm to the returned alarm ID in the alarm state
            alarmState->setCctvPreConflictAlarmId(
                m_alarms.raiseCctvPreConflictAlarm( trainId, conflictingTrainId, time( NULL ) ) );
        }

        // release the state lock

        FUNCTION_EXIT;
    }


    void TimsCctvManager::closePreConflictAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "closePreConflictAlarm" );

        // get the train alarm state for this train
        TrainAlarmStatePtr alarmState = m_statusManager.getTrainAlarmState( trainId );

        // acquire the lock for the train state
        TA_IRS_Bus::TrainStateLock stateLock( alarmState );

        // if there is a pre conflict alarm (not "")
        std::string alarmId = alarmState->getCctvPreConflictAlarmId();

        if ( false == alarmId.empty() )
        {
            // close the alarm using m_alarms
            m_alarms.closeAlarm( alarmId );

            // set the pre conflict alarm to "" in the alarm state
            alarmState->setCctvPreConflictAlarmId();
        }

        // release the state lock

        FUNCTION_EXIT;
    }

	TA_IRS_Bus::CctvTypes::CctvSwitchState	TimsCctvManager::getCctvState( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
	{
		return m_statusManager.getTrainCctvState( trainId )->getCctvSwitchState();
	}
}

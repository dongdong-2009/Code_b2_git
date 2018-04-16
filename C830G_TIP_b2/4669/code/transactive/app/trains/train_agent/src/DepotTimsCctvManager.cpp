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

#include "app/trains/train_agent/src/DepotTimsCctvManager.h"

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

#include "bus/trains/TrainCommonLibrary/src/CctvException.h"
#include "bus/trains/TrainTransactionModel/src/ITrainTransactionManager.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{
    DepotTimsCctvManager::DepotTimsCctvManager( ITrainAgentConfiguration& configuration,
                                                ITrainStatusManager& statusManager,
                                                TA_IRS_Bus::ITrainTransactionManager& transactionManager,
                                                IOperationModeManager& operationModeManager,
                                                IAlarmUtility& alarms,
                                                IRightsChecker& rightsChecker,
                                                IMessageSender& messageSender,
                                                IAuditSender& auditSender,
                                                AtsMessageProcessor& atsMessageProcessor,
                                                ICctvAgentInterface& switchAgent )
            : TimsCctvManager( configuration,
                               statusManager,
                               transactionManager,
                               operationModeManager,
                               alarms,
                               rightsChecker,
                               messageSender,
                               auditSender,
                               atsMessageProcessor,
                               switchAgent )
    {
        FUNCTION_ENTRY( "DepotTimsCctvManager" );
        FUNCTION_EXIT;
    }


    DepotTimsCctvManager::~DepotTimsCctvManager()
    {
        FUNCTION_ENTRY( "~DepotTimsCctvManager" );
        FUNCTION_EXIT;
    }


    void DepotTimsCctvManager::setCarrierOn( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                             const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setCarrierOn" );

        // This is not supported in the depot
        TA_THROW( TA_IRS_Bus::CctvException( "Operation not supported in the depot",
                                             TA_IRS_Bus::ProtocolCctvTypes::CctvFailed ) );

        FUNCTION_EXIT;
    }


    void DepotTimsCctvManager::setCarrierOnDelocalisedTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                             unsigned long trainLocation,
                                                             const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setCarrierOnDelocalisedTrain" );

        // This is not supported in the depot
        TA_THROW( TA_IRS_Bus::CctvException( "Operation not supported in the depot",
                                             TA_IRS_Bus::ProtocolCctvTypes::CctvFailed ) );

        FUNCTION_EXIT;
    }


    void DepotTimsCctvManager::setCarrierOff( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                              const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setCarrierOff" );

        // This is not supported in the depot
        TA_THROW( TA_IRS_Bus::CctvException( "Operation not supported in the depot",
                                             TA_IRS_Bus::ProtocolCctvTypes::CctvFailed ) );

        FUNCTION_EXIT;
    }


    void DepotTimsCctvManager::setSingleScreen( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera,
                                                const std::string& sessionId,
                                                unsigned long trainLocation )
    {
        FUNCTION_ENTRY( "setSingleScreen" );

        // This is not supported in the depot
        TA_THROW( TA_IRS_Bus::CctvException( "Operation not supported in the depot",
                                             TA_IRS_Bus::ProtocolCctvTypes::CctvFailed ) );

        FUNCTION_EXIT;
    }


    void DepotTimsCctvManager::setQuadScreen( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                              TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraW,
                                              TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraX,
                                              TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraY,
                                              TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraZ,
                                              const std::string& sessionId,
                                              unsigned long trainLocation )
    {
        FUNCTION_ENTRY( "setQuadScreen" );

        // This is not supported in the depot
        TA_THROW( TA_IRS_Bus::CctvException( "Operation not supported in the depot",
                                             TA_IRS_Bus::ProtocolCctvTypes::CctvFailed ) );

        FUNCTION_EXIT;
    }


    void DepotTimsCctvManager::setSequence( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                            TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceNumber sequenceNo,
                                            TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceMode sequenceMode,
                                            const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setSequence" );

        // This is not supported in the depot
        TA_THROW( TA_IRS_Bus::CctvException( "Operation not supported in the depot",
                                             TA_IRS_Bus::ProtocolCctvTypes::CctvFailed ) );

        FUNCTION_EXIT;
    }


    void DepotTimsCctvManager::addAlarmImage( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                              const TA_IRS_Bus::CctvTypes::TrainCameraSequence& cameraIds,
                                              const std::string& sessionId )
    {
        FUNCTION_ENTRY( "addAlarmImage" );

        // This is not supported in the depot
        TA_THROW( TA_IRS_Bus::CctvException( "Operation not supported in the depot",
                                             TA_IRS_Bus::ProtocolCctvTypes::CctvFailed ) );

        FUNCTION_EXIT;
    }


    void DepotTimsCctvManager::addAlarmImageDelocalisedTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                              const TA_IRS_Bus::CctvTypes::TrainCameraSequence& cameraIds,
                                                              unsigned long trainLocation,
                                                              const std::string& sessionId )
    {
        FUNCTION_ENTRY( "addAlarmImageDelocalisedTrain" );

        // This is not supported in the depot
        TA_THROW( TA_IRS_Bus::CctvException( "Operation not supported in the depot",
                                             TA_IRS_Bus::ProtocolCctvTypes::CctvFailed ) );

        FUNCTION_EXIT;
    }


    void DepotTimsCctvManager::removeAlarmImage( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                 TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraId,
                                                 const std::string& sessionId )
    {
        FUNCTION_ENTRY( "removeAlarmImage" );

        // This is not supported in the depot
        TA_THROW( TA_IRS_Bus::CctvException( "Operation not supported in the depot",
                                             TA_IRS_Bus::ProtocolCctvTypes::CctvFailed ) );

        FUNCTION_EXIT;
    }


    void DepotTimsCctvManager::locationControlAcquired( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "locationControlAcquired" );

        // This agent doesnt care about this, do nothing.
        // this simply overrides the base behaviour

        FUNCTION_EXIT;
    }


    void DepotTimsCctvManager::locationControlReleased( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "locationControlReleased" );

        // This agent doesnt care about this, do nothing.
        // this simply overrides the base behaviour

        FUNCTION_EXIT;
    }


    void DepotTimsCctvManager::processAtsUpdate( const AtsTrainInformation& oldInformation,
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
                // FUNCTION_EXIT;  // limin-- CL-20746
                // return; // limin-- CL-20746
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "LIMINTEST CL-20746: [train id=%d][isCctvActivated=%d]", trainId, cctvState->isCctvActivated() );
            // release the train cctv state lock
        }

        // isInControlArea = ( new ATS valid ) AND ( agent is in control of new location )
        bool isInControlArea = ( true == newInformation.trainValid ) &&
                               ( m_operationModeManager.isInControlOfLocation( newInformation.localisation.currentLocation ) );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "LIMINTEST CL-20746: [train id=%d][trainValid=%d][isInControlOfLocation=%d][isInControlArea=%d]",
            trainId, newInformation.trainValid, m_operationModeManager.isInControlOfLocation( newInformation.localisation.currentLocation ), isInControlArea );
        // first check if this agent is not in control of the train's current location

        if ( false == isInControlArea )
        {
            // cancel any queued carrier off tasks
            cancelCarrierOffTask( trainId );

            // nothing can be done to this train if it is not controlled by this agent, return
            FUNCTION_EXIT;
            return;
        }
        else
        {
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
        }

        FUNCTION_EXIT;
    }


    void DepotTimsCctvManager::notifyOperationStateChange( TA_Base_Core::EOperationMode operationState )
    {
        FUNCTION_ENTRY( "notifyOperationStateChange" );
        // If the operation state is control

        if ( TA_Base_Core::Control == operationState )
        {
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

                {
                    // get the cctv state lock
                    TA_IRS_Bus::TrainStateLock stateLock( cctvState );

                    // if the train is not active, continue (skip this train)

                    if ( false == cctvState->isCctvActivated() )
                    {
                        continue;
                    }

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
                    if ( false == commsState->isValid() )
                    {
                        continue;
                    }

                    // if not in control of train location, continue (skip this train)
                    if ( ( false == commsState->isAtsValid() ) ||
                         ( false == m_operationModeManager.isInControlOfLocation( commsState->getLocation() ) ) )
                    {
                        continue;
                    }

                    // release the comms state lock
                }

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "Train %d is active in the depot. Deactivating train CCTV.",
                             cctvStateIter->first );

                // at this point, this is an active train controlled by this agent, we must deactivate it
                scheduleCarrierOffTask( cctvStateIter->first, 0, true );
            }
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

}

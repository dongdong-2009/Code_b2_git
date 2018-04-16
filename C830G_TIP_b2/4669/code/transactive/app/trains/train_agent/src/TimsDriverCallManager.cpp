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

#ifdef WIN32
// do not use windows define min/max macro
#define NOMINMAX
#endif


#include "app/trains/train_agent/src/DriverCallTasks.h"
#include "app/trains/train_agent/src/IAuditSender.h"
#include "app/trains/train_agent/src/IMessageSender.h"
#include "app/trains/train_agent/src/IOperationModeManager.h"
#include "app/trains/train_agent/src/IRadioAgentInterface.h"
#include "app/trains/train_agent/src/IRadioGroupCoordinator.h"
#include "app/trains/train_agent/src/IRightsChecker.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/RadioMessageProcessor.h"
#include "app/trains/train_agent/src/TimsDriverCallManager.h"
#include "app/trains/train_agent/src/TrainVoiceCallRequest.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

//#include "bus/managed_agent/src/AgentManager.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainAccessDeniedException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTimeoutException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTransmissionFailureException.h"
#include "bus/trains/TrainCommonLibrary/src/TransactionCancelledException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainRadioException.h"
#include "bus/trains/TrainProtocolLibrary/src/CallTrainCommandReceivedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/EndOfVoiceCallCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/OccCallCommandReceivedCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/OccCallResetCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/RequestForCallTrainCommand.h"
#include "bus/trains/TrainTransactionModel/src/ITrainTransactionManager.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"

#include "core/synchronisation/src/SingleThreadBarrier.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include <ace/Synch.h>
#include <boost/lexical_cast.hpp>


namespace TA_IRS_App
{

    const std::string TimsDriverCallManager::RPARAM_WAIT_END_OF_OCC_CALL_EVENT_TIMEOUT = "WaitEndOfOccCallEventTimeout";


    TimsDriverCallManager::TimsDriverCallManager( ITrainAgentConfiguration& configuration,
                                                  ITrainStatusManager& statusManager,
                                                  TA_IRS_Bus::ITrainTransactionManager& transactionManager,
                                                  IOperationModeManager& operationModeManager,
                                                  IMessageSender& messageSender,
                                                  IRightsChecker& rightsChecker,
                                                  IAuditSender& auditSender,
                                                  IRadioAgentInterface& radioAgent,
                                                  RadioMessageProcessor& radioMessageProcessor,
                                                  IRadioGroupCoordinator& radioGroupCoordinator )
        : m_configuration( configuration ),
          m_statusManager( statusManager ),
          m_transactionManager( transactionManager ),
          m_operationModeManager( operationModeManager ),
          m_messageSender( messageSender ),
          m_rightsChecker( rightsChecker ),
          m_auditSender( auditSender ),
          m_radioAgent( radioAgent ),
          m_radioMessageProcessor( radioMessageProcessor ),
          m_threadPool( *TA_Base_Core::ThreadPoolSingletonManager::getInstance() ),
          m_radioGroupCoordinator( radioGroupCoordinator )
    {
        FUNCTION_ENTRY( "TimsDriverCallManager" );

        // using m_transactionManager register for the following train events
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::RequestForOccCallEvent::getStaticType(), this );
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::EndOfOccCallEvent::getStaticType(), this );
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::ChangeoverStatusEvent::getStaticType(), this );

        // using m_statusManager register for state changes for CoreResource
        m_statusManager.registerForStateChanges( TA_IRS_Bus::CommonTypes::CoreResource, *this );

        // using m_radioMessageProcessor, subscribe for call end events for TrainCallResource
        m_radioMessageProcessor.addCallEndObserver( this, TA_IRS_Bus::CommonTypes::TrainCallResource );

        // register for operation state callbacks
        //TA_Base_Bus::AgentManager::registerForStateChangeCallbacks( this );

        FUNCTION_EXIT;
    }


    TimsDriverCallManager::~TimsDriverCallManager()
    {
        FUNCTION_ENTRY( "~TimsDriverCallManager" );

        // deregister for everything registered for in the constructor

        // register for operation state callbacks
        //TA_Base_Bus::AgentManager::deregisterForStateChangeCallbacks( this );

        // using m_statusManager unsubscribe to changes in CoreResource (TrainCommsState)
        m_statusManager.deregisterForStateChanges( TA_IRS_Bus::CommonTypes::CoreResource, *this );

        m_radioMessageProcessor.removeCallEndObserver( this, TA_IRS_Bus::CommonTypes::TrainCallResource );

        m_statusManager.deregisterForStateChanges( TA_IRS_Bus::CommonTypes::CoreResource, *this );

        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::ChangeoverStatusEvent::getStaticType(), this );
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::EndOfOccCallEvent::getStaticType(), this );
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::RequestForOccCallEvent::getStaticType(), this );

        // remove the thread pool instance
        TA_Base_Core::ThreadPoolSingletonManager::removeInstance( &m_threadPool );

        FUNCTION_EXIT;
    }


    unsigned short TimsDriverCallManager::getFlexibleTimeOutValue()
    {
        FUNCTION_ENTRY( "getFlexibleTimeOutValue" );

        // from m_configuration, get the number of retries, timer vc1 and timer vc2
        unsigned long retries = m_configuration.getTimsRetryLimit();

        // get the biggest of timer vc1 and timer vc2
        unsigned long biggestTimer = std::max( m_configuration.getTimerVC1(), m_configuration.getTimerVC2() );

        // return ( ( (the number of retries + 1) * the biggest timer ) /1000 ) + 5 (for procesing overhead)
        FUNCTION_EXIT;
        return static_cast< unsigned short >( ( ( ( retries + 1 ) * biggestTimer ) / 1000 ) + 5 );
    }


    TA_IRS_Bus::CallTypes::OccCallRequestList TimsDriverCallManager::getAllOccCallRequests()
    {
        FUNCTION_ENTRY( "getAllOccCallRequests" );

        TA_IRS_Bus::CallTypes::OccCallRequestList result;

        // get all train call states
        TA_IRS_Bus::TrainIdToStateMap callStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainCallResource );

        // for each one
        for ( TA_IRS_Bus::TrainIdToStateMap::iterator iter = callStates.begin();
              callStates.end() != iter; ++iter )
        {
            // downcast to the correct state
            TrainCallStatePtr callState = boost::dynamic_pointer_cast< TrainCallState >( iter->second );
            TA_ASSERT( NULL != callState.get(), "Cannot cast to correct state" );

            TA_IRS_Bus::TrainStateLock lock( callState );

            // if it has an incoming call, add the call request to the call request list
            if ( callState->hasIncomingCall() )
            {
                TA_IRS_Bus::CallTypes::OccCallRequest request;
                request.trainId = callState->getTrainId();
                request.timestamp = callState->getIncomingCallTime();

                result.push_back( request );
            }
        }

        FUNCTION_EXIT;
        return result;
    }


    void TimsDriverCallManager::answerOccCallRequest( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                      const std::string& sessionId )
    {
        FUNCTION_ENTRY( "answerOccCallRequest" );

        // This is an operator acknowledgement of an OCC Call request from a train, these things must be done:
        // 1. Ensure the operator has the correct permissions, the correct control modes exist, and there is an outstanding call request
        // 2. The acknowledgement reply needs to be sent to the train.
        // 3. The audio call to the train needs to be established
        // 4. The details of the call must be stored
        // 5. The OCC call request needs to be cleared

        LOG_GENERIC( SourceInfo,
                     TA_Base_Core::DebugUtil::DebugInfo,
                     "An operator with session ID [%s] is attempting to answer the call for train [%d]",
                     sessionId.c_str(),
                     trainId );

        // check the operator has rights for this operation
        if ( false == m_rightsChecker.canCallTrain( sessionId ) )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "The operator has no right to answer train call request" ) );
        }

        // check if the train has a call request outstanding
        TrainCallStatePtr callState = m_statusManager.getTrainCallState( trainId );
        TA_ASSERT( NULL != callState.get(), "Call state of train should not be NULL" );

        {
            // acquire the train state lock
            TA_IRS_Bus::TrainStateLock lock( callState );

            // if the train does not have an outstanding call request, throw an InvalidTrainException ( No outstanding Call OCC for this train )
            if ( false == callState->hasIncomingCall() )
            {
                TA_THROW( TA_IRS_Bus::InvalidTrainException( "No outstanding Call OCC for this train", trainId ) );
            }

            // release the train state lock
        }

        // check the action can be performed using performPreCallChecks
        performPreCallChecks( trainId );

        // get the operator's origin
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        // create a OccCallCommandReceivedCommand
        TA_IRS_Bus::TrainCommandPtr receivedCommand( new TA_IRS_Bus::OccCallCommandReceivedCommand( trainId, origin ) );

        // send the command using the defaults for timeout and retries - because this command has no response
        m_transactionManager.sendCommand( receivedCommand );

        // get the train comms state
        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );
        TA_ASSERT( NULL != commsState.get(), "Comms state of train should not be NULL" );

        std::string primaryTsi;

        {
            // acquire the state lock
            TA_IRS_Bus::TrainStateLock lock( commsState );

            // get the active TSI
            primaryTsi = commsState->getActiveTsi();
            // release the state lock
        }

        // make call to the train's primary TSI using m_radioAgent.makeSingleDriverCall
        TA_IRS_Bus::StateUpdateTypes::RadioCallDetails callDetails = m_radioAgent.makeSingleDriverCall( primaryTsi,
                                                                                                        sessionId,
                                                                                                        TA_IRS_Bus::CallTypes::DefaultNormalPriority );

        time_t timestamp;
        time( &timestamp );

        {
            // acquire the train call state lock
            TA_IRS_Bus::TrainStateLock lock( callState );

            // clear the OCC call request
            callState->clearIncomingCall( timestamp );

            TA_IRS_Bus::CallTypes::OccCallReset callReset;
            callReset.trainId = trainId;

            // send an OCC call reset message using m_messageSender
            m_messageSender.sendClientUpdate( callReset );

            // set the call details in the state (sessionId, OccCallTrain, current time, consoleId, callId)
            callState->setCallDetails( callDetails.sessionId,
                                       TA_IRS_Bus::CallTypes::OccCallTrain,
                                       timestamp,
                                       callDetails.consoleId,
                                       callDetails.callId );
            // release the train call state lock
        }

        // Log the details of the call that was made
        LOG_GENERIC( SourceInfo,
                     TA_Base_Core::DebugUtil::DebugInfo,
                     "Made call [Train: %d] %s",
                     trainId,
                     callDetails.getLogString().c_str() );

        // audit the operator action using m_auditSender.auditTrainCallStarted
        m_auditSender.auditTrainCallStarted( sessionId, trainId, timestamp );

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::CallTypes::DriverCallResult TimsDriverCallManager::makeDriverCall( const TA_IRS_Bus::CommonTypes::TrainIdList& trainList,
                                                                                   const TA_IRS_Bus::CallTypes::ItsiList& individualSubscribers,
                                                                                   bool override,
                                                                                   TA_IRS_Bus::CallTypes::ECallType callType,
                                                                                   TA_IRS_Bus::CallTypes::ECallPriority callPriority,
                                                                                   const std::string& sessionId )
    {
        FUNCTION_ENTRY( "makeDriverCall" );

        // this is a group call if trainList.size() + individualSubscribers.size() is more than 1
        bool isGroupCall = ( 1 < ( trainList.size() + individualSubscribers.size() ) );

        // perform the first phase of call setup using setupDriverCall()
        TrainVoiceCallRequestList callRequests = setupDriverCall( trainList, override, isGroupCall, sessionId );

        // create a DriverCallResult
        // set the call and console ID to 0
        TA_IRS_Bus::CallTypes::DriverCallResult driverCallResult;
        driverCallResult.callId = 0;
        driverCallResult.consoleId = 0;

        // create a tsiList as a copy of individualSubscribers
        TA_IRS_Bus::CallTypes::ItsiList individualSubscribersCopy = individualSubscribers;

        // successfulTrains = 0
        unsigned long successfulTrains = 0;

        // for each item in TrainVoiceCallRequestList
        for ( TrainVoiceCallRequestList::iterator voiceCallRequestIter = callRequests.begin();
              voiceCallRequestIter != callRequests.end(); ++voiceCallRequestIter )
        {
            // getCommandResult, and add it to the resultList
            TA_IRS_Bus::CallTypes::VoiceCallResponse commandResult = (*voiceCallRequestIter)->getCommandResult();
            driverCallResult.resultList.push_back( commandResult );

            // if the command was successful (isSuccessful)
            if ( true == (*voiceCallRequestIter)->isSuccessful() )
            {
                // increment successfulTrains
                ++successfulTrains;

                // get the comms state from the train
                TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( (*voiceCallRequestIter)->getTrainId() );

                // acquire the state lock
                TA_IRS_Bus::TrainStateLock lock( commsState );

                // get the audio mode from the TrainVoiceCallRequest
                TA_IRS_Bus::CallTrainCommandReceivedEvent::EAudioMode audioMode = (*voiceCallRequestIter)->getAudioMode();

                // if it is normal, add the train's primary TSI to tsiList
                if ( TA_IRS_Bus::CallTrainCommandReceivedEvent::Normal == audioMode )
                {
                    individualSubscribersCopy.push_back( commsState->getActiveTsi() );
                }
                // if it is other, add the train's secondary TSI to tsiList
                else
                {
                    individualSubscribersCopy.push_back( commsState->getStandbyTsi() );
                }

                // release the state lock

            }
        }

        // check to see if any trains were successful
        // if there are no successful trains
        if ( 0 == successfulTrains )
        {
            // no call will be made, return the DriverCallResult
            FUNCTION_EXIT;
            return driverCallResult;
        }

        // arriving here means that there was at least one successful train

        // if this is a group call
        if ( isGroupCall )
        {
            // make a DGNA using m_radioAgent.createDynamicGroup
            std::string dgnaTsi = m_radioAgent.createDynamicGroup( individualSubscribersCopy, sessionId );

            // make the call to the DGNA TSI using m_radioAgent.makeGroupDriverCall
            TA_IRS_Bus::StateUpdateTypes::RadioCallDetails callDetails = m_radioAgent.makeGroupDriverCall( dgnaTsi,
                                                                                                           sessionId,
                                                                                                           callType,
                                                                                                           callPriority );
            // set the call details in the DriverCallResult
            driverCallResult.callId = callDetails.callId;
            driverCallResult.consoleId = callDetails.consoleId;
        }
        // otherwise, its a single call
        else
        {
            // make the call to tsiList[0] using m_radioAgent.makeSingleDriverCall
            TA_IRS_Bus::StateUpdateTypes::RadioCallDetails callDetails = m_radioAgent.makeSingleDriverCall( individualSubscribersCopy[0],
                                                                                                            sessionId,
                                                                                                            callPriority );
            // set the call details in the DriverCallResult
            driverCallResult.callId = callDetails.callId;
            driverCallResult.consoleId = callDetails.consoleId;
        }

        time_t timestamp;
        time( &timestamp );

        // for each successful item in TrainVoiceCallRequestList
        for ( TrainVoiceCallRequestList::iterator clearVoiceCallRequestIter = callRequests.begin();
              clearVoiceCallRequestIter != callRequests.end(); ++clearVoiceCallRequestIter )
        {

            if ( true == (*clearVoiceCallRequestIter)->isSuccessful() )
            {
                TA_IRS_Bus::CommonTypes::TrainIdType trainId = (*clearVoiceCallRequestIter)->getTrainId();

                // get the train call state
                TrainCallStatePtr callState = m_statusManager.getTrainCallState( trainId );

                // acquire the state lock
                TA_IRS_Bus::TrainStateLock lock( callState );

                // set the call details
                callState->setCallDetails( sessionId,
                                           TA_IRS_Bus::CallTypes::TrainCallOcc,
                                           timestamp,
                                           driverCallResult.consoleId,
                                           driverCallResult.callId );

                // if there is an incoming call request
                if ( callState->hasIncomingCall() )
                {
                    // clear the call request
                    callState->clearIncomingCall( timestamp );

                    // send the occ call reset client update using m_messageSender
                    TA_IRS_Bus::CallTypes::OccCallReset callReset;
                    callReset.trainId = trainId;

                    m_messageSender.sendClientUpdate( callReset );
                    // release the state lock
                }

                // audit the operator action using m_auditSender.auditTrainCallStarted
                m_auditSender.auditTrainCallStarted( sessionId, trainId, timestamp );
            }
        }

        FUNCTION_EXIT;
        return driverCallResult;
    }


    TA_IRS_Bus::CallTypes::DriverCallResult TimsDriverCallManager::makeDriverCall( const TA_IRS_Bus::CommonTypes::TrainIdList& trainList,
                                                                                   const std::string& groupTsi,
                                                                                   bool override,
                                                                                   TA_IRS_Bus::CallTypes::ECallType callType,
                                                                                   TA_IRS_Bus::CallTypes::ECallPriority callPriority,
                                                                                   const std::string& sessionId )
    {
        FUNCTION_ENTRY( "makeDriverCall" );

        // if this is a train group TSI, then limit the trains set up for the call to those in the coverage area
        // theres no point setting up a call on trains not in the coverage area

        TA_IRS_Bus::CommonTypes::TrainIdList filteredTrainList;

        try
        {
            TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup group = m_radioGroupCoordinator.getRadioGroup( groupTsi );

            // for each train, check its location
            for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter = trainList.begin();
                  trainList.end() != trainIter; ++trainIter )
            {
                unsigned long trainLocation = 0;

                {
                    // get the train comms state for this train
                    TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( *trainIter );

                    // acquire the train state lock
                    TA_IRS_Bus::TrainStateLock lock( commsState );

                    // use ATS info
                    if ( true == commsState->isAtsValid() )
                    {
                        trainLocation = commsState->getLocation();
                    }

                    // release the train state lock
                }

                // if the train location is not known,
                // or in the coverage, include it
                if ( ( trainLocation == 0 ) ||
                     ( group.locationCoverage.end() != group.locationCoverage.find( trainLocation ) ) )
                {
                    filteredTrainList.insert( *trainIter );
                }
            }


            // in case groupTsi is a GTSI managed by this agent, lock it using m_radioGroupCoordinator
            // (this should not cause an error if its not a GTSI) it will throw a GroupInUseException if it is a GTSI and is already locked
            m_radioGroupCoordinator.lockRadioGroup( groupTsi );
        }
        catch( TA_IRS_Bus::InvalidParameterException& )
        {
            // send to all the trains, this is not GOA
            filteredTrainList = trainList;
        }

        // create a DriverCallResult
        // set the call and console ID to 0
        TA_IRS_Bus::CallTypes::DriverCallResult driverCallResult;
        driverCallResult.callId = 0;
        driverCallResult.consoleId = 0;

        try
        {
            // perform the first phase of call setup using setupDriverCall (group call is true)
            TrainVoiceCallRequestList trainVoiceCallRequestList = setupDriverCall( filteredTrainList, override, true, sessionId );

            // successfulTrains = 0
            unsigned long successfulTrains = 0;

            // for each item in TrainVoiceCallRequestList
            for ( TrainVoiceCallRequestList::iterator collectCommandResultIter = trainVoiceCallRequestList.begin();
                  collectCommandResultIter != trainVoiceCallRequestList.end(); ++collectCommandResultIter )
            {
                // getCommandResult, and add it to the resultList
                // however if there was a no permission error, filter this out
                // an operator is likely to call a group containing many trains he doesnt have
                // permission to call, or groups containing trains not in service
                // he doesnt need a huge long error list
                if ( TA_IRS_Bus::CallTypes::InvalidTrainError != (*collectCommandResultIter)->getResultCode() )
                {
                    driverCallResult.resultList.push_back( (*collectCommandResultIter)->getCommandResult() );
                }

                // if the command was successful (isSuccessful), then increment successfulTrains
                if ( true == (*collectCommandResultIter)->isSuccessful() )
                {
                    ++successfulTrains;
                }
            }

            // if there are no successful trains
            if ( 0 == successfulTrains )
            {
                m_radioGroupCoordinator.unlockRadioGroup( groupTsi );

                // no call will be made, return the DriverCallResult
                FUNCTION_EXIT;
                return driverCallResult;
            }

            // arriving here means that there was at least one successful train

            // make the call using m_radioAgent.makeGroupDriverCall
            TA_IRS_Bus::StateUpdateTypes::RadioCallDetails callDetails = m_radioAgent.makeGroupDriverCall( groupTsi,
                                                                                                           sessionId,
                                                                                                           callType,
                                                                                                           callPriority );
            // set the call details in the DriverCallResult
            driverCallResult.callId = callDetails.callId;
            driverCallResult.consoleId = callDetails.consoleId;

            time_t timestamp;
            time( &timestamp );

            TA_IRS_Bus::CommonTypes::TrainIdType trainId;

            // for each successful item in TrainVoiceCallRequestList
            for ( TrainVoiceCallRequestList::iterator clearVoiceCallRequestIter = trainVoiceCallRequestList.begin();
                  clearVoiceCallRequestIter != trainVoiceCallRequestList.end(); ++clearVoiceCallRequestIter )
            {
                if ( true == (*clearVoiceCallRequestIter)->isSuccessful() )
                {
                    trainId = (*clearVoiceCallRequestIter)->getTrainId();

                    // get the train call state
                    TrainCallStatePtr callState = m_statusManager.getTrainCallState( trainId );

                    {
                        // acquire the state lock
                        TA_IRS_Bus::TrainStateLock lock( callState );

                        // set the call details
                        callState->setCallDetails( sessionId,
                                                   TA_IRS_Bus::CallTypes::TrainCallOcc,
                                                   timestamp,
                                                   callDetails.consoleId,
                                                   callDetails.callId );

                        // if there is an incoming call request
                        if ( callState->hasIncomingCall() )
                        {
                            // clear the call request
                            callState->clearIncomingCall( timestamp );

                            // send the occ call reset client update using m_messageSender
                            TA_IRS_Bus::CallTypes::OccCallReset callReset;
                            callReset.trainId = trainId;

                            m_messageSender.sendClientUpdate( callReset );
                        }

                        // release the state lock
                    }

                    // audit the operator action using m_auditSender.auditTrainCallStarted
                    m_auditSender.auditTrainCallStarted( sessionId, trainId, timestamp );
                }
            }
        }
        catch ( ... )
        {
            // using m_radioGroupCoordinator unlock the groupTsi
            // re-throw the exception
            m_radioGroupCoordinator.unlockRadioGroup( groupTsi );
            throw;
        }

        FUNCTION_EXIT;
        return driverCallResult;
    }


    void TimsDriverCallManager::processEvent( TA_IRS_Bus::TrainEventPtr event )
    {
        FUNCTION_ENTRY( "processEvent" );

        // check operation state using AgentManager, if not control, return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // Get the event type, and downcast to the correct event:
        // RequestForOccCallEventPtr
        // EndOfOccCallEventPtr
        // ChangeoverStatusEventPtr
        
        TA_IRS_Bus::TrainMessageType messageType = event->getEventType();

        if ( messageType == TA_IRS_Bus::RequestForOccCallEvent::getStaticType() )
        {
            TA_IRS_Bus::RequestForOccCallEventPtr requestForOccCallEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::RequestForOccCallEvent >( event );
            
            TA_ASSERT( NULL != requestForOccCallEvent.get(), "cast failed" );

            processRequestForOccCallEvent( requestForOccCallEvent );
        }
        else if ( messageType == TA_IRS_Bus::EndOfOccCallEvent::getStaticType() )
        {
            TA_IRS_Bus::EndOfOccCallEventPtr endOfOccCallEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::EndOfOccCallEvent >( event );
            
            TA_ASSERT( NULL != endOfOccCallEvent.get(), "cast failed" );

            processEndOfOccCallEvent( endOfOccCallEvent );
        }
        else if ( messageType == TA_IRS_Bus::ChangeoverStatusEvent::getStaticType() )
        {
            TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover =
                boost::dynamic_pointer_cast< TA_IRS_Bus::ChangeoverStatusEvent >( event );
            
            TA_ASSERT( NULL != mpuChangeover.get(), "cast failed" );

            processChangeoverStatusEvent( mpuChangeover );
        }
        else
        {
            TA_ASSERT( false, "The event type received was not subscribed for" );
        }

        FUNCTION_EXIT;
    }


    void TimsDriverCallManager::processTrainStateChanged( TA_IRS_Bus::TrainStatePtr state )
    {
        FUNCTION_ENTRY( "processTrainStateChanged" );

        // If a train has become invalid, or started communicating with another agent
        // any outstanding call requests must be cleared

        // check operation state using AgentManager, if not control, return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // down cast this to a TrainCommsStatePtr
        TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( state );
        TA_ASSERT( NULL != commsState.get(), "Cast failed" );

        bool communicationValid = false;
        unsigned long communicatingLocation = 0;
        bool overallValid = false;

        {
            // acquire the train state lock
            TA_IRS_Bus::TrainStateLock lock( commsState );

            // get the communication validity
            communicationValid = commsState->isTrainCommunicationValid();

            // get the communicating location
            communicatingLocation = commsState->getCommunicatingLocation();

            // get the overall validity
            overallValid = commsState->isValid();

            // release the train state lock
        }

        // if the train is not valid or it has started to communicate with another agent
        if ( ( false == overallValid ) ||
             ( ( true == communicationValid ) &&
               ( communicatingLocation != m_configuration.getLocationKey() ) ) )
        {
            // get the train call state
            TrainCallStatePtr trainCallState = m_statusManager.getTrainCallState( state->getTrainId() );

            // acquire the train state lock
            TA_IRS_Bus::TrainStateLock lock( trainCallState );

            // if the train has an incoming call
            if ( trainCallState->hasIncomingCall() )
            {
                // log that the call driver request is no loger valid due to the train validity change
                LOG_GENERIC( SourceInfo,
                             TA_Base_Core::DebugUtil::DebugInfo,
                             "The call driver request for train %d is no loger valid due to the train validity change",
                             state->getTrainId() );

                // clear the incoming call
                trainCallState->clearIncomingCall( time( NULL ) );

                // using m_messageSender, send the OCC Call reset message
                TA_IRS_Bus::CallTypes::OccCallReset callReset;
                callReset.trainId = state->getTrainId();

                m_messageSender.sendClientUpdate( callReset );
            }
            // release the train state lock
        }

        FUNCTION_EXIT;
    }


    void TimsDriverCallManager::processCallEndEvent( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetails& callEnd,
                                                     const TA_IRS_Bus::CommonTypes::TrainIdList& trainList )
    {
        FUNCTION_ENTRY( "processCallEndEvent" );

        // This means that a call to a train or trains has ended outside of our control.
        // For each train involved in the call, we must reset the state of the train (no need to end any calls as this is already done)
        // We must send an end of voice call command to every train that was involved in the call (this must be done to all trains in parallel)

        // check operation state using AgentManager, if not control, return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // Log that a Call Driver/OCC call has ended, log the call details, and the trains involed in the call
        LOG_GENERIC( SourceInfo,
                     TA_Base_Core::DebugUtil::DebugInfo,
                     "A Call Driver/OCC call has ended: %s for trains %s",
                     callEnd.getLogString().c_str(),
                     TA_IRS_Bus::CommonTypes::trainIdListToString( trainList ).c_str() );

        // In case this was using a GTSI, unlock the group 'callEnd.calledTsi' using m_radioGroupCoordinator
        m_radioGroupCoordinator.unlockRadioGroup( callEnd.calledTsi );

        // If there is only one train in the given list
        if ( 1 == trainList.size() )
        {
            // then there is no need to put this in a separate thread,
            // performTimsCallReset() to send the command to the train
            performTimsCallReset( *trainList.begin() );
        }
        // otherwise there is more than one train
        else
        {
            // each train must have its call reset in another thread

            // for each train in the list
            for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator iter = trainList.begin();
                  iter != trainList.end(); ++iter )
            {
                // create a new task to reset the call on the train using scheduleCallResetTask()
                scheduleCallResetTask( *iter );
            }
        }

        FUNCTION_EXIT;
    }


    void TimsDriverCallManager::notifyOperationStateChange( TA_Base_Core::EOperationMode operationState )
    {
        FUNCTION_ENTRY( "notifyOperationStateChange" );

        // if the mode is control
        if ( TA_Base_Core::Control == operationState )
        {
            // deregister for operation state callbacks
            //TA_Base_Bus::AgentManager::deregisterForStateChangeCallbacks( this );

            // get all train call states
            TA_IRS_Bus::TrainIdToStateMap stateMap = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainCallResource );

            for ( TA_IRS_Bus::TrainIdToStateMap::iterator stateIter = stateMap.begin();
                  stateMap.end() != stateIter; ++stateIter )
            {
                TrainCallStatePtr callState = boost::dynamic_pointer_cast< TrainCallState >( stateIter->second );
                TA_ASSERT( NULL != callState.get(), "Failed to cast to the correct type" );

                TA_IRS_Bus::TrainStateLock stateLock( callState );

                if ( true == callState->hasIncomingCall() )
                {
                    // send the update
                    TA_IRS_Bus::CallTypes::OccCallRequest callRequest;
                    callRequest.trainId = stateIter->first;
                    callRequest.timestamp = callState->getIncomingCallTime();

                    m_messageSender.sendClientUpdate( callRequest );
                }
                // if the train has no incoming call
                else
                {
                    // clear the call from any clients
                    TA_IRS_Bus::CallTypes::OccCallReset callReset;

                    callReset.trainId = stateIter->first;

                    m_messageSender.sendClientUpdate( callReset );
                }
            }
        }
        // otherwise if its monitor
        else if ( TA_Base_Core::Monitor == operationState )
        {
            // just unsubscribe for updates, this means another agent in control
            // has already broadcast states
            //TA_Base_Bus::AgentManager::deregisterForStateChangeCallbacks( this );
        }

        FUNCTION_EXIT;
    }


    void TimsDriverCallManager::processRequestForOccCallEvent( TA_IRS_Bus::RequestForOccCallEventPtr event )
    {
        FUNCTION_ENTRY( "processRequestForOccCallEvent" );

        // On an Request for OCC Call, 2 things must be done:
        // 1. The train state must be updated and an OCC call request message sent out to clients
        // 2. The acknowledgement reply needs to be sent to the train.
        //    This is not handled by this method, this must only be done when the operator acknowledges the request. This is done in answerOccCallRequest()

        // get the train ID from the event
        TA_IRS_Bus::CommonTypes::TrainIdType trainId = event->getTrainId();

        // get the train call state for the train
        TrainCallStatePtr callState = m_statusManager.getTrainCallState( trainId );

        {
            time_t timestamp;

            // acquire the train state lock
            TA_IRS_Bus::TrainStateLock lock( callState );

            // if the train does not have an incoming call
            if ( false == callState->hasIncomingCall() )
            {
                // time = train event time
                timestamp = event->getTime();

                // set the incoming call time from the train event
                callState->setIncomingCall( timestamp );
            }
            // if the train has an incoming call
            else
            {
                // time = incoming call time from the train state
                timestamp = callState->getIncomingCallTime();
            }

            // using m_messageSender, send the OCC Call request message with the correct time determined above
            TA_IRS_Bus::CallTypes::OccCallRequest callRequest;
            callRequest.trainId = trainId;
            callRequest.timestamp = timestamp;

            m_messageSender.sendClientUpdate( callRequest );

            // release the train state lock
        }

        LOG_GENERIC( SourceInfo,
                     TA_Base_Core::DebugUtil::DebugInfo,
                     "There is an incoming call from train [%d]",
                     trainId );

        FUNCTION_EXIT;
    }


    void TimsDriverCallManager::processEndOfOccCallEvent( TA_IRS_Bus::EndOfOccCallEventPtr event )
    {
        FUNCTION_ENTRY( "processEndOfOccCallEvent" );

        // This is train termination of an OCC Call or call driver, these things must be done:
        // 1. If this is the only train in the call, the audio must be ended
        // 2. The details of the call must be cleared from the train state
        // 3. Any outstanding OCC call request needs to be cleared, and the message sent to clients
        // 4. The reply needs to be sent to the train.

        signalEndOfOccCallEvent( event->getTrainId() ); //limin++, ECP-0654 CL-20794

        try
        {
            // get the train ID from the event
            TA_IRS_Bus::CommonTypes::TrainIdType trainId = event->getTrainId();

            // get the timestamp from the event
            time_t timestamp = event->getTime();

            LOG_GENERIC( SourceInfo,
                         TA_Base_Core::DebugUtil::DebugInfo,
                         "Train [%d] is requesting an end of OCC call",
                         trainId );

            std::string sessionId;

            {
                TrainCallStatePtr callState = m_statusManager.getTrainCallState( trainId );
                TA_IRS_Bus::TrainStateLock lock( callState );
                sessionId = callState->getSessionId();
            }

            // use endRadioCallForSingleTrain to clean up the call details and call requests (steps 1 to 3)
            endRadioCallForSingleTrain( trainId, timestamp );

            // now send the reply to the train
            TrainCallStatePtr trainCallState = m_statusManager.getTrainCallState( trainId );

            // get the operator's origin
            unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

            // create a OccCallResetCommand
            TA_IRS_Bus::TrainCommandPtr command( new TA_IRS_Bus::OccCallResetCommand( trainId, origin ) );

            // send the command using the defaults for timeout and retries - because this command has no response
            m_transactionManager.sendCommand( command );
        }
        // catch any exceptions and log them, do not let them escape as this is not a user initiated action
        catch ( TA_IRS_Bus::InvalidTrainException& ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", ex.what() );
        }
        catch ( TA_IRS_Bus::TrainTransmissionFailureException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );
        }
        catch ( const TA_IRS_Bus::TrainTimeoutException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TrainTimeoutException", e.what() );
        }
        catch ( TA_IRS_Bus::ProtocolException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::ProtocolException", e.what() );
        }
        catch ( TA_IRS_Bus::TrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainException", e.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "while sending Occ Call Reset command" );
        }

        FUNCTION_EXIT;
    }


    void TimsDriverCallManager::processChangeoverStatusEvent( TA_IRS_Bus::ChangeoverStatusEventPtr event )
    {
        FUNCTION_ENTRY( "processChangeoverStatusEvent" );

        // On an MPU changeover, 2 things must be done:
        // 1. if there is a call active, that is a single call (this is the only train invovled in the call) the call must be ended.
        //    No messages need be sent to the train
        // 2. If there are any outstanding OCC call requests these must be cancelled

        // get the train ID from the event
        TA_IRS_Bus::CommonTypes::TrainIdType trainId = event->getTrainId();

        // Log the MPU changeover for the given train
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "MPU changeover for train [%d]",
                     trainId );

        // call endRadioCallForSingleTrain to do steps 1 and 2 above
        endRadioCallForSingleTrain( trainId );

        FUNCTION_EXIT;
    }


    void TimsDriverCallManager::performPreCallChecks( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "performPreCallChecks" );

        // using m_operationModeManager check this agent is in control of this train
        // if it is not
        if ( false == m_operationModeManager.isInControlOfTrain( trainId, true, false ) )
        {
            // check if the train is still communicating with this agent:

            {
                // get the train comms state for this train
                TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

                // acquire the train state lock
                TA_IRS_Bus::TrainStateLock lock( commsState );

                // check if communication is valid, and if the communicating location is this agent location
                bool communicationValid = commsState->isTrainCommunicationValid();

                unsigned long location = commsState->getCommunicatingLocation();

                // if not, throw an InvalidTrainException ( Agent not in control of this train )
                if ( ( false == communicationValid ) || ( location != m_configuration.getLocationKey() ) )
                {
                    TA_THROW( TA_IRS_Bus::InvalidTrainException( "Agent not in control of this train", trainId ) );
                }

                // release the train state lock
            }

            LOG_GENERIC( SourceInfo,
                         TA_Base_Core::DebugUtil::DebugInfo,
                         "The operation will be allowed because the train [%d] is still communicating with the current agent",
                         trainId );
        }

        // if the above tests pass, the operator can call the train (or answer the call)

        FUNCTION_EXIT;
    }


    void TimsDriverCallManager::endRadioCallForSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                            time_t timestamp )
    {
        FUNCTION_ENTRY( "endRadioCallForSingleTrain" );

        // if the timestamp is 0, get the current system time
        if ( 0 == timestamp )
        {
            time( &timestamp );
        }

        TA_IRS_Bus::CallTypes::ETrainCallType callType;
        unsigned long callId;
        unsigned long consoleId;
        std::string sessionId;

        {
            // get the train call state for the train
            TrainCallStatePtr trainState = m_statusManager.getTrainCallState( trainId );

            // acquire the train state lock
            TA_IRS_Bus::TrainStateLock lock( trainState );

            // if the train has an incoming call
            if ( trainState->hasIncomingCall() )
            {
                // clear the incoming call
                trainState->clearIncomingCall( timestamp );

                // using m_messageSender, send the OCC Call reset message
                TA_IRS_Bus::CallTypes::OccCallReset request;
                request.trainId = trainId;

                m_messageSender.sendClientUpdate( request );
            }

            // get the call type
            callType = trainState->getCallType();

            // if the call type is not NoCall
            if ( callType != TA_IRS_Bus::CallTypes::NoCall )
            {
                // get the call ID and console ID from the state
                callId = trainState->getCallId();
                consoleId = trainState->getConsoleId();

                // also get the operator session id (for the audit message)
                sessionId = trainState->getSessionId();

                // clear the call
                trainState->clearCall( timestamp );
            }

            // release the train state lock
        }

        // if the call type is not NoCall
        if ( callType != TA_IRS_Bus::CallTypes::NoCall )
        {
            try
            {

                // using the call ID and console ID, getDetailsForRadioCall from m_statusManager
                TA_IRS_Bus::StateUpdateTypes::RadioCallDetails callDetails =
                    m_statusManager.getDetailsForRadioCall( consoleId, callId );

                // using the call details returned, and m_statusManager again, getTrainsInvolvedInRadioCall
                TA_IRS_Bus::CommonTypes::TrainIdList trainList = m_statusManager.getTrainsInvolvedInRadioCall( callDetails );

                // log the radio call details, and the trains involved in the call
                LOG_GENERIC( SourceInfo,
                     TA_Base_Core::DebugUtil::DebugInfo,
                     "Radio call: %s is for trains %s",
                     callDetails.getLogString().c_str(),
                     TA_IRS_Bus::CommonTypes::trainIdListToString( trainList ).c_str() );

                // this train was removed from the call above, so if it was the last one, end the call
                if ( true == trainList.empty() )
                {
                    // log the call is being ended because this is the only train in the call
                    LOG_GENERIC( SourceInfo,
                                 TA_Base_Core::DebugUtil::DebugInfo,
                                 "The call is being ended because this is the only train [%d] in the call",
                                 trainId );

                    // in case this was a GTSI call, unlock the group in m_radioGroupCoordinator (the group TSI is the calledTsi in the RadioCallDetails)
                    m_radioGroupCoordinator.unlockRadioGroup( callDetails.calledTsi );

                    // end the call using m_radioAgent
                    m_radioAgent.endCall( callDetails.callId, sessionId );
                }
            }
            catch( TA_IRS_Bus::TrainRadioException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainRadioException", e.what() );
            }

            // audit the action using m_auditSender.auditTrainCallEnded
            m_auditSender.auditTrainCallEnded( sessionId, trainId, timestamp );
        }

        FUNCTION_EXIT;
    }


    void TimsDriverCallManager::scheduleCallResetTask( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "scheduleCallResetTask" );

        // create a new CallResetTask, put it into an IWorkItemPtr and schedule it using m_threadPool
        m_threadPool.queueWorkItem( TA_Base_Core::IWorkItemPtr( new CallResetTask( *this, trainId ) ) );

        FUNCTION_EXIT;
    }


    void TimsDriverCallManager::performTimsCallReset( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "performTimsCallReset" );

        // This is triggered once the call for the train has been ended, so there is no need to end the call
        // This must do the following:
        // Clear any call details or incoming call details in the state
        // send the OCC call reset client update if appropriate
        // Send the end of voice call command to the train
        // audit the ending of the train call

        {   //limin++, ECP-0654 CL-20794
            if ( 0 != waitEndOfOccCallEvent( trainId ) )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ECP-0652: Received EndOfOccCallEvent in time, ignore CallEndEvent [train id=%d]", trainId );

                FUNCTION_EXIT;
                return;
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ECP-0652: Receive EndOfOccCallEvent timeout [train id=%d]", trainId );
            }
        }   //++limin, ECP-0654 CL-20794

        std::string sessionId;
        time_t timestamp;
        time( &timestamp );

        try
        {
            // get the train call state for the train
            TrainCallStatePtr trainCallState = m_statusManager.getTrainCallState( trainId );

            {
                // acquire the train state lock
                TA_IRS_Bus::TrainStateLock lock( trainCallState );

                // if the train has an incoming call
                if ( trainCallState->hasIncomingCall() )
                {
                    // clear the incoming call
                    trainCallState->clearCall( timestamp );

                    // using m_messageSender, send the OCC Call reset message
                    TA_IRS_Bus::CallTypes::OccCallReset callReset;
                    callReset.trainId = trainId;

                    m_messageSender.sendClientUpdate( callReset );
                }

                // get the session ID from the train state
                sessionId = trainCallState->getSessionId();

                // clear the call
                trainCallState->clearCall( timestamp );

                // release the train state lock
            }

            // get the operator's origin
            unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

            // create an EndOfVoiceCallCommand
            TA_IRS_Bus::EndOfVoiceCallCommandPtr endOfVoiceCallCommand(
                new TA_IRS_Bus::EndOfVoiceCallCommand( trainId, origin ) );

            {
                // acquire the command lock from the call state using a Threadguard
                TA_Base_Core::IThreadLockable& commandLock = trainCallState->getCommandLock();
                TA_THREADGUARD( commandLock );

                // send the command using the retries from m_configuration, and using the TimerVC2 from m_configuration
                m_transactionManager.sendCommand( endOfVoiceCallCommand,
                                                  m_configuration.getTimerVC2(),
                                                  m_configuration.getTimsRetryLimit() );
                // release the command lock
            }

            // get the CallTrainCommandReceived response, the bad command response, and the changeover status response from the command
            TA_IRS_Bus::CallTrainCommandReceivedEventPtr commandReceived = endOfVoiceCallCommand->getCommandReceivedResponse();
            TA_IRS_Bus::BadCommandEventPtr badCommand = endOfVoiceCallCommand->getBadCommandResponse();
            TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = endOfVoiceCallCommand->getChangeoverStatusEventResponse();

            // if it is a CallTrainCommandReceivedEvent event
            if ( commandReceived )
            {
                // get the result of the command
                TA_IRS_Bus::ProtocolCallTypes::ECallTrainCommandStatus status = commandReceived->getStatus();

                // if it is not a CallTrainProceeding or CallTrainReset, log the error
                if ( ( status != TA_IRS_Bus::ProtocolCallTypes::CallTrainProceeding ) &&
                     ( status != TA_IRS_Bus::ProtocolCallTypes::CallTrainReset ) )
                {
                    LOG_GENERIC( SourceInfo,
                                 TA_Base_Core::DebugUtil::DebugError,
                                 "Failed to send EndOfVoiceCallCommand, status is [%s]",
                                 TA_IRS_Bus::ProtocolCallTypes::callTrainCommandStatusToString( status ).c_str() );
                }
            }
            // if the bad command event is returned
            else if ( badCommand )
            {
                // get the error code from the event
                TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError errorCode = badCommand->getErrorCode();

                // log the error
                LOG_GENERIC( SourceInfo,
                             TA_Base_Core::DebugUtil::DebugError,
                             "End of voice call command for train %d returned an error: %s",
                             trainId,
                             TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode ).c_str() );
            }
            // if the mpu changeover event is returned
            else if ( mpuChangeover )
            {
                // log an error
                LOG_GENERIC( SourceInfo,
                             TA_Base_Core::DebugUtil::DebugError,
                             "End Of Voice Call command for train %d returned an MPU changeover",
                             trainId );
            }
            // if none of these are returned,
            else
            {
                TA_ASSERT( false, "The command should not exit normally without a valid resposne" );
            }
        }
        // catch and log any exceptions, do not let any excaptions escape
        catch ( TA_IRS_Bus::InvalidTrainException& ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", ex.what() );
        }
        catch ( TA_IRS_Bus::TrainTransmissionFailureException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );
        }
        catch ( const TA_IRS_Bus::TrainTimeoutException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TrainTimeoutException", e.what() );
        }
        catch ( TA_IRS_Bus::ProtocolException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::ProtocolException", e.what() );
        }
        catch ( TA_IRS_Bus::TrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainException", e.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "while sending End Of Voice Call command" );
        }

        // audit the action using m_auditSender.auditTrainCallEnded
        m_auditSender.auditTrainCallEnded( sessionId, trainId, timestamp );

        FUNCTION_EXIT;
    }


    void TimsDriverCallManager::performTimsCallSetup( TrainVoiceCallRequestPtr voiceCallRequest )
    {
        FUNCTION_ENTRY( "performTimsCallSetup" );

        // This will set up the call to the train, it must send the command to the train and process the result.

        try
        {
            TA_IRS_Bus::CommonTypes::TrainIdType trainId = voiceCallRequest->getTrainId();

            // first check the agent can control the train using performPreCallChecks
            performPreCallChecks( trainId );

            // create a RequestForCallTrainCommand using the parameters from voiceCallRequest
            TA_IRS_Bus::RequestForCallTrainCommandPtr requestForCallTrainCommand(
                new TA_IRS_Bus::RequestForCallTrainCommand( trainId,
                                                            voiceCallRequest->getOrigin(),
                                                            voiceCallRequest->getRequestMode() ) );

            // get the Call state
            TrainCallStatePtr callState = m_statusManager.getTrainCallState( trainId );

            {
                // get the command lock and acquire it using a threadguard
                TA_Base_Core::IThreadLockable& lock = callState->getLock();
                TA_THREADGUARD( lock );

                // send the command using the retries from m_configuration, and using the TimerVC1 from m_configuration
                m_transactionManager.sendCommand( requestForCallTrainCommand,
                                                  m_configuration.getTimerVC1(),
                                                  m_configuration.getTimsRetryLimit() );
                // release the command lock
            }

            // get the CallTrainCommandReceivedEvent response, the bad command response, and the changeover status response from the command
            TA_IRS_Bus::CallTrainCommandReceivedEventPtr commandReceived = requestForCallTrainCommand->getCommandReceivedResponse();
            TA_IRS_Bus::BadCommandEventPtr badCommand = requestForCallTrainCommand->getBadCommandResponse();
            TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = requestForCallTrainCommand->getChangeoverStatusEventResponse();

            // if it is a CallTrainCommandReceivedEvent
            if ( commandReceived )
            {
                // get the result of the command
                TA_IRS_Bus::ProtocolCallTypes::ECallTrainCommandStatus status = commandReceived->getStatus();
                TA_IRS_Bus::CallTrainCommandReceivedEvent::EAudioMode audioMode = commandReceived->getAudioMode();

                // log the result
                LOG_GENERIC( SourceInfo,
                             TA_Base_Core::DebugUtil::DebugInfo,
                             "Success to send command RequestForCallTrainCommand returned: [%s]",
                             TA_IRS_Bus::ProtocolCallTypes::callTrainCommandStatusToString( status ).c_str() );

                // set the result in voiceCallRequest
                voiceCallRequest->setResult( status, audioMode );
            }
            // if the bad command event is returned
            else if ( badCommand )
            {
                // get the error code from the event
                TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError errorCode = badCommand->getErrorCode();

                std::stringstream errorDetails;
                errorDetails << "Request For Call Train command for train " << trainId << " returned an error: "
                             << TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode );

                // log the error
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorDetails.str().c_str() );

                // set the result in voiceCallRequest
                voiceCallRequest->setResult( errorCode, errorDetails.str() );
            }
            // if the mpu changeover event is returned
            else if ( mpuChangeover )
            {
                TA_IRS_Bus::ChangeoverStatusEvent::EMpuMode mpuMode = mpuChangeover->getMpuMode();

                std::stringstream errorDetails;
                errorDetails << "Request For Call Train command for train " << trainId << " returned an MPU changeover";

                // log an error
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorDetails.str().c_str() );

                // set the result in voiceCallRequest
                voiceCallRequest->setResult( mpuMode, errorDetails.str() );
            }
            else
            {
                // if none of these are returned, ASSERT - the command should not exit normally without a valid resposne
                TA_ASSERT( false, "The command should not exit normally without a valid resposne" );
            }
        }
        // based on the exception caught, set the appropriate result in voiceCallRequest
        catch ( TA_IRS_Bus::InvalidTrainException& ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", ex.what() );

            voiceCallRequest->setInvalidTrainResult( ex.what() );
        }
        catch ( const TA_IRS_Bus::InvalidParameterException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidParameterException", e.what() );

            voiceCallRequest->setInvalidTrainResult( e.what() );
        }
        catch ( TA_IRS_Bus::TrainTransmissionFailureException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );

            voiceCallRequest->setTransmissionFailureResult( e.what() );
        }
        catch ( TA_IRS_Bus::TransactionCancelledException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactionCancelledException", e.what() );

            voiceCallRequest->setTransactionCancelledResult( e.what() );
        }
        catch ( const TA_IRS_Bus::TrainTimeoutException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TrainTimeoutException", e.what() );

            voiceCallRequest->setTransactionTimeoutResult( e.what() );
        }
        catch ( TA_IRS_Bus::ProtocolException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::ProtocolException", e.what() );
        }
        catch ( TA_IRS_Bus::TrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainException", e.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "while sending Request For Call Train command" );
        }

        // log the result
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Voice Call setup result %s",
                     voiceCallRequest->getCommandResult().getLogString().c_str() );

        FUNCTION_EXIT;
    }


    TrainVoiceCallRequestList TimsDriverCallManager::setupDriverCall( const TA_IRS_Bus::CommonTypes::TrainIdList& trainList,
                                                                      bool override,
                                                                      bool groupCall,
                                                                      const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setupDriverCall" );

        // check that the size of trainList is > 0, if not throw an InvalidParameterException (no trains given to call)
        if ( 0 == trainList.size() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "no trains given to call",
                                                             "trainList",
                                                             "" ) );
        }

        // log that operator with session X is attempting to call a trainList.size() trains
        LOG_GENERIC( SourceInfo,
                     TA_Base_Core::DebugUtil::DebugInfo,
                     "Operator with session [%s] is attempting to call a [%d] trains",
                     sessionId.c_str(),
                     trainList.size() );

        // check the operator has the required permission to call trains using m_rightsChecker
        if ( false == m_rightsChecker.canCallTrain( sessionId ) )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not call train" ) );
        }

        // get the operator's origin
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        // get the request mode:
        TA_IRS_Bus::RequestForCallTrainCommand::ECallTrainRequestMode requestMode;

        // override is not set, groupCall is not set = CallTrainRequestIndividual
        if ( ( false == override ) && ( false == groupCall ) )
        {
            requestMode = TA_IRS_Bus::RequestForCallTrainCommand::CallTrainRequestIndividual;
        }
        // override is set, groupCall is not set = CallTrainOverrideIndividual
        else if ( ( true == override ) && ( false == groupCall ) )
        {
            requestMode = TA_IRS_Bus::RequestForCallTrainCommand::CallTrainOverrideIndividual;
        }
        // override is not set, groupCall is set = CallTrainRequestGroup
        else if ( ( false == override ) && ( true == groupCall ) )
        {
            requestMode = TA_IRS_Bus::RequestForCallTrainCommand::CallTrainRequestGroup;
        }
        // override is set, groupCall is set = CallTrainOverrideGroup
        else if ( ( true == override ) && ( true == groupCall ) )
        {
            requestMode = TA_IRS_Bus::RequestForCallTrainCommand::CallTrainOverrideGroup;
        }

        // create a TrainVoiceCallRequestList to hold all command statuses
        TrainVoiceCallRequestList trainVoiceCallRequestList;

        // create a barrier, initialised to the number of trains
        TA_Base_Core::SingleThreadBarrier barrier( trainList.size() );

        // for each train in trainList
        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator toMakeWorkItemTrainIter = trainList.begin();
              toMakeWorkItemTrainIter != trainList.end(); ++toMakeWorkItemTrainIter )
        {
            // create a TrainVoiceCallRequest object
            TrainVoiceCallRequestPtr newRequest( new TrainVoiceCallRequest( *toMakeWorkItemTrainIter, origin, requestMode ) );
            
            // push it back onto the TrainVoiceCallRequestList
            trainVoiceCallRequestList.push_back( newRequest );

            // create a CallSetupTask using the new TrainVoiceCallRequest object
            TA_Base_Core::IWorkItemPtr workItem( new CallSetupTask( *this, newRequest, barrier ) );

            // schedule the work item in m_threadPool
            m_threadPool.queueWorkItem( workItem );
        }

        // wait on the ACE_Barrier
        // when the wait unblocks, all commands to all trains have completed execution and had their status updated
        barrier.wait();

        // return the TrainVoiceCallRequestList (this contains the status of all commands just executed)
        FUNCTION_EXIT;
        return trainVoiceCallRequestList;
    }


    //limin++, ECP-0654 CL-20794
    int TimsDriverCallManager::waitEndOfOccCallEvent( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "waitEndOfOccCallEvent" );

        static const unsigned long s_waitEndOfOccCallEventTimeout = boost::lexical_cast< unsigned long >(
            TA_Base_Core::RunParams::getInstance().get( RPARAM_WAIT_END_OF_OCC_CALL_EVENT_TIMEOUT.c_str() ).empty() ? 
            std::string( "5000" ) :
            TA_Base_Core::RunParams::getInstance().get( RPARAM_WAIT_END_OF_OCC_CALL_EVENT_TIMEOUT.c_str() ) );

        boost::shared_ptr< TA_Base_Core::Condition > condition( new TA_Base_Core::Condition );

        {
            // acquire lock

            TA_THREADGUARD( m_EndOfOccCallEventConditionMapLock );
            
            ConditionMap::iterator findIter = m_EndOfOccCallEventConditionMap.find( trainId );
            
            // delete existing condition
            if ( findIter != m_EndOfOccCallEventConditionMap.end() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "ECP-0652: found previous condition [train id=%d]", trainId );
                
                findIter->second->signal();
                m_EndOfOccCallEventConditionMap.erase( findIter );
            }

            m_EndOfOccCallEventConditionMap.insert( ConditionMap::value_type( trainId, condition ) );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ECP-0652: wait condition EndOfOccCallEvent [train id=%d][timer=%d]", trainId, s_waitEndOfOccCallEventTimeout );

            // release lock
        }

        FUNCTION_EXIT;
        return condition->timedWait( s_waitEndOfOccCallEventTimeout );
    }
    
    
    void TimsDriverCallManager::signalEndOfOccCallEvent( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "signalEndOfOccCallEvent" );

        boost::shared_ptr< TA_Base_Core::Condition > condition;
        
        {
            // acquire lock

            TA_THREADGUARD( m_EndOfOccCallEventConditionMapLock );
            
            ConditionMap::iterator findIter = m_EndOfOccCallEventConditionMap.find( trainId );
            
            if ( findIter != m_EndOfOccCallEventConditionMap.end() )
            {
                condition = findIter->second;
                m_EndOfOccCallEventConditionMap.erase( findIter );// don't worry, we are using smart pointer!

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ECP-0652: found condition EndOfOccCallEvent [train id=%d]", trainId );
            }
            else
            {
                if ( false == m_EndOfOccCallEventConditionMap.empty() )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "ECP-0652: not found condition EndOfOccCallEvent [train id=%d]", trainId );
                    
                    for ( ConditionMap::iterator it = m_EndOfOccCallEventConditionMap.begin(); it != m_EndOfOccCallEventConditionMap.end(); ++it )
                    {
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "ECP-0652: current existing condition for [train id=%d]", it->first );
                    }
                }
            }

            // release lock
        }

        if ( condition )
        {
            condition->signal();

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDatabase, "ECP-0652: signal condition EndOfOccCallEvent [train id=%d]", trainId );
        }

        FUNCTION_EXIT;
    }
    //++limin, ECP-0654 CL-20794

} // TA_IRS_App

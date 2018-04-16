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

#include "app/trains/train_agent/src/AtsDataTypes.h"
#include "app/trains/train_agent/src/AtsMessageProcessor.h"
#include "app/trains/train_agent/src/IAuditSender.h"
#include "app/trains/train_agent/src/IMessageSender.h"
#include "app/trains/train_agent/src/IOperationModeManager.h"
#include "app/trains/train_agent/src/IRadioAgentInterface.h"
#include "app/trains/train_agent/src/IRightsChecker.h"
#include "app/trains/train_agent/src/ITimsCctvManager.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"
#include "app/trains/train_agent/src/RadioMessageProcessor.h"
#include "app/trains/train_agent/src/TimsPecManager.h"
#include "app/trains/train_agent/src/TrainVoiceCallRequest.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

//#include "bus/managed_agent/src/AgentManager.h"
#include "bus/trains/TrainCommonLibrary/src/BadCommandException.h"
#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
#include "bus/trains/TrainCommonLibrary/src/MpuChangeoverException.h"
#include "bus/trains/TrainCommonLibrary/src/PecException.h"
#include "bus/trains/TrainCommonLibrary/src/PecTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCctvTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPecTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TrainAccessDeniedException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainRadioException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTimeoutException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTimerException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTransmissionFailureException.h"
#include "bus/trains/TrainCommonLibrary/src/TransactionCancelledException.h"
#include "bus/trains/TrainProtocolLibrary/src/PecActivatedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecAnswerCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PecCommandReceivedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecContinueCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PecReadyForConversationEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecRequestForResetEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecResetCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PecResetEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecSelectedByDriverEvent.h"
#include "bus/trains/TrainTransactionModel/src/ITrainTransactionManager.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCctvTypes.h"
#include "bus/trains/TrainProtocolLibrary/src/PecHoldCommand.h"

#include "core/synchronisation/src/IThreadLockable.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/tvssagententitydata.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "bus/trains/TrainProtocolLibrary/src/PecResetCommandReceivedCommand.h""


#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>


//limin++, TD20554
namespace
{
    class TrainTimerPecGuard
    {
    public:
        
        TrainTimerPecGuard( TA_IRS_Bus::ITrainTransactionManager& transactionManager,
            TA_IRS_Bus::CommonTypes::TrainIdType trainId,
            time_t timeOut )
            : m_transactionManager( transactionManager ),
            m_trainId( trainId ),
            m_timeOut( timeOut )
        {
            m_transactionManager.startTrainTimer( m_trainId, TA_IRS_Bus::CommonTypes::TrainPecResource, m_timeOut );
        }
        
        ~TrainTimerPecGuard()
        {
            m_transactionManager.endTrainTimer( m_trainId, TA_IRS_Bus::CommonTypes::TrainPecResource );
        }
        
    private:
        
        TA_IRS_Bus::ITrainTransactionManager& m_transactionManager;
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;
        time_t m_timeOut;
    };
}
//++limin


namespace TA_IRS_App
{

    TimsPecManager::TimsPecManager( ITrainAgentConfiguration& configuration,
                                    ITrainStatusManager& statusManager,
                                    TA_IRS_Bus::ITrainTransactionManager& transactionManager,
                                    IOperationModeManager& operationModeManager,
                                    IMessageSender& messageSender,
                                    IRightsChecker& rightsChecker,
                                    IAuditSender& auditSender,
                                    //IRadioAgentInterface& radioAgent,
									RadioAccess& radioAccess,
                                    RadioMessageProcessor& radioMessageProcessor,
                                    AtsMessageProcessor& atsMessageProcessor,
                                    ITimsCctvManager& cctvManager )
        : m_configuration( configuration ),
          m_statusManager( statusManager ),
          m_transactionManager( transactionManager ),
          m_operationModeManager( operationModeManager ),
          m_messageSender( messageSender ),
          m_rightsChecker( rightsChecker ),
          m_auditSender( auditSender ),
          //m_radioAgent( radioAgent ),
		  m_radioAccess(radioAccess),
          m_atsMessageProcessor( atsMessageProcessor ),
          m_radioMessageProcessor( radioMessageProcessor ),
          m_threadPool( *TA_Base_Core::ThreadPoolSingletonManager::getInstance() ),
          m_cctvManager( cctvManager )
    {
        FUNCTION_ENTRY( "TimsPecManager" );

        // register for ats changes from m_atsMessageProcessor

        m_statusManager.registerForStateChanges( TA_IRS_Bus::CommonTypes::CoreResource, *this );

        // using m_transactionManager register for the following train events
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::PecActivatedEvent::getStaticType(), this );
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::PecReadyForConversationEvent::getStaticType(), this ); //Lucky : SDS#56 has been removed
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::PecRequestForResetEvent::getStaticType(), this );
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::PecResetEvent::getStaticType(), this );
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::PecSelectedByDriverEvent::getStaticType(), this );
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::ChangeoverStatusEvent::getStaticType(), this );

        // using m_radioMessageProcessor, subscribe for call end events for TrainPecResource
        m_radioMessageProcessor.addCallEndObserver( this, TA_IRS_Bus::CommonTypes::TrainPecResource );

        // register for operation state callbacks
        //TA_Base_Bus::AgentManager::registerForStateChangeCallbacks( this );

        initTvssManagerNamedObject();
        initCarPec2CameraMap();
        initConsole2MonitorMap();

		m_operationModeManager.registerDutyReleaseUpdate(this);

        FUNCTION_EXIT;
    }


    TimsPecManager::~TimsPecManager()
    {
        FUNCTION_ENTRY( "~TimsPecManager" );

        // deregister for everything registered for in the constructor

        // deregister for operation state callbacks
        //TA_Base_Bus::AgentManager::deregisterForStateChangeCallbacks( this );

        m_radioMessageProcessor.removeCallEndObserver( this, TA_IRS_Bus::CommonTypes::TrainPecResource );

        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::ChangeoverStatusEvent::getStaticType(), this );
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::PecSelectedByDriverEvent::getStaticType(), this );
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::PecResetEvent::getStaticType(), this );
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::PecRequestForResetEvent::getStaticType(), this );
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::PecReadyForConversationEvent::getStaticType(), this );
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::PecActivatedEvent::getStaticType(), this );

        m_statusManager.deregisterForStateChanges( TA_IRS_Bus::CommonTypes::CoreResource, *this );

        // remove the thread pool instance
        TA_Base_Core::ThreadPoolSingletonManager::removeInstance( &m_threadPool );

        FUNCTION_EXIT;
    }


    unsigned short TimsPecManager::getFlexibleTimeOutValue()
    {
        FUNCTION_ENTRY( "getFlexibleTimeOutValue" );

        // from m_configuration, get the number of retries, timer pec2 and pec4
        unsigned long retries = m_configuration.getTimsRetryLimit();

        // get the biggest of the timers
        unsigned long biggestTimer = std::max( m_configuration.getTimerPEC2(),
                                               m_configuration.getTimerPEC4() );

        // return ( ( (the number of retries + 1) * the biggest timer ) /1000 ) + 5 (for procesing overhead)
        FUNCTION_EXIT;
        return static_cast< unsigned short >( ( ( ( retries + 1 ) * biggestTimer ) / 1000 ) + 5 );
    }


    TA_IRS_Bus::PecTypes::PecUpdateList TimsPecManager::getAllActivePec()
    {
        FUNCTION_ENTRY( "getAllActivePec" );

        // This needs to check all trains for any active PEC, and return a complete list to the client

        // create a PecUpdateList
        TA_IRS_Bus::PecTypes::PecUpdateList pecUpdates;

        // get all train pec states
        TA_IRS_Bus::TrainIdToStateMap pecStates = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainPecResource );

        // for each state
        for ( TA_IRS_Bus::TrainIdToStateMap::iterator stateIter = pecStates.begin();
              stateIter != pecStates.end(); ++stateIter )
        {
            TA_IRS_Bus::PecTypes::ActivePecPointList activePecPoints;
            std::string sessionId;
            TA_IRS_Bus::CommonTypes::TrainIdType trainId;

            TrainPecStatePtr pecState = boost::dynamic_pointer_cast< TrainPecState >( stateIter->second );
            TA_ASSERT( pecState, "Cannot cast to correct state" );

            {
                // acquire the state lock
                TA_IRS_Bus::TrainStateLock lock( pecState );

                // get the active Pec Points from the state
                activePecPoints = pecState->getActivePecPoints();

                // get the sessionId
                sessionId = pecState->getSessionId();

                trainId = pecState->getTrainId();

                // release the state lock
            }

            // for each entry in the active pec point set
            for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator pecIter = activePecPoints.begin();
                  pecIter != activePecPoints.end(); ++pecIter )
            {
                // create a PecUpdate structure
                TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

                // set the pec to the entry in the set
                pecUpdate.pec = *pecIter;

                // set the train ID
                pecUpdate.trainId = trainId;

                // set the sessionId to the session id from the state
                pecUpdate.sessionId = sessionId;

                // set the updateType to PECUpdated
                pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECUpdated;

                // push the PecUpdate onto the PecUpdateList
                pecUpdates.push_back( pecUpdate );
            }
        }

        FUNCTION_EXIT;
        return pecUpdates;
    }


    void TimsPecManager::answerPec( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                    TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                    TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
									const std::string& sessionId, TA_IRS_Bus::PecTypes::EPecTrainSource pecTrainSource )
    {
        FUNCTION_ENTRY( "answerPec" );

        // This will attempt to answer an active, unanswered PEC

        // Check the operator can perform this action using performPreOperatorActionChecks
        performPreOperatorActionChecks( trainId, sessionId );

        // get the train PEC state
        TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

        {
            // acquire the state lock
            TA_IRS_Bus::TrainStateLock lock( pecState );

            // Check that the given PEC can be answered by calling canAnswerPec
            if ( false == pecState->canAnswerPec( car, pec ) )
            {
                TA_THROW( TA_IRS_Bus::PecException( "PEC is in use",
                                                    TA_IRS_Bus::ProtocolPecTypes::PecInUse ) );
            }

            // otherwise, at this point the answer process can proceed

            // call pecAnswered, with 0 for the call Id and console ID
            // This will signify that an answer is in progress
            pecState->pecAnswered( car, pec, time( NULL ), sessionId, 0, 0, pecTrainSource );

            // release the state lock
        }

        // get the operator's origin
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        // create a PecAnswerCommand
        TA_IRS_Bus::PecAnswerCommandPtr answerCommand( new TA_IRS_Bus::PecAnswerCommand( trainId,
                                                                                         origin,
                                                                                         car,
                                                                                         pec, pecTrainSource ) );
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"PecAnswerCommand : trainId=%d, origin=%d, car=%d, pec=%d, pecTrainSource=%d",
			static_cast<int>( trainId ), static_cast<int>( origin ),
			static_cast<TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber>( car ),
			static_cast<TA_IRS_Bus::ProtocolPecTypes::EPecNumber>( pec ),
			static_cast<TA_IRS_Bus::PecTypes::EPecTrainSource>( pecTrainSource ) );

        try
        {
            unsigned long retries = 0;

            bool answered = false;

            // can only exit when answered or via exception
            while ( false == answered )
            {
				bool makeCallToRadio = false;
                try
                {
                    // get the current time
                    time_t currentTime = time( NULL );

                    {
                        // get the command lock and acquire it using a threadguard
                        TA_Base_Core::IThreadLockable& lock = pecState->getCommandLock();

                        TA_THREADGUARD( lock );
                        
						m_transactionManager.sendCommand( answerCommand,
                                                          m_configuration.getTimerPEC2(),
                                                          0 );

						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"answerPec: getTimerPEC2 = %u", m_configuration.getTimerPEC2() );

                        // release the command lock
                    }

                    // get the CommandReceivedResponse, the bad command response, and the changeover status response from the command
                    TA_IRS_Bus::PecCommandReceivedEventPtr receivedCommand = answerCommand->getCommandReceivedResponse();
                    TA_IRS_Bus::BadCommandEventPtr badCommand = answerCommand->getBadCommandResponse();
                    TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = answerCommand->getChangeoverStatusEventResponse();

                    // if it is a PecCommandReceivedEventPtr
                    if ( receivedCommand )
                    {
                        // get the result of the command
                        TA_IRS_Bus::ProtocolPecTypes::EPecCommandResult result = receivedCommand->getResult();

                        // log the result
                        LOG_GENERIC( SourceInfo,
                                     TA_Base_Core::DebugUtil::DebugInfo,
									 "Pec answer command for train %d returned a result: %s",
                                     trainId,
                                     TA_IRS_Bus::ProtocolPecTypes::pecCommandResultToString( result ).c_str() );

                        // if the result is not PecProceeding
                        if ( result != TA_IRS_Bus::ProtocolPecTypes::PecProceeding )
                        {
                            // throw a PecException with the result
                            TA_THROW( TA_IRS_Bus::PecException( "Pec Answer Command Failed", result ) );
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
                                     "Pec Answer Command for train %d returned an error: %s",
                                     trainId,
                                     TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode ).c_str() );

                        TA_THROW( TA_IRS_Bus::BadCommandException( "Pec Answer Command Failed", errorCode ) );
                    }
                    // if the mpu changeover event is returned
                    else if ( mpuChangeover )
                    {
                        // log the result
                        LOG_GENERIC( SourceInfo,
                                     TA_Base_Core::DebugUtil::DebugError,
                                     "Pec Answer Command for train %d returned an MPU changeover",
                                     trainId );

                        // throw an MpuChangeoverException
                        TA_THROW( TA_IRS_Bus::MpuChangeoverException() );
                    }
                    // if none of these are returned, ASSERT - the command should not exit normally without a valid resposne
                    else
                    {
                        TA_ASSERT( false, "The command should not exit normally without a valid resposne" );
                    }

                    // get the time from the event, and the time taken before sending the event. calculate the elapsed time
                    time_t elapsedTime = currentTime - receivedCommand->getTime();

                    // subtract the elapsed time from the timerPEC3 and use this to start the timer
                    time_t timeOut = m_configuration.getTimerPEC3() - elapsedTime;

					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						"Timeout = %d, getTimerPEC3 = %d, elpaseTime = %d",timeOut,m_configuration.getTimerPEC3(),elapsedTime);

					//Lucky : SDS#56 has been removed, no need to wait for it
					
                    {//limin++, TD20554
                        //acquire train timer pec guard
                        TrainTimerPecGuard trainTimerGuard( m_transactionManager, trainId, timeOut );//limin++, TD20554

						LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
									"Test Log : Calling waitTrainTimer : Waiting for Ready Conversation Event");

                        // using m_transactionManager.waitTrainTimer, wait on the timer to be signalled or expire
                        m_transactionManager.waitTrainTimer( trainId, TA_IRS_Bus::CommonTypes::TrainPecResource );

                        // using m_transactionManager.endTrainTimer, end the timer as this is now done
                        //m_transactionManager.endTrainTimer( trainId, TA_IRS_Bus::CommonTypes::TrainPecResource );//limin--

                        //release train timer pec guard
                    }//limin++, TD20554
					

					unsigned long primaryTsi = -1;
					unsigned long secondaryTsi = -1;

					// wong.peter, if PEC Ready for conversation received before timer expires, continue to make actuall call
					{
						// wong.peter, not using TSI, but talkgroup Id
                        //std::string activeTsi;
						unsigned long talkgroupId = -1;

						//The active TSI to use to make a PEC Call
						std::string activeTsi;

                        // get the train comms state for the train
                        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

                        {
                            // acquire the comms state lock
                            TA_IRS_Bus::TrainStateLock lock( commsState );

                            // get the primary TSI
							// The active TSI to use to make a PEC Call
                            activeTsi = commsState->getActiveTsi();

							// get talkgroup Id instead
							//talkgroupId = commsState->getTalkgroupId();
							try
							{
								talkgroupId = boost::lexical_cast<unsigned long>(activeTsi);
								primaryTsi = talkgroupId;
							}
							catch(...)
							{
								//If it failed to convert set talkgroupId = 0
								LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Incorrect expected Primary return, Primary : %s",
									commsState->getSecondaryTsi().c_str());
								talkgroupId = 0;
								primaryTsi = 0;
							}

							//get the secondaryTsi
							try
							{
								secondaryTsi = boost::lexical_cast<unsigned long>(commsState->getSecondaryTsi());
							}
							catch(...)
							{
								LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Incorrect expected SecondaryTsi return, Secondary : %s",
									commsState->getSecondaryTsi().c_str());
								secondaryTsi = 0;
							}

                            // release the comms state lock
                        }

                        // make the PEC call using m_radioAgent

						LOG_GENERIC (SourceInfo, 
									 TA_Base_Core::DebugUtil::DebugInfo,
									 "Test Log : calling RadioFunction makePrivateCall, resource ID : %u",
									  talkgroupId);

						m_radioAccess.makePrivateCall( sessionId,talkgroupId);
						makeCallToRadio = true;
						
						LOG_GENERIC (SourceInfo, 
							TA_Base_Core::DebugUtil::DebugInfo,
							"Test Log : done calling RadioFunction makePrivateCall");

                        {
                            //limin++, TD20554
                            // acquire the PEC state lock
                            TA_IRS_Bus::TrainStateLock lock( pecState );
                            //++limin*/

                            // call pecAnswered, with the call Id and console ID
							 unsigned long iConsoleId = m_configuration.getConsoleFromSession( sessionId );
                            pecState->pecAnswered( car,
                                                   pec,
                                                   time( NULL ),
                                                   sessionId,
                                                   iConsoleId,
                                                   talkgroupId,
												   pecTrainSource ); //set console id as talkgroupid

                            // release the PEC state lock
                        }
					}

                    {
                        // acquire the PEC state lock
                        TA_Base_Core::IThreadLockable& lock = pecState->getLock();

                        TA_THREADGUARD( lock );

                        TA_IRS_Bus::PecTypes::PecPoint pecPoint;

                        // get the connected PEC point
                        // (this will throw an exception if this PEC has been reset during the waiting time for the ready for conversation)
                        pecPoint = pecState->getConnectedPecPoint();

                        // when here, the PEC has been successfully answered

                        // the status of each PEC must be resent out to the banner so all PEC update to show the train is busy

                        // get all active PEC points from the state
                        TA_IRS_Bus::PecTypes::ActivePecPointList activePecPoints = pecState->getActivePecPoints();

                        // for each PECPoint
                        for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator iter = activePecPoints.begin();
                              iter != activePecPoints.end(); ++iter )
                        {
                            // create a PecUpdate with the PEC Point
                            TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

                            // set the train and session ID
                            pecUpdate.sessionId = sessionId;
                            pecUpdate.trainId = trainId;

							pecUpdate.primaryTsi = primaryTsi;
							pecUpdate.secondaryTsi = secondaryTsi;

                            pecUpdate.pec = *iter;

                            // set the updateType to PECUpdated
                            pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECUpdated;

                            // send the message using m_messageSender
							LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo, "answered pec call : Sending PEC Updates to Client");
                            m_messageSender.sendClientUpdate( pecUpdate );
                        }

                        // release the state lock
                    }

                    // audit the event using m_auditSender.auditPecAnswer
                    m_auditSender.auditPecAnswer( sessionId, trainId, time( NULL ), car, pec );

                    answered = true;
                }
                catch ( TA_IRS_Bus::TrainTransmissionFailureException& e )
                {
                    // (feel free to define new methods if possible so this code is not repeated in each catch block, eg pass retry by reference etc)
                    // this means some point of the setup failed or was cancelled, there should be no audio call active
                    // simply check for a retry
                    checkRetries( retries, e );
                }
                catch ( TA_IRS_Bus::TrainTimeoutException& e )
                {
                    checkRetries( retries, e );
                }
                catch ( TA_IRS_Bus::TrainTimerException& e )
                {
                    // these errors occur after the radio call was made, the call must be ended - obsolete
					// wong.peter, radio call will only be made when timer is not expired.
                    //endRadioCall( trainId, car, pec, sessionId, pecTrainSource );

                    checkRetries( retries, e );
                }
                catch ( TA_IRS_Bus::InvalidParameterException& )
                {
                    // these errors occur after the radio call was made, the call must be ended
					if (makeCallToRadio)
						endRadioCall( trainId, car, pec, sessionId, pecTrainSource );

                    // this occurs if the PEC was reset during the answer process, there will be no retries
                    TA_THROW( TA_IRS_Bus::PecException( "Pec Reset during answer",
                                                        TA_IRS_Bus::ProtocolPecTypes::PecFailed ) );
                }
            }
        }
        catch ( ... )
        {
            // this means the operation has totally failed
            // the PEC state must be reset back to how it was, and all clients updated

            {
                // acquire the pec state lock
                TA_IRS_Bus::TrainStateLock lock( pecState );

                // reset the session ID
                pecState->resetSessionId();

                // clear the call
                pecState->clearCall();

                // call pecActivated with the PEC and CAR given in the parameters, this will reset the pec to unassigned
                pecState->pecActivated( car, pec, time( NULL ),pecTrainSource );

                // all clients can now be updated

                // get all active PEC points from the state
                TA_IRS_Bus::PecTypes::ActivePecPointList activePecPoints = pecState->getActivePecPoints();

                // for each PECPoint
                for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator iter = activePecPoints.begin();
                      iter != activePecPoints.end(); ++iter )
                {
                    // create a PecUpdate with the PEC Point
                    TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

                    // set the train and session ID
                    pecUpdate.trainId = trainId;
                    pecUpdate.sessionId = "";

                    pecUpdate.pec = *iter;

                    // set the updateType to PECUpdated
                    pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECUpdated;

                    // send the message using m_messageSender
                    m_messageSender.sendClientUpdate( pecUpdate );
                }

                // release the pec state lock
            }

            throw;
        }

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::CctvTypes::TrainCameraSequence TimsPecManager::activatePecCamera( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                                                  TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                                                                  TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                                                                                  const std::string& sessionId )
    {
        FUNCTION_ENTRY( "activatePecCamera" );

        {
            TA_IRS_Bus::CctvTypes::TrainCameraSequence cameraSequence;
            TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraId;

            CarPec2CameraMapType::iterator findIter = m_CarPpec2CameraMap.find( std::make_pair( car, pec ) );

            if ( findIter != m_CarPpec2CameraMap.end() )
            {
                cameraId = findIter->second;
                cameraSequence.push_back( findIter->second );
            }
            else
            {
                TA_THROW( TA_IRS_Bus::InvalidParameterException( "Cannot find camera by given car pec",
                    "car, pec",
                    TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( car ) +
                    ", " +
                    TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pec ) ) );
            }

            unsigned long long monitorId = getMonitorIdFromSession( sessionId );

            if ( 0 == monitorId )
            {
                //TA_THROW( TA_IRS_Bus::InvalidParameterException( "Cannot find monitor id", "monitor", "0" ) );
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"Cannot find monitor id, monitor 0" );
            }

            try
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "calling tvssPecSingleScreen with trainId[%d], monitorId[%d], cameraId[%d]", trainId, monitorId, cameraId );

				//bihui dtl-733
				CORBA_CALL( m_tvssManagerNamedObject, tvssClearImage, ( trainId, monitorId ) );
                //end dtl-733
				CORBA_CALL( m_tvssManagerNamedObject, tvssPecSingleScreen, ( trainId, monitorId, cameraId ) );
		
				m_messageSender.sendStateUpdate (trainId, cameraId, monitorId ) ;

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"Successfully called tvssPecSingleScreen..."
					"\nsendStateUpdate ..."
					"\trainId = %d , cameraId = %d, monitorId = %d",trainId,cameraId,monitorId);

            }
            catch ( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "" );
                //throw;
            }

            FUNCTION_EXIT;
            return cameraSequence;
        }

        // Given an active PEC, this will need to look up the associated camera and activate it as an alarm image
		/*

        TA_IRS_Bus::CctvTypes::TrainCameraSequence cameraSequence;

        // if the PEC given has either AllCar or AllPec, then throw an InvalidParameterException ( Cannot use AllCar or AllPec )
        if ( ( TA_IRS_Bus::ProtocolPecTypes::AllCars == car ) ||
             ( TA_IRS_Bus::ProtocolPecTypes::AllPec == pec ) )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Cannot use AllCar or AllPec",
                                                             "car, pec",
                                                             TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( car ) +
                                                             ", " +
                                                             TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pec ) ) );
        }

        // get the camera for the given PEC from m_configuration
        TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera = m_configuration.getPecCamera( car, pec );

        // if it is CameraNone, then throw an InvalidParameterException ( No camera configured for this PEC )
        if ( TA_IRS_Bus::ProtocolCctvTypes::CameraNone == camera )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No camera configured for this PEC",
                                                             "camera",
                                                             TA_IRS_Bus::ProtocolCctvTypes::cctvCameraToString( camera ) ) );
        }

        cameraSequence.push_back( camera );

        // using m_cctvManager, add the PEC camera as an alarm image
        m_cctvManager.addAlarmImage( trainId, cameraSequence, sessionId );

        // audit the event using m_auditSender.auditPecCctvActivated
        m_auditSender.auditPecCctvActivated( sessionId, trainId, time( NULL ), camera );

        // insert the camera into a sequence and return it
        FUNCTION_EXIT;
        return cameraSequence;*/

    }


    TA_IRS_Bus::CctvTypes::TrainCameraSequence TimsPecManager::activatePecCameraDelocalisedTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                                                                  TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                                                                                  TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                                                                                                  unsigned long trainLocation,
                                                                                                  const std::string& sessionId )
    {
        FUNCTION_ENTRY( "activatePecCameraDelocalisedTrain" );

        // Given an active PEC, this will need to look up the associated camera and activate it as an alarm image
        // This version is used for a delocalised train with a manually specified location

        TA_IRS_Bus::CctvTypes::TrainCameraSequence cameraSequence;

        // if the PEC given has either AllCar or AllPec, then throw an InvalidParameterException ( Cannot use AllCar or AllPec )
        if ( ( TA_IRS_Bus::ProtocolPecTypes::AllCars == car ) ||
             ( TA_IRS_Bus::ProtocolPecTypes::AllPec == pec ) )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Cannot use AllCar or AllPec",
                                                             "car, pec",
                                                             TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( car ) +
                                                             ", " +
                                                             TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pec ) ) );
        }

        // get the camera for the given PEC from m_configuration
        TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera = m_configuration.getPecCamera( car, pec );

        // if it is CameraNone, then throw an InvalidParameterException ( No camera configured for this PEC )
        if ( TA_IRS_Bus::ProtocolCctvTypes::CameraNone == camera )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No camera configured for this PEC",
                                                             "camera",
                                                             TA_IRS_Bus::ProtocolCctvTypes::cctvCameraToString( camera ) ) );
        }

        cameraSequence.push_back( camera );

        // using m_cctvManager, add the PEC camera as an alarm image for a delocalised train
        m_cctvManager.addAlarmImageDelocalisedTrain( trainId,
                                                     cameraSequence,
                                                     trainLocation,
                                                     sessionId );

        // audit the event using m_auditSender.auditPecCctvActivated
        m_auditSender.auditPecCctvActivated( sessionId, trainId, time( NULL ), camera );

        // insert the camera into a sequence and return it
        FUNCTION_EXIT;
        return cameraSequence;
    }


    void TimsPecManager::resetPec( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                   TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                   TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                                   const std::string& sessionId,
								   TA_IRS_Bus::PecTypes::EPecTrainSource pecTrainSource,
								   bool bIsTerminated)
    {
        FUNCTION_ENTRY( "resetPec" );

        // An operator is resetting a single PEC on a train
        // We must
        // 1. End the audio call if it is in progress for the PEC being reset
        // 2. Send the PEC reset message to TIMS
        // 3. Stop a PEC answer if it was in progress for the PEC being reset
        //
        // then if successful, for the reset PEC we:
        // 1. clear the PEC in the state
        // 2. send the client update
        //
        // However, if this train has left our area, and the PEC isnt valid
        // then the PEC must simply be cleared (no reset sent to the train)

        // check if this operator can perform PEC operations, using m_rightsChecker.canAnswerPec

        // Bohong++ for CL-20793 to do rights check later
        /*
        if ( false == m_rightsChecker.canAnswerPec( trainId, sessionId ) )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Cannot reset PEC for this train" ) );
        }
        */

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"ResetPEC : bIsTerminated : %d",bIsTerminated);

        // answeredByOperator = false
        bool answeredByOperator = false;

        unsigned long consoleId = 0;
        unsigned long callId = 0;

        // get the train PEC state
        TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

        TA_IRS_Bus::PecTypes::PecPoint pecToReset;

        pecToReset.car = car;
        pecToReset.pec = pec;
		pecToReset.pecTrainSource = pecTrainSource; //Lucky

        {
            // acquire the state lock
            TA_IRS_Bus::TrainStateLock lock( pecState );

            // get the call id, console id, and session id
            consoleId = pecState->getConsoleId();
            callId = pecState->getCallId();
            std::string stateSessionId = pecState->getSessionId();

            // get all active PEC from the state
            TA_IRS_Bus::PecTypes::ActivePecPointList activePecPoints = pecState->getActivePecPoints();

            // check if the PEC being reset is in the list of active PEC
            TA_IRS_Bus::PecTypes::ActivePecPointList::iterator findIter = activePecPoints.find( pecToReset );

            if ( findIter != activePecPoints.end() )
            {
                pecToReset = *findIter;

                // Bohong++, for CL-20793, we move the rights check module here 
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "pecToReset.valid = %d", pecToReset.valid );
                if ( true == pecToReset.valid )
                {
                    if ( false == m_rightsChecker.canAnswerPec( trainId, sessionId ) )
                    {
                       // TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Cannot reset PEC for this train" ) );
						//setupLiveAnnouncement
						 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
									  "DVA announcement for trains No Right for resetPec: %d",
									  trainId );

						 resetInvalidPec(trainId,sessionId,pecToReset);
                    }
                }
				else if (false == pecToReset.valid)
				{
					
					LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
								"Train : %d was invalid, assuming clear option selected",trainId);

					resetInvalidPec(trainId,sessionId,pecToReset);
					return;
				}
	
                // ++Bohong

                // check if it is answered (if the source is operator)
                if ( TA_IRS_Bus::PecTypes::PecSourceOperator == pecToReset.source )
                {
                    // check if the session id is this operators session ID
                    if ( stateSessionId != sessionId )
                    {
                        // throw an AccessDeniedException (Cant reset a PEC in use by another operator)
                        TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Cannot reset PEC in use by other operator" ) );
                    }

                    // then this call will be ended, clear the call
                    pecState->clearCall();

                    // answeredByOperator = true
                    answeredByOperator = true;
                }
            }
            else
            {
                // throw an InvalidParameterException (Given PEC is not active)
                TA_THROW( TA_IRS_Bus::InvalidParameterException( "Given PEC is not active",
                                                                 "pec",
                                                                 TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pec ) ) );
            }

            // release the state lock
        }

        // if the pec is answered by this operator
        if ( true == answeredByOperator )
        {
            // the call must be ended

            try
            {
				unsigned long talkgroupId = -1;

				//The active TSI to use to end a PEC Call
				std::string activeTsi;

				// get the train comms state for the train
				TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

				{
					// acquire the comms state lock
					TA_IRS_Bus::TrainStateLock lock( commsState );

					// get the primary TSI
					activeTsi = commsState->getActiveTsi();

					// get talkgroup Id instead
					//talkgroupId = commsState->getTalkgroupId();
					try
					{
						talkgroupId = boost::lexical_cast<unsigned long>(activeTsi);
					}
					catch(...)
					{
						//If it failed to convert set talkgroupId = 0
						talkgroupId = 0;
					}

					// release the comms state lock
				}
                // end the call using m_radioAgent
				if (!bIsTerminated)
				{
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
								"Performing endPECCall using MCC");
					m_radioAccess.endPecCall( sessionId, talkgroupId);//callId );
				}
				
            }
            // catch and log the TrainRadioException
            catch( TA_IRS_Bus::TrainRadioException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainRadioException", e.what() );
            }
        }

        bool timedOut = false;
        std::string errorReason;
        time_t timestamp = time( NULL );

        // if the PEC being reset is valid, and the train is valid (checkPecValidity)
        if ( ( true == pecToReset.valid ) &&
             ( true == checkPecValidity( trainId ) ) )
        {
            // then the reset must actually be sent to the train

            // get the operator's origin
            unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

            try
            {
                // create a PecResetCommand, with the given pec and car
                TA_IRS_Bus::PecResetCommandPtr command( new TA_IRS_Bus::PecResetCommand( trainId,
                                                                                         origin,
                                                                                         car,
                                                                                         pec,
																						 TA_IRS_Bus::ProtocolPecTypes::Request,
																						 pecTrainSource
																						 ) );
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"PecResetCommand : trainId=%d, origin=%d, car=%d, pec=%d, pecTrainSource=%d",
					static_cast<int>( trainId ), static_cast<int>( origin ),
					static_cast<TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber>( car ),
					static_cast<TA_IRS_Bus::ProtocolPecTypes::EPecNumber>( pec ),
					static_cast<TA_IRS_Bus::PecTypes::EPecTrainSource>( pecTrainSource ) );


                {
                    // acquire the command lock
                    TA_Base_Core::IThreadLockable& lock = pecState->getCommandLock();

                    TA_THREADGUARD( lock );

                    // send the command using TimerPEC4, and the retries from m_configuration
                    m_transactionManager.sendCommand( command,
                                                      m_configuration.getTimerPEC4(),
                                                      m_configuration.getTimsRetryLimit() );

					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						"resetPEC : getTimerPEC4 = %u, getTimsRetryLimit = %u", m_configuration.getTimerPEC4(), m_configuration.getTimsRetryLimit());

                    // release the command lock
                }

                // get the PecResetResponse, the bad command response, and the changeover status response from the command
                TA_IRS_Bus::PecResetCommandReceivedEventPtr resetCommand = command->getPecResetResponse();
                TA_IRS_Bus::BadCommandEventPtr badCommand = command->getBadCommandResponse();
                TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = command->getChangeoverStatusEventResponse();

                // if it is a PecResetEventPtr
                if ( resetCommand )
                {
                    // get the PEC and CAR
                    TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec = resetCommand->getPec();
                    TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car = resetCommand->getCar();

                    timestamp = resetCommand->getTime();

                    // log the result
                    // this indicates success
                    LOG_GENERIC( SourceInfo,
                                 TA_Base_Core::DebugUtil::DebugInfo,
                                 "Success to send Pec Reset command for CAR: %s, PEC: %s",
                                 TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( car ).c_str(),
                                 TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pec ).c_str() );
                }
                // if the bad command event is returned
                else if ( badCommand )
                {
                    TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError errorCode = badCommand->getErrorCode();

                    timestamp = badCommand->getTime();

                    // log the error
                    LOG_GENERIC( SourceInfo,
                                 TA_Base_Core::DebugUtil::DebugError,
                                 "Pec Reset command for train %d returned an error: %s",
                                 trainId,
                                 TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode ).c_str() );

                    TA_THROW( TA_IRS_Bus::BadCommandException( "Failed to send Pec Reset command", errorCode ) );
                }
                // if the mpu changeover event is returned
                else if ( mpuChangeover )
                {
                    timestamp = mpuChangeover->getTime();

                    // log the result
                    LOG_GENERIC( SourceInfo,
                                 TA_Base_Core::DebugUtil::DebugError,
                                 "Pec Reset command for train %d returned an MPU changeover",
                                 trainId );

                    // throw an MpuChangeoverException
                    TA_THROW( TA_IRS_Bus::MpuChangeoverException() );
                }
                // if none of these are returned, ASSERT - the command should not exit normally without a valid resposne
                else
                {
                    TA_ASSERT( false, "The command should not exit normally without a valid resposne" );
                }
            }
            catch ( TA_IRS_Bus::TrainTimeoutException& e )
            {
                timedOut = true;
                errorReason = e.what();
            }

            // if the source in the PEC is unassigned
            if ( TA_IRS_Bus::PecTypes::PecSourceUnassigned == pecToReset.source )
            {
                // audit the event using m_auditSender.auditPecResetWithoutAnswer
                m_auditSender.auditPecResetWithoutAnswer( sessionId,
                                                          trainId,
                                                          timestamp,
                                                          car,
                                                          pec );
            }
            else
            {
                // audit the event using m_auditSender.auditPecReset
                m_auditSender.auditPecReset( sessionId, trainId, timestamp, car, pec );
            }
        }

        {
            // acquire the train state lock
            TA_IRS_Bus::TrainStateLock lock( pecState );

            // call state->pecReset and hold onto the return value
            TA_IRS_Bus::PecTypes::ActivePecPointList activePecPoints = pecState->pecReset( car,
                                                                                           pec,
                                                                                           timestamp,
																						   pecTrainSource );

            // for the PECPoint returned from the reset
            for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator iter = activePecPoints.begin();
                  iter != activePecPoints.end(); ++iter )
            {
                // create a PecUpdate with the PEC Point
                TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

                // set the train and session ID
                pecUpdate.sessionId = sessionId;
                pecUpdate.trainId = trainId;

                pecUpdate.pec = *iter;

                // set the updateType to PECReset
                pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECReset;

                // send the message using m_messageSender
                m_messageSender.sendClientUpdate( pecUpdate );
            }

            // now, any still activated PEC need to be re-sent to the banner to update their status to available

            // get all active PEC points from the state
            activePecPoints.clear();
            activePecPoints = pecState->getActivePecPoints();

            // for each PECPoint
            for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator pecIterForReSent = activePecPoints.begin();
                  pecIterForReSent != activePecPoints.end(); ++pecIterForReSent )
            {
                // create a PecUpdate with the PEC Point
                TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

                // set the train and session ID
                pecUpdate.trainId = trainId;
                pecUpdate.sessionId = pecState->getSessionId();

                // set the updateType to PECUpdated
                pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECUpdated;

                pecUpdate.pec = *pecIterForReSent;

                // send the message using m_messageSender
                m_messageSender.sendClientUpdate( pecUpdate );
            }

            // release the train state lock
        }

        // if the source in the PEC was operator
        if ( TA_IRS_Bus::PecTypes::PecSourceOperator == pecToReset.source )
        {
            // ublock any possibly waiting PEC answer

            try
            {
                // using m_transactionManager.signalTrainTimer signal the timer for this train for TrainPecResource
                m_transactionManager.signalTrainTimer( trainId, TA_IRS_Bus::CommonTypes::TrainPecResource );
            }
            // catch and log the InvalidTrainException in case there was no timer started
            catch ( TA_IRS_Bus::InvalidTrainException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );
            }
        }

        if ( true == timedOut )
        {
            TA_THROW( TA_IRS_Bus::TrainTimeoutException( errorReason ) );
        }

        FUNCTION_EXIT;
    }


    void TimsPecManager::holdPec( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                   TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                   TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
								   const std::string& sessionId, TA_IRS_Bus::PecTypes::EPecTrainSource pecTrainSource  )
    {
        FUNCTION_ENTRY( "holdPec" );

        // get the train PEC state
        TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

        bool timedOut = false;
        std::string errorReason;
        time_t timestamp = time( NULL );

		//check whether answer by operator
        bool answeredByOperator = isAnswerByOperator ( trainId ,car,pec, sessionId) ;

        // if the PEC being hold is valid, and the train is valid (checkPecValidity)
        if ( ( true == checkPecValidity( trainId ) ) )
        {
            // then the hold must actually be sent to the train

            // get the operator's origin
            unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

			if ( true == answeredByOperator )
			{
				// the call must be ended

				try
				{
					unsigned long talkgroupId = -1;
					TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

					//The active TSI to use to end a PEC Call
					std::string activeTsi;
					{
						TA_IRS_Bus::TrainStateLock lock( commsState );
						//talkgroupId = commsState->getTalkgroupId();
						
						//The active TSI to use to end a PEC Call
						activeTsi = commsState->getActiveTsi();

						try
						{
							talkgroupId = boost::lexical_cast<unsigned long>(activeTsi);
						}
						catch(...)
						{
							//If it failed to convert set talkgroupId = 0
							talkgroupId = 0;
						}
					}
					// end the call using m_radioAgent
					m_radioAccess.endPecCall( sessionId, talkgroupId);//callId );
				}
				// catch and log the TrainRadioException
				catch( TA_IRS_Bus::TrainRadioException& e )
				{
					LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainRadioException", e.what() );
				}
			}

            try
            {
                // create a PecHoldCommand, with the given pec and car
                TA_IRS_Bus::PecHoldCommandPtr command( new TA_IRS_Bus::PecHoldCommand( trainId,
                                                                                         origin,
                                                                                         car,
                                                                                         pec,
																						 pecTrainSource
																						 ) );

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"PecHoldCommand : trainId=%d, origin=%d, car=%d, pec=%d, pecTrainSource=%d",
					static_cast<int>( trainId ), static_cast<int>( origin ),
					static_cast<TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber>( car ),
					static_cast<TA_IRS_Bus::ProtocolPecTypes::EPecNumber>( pec ),
					static_cast<TA_IRS_Bus::PecTypes::EPecTrainSource>( pecTrainSource ) );

                {
                    // acquire the command lock
                    TA_Base_Core::IThreadLockable& lock = pecState->getCommandLock();

                    TA_THREADGUARD( lock );

                    // send the command using TimerPEC4, and the retries from m_configuration
                    m_transactionManager.sendCommand( command,
                                                      m_configuration.getTimerPEC5(), //TODO: getTimerPEC5
                                                      m_configuration.getTimsRetryLimit() );

					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						"getTimerPEC5 = %u, getTimsRetryLimit = %d", m_configuration.getTimerPEC5(), m_configuration.getTimsRetryLimit());

                    // release the command lock
                }

                // get the PecResetResponse, the bad command response, and the changeover status response from the command
                TA_IRS_Bus::PecActivatedEventPtr activatedEvent = command->getPecActivatedResponse();
                TA_IRS_Bus::BadCommandEventPtr badCommand = command->getBadCommandResponse();
                TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = command->getChangeoverStatusEventResponse();

                // if it is a PecActivatedEvent
                if ( activatedEvent )
                {
                    // get the PEC and CAR
                    TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec = activatedEvent->getPec();
                    TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car = activatedEvent->getCar();

                    timestamp = activatedEvent->getTime();

                    // log the result
                    // this indicates success
                    LOG_GENERIC( SourceInfo,
                                 TA_Base_Core::DebugUtil::DebugInfo,
                                 "Success to send Pec Hold command for CAR: %s, PEC: %s",
                                 TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( car ).c_str(),
                                 TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pec ).c_str() );

					

					 {
						 // acquire the train state lock
						 TA_IRS_Bus::TrainStateLock lock( pecState );

						 // call state->pecReset and hold onto the return value
						 TA_IRS_Bus::PecTypes::ActivePecPointList activePecPoints = pecState->pecHold( car,
							 pec,
							 timestamp, pecTrainSource );

						 // for the PECPoint returned from the reset
						 for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator iter = activePecPoints.begin();
							 iter != activePecPoints.end(); ++iter )
						 {
							 // create a PecUpdate with the PEC Point
							 TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

							 // set the train and session ID
							 pecUpdate.sessionId = sessionId;
							 pecUpdate.trainId = trainId;

							 pecUpdate.pec = *iter;

							 // set the updateType to PECReset
							 pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECHold;

							 // send the message using m_messageSender
							 LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
										  "hold pec call, Sending PEC Updates to Client");
							 m_messageSender.sendClientUpdate( pecUpdate );
						 }

						 // now, any still activated PEC need to be re-sent to the banner to update their status to available

						 // get all active PEC points from the state
						 activePecPoints.clear();
						 activePecPoints = pecState->getActivePecPoints();

						 // for each PECPoint
						 for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator pecIterForReSent = activePecPoints.begin();
							 pecIterForReSent != activePecPoints.end(); ++pecIterForReSent )
						 {
							 // create a PecUpdate with the PEC Point
							 TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

							 // set the train and session ID
							 pecUpdate.trainId = trainId;
							 pecUpdate.sessionId = pecState->getSessionId();

							 // set the updateType to PECUpdated
							 pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECUpdated;

							 pecUpdate.pec = *pecIterForReSent;

							 // send the message using m_messageSender
							 LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
										  "after holding call, need to send update again, Sending PEC Updates to Client");
							 m_messageSender.sendClientUpdate( pecUpdate );
						 }

						 // release the train state lock
					 }

					 //then process the pecactivatedevent ;
					  this->processPecActivatedEvent( activatedEvent );


                }
                // if the bad command event is returned
                else if ( badCommand )
                {
                    TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError errorCode = badCommand->getErrorCode();

                    timestamp = badCommand->getTime();

                    // log the error
                    LOG_GENERIC( SourceInfo,
                                 TA_Base_Core::DebugUtil::DebugError,
                                 "Pec Hold command for train %d returned an error: %s",
                                 trainId,
                                 TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode ).c_str() );

                    TA_THROW( TA_IRS_Bus::BadCommandException( "Failed to send Pec Reset command", errorCode ) );
                }
                // if the mpu changeover event is returned
                else if ( mpuChangeover )
                {
                    timestamp = mpuChangeover->getTime();

                    // log the result
                    LOG_GENERIC( SourceInfo,
                                 TA_Base_Core::DebugUtil::DebugError,
                                 "Pec Hold command for train %d returned an MPU changeover",
                                 trainId );

                    // throw an MpuChangeoverException
                    TA_THROW( TA_IRS_Bus::MpuChangeoverException() );
                }
                // if none of these are returned, ASSERT - the command should not exit normally without a valid resposne
                else
                {
                    TA_ASSERT( false, "The command should not exit normally without a valid resposne" );
                }
            }
            catch ( TA_IRS_Bus::TrainTimeoutException& e )
            {
                timedOut = true;
                errorReason = e.what();
            }
        }

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"Send auditPecHold with sesssionID : %s, trainID : %d, carNumber : %d, pecNumber : %d",
			sessionId.c_str(), trainId, car,pec );
		//Send audit message
		m_auditSender.auditPecHold( sessionId, trainId, timestamp, car, pec );

        if ( true == timedOut )
        {
            TA_THROW( TA_IRS_Bus::TrainTimeoutException( errorReason ) );
        }

       

        FUNCTION_EXIT;
    }


    void TimsPecManager::resetAllPec( const TA_IRS_Bus::CommonTypes::TrainIdType trainId,
		const std::string& sessionId, TA_IRS_Bus::PecTypes::EPecTrainSource pecTrainSource,
		bool bIsTerminated)
    {
        FUNCTION_ENTRY( "resetAllPec" );

        // An operator is resetting all PEC on a train
        // We must
        // 1. End the audio call if it is in progress
        // 2. Send the PEC reset message to TIMS
        // 3. Stop a PEC answer if it was in progress
        //
        // then if successful, for each reset PEC we:
        // 1. clear the PEC in the state
        // 2. send the client update
        //
        // However, if this train has left our area, and there are no valid PEC on the train
        // then the PEC must simply be cleared

        // check if this operator can perform PEC operations, using m_rightsChecker.canAnswerPec
        if ( false == m_rightsChecker.canAnswerPec( trainId, sessionId ) )
        {
            // if not, throw an AccessDeniedException
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						"DVA announcement for trains No Rightb for resetAllPec: %d",
						trainId );			 
			TA_THROW( TA_IRS_Bus::TrainAccessDeniedException() );
			 
        }

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"ResetAllPEC : bIsTerminated : %d",bIsTerminated);

        TA_IRS_Bus::PecTypes::ActivePecPointList activePecPoints;
        unsigned long consoleId = 0;
        unsigned long callId = 0;

        // get the train PEC state
        TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

        {
            // acquire the state lock
            TA_IRS_Bus::TrainStateLock lock( pecState );

            // get the session id
            // if there is a session ID, and it is not this operators session ID
            if ( ( false == pecState->getSessionId().empty() ) &&
                 ( sessionId != pecState->getSessionId() ) )
            {
                // throw an AccessDeniedException (Cant reset a PEC in use by another operator)
                TA_THROW( TA_IRS_Bus::TrainAccessDeniedException() );
            }

            // get the call id, console id
            callId = pecState->getCallId();
            consoleId = pecState->getConsoleId();

            // clear the call
            pecState->clearCall();

            // get all active PEC from the state
            activePecPoints = pecState->getActivePecPoints();

            // release the state lock
        }
		
		bool bResetValidation = false;

		{
			// check if the call is already answered
			// this is to ensure that throwing of exception
			// during reset will only happen at the process of 
			// answering,
			// note user can reset still if the call entry is not 
			// on asnwering state.
			TA_IRS_Bus::TrainStateLock lock( pecState );
			
			try 
			{
				if (pecState->getConnectedPecPoint().source ==  TA_IRS_Bus::PecTypes::PecSourceOperator)
				{
					bResetValidation = true;
				}
			}
			catch (...)
			{
				bResetValidation = false;
			}

		}
		
		if ( callId == 0 && bResetValidation)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						"No Valid Call Yet for this Train: %d",
						trainId );	
			TA_THROW( TA_IRS_Bus::TrainAccessDeniedException() );
		}
        // if there was a call in progress
		else
        {
            try
			{
				unsigned long talkgroupId = -1;

				//The active TSI to use to end a PEC Call
				std::string activeTsi;

				// get the train comms state for the train
				TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );
				{
					// acquire the comms state lock
					TA_IRS_Bus::TrainStateLock lock( commsState );

					//The active TSI to use to end a PEC Call
					activeTsi = commsState->getActiveTsi();

					// get talkgroup Id instead
					//talkgroupId = commsState->getTalkgroupId();
					try
					{
						talkgroupId = boost::lexical_cast<unsigned long>(activeTsi);
					}
					catch(...)
					{
						//If it failed to convert set talkgroupId = 0
						talkgroupId = 0;
					}

					// release the comms state lock
				}
                 // end the call using m_radioAgent
				if (!bIsTerminated)
				{
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
								"Performing endPECCall using MCC");
					m_radioAccess.endPecCall( sessionId, talkgroupId );
				}
            }
            // catch and log any exceptions (it is possible this call may have aready been dropped)
            catch (...)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "...", "It is possible this call may have aready been dropped" );
            }
        }


        bool hasValidPec = true;
        
        // check if there is at least one valid pec
        for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator validityCheckIter = activePecPoints.begin();
              activePecPoints.end() != validityCheckIter; ++validityCheckIter )
        {
            hasValidPec = hasValidPec && validityCheckIter->valid;
        }

        bool timedOut = false;
        std::string errorReason;
        time_t timestamp = time( NULL );

        // check if this agent is in control of this train, or has the train communication
        // and check if any pec are valid
        if ( ( true == hasValidPec ) &&
             ( true == checkPecValidity( trainId ) ) )
        {
            // then the reset must actually be sent to the train

            // get the operator's origin
            unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

            try
            {
                // create a PecResetCommand, with AllPec and AllCar
                TA_IRS_Bus::PecResetCommandPtr command(
                    new TA_IRS_Bus::PecResetCommand( trainId,
                                                     origin,
                                                     TA_IRS_Bus::ProtocolPecTypes::AllCars,
                                                     TA_IRS_Bus::ProtocolPecTypes::AllPec,
													 TA_IRS_Bus::ProtocolPecTypes::Request,
													 pecTrainSource
													 ) );

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"PecResetCommand : trainId=%d, origin=%d, pecTrainSource=%d",
					static_cast<int>( trainId ), static_cast<int>( origin ),
					static_cast<TA_IRS_Bus::PecTypes::EPecTrainSource>( pecTrainSource ) );
		

                {
                    // acquire the command lock
                    TA_Base_Core::IThreadLockable& lock = pecState->getCommandLock();

                    TA_THREADGUARD( lock );

                    // send the command using TimerPEC4, and the retries from m_configuration
                    m_transactionManager.sendCommand( command,
                                                      m_configuration.getTimerPEC4(),
                                                      m_configuration.getTimsRetryLimit()  );

                    // release the command lock
                }

                // get the PecResetResponse, the bad command response, and the changeover status response from the command
                TA_IRS_Bus::PecResetCommandReceivedEventPtr resetCommand = command->getPecResetResponse();
                TA_IRS_Bus::BadCommandEventPtr badCommand = command->getBadCommandResponse();
                TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = command->getChangeoverStatusEventResponse();

                // if it is a PecResetEventPtr
                if ( resetCommand )
                {
                    // get the PEC and CAR
                    TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec = resetCommand->getPec();
                    TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car = resetCommand->getCar();

                    timestamp = resetCommand->getTime();

                    // log the result
                    // this indicates success
                    LOG_GENERIC( SourceInfo,
                        TA_Base_Core::DebugUtil::DebugInfo,
                        "Success to send Pec Reset command for CAR: %s, PEC: %s",
                        TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( car ).c_str(),
                        TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pec ).c_str() );
                }
                // if the bad command event is returned
                else if ( badCommand )
                {
                    TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError errorCode = badCommand->getErrorCode();

                    timestamp = badCommand->getTime();

                    // log the error
                    LOG_GENERIC( SourceInfo,
                        TA_Base_Core::DebugUtil::DebugError,
                        "Pec Reset command for train %d returned an error: %d",
                        trainId,
                        TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode ).c_str() );

                    TA_THROW( TA_IRS_Bus::BadCommandException( "Failed to send Pec Reset command", errorCode ) );
                }
                // if the mpu changeover event is returned
                else if ( mpuChangeover )
                {
                    timestamp = mpuChangeover->getTime();

                    // log the result
                    LOG_GENERIC( SourceInfo,
                        TA_Base_Core::DebugUtil::DebugError,
                        "Pec Reset command for train %d returned an MPU changeover",
                        trainId );

                    // throw an MpuChangeoverException
                    TA_THROW( TA_IRS_Bus::MpuChangeoverException() );
                }
                // if none of these are returned, ASSERT - the command should not exit normally without a valid resposne
                else
                {
                    TA_ASSERT( false, "The command should not exit normally without a valid resposne" );
                }
            }
            catch ( TA_IRS_Bus::TrainTimeoutException& e )
            {
                timedOut = true;
                errorReason = e.what();
            }

            // audit the event using m_auditSender.auditPecResetAll
            m_auditSender.auditPecResetAll( sessionId, trainId, timestamp );
        }

        // now the PEC in the state must be cleared

        bool connectedPointReset = false;

        {
            // acquire the train state lock
            TA_IRS_Bus::TrainStateLock lock( pecState );

            // call state->resetAllPec and hold onto the return value
            TA_IRS_Bus::PecTypes::ActivePecPointList activePecPoints = pecState->resetAllPec( time( NULL ) );

            // for each PECPoint returned from the reset
            for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator iter = activePecPoints.begin();
                  iter != activePecPoints.end(); ++iter )
            {
                // create a PecUpdate with the PEC Point
                TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

                // set the train and session ID
                pecUpdate.trainId = trainId;
                pecUpdate.sessionId = sessionId;

                pecUpdate.pec = *iter;

                // set the updateType to PECReset
                pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECReset;

                // send the message using m_messageSender
                m_messageSender.sendClientUpdate( pecUpdate );

                // if the source in this PEC is operator, then connectedPointReset = true
                if ( TA_IRS_Bus::PecTypes::PecSourceOperator == iter->source )
                {
                    connectedPointReset = true;
                }
            }

            // release the train state lock
        }

        // if the connected point was reset
        if ( true == connectedPointReset )
        {
            // ublock any possibly waiting PEC answer

            try
            {
                // using m_transactionManager.signalTrainTimer signal the timer for this train for TrainPecResource
                m_transactionManager.signalTrainTimer( trainId, TA_IRS_Bus::CommonTypes::TrainPecResource );
            }
            // catch and log the InvalidTrainException in case there was no timer started
            catch ( TA_IRS_Bus::InvalidTrainException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );
            }
        }

        if ( true == timedOut )
        {
            TA_THROW( TA_IRS_Bus::TrainTimeoutException( errorReason ) );
        }

        FUNCTION_EXIT;
    }


    /*
    void TimsPecManager::processCallEndEvent( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetails& callEnd,
                                              const TA_IRS_Bus::CommonTypes::TrainIdList& trainList )
    {
        FUNCTION_ENTRY( "processCallEndEvent" );

        // This means that a PEC that was in progress has lost its audio call.
        // No messages should be sent to the train, only the operator must be notified that audio has been lost.
        // The operator then decides what to do.

        // check this agent is in the control state using AgentManager
        if ( false == OperationModeManager::isRunningControl() )
        {
            // return if not in control
            FUNCTION_EXIT;
            return;
        }

        // ASSERT that the list only contains 1 train, there should never be more than 1 for a PEC call
        TA_ASSERT( 1 == trainList.size(), "The list only contains 1 train, there should never be more than 1 for a PEC call" );

        TA_IRS_Bus::CommonTypes::TrainIdType trainId = *trainList.begin();

        TA_IRS_Bus::PecTypes::PecPoint connectedPec;

        // get the train PEC state
        TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

        {
            // acquire the train state lock
            TA_IRS_Bus::TrainStateLock lock( pecState );

            // clear the PEC call
            pecState->clearCall();

            // get the connected PEC point
            connectedPec = pecState->getConnectedPecPoint();

            // get the session ID
            std::string sessionId = pecState->getSessionId();

            // create a PecUpdate with the connected PEC point
            TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

            // set the train and session ID
            pecUpdate.sessionId = sessionId;
            pecUpdate.trainId = trainId;

            // set the updateType to PECCallLost
            pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECCallLost;

            pecUpdate.pec = connectedPec;

            // send the message using m_messageSender
            m_messageSender.sendClientUpdate( pecUpdate );

            // release the train state lock
        }


        try
        {
            // audit the event using m_auditSender.auditPecCallLost
            m_auditSender.auditPecCallLost( trainId,
                                            time( NULL ),
                                            connectedPec.car,
                                            connectedPec.pec );
        }
        // catch any exceptions (InvalidParameterException ) and log them
        catch ( TA_IRS_Bus::InvalidParameterException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidParameterException", e.what() );
        }

        // possibly this PEC has been ended and this is just timing

        FUNCTION_EXIT;
    }
    */

    // Bohong++ for CL-20793 to replace the function commented above
    void TimsPecManager::processCallEndEvent( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetails& callEnd,
                                              const TA_IRS_Bus::CommonTypes::TrainIdList& trainList )
    {
        FUNCTION_ENTRY( "processCallEndEvent" );

        // This means that a PEC that was in progress has lost its audio call.
        // No messages should be sent to the train, only the operator must be notified that audio has been lost.
        // The operator then decides what to do.

        // check this agent is in the control state using AgentManager
        if ( false == OperationModeManager::isRunningControl() )
        {
            // return if not in control
            FUNCTION_EXIT;
            return;
        }

        unsigned long consoleId = 0;
        unsigned long callId = 0;
        std::string sessionId;

        // ASSERT that the list only contains 1 train, there should never be more than 1 for a PEC call
        TA_ASSERT( 1 == trainList.size(), "The list only contains 1 train, there should never be more than 1 for a PEC call" );

        TA_IRS_Bus::CommonTypes::TrainIdType trainId = *trainList.begin();

        // get the train PEC state
        TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

        {
            // acquire the train state lock
            TA_IRS_Bus::TrainStateLock lock( pecState );

            // get the call, console, and session ID
            consoleId = pecState->getConsoleId();
            callId = pecState->getCallId();
            sessionId = pecState->getSessionId();

            TA_IRS_Bus::PecTypes::ActivePecPointList activePecPoints = pecState->resetAllPec( time( NULL ) );
            
            // for each PECPoint returned from the reset
            for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator iter = activePecPoints.begin();
            iter != activePecPoints.end(); ++iter )
            {
                // create a PecUpdate with the PEC Point
                TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;
                
                // set the train and session ID
                pecUpdate.trainId = trainId;
                pecUpdate.sessionId = sessionId;
                
                pecUpdate.pec = *iter;
                
                // set the updateType to PECReset
                pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECCallLost;
                
                // send the message using m_messageSender
                m_messageSender.sendClientUpdate( pecUpdate );

                try
                {
                    // audit the event using m_auditSender.auditPecCallLost
                    m_auditSender.auditPecCallLost( trainId,
                        time( NULL ),
                        iter->car,
                        iter->pec );
                }
                // catch any exceptions (InvalidParameterException ) and log them
                catch ( TA_IRS_Bus::InvalidParameterException& e )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidParameterException", e.what() );
                }
            }
            // release the train state lock
        }

        // possibly this PEC has been ended and this is just timing

        FUNCTION_EXIT;
    }

    void TimsPecManager::processTrainStateChanged( TA_IRS_Bus::TrainStatePtr state )
    {
        FUNCTION_ENTRY( "processTrainStateChanged" );

        // If a train has become invalid, or started communicating with another agent
        // and the train is not within our control area
        // any outstanding PEC calls should be invalidated

        // check operation state using AgentManager, if not control, return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // down cast this to a TrainCommsStatePtr
        TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( state );
        TA_ASSERT( NULL != commsState.get(), "Cast failed" );

        bool commsValid = false;
        bool atsValid = false;
        unsigned long location = 0;
        TA_IRS_Bus::CommonTypes::TrainIdType trainId = 0;

        {
            // acquire the train state lock
            TA_IRS_Bus::TrainStateLock lock( commsState );

            // check if communication is valid, and if the communicating location is this agent location
            if ( ( true == commsState->isTrainCommunicationValid() ) &&
                 ( m_configuration.getLocationKey() == commsState->getCommunicatingLocation() ) )
            {
                // if so, commsValid = true, otherwise commsValid = false
                commsValid = true;
            }
            else
            {
                commsValid = false;
            }

            // get the ats validity, and the location (ignore localisation validity)
            atsValid = commsState->isAtsValid();

            location = commsState->getLocation();

            trainId = commsState->getTrainId();

            // release the train state lock
        }

        // if (not commsValid)
        if ( false == commsValid )
        {
            // inControlOfTrain = ats valid AND in control of location
            if ( ( false == atsValid ) ||
                 ( false == m_operationModeManager.isInControlOfLocation( location ) ) )
            {
                // if it is not valid
                invalidatePecForTrain( trainId );
            }
        }

        FUNCTION_EXIT;
    }


    void TimsPecManager::processAtsUpdate( const AtsTrainInformation& oldInformation,
                                           const AtsTrainInformation& newInformation )
    {
        FUNCTION_ENTRY( "processAtsUpdate" );

        // check the agent is in the control state using AgentManager, if not, return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // wasInControlArea = ( old ATS valid ) AND ( agent is in control of old location - m_operationModeManager.isInCOntrolOfLocation (old location) )
        bool wasInControlArea = ( true == oldInformation.trainValid ) &&
                                ( true == m_operationModeManager.isInControlOfLocation( oldInformation.localisation.currentLocation ) );

        // isInControlArea = ( new ATS valid ) AND ( agent is in control of new location - m_operationModeManager.isInCOntrolOfLocation (new location) )
        bool isInControlArea = ( true == newInformation.trainValid ) &&
                               ( true == m_operationModeManager.isInControlOfLocation( newInformation.localisation.currentLocation ) );

        // if the train was in this agents area (true == wasInArea) and it now isnt (false == isInArea)
        if ( ( true == wasInControlArea ) &&
             ( false == isInControlArea ) )
        {
            // check the trains communication validity - checkTrainCommunicationValidity
            if ( false == checkTrainCommunicationValidity( newInformation.physicalTrainNumber ) )
            {
                // if it is not valid, invalidatePecForTrain
                invalidatePecForTrain( newInformation.physicalTrainNumber );
            }
        }

        FUNCTION_EXIT;
    }


    void TimsPecManager::notifyOperationStateChange( TA_Base_Core::EOperationMode operationState )
    {
        FUNCTION_ENTRY( "notifyOperationStateChange" );

        // if the mode is control
        if ( TA_Base_Core::Control == operationState )
        {
            // deregister for operation state callbacks
            //TA_Base_Bus::AgentManager::deregisterForStateChangeCallbacks( this );

            // get all train comms states
            TA_IRS_Bus::TrainIdToStateMap stateMap = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainPecResource );

            for ( TA_IRS_Bus::TrainIdToStateMap::iterator stateIter = stateMap.begin();
                  stateMap.end() != stateIter; ++stateIter )
            {
                TrainPecStatePtr pecState = boost::dynamic_pointer_cast< TrainPecState >( stateIter->second );
                TA_ASSERT( NULL != pecState.get(), "Failed to cast to the correct type" );

                TA_IRS_Bus::TrainStateLock stateLock( pecState );

                TA_IRS_Bus::PecTypes::ActivePecPointList activePecPoints = pecState->getActivePecPoints();

                // for each PECPoint
                for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator iter = activePecPoints.begin();
                      iter != activePecPoints.end(); ++iter )
                {
                    // create a PecUpdate with the PEC Point
                    TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

                    // set the train and session ID
                    pecUpdate.trainId = stateIter->first;
                    pecUpdate.sessionId = pecState->getSessionId();

                    pecUpdate.pec = *iter;

                    // set the updateType to PECUpdated
                    pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECUpdated;

                    // send the message using m_messageSender
                    m_messageSender.sendClientUpdate( pecUpdate );
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


    void TimsPecManager::processEvent( TA_IRS_Bus::TrainEventPtr event )
    {
        FUNCTION_ENTRY( "processEvent" );

        // if not in control state (from agentmanager) return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the event type, based on its type, downcast to one of the following types and call the appropriate handler function:
        TA_IRS_Bus::TrainMessageType messageType = event->getEventType();
		
		LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"Test Log : processEvent, incoming event : %d",messageType);

        // PecActivatedEventPtr : processPecActivatedEvent
        if ( messageType == TA_IRS_Bus::PecActivatedEvent::getStaticType() )
        {
            TA_IRS_Bus::PecActivatedEventPtr pecActivatedEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::PecActivatedEvent >( event );

            TA_ASSERT( NULL != pecActivatedEvent.get(), "cast failed" );

            processPecActivatedEvent( pecActivatedEvent );
        }
        // PecReadyForConversationEventPtr : processPecReadyForConversationEvent
        else if ( messageType == TA_IRS_Bus::PecReadyForConversationEvent::getStaticType() )
        {
            TA_IRS_Bus::PecReadyForConversationEventPtr pecReadyForConversationEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::PecReadyForConversationEvent >( event );

            TA_ASSERT( NULL != pecReadyForConversationEvent.get(), "cast failed" );

            processPecReadyForConversationEvent( pecReadyForConversationEvent );
        }
        // PecRequestForResetEventPtr : processPecRequestForResetEvent
        else if ( messageType == TA_IRS_Bus::PecRequestForResetEvent::getStaticType() )
        {
            TA_IRS_Bus::PecRequestForResetEventPtr pecRequestForResetEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::PecRequestForResetEvent >( event );

            TA_ASSERT( NULL != pecRequestForResetEvent.get(), "cast failed" );

            processPecRequestForResetEvent( pecRequestForResetEvent );
        }
        // PecResetEventPtr : processPecResetEvent
        else if ( messageType == TA_IRS_Bus::PecResetEvent::getStaticType() )
        {
            TA_IRS_Bus::PecResetEventPtr pecResetEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::PecResetEvent >( event );

            TA_ASSERT( NULL != pecResetEvent.get(), "cast failed" );

            processPecResetEvent( pecResetEvent );
        }
        // PecSelectedByDriverEventPtr : processPecSelectedByDriverEvent
        else if ( messageType == TA_IRS_Bus::PecSelectedByDriverEvent::getStaticType() )
        {
            TA_IRS_Bus::PecSelectedByDriverEventPtr pecSelectedByDriverEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::PecSelectedByDriverEvent >( event );

            TA_ASSERT( NULL != pecSelectedByDriverEvent.get(), "cast failed" );

            processPecSelectedByDriverEvent( pecSelectedByDriverEvent );
        }
        // ChangeoverStatusEventPtr : processChangeoverStatusEvent
        else if ( messageType == TA_IRS_Bus::ChangeoverStatusEvent::getStaticType() )
        {
            TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeoverEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::ChangeoverStatusEvent >( event );

            TA_ASSERT( NULL != mpuChangeoverEvent.get(), "cast failed" );

            processChangeoverStatusEvent( mpuChangeoverEvent );
        }
        else
        {
            TA_ASSERT( false, "The event type received was not subscribed for" );
        }

        FUNCTION_EXIT;
    }


    void TimsPecManager::processPecActivatedEvent( TA_IRS_Bus::PecActivatedEventPtr event )
    {
        FUNCTION_ENTRY( "processPecActivatedEvent" );

        // TIMS has sent a PEC activation message
        // We do not need to respond, however we must update state and send out the message
        // If this is a re-send of an already active PEC we need to keep the original timestamp
        // We will only audit a PEC activation when it is newly activated, not on resends.

        // get the train ID from the event
        TA_IRS_Bus::CommonTypes::TrainIdType trainId = event->getTrainId();

        TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car = event->getCar();
        TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec = event->getPec();

		TA_IRS_Bus::PecTypes::EPecTrainSource pecTrainSource = event->getPecTrainSource();

		//Get the Primary and Secondary Tsi
		unsigned long primaryTsi = -1;
		unsigned long secondaryTsi = -1;
		TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState(trainId);
		{
			//acquire the comms state lock
			TA_IRS_Bus::TrainStateLock lock(commsState);

			try
			{
				//get the Primary Tsi
				primaryTsi = boost::lexical_cast<unsigned long>(commsState->getPrimaryTsi());
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Incorrect expected PrimaryTsi return, primaryTsi : %s",
					commsState->getPrimaryTsi().c_str());
				//Just set primaryTsi to 0
				primaryTsi = 0;
			}
			
			try
			{
				//get the Secondary Tsi
				secondaryTsi = boost::lexical_cast<unsigned long>(commsState->getSecondaryTsi());
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Incorrect expected SecondaryTsi return, Secondary : %s",
					commsState->getSecondaryTsi().c_str());
				//Just set secndaryTsi to 0
				secondaryTsi = 0;
			}		
		}

        time_t eventTime = event->getTime();
        time_t pecActivatedTime = 0;

		
		LOG_GENERIC( SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
			"processPecActivatedEvent: TrainId: %d, Car: %d, Pec: %d, PecTrainSource: %d",static_cast<int>(trainId),
			static_cast<TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber>( car ),
			static_cast<TA_IRS_Bus::ProtocolPecTypes::EPecNumber>( pec ),
			static_cast<TA_IRS_Bus::PecTypes::EPecTrainSource>( pecTrainSource ));

			

        // get the train PEC state
        TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

        {
            // acquire the train state lock
            TA_IRS_Bus::TrainStateLock lock( pecState );
			

			LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						 "Test Log : processPecActivatedEvent, Updating PEC points for this train : %d",trainId);

			// create a PecPoint with the given pec and car number
			TA_IRS_Bus::PecTypes::PecPoint newPoint;
			newPoint.car = car;
			newPoint.pec = pec;

			// search the active PEC points for this PEC
			TA_IRS_Bus::PecTypes::ActivePecPointList pecPointList = pecState->getActivePecPoints();
			TA_IRS_Bus::PecTypes::ActivePecPointList::iterator findIter = pecPointList.find(newPoint);
			if ( pecPointList.end() != findIter )
			{
				//Check if the PEC State is already answered by Operator
				if ( (*findIter).source == TA_IRS_Bus::PecTypes::PecSourceOperator)
				{
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"PEC Activate, TrainID: %d, Car: %d and PEC Number: %d will be ignored since the current PEC state is already answered by Operator.",
						trainId,static_cast<int>(car),static_cast<int>(pec));
					return;
				}
			}


            // call pecActivated to set the activation into the state
            TA_IRS_Bus::PecTypes::PecPoint pecPoint = pecState->pecActivated( car,
                                                                              pec,
                                                                              eventTime, pecTrainSource );

            // using the returned PEC point
            pecActivatedTime = pecPoint.creationTime;

            // create a PecUpdate with the PEC Point
            TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

            pecUpdate.pec = pecPoint;

            // set the train and session ID
            pecUpdate.sessionId = pecState->getSessionId();
            pecUpdate.trainId = trainId;

			//set the Primary Tsi
			pecUpdate.primaryTsi = primaryTsi;

			//set the Secondary Tsi
			pecUpdate.secondaryTsi = secondaryTsi;

            // set the updateType to PECUpdated
            pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECUpdated;
			
			// check whether we do have rights
			// to process the activated PEC
			// if no, then set validity to false

			if (!m_rightsChecker.canAnswerPec(trainId,pecUpdate.sessionId))
			{
				pecUpdate.pec.valid = false;
			}

            // send the message using m_messageSender
            m_messageSender.sendClientUpdate( pecUpdate );

            // release the train state lock
        }

        // if the timestamp in the returned PEC point is the same as the event timestamp
        if ( eventTime == pecActivatedTime )
        {
            // this is a new PEC activation, not an existing PEC being resent

            // audit the event using m_auditSender.auditPecActivated
            m_auditSender.auditPecActivated( trainId, eventTime, car, pec );
        }

        FUNCTION_EXIT;
    }


    void TimsPecManager::processPecReadyForConversationEvent( TA_IRS_Bus::PecReadyForConversationEventPtr event )
    {
        FUNCTION_ENTRY( "processPecReadyForConversationEvent" );

        // This is part of the PEC answer process, it means that the audio line is set up and we can make the call

        // get the train ID from the event
        TA_IRS_Bus::CommonTypes::TrainIdType trainId = event->getTrainId();

        // get the PEC and CAR from the event, log that this PEC is ready for conversation
        TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car = event->getCar();
        TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec = event->getPec();

        LOG_GENERIC( SourceInfo,
                     TA_Base_Core::DebugUtil::DebugInfo,
                     "The PEC [%s] on Car [%s] is ready for conversation",
                     TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pec ).c_str(),
                     TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( car ).c_str() );

        // get the PEC state for this train
        TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

        {
            // acquire the state lock
            TA_IRS_Bus::TrainStateLock lock( pecState );

            TA_IRS_Bus::PecTypes::PecPoint pecPoint;

            try
            {
                // get the connected PEC point
                pecPoint = pecState->getConnectedPecPoint();
            }
            // catch the InvalidParameterException
            catch ( TA_IRS_Bus::InvalidParameterException& )
            {
                // this means that there was no PEC in the progress of answering and this is an error

                // log that there is a PEC ready for conversation when no PEC answer is in progress
                LOG_GENERIC( SourceInfo,
                             TA_Base_Core::DebugUtil::DebugInfo,
                             "There is a PEC [%s] ready for conversation when no PEC answer is in progress",
                             TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pec ).c_str() );

                FUNCTION_EXIT;
                return;
            }

            // compare the PEC and CAR from the active PEC point to those from the event
            if ( ( car != pecPoint.car ) ||
                 ( pec != pecPoint.pec ) )
            {
                // if they differ
                // this means that this event does not match the current PEC answer

                // log that there is a PEC ready for conversation received that differs from the PEC answer in progress
                LOG_GENERIC( SourceInfo,
                             TA_Base_Core::DebugUtil::DebugInfo,
                             "There is a PEC [%s] ready for conversation received that differs from the PEC answer in progress",
                             TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pec ).c_str() );

                FUNCTION_EXIT;
                return;
            }

            // release the state lock
        }

        try
        {
            // using m_transactionManager.signalTrainTimer signal the timer for this train for TrainPecResource
            
			LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						 "Test Log : processPecReadyForConversationEvent, PEC has been answered, singalling train timer to proceed the call");

			m_transactionManager.signalTrainTimer( trainId, TA_IRS_Bus::CommonTypes::TrainPecResource );
        }
        // catch and the InvalidTrainException if there was no timer started
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );
        }

        FUNCTION_EXIT;
    }


    void TimsPecManager::processPecRequestForResetEvent( TA_IRS_Bus::PecRequestForResetEventPtr event )
    {
        FUNCTION_ENTRY( "processPecRequestForResetEvent" );

        // This is sent every 20s during an ongoing PEC, and ISCS must auto-answer with a continue the first X times
        // after the auto continue limit is reached, no answer is sent

        std::string sessionId;

        // get the train ID from the event
        TA_IRS_Bus::CommonTypes::TrainIdType trainId = event->getTrainId();
		
		
		LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"Test Log : processPecRequestForResetEvent, processing Reset Request for train : %d",trainId);
		
        // get the train PEC state
        TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

        {
            // acquire the train state lock
            TA_IRS_Bus::TrainStateLock lock( pecState );

            // get the connected PEC point
            TA_IRS_Bus::PecTypes::PecPoint pecPoint = pecState->getConnectedPecPoint();

            // if the connected PEC point and the PEC sent in the event dont match
            if ( pecPoint.pec != event->getPec() )
            {
                // ignore this request
                // log a message that a PEC request for reset was received for a PEC other than the connected PEC
                LOG_GENERIC( SourceInfo,
                             TA_Base_Core::DebugUtil::DebugInfo,
                             "A PEC [%s] request for reset was received for a PEC [%s] other than the connected PEC",
                             TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( event->getPec() ).c_str(),
                             TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pecPoint.pec ).c_str() );

                FUNCTION_EXIT;
                return;
            }

            // get the session ID
            sessionId = pecState->getSessionId();

            // get the auto continue ack count
            unsigned char autoContinueCount = pecState->getAutoContinueCount();

            // if it is less than the auto continue ack limit (from m_configuration), continue is not required, otherwise it is
            bool continueNotRequired = ( autoContinueCount <
                                         m_configuration.getTimsAutoContinueAckLimit() );

            // if continue is not required
            if ( true == continueNotRequired )
            {
                // increment the auto continue count
                ++autoContinueCount;
            }
            // otherwise if it is
            {
                // set that continue is required
                pecState->setContinueRequired();

                // return (do not send anything to the train)
                FUNCTION_EXIT;
                return;
            }

            // release the train state lock
        }

        // an automatic continue needs to be sent

        // get the operator's origin
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        // create a PecContinueCommand with the PEC details from the incoming message
        TA_IRS_Bus::PecContinueCommandPtr command( new TA_IRS_Bus::PecContinueCommand( trainId,
                                                                                       origin,
                                                                                       event->getCar(),
                                                                                       event->getPec(),
																					   event->getPecTrainSource() ) );

		LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					 "Test Log : processPecReadyForConversationEvent, can reset , sending reset recv cmd");

        try
        {
            // using m_transactionManager send the command (use the default timeout and retries as this command has no response)
			m_transactionManager.sendCommand( command );
        }
        // catch the InvalidParameterException, log it and return, it means that there was no active PEC point for this train
        catch ( TA_IRS_Bus::InvalidParameterException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidParameterException", e.getParameterName() + ", " + e.getParameterValue() );

            FUNCTION_EXIT;
            return;
        }
        // catch and log any other exceptions
        catch ( TA_IRS_Bus::TrainAccessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );
        }
        catch ( TA_IRS_Bus::TrainTransmissionFailureException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );
        }
        catch ( TA_IRS_Bus::TrainTimeoutException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimeoutException", e.what() );
        }
        catch ( TA_IRS_Bus::TransactionCancelledException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactionCancelledException", e.what() );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "" );
        }

        FUNCTION_EXIT;
    }


    void TimsPecManager::processPecResetEvent( TA_IRS_Bus::PecResetEventPtr event )
    {
        FUNCTION_ENTRY( "processPecResetEvent" );

        // A single PEC or all PEC has been reset by TIMS
        //
        // for each reset PEC not answered by an operator, we:
        // 1. clear the PEC in the state
        // 2. send the client update
        // 3. Stop a PEC answer if it was in progress and the reset resets the PEC being answered
        // 4. reply to TIMS if the PEC reset was for all PEC
        // If one of the reset PEC was answered by an operator, a call is in progress, we must end the audio call

        // get the train ID from the event
        TA_IRS_Bus::CommonTypes::TrainIdType trainId = event->getTrainId();

        // get the PEC and CAR from the event
        TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car = event->getCar();
        TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec = event->getPec();
		TA_IRS_Bus::ProtocolPecTypes::EPECFlagNumber rqsOrAck = event->getRqsOrAck();


        // Log that PEC X, CAR X was reset on train X
        LOG_GENERIC( SourceInfo,
                     TA_Base_Core::DebugUtil::DebugInfo,
                     "PEC %s, CAR %s was reset on train: %d",
                     TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( car ).c_str(),
                     TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pec ).c_str(),
                     trainId );

        // connectedPointReset = false
        bool connectedPointReset = false;

        unsigned long callId = 0;
        unsigned long callIdAfterReset = 0;
        unsigned long consoleId = 0;
        std::string sessionId;

        // get the train PEC state
        TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

        {
            // acquire the train state lock
            TA_IRS_Bus::TrainStateLock lock( pecState );

            // get the PEC call details before doing a reset
            callId = pecState->getCallId();
            consoleId = pecState->getConsoleId();
            sessionId = pecState->getSessionId();

			

            // call state->pecReset and hold onto the return value
            TA_IRS_Bus::PecTypes::ActivePecPointList resetPecPoints = pecState->pecReset( car,
                                                                                          pec,
                                                                                          event->getTime(),
																						  event->getPecTrainSource());

			LOG_GENERIC( SourceInfo,
				TA_Base_Core::DebugUtil::DebugInfo,
				"size of pecpoints is %d,callId %d, consoleId %d was reset on train: %d",
				resetPecPoints.size(),callId , consoleId ,trainId );
            // get the PEC call details after doing a reset
            callIdAfterReset = pecState->getCallId();

            // for each PECPoint returned from the reset
            for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator iter = resetPecPoints.begin();
                  iter != resetPecPoints.end(); ++iter )
            {
                // create a PecUpdate with the PEC Point
                TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

                // set the train and session ID
                pecUpdate.trainId = trainId;
                pecUpdate.sessionId = sessionId;

                // set the updateType to PECReset
                pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECReset;

                pecUpdate.pec = *iter;

                // send the message using m_messageSender

                m_messageSender.sendClientUpdate( pecUpdate );

                // audit the event using m_auditSender.auditPecResetByTims
                m_auditSender.auditPecResetByTims( trainId, time( NULL ), iter->car, iter->pec );

                // if the source in this PEC is operator, then connectedPointReset = true
                if ( TA_IRS_Bus::PecTypes::PecSourceOperator == iter->source )
                {
                    connectedPointReset = true;
                }
            }

            // now, any still activated PEC need to be re-sent to the banner to update their status to available

            // get all active PEC points from the state
            TA_IRS_Bus::PecTypes::ActivePecPointList activePecPoints = pecState->getActivePecPoints();

            // for each PECPoint
            for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator pecIterForReSent = activePecPoints.begin();
                  pecIterForReSent != activePecPoints.end(); ++pecIterForReSent )
            {
                // create a PecUpdate with the PEC Point
                TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

                // set the train and session ID
                pecUpdate.trainId = trainId;
                pecUpdate.sessionId = pecState->getSessionId();

                // set the updateType to PECUpdated
                pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECUpdated;

                pecUpdate.pec = *pecIterForReSent;

                // send the message using m_messageSender
                m_messageSender.sendClientUpdate( pecUpdate );
            }

            // release the train state lock
        }

        // if there was a call ID before the reset, and not one after the reset
        if ( ( callId != 0 ) &&
             ( callId != callIdAfterReset ) )
        {
            // then there was a call in progress that must be ended

            try
            {
				unsigned long talkgroupId = -1;

				// get the train comms state for the train
				TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

				//The active TSI to use to end a PEC Call
				std::string activeTsi;
				{
					// acquire the comms state lock
					TA_IRS_Bus::TrainStateLock lock( commsState );

					//The active TSI to use to end a PEC Call
					activeTsi = commsState->getActiveTsi();

					// get talkgroup Id instead
					//talkgroupId = commsState->getTalkgroupId();
					try
					{
						talkgroupId = boost::lexical_cast<unsigned long>(activeTsi);
					}
					catch(...)
					{
						//If it failed to convert set talkgroupId = 0
						talkgroupId = 0;
					}

					// release the comms state lock
				}
                // end the call using m_radioAgent
                m_radioAccess.endPecCall( sessionId, talkgroupId );
            }
            // catch and log any exceptions
            catch ( TA_IRS_Bus::TrainRadioException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainRadioException", e.what() );
            }
        }

        // if the connected PEC point was reset
        if ( true == connectedPointReset )
        {
            // in case this PEC point was in the process of answering,

            try
            {
                // using m_transactionManager.signalTrainTimer signal the timer for this train for TrainPecResource
                m_transactionManager.signalTrainTimer( trainId,
                                                       TA_IRS_Bus::CommonTypes::TrainPecResource );
            }
            // catch and log the InvalidTrainException if there was no timer started
            catch ( TA_IRS_Bus::InvalidTrainException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );
            }
        }

        // The PEC reset reply needs to be sent to TIMS if this was a reset all
        // using the PEC and CAR from the event
        // if it is AllPec and AllCar
     //   if ( TA_IRS_Bus::ProtocolPecTypes::AllCars == car &&
	//		TA_IRS_Bus::ProtocolPecTypes::AllPec == pec &&
		//	TA_IRS_Bus::ProtocolPecTypes::Request == rqsOrAck )
        {
            unsigned char origin = m_configuration.getAgentOrigin();

            // create a PecResetCommand without an expected response
            TA_IRS_Bus::PecResetCommandReceivedCommandPtr command(
                new TA_IRS_Bus::PecResetCommandReceivedCommand( trainId,
                                                 origin,
                                                 car,
                                                 pec,
												 TA_IRS_Bus::ProtocolPecTypes::PecProceeding,
												 event->getPecTrainSource()
                                                  ) );

            try
            {
                // send the command with no retries and the default timeout as no response is expected
                m_transactionManager.sendCommand( command );
            }
            // catch and log any exceptions
            catch ( TA_IRS_Bus::InvalidParameterException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidParameterException", e.getParameterName() + ", " + e.getParameterValue() );
            }
            catch ( TA_IRS_Bus::TrainAccessDeniedException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );
            }
            catch ( TA_IRS_Bus::TrainTransmissionFailureException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );
            }
            catch ( TA_IRS_Bus::TrainTimeoutException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimeoutException", e.what() );
            }
            catch ( TA_IRS_Bus::TransactionCancelledException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactionCancelledException", e.what() );
            }
            catch ( TA_Base_Core::TransactiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
            }
            catch ( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "...", "" );
            }
        }

        FUNCTION_EXIT;
    }


    void TimsPecManager::processPecSelectedByDriverEvent( TA_IRS_Bus::PecSelectedByDriverEventPtr event )
    {
        FUNCTION_ENTRY( "processPecSelectedByDriverEvent" );

        // TIMS has sent a PEC selected by driver message
        // We do not need to respond, however we must update state and send out the message

        // if the driver has selected a PEC that was in use by the operator
        // (or any pec in the same train was in use by the operator)
        // then the operator call must be ended
        // and the details need to be sent out again for all active points

        // get the train ID from the event
        TA_IRS_Bus::CommonTypes::TrainIdType trainId = event->getTrainId();

        LOG_GENERIC( SourceInfo,
                     TA_Base_Core::DebugUtil::DebugInfo,
                     "PEC %s, CAR %s on train: %d was answered by the driver",
                     TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( event->getCar() ).c_str(),
                     TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( event->getPec() ).c_str(),
                     trainId );

        // get the train PEC state
        TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

        std::string answeredSessionId("");
        unsigned long answeredConsoleId = 0;
        unsigned long answeredCallId = 0;

        {
            // acquire the train state lock
            TA_IRS_Bus::TrainStateLock lock( pecState );

            answeredSessionId = pecState->getSessionId();
            answeredConsoleId = pecState->getConsoleId();
            answeredCallId = pecState->getCallId();

            // call pecAnsweredByDriver to set the status into the state
            // this will clear any operator answered PEC
            // it will return a list of all the affected PEC so updates can be sent out
            TA_IRS_Bus::PecTypes::ActivePecPointList affectedPoints =
                pecState->pecAnsweredByDriver( event->getCar(),
                                               event->getPec(),
                                               event->getTime(),
											   event->getPecTrainSource());

            // for each PECPoint
            for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator pecIter = affectedPoints.begin();
                  affectedPoints.end() != pecIter; ++pecIter )
            {
                // create a PecUpdate with the PEC Point
                TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

                // set the train and session ID
                pecUpdate.trainId = trainId;
                pecUpdate.sessionId = pecState->getSessionId();

                // set the updateType to PECUpdated
                pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECUpdated;

                pecUpdate.pec = *pecIter;

                // send the message using m_messageSender
                m_messageSender.sendClientUpdate( pecUpdate );
            }
            // release the train state lock
        }

        // end the call if necessary
        if ( 0 != answeredCallId )
        {
            try
            {
				unsigned long talkgroupId = -1;
				TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

				//The active TSI to use to end a PEC Call
				std::string activeTsi;
				{
					TA_IRS_Bus::TrainStateLock lock( commsState );

					//The active TSI to use to end a PEC Call
					activeTsi = commsState->getActiveTsi();

					// get talkgroup Id instead
					//talkgroupId = commsState->getTalkgroupId();
					try
					{
						talkgroupId = boost::lexical_cast<unsigned long>(activeTsi);
					}
					catch(...)
					{
						//If it failed to convert set talkgroupId = 0
						talkgroupId = 0;
					}
				}
                // end the call using m_radioAgent
				m_radioAccess.endPecCall( answeredSessionId, talkgroupId );
            }
            // catch and log any exceptions
            catch ( TA_IRS_Bus::TrainRadioException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainRadioException", e.what() );
            }
        }

        // also if there was a pec answer in progress it needs to be stopped
        try
        {
            // using m_transactionManager.signalTrainTimer signal the timer for this train for TrainPecResource
            m_transactionManager.signalTrainTimer( trainId, TA_IRS_Bus::CommonTypes::TrainPecResource );
        }
        // catch and log the InvalidTrainException in case there was no timer started
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );
        }

        FUNCTION_EXIT;
    }


    void TimsPecManager::processChangeoverStatusEvent( TA_IRS_Bus::ChangeoverStatusEventPtr event )
    {
        FUNCTION_ENTRY( "processChangeoverStatusEvent" );

        // The active MPU on the train has failed.
        // Consider any ongoing PEC reset. Any calls must be ended, no message sent to train.
        // Consider any unanswered PEC reset. They will be resent by the new MPU.

        unsigned long consoleId = 0;
        unsigned long callId = 0;
        std::string sessionId;

        // get the train ID from the event
        TA_IRS_Bus::CommonTypes::TrainIdType trainId = event->getTrainId();

        // get the train PEC state
        TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

        {
            // acquire the train state lock
            TA_IRS_Bus::TrainStateLock lock( pecState );

            // get the call, console, and session ID
            consoleId = pecState->getConsoleId();
            callId = pecState->getCallId();
            sessionId = pecState->getSessionId();

            // resetAllPec ( with the time for the event )
            TA_IRS_Bus::PecTypes::ActivePecPointList activePecPoints = pecState->resetAllPec( event->getTime() );

            // for each returned PEC point
            for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator iter = activePecPoints.begin();
                  iter != activePecPoints.end(); ++iter )
            {
                // create a PecUpdate with the PEC Point
                TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

                // set the train and session ID
                pecUpdate.trainId = trainId;
                pecUpdate.sessionId = sessionId;

                // set the updateType to PECMpuChangeover
                pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECMpuChangeover;

                pecUpdate.pec = *iter;

                // send the message using m_messageSender
                m_messageSender.sendClientUpdate( pecUpdate );
            }

            // release the train state lock
        }

        // if there was an ongoing call
        if ( 0 != callId )
        {
            // it must be ended
            try
            {
               	TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover (event);
				// reset pec call when the status failure reason is due to ACSU failure
				if (mpuChangeover->getStatus ())
				{
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"ChangeOver Due to ACSU Failure-- Ending Current Call");
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Active Train Tsi use as callid : %d", callId);
					
					unsigned long lSourceTsi = 0;
					try
					{
						lSourceTsi = boost::lexical_cast<unsigned long>(event->getTsi());
					}
					catch (...)
					{}
					
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Train switch over to radio : %d", lSourceTsi);
					
					// end active call
					m_radioAccess.endPecCall( sessionId, callId );
				}
            }
            // catch and log any exceptions
            catch ( TA_IRS_Bus::TrainRadioException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainRadioException", e.what() );
            }
        }

        // audit the event using m_auditSender.auditPecResetByTims
        m_auditSender.auditPecResetByTims( trainId, event->getTime(),
                                           TA_IRS_Bus::ProtocolPecTypes::AllCars,
                                           TA_IRS_Bus::ProtocolPecTypes::AllPec );

        FUNCTION_EXIT;
    }


    void TimsPecManager::performPreOperatorActionChecks( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                         const std::string& sessionId )
    {
        FUNCTION_ENTRY( "performPreOperatorActionChecks" );

        // first, check if active PEC for this train are considered valid using checkPecValidity
        if ( false == checkPecValidity( trainId ) )
        {
            // if it returns false, throw an InvalidTrainException ( Agent not in control of this train )
            TA_THROW( TA_IRS_Bus::InvalidTrainException( "Agent not in control of this train",
                                                         trainId ) );
        }

        // second, check if this operator can perform PEC operations, using m_rightsChecker.canAnswerPec
        if ( false == m_rightsChecker.canAnswerPec( trainId, sessionId ) )
        {
            // if not, throw an AccessDeniedException
        //    TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Cannot answer PEC for this train" ) );
			 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "DVA announcement for trains No Rightb for canAnswerPec: %d",
                     trainId );
        }

        FUNCTION_EXIT;
    }


    bool TimsPecManager::checkPecValidity( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "checkPecValidity" );

        // This checks if a train is in this agent's control area,
        // or is communicating with this agent
        // If either one of these conditions is met, then PEC operations are valid for this train

        // using m_operationModeManager check this agent is in control of this train
        // if it is not
        if ( false == m_operationModeManager.isInControlOfTrain( trainId ) )
        {
            // check if the train is still communicating with this agent:
            FUNCTION_EXIT;
            return checkTrainCommunicationValidity( trainId );
        }

        // if the above tests pass, the PEC is still valid for this train
        FUNCTION_EXIT;
        return true;
    }


    bool TimsPecManager::checkTrainCommunicationValidity( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "checkTrainCommunicationValidity" );

        // get the train comms state for this train
        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

        {
            // acquire the train state lock
            TA_IRS_Bus::TrainStateLock lock( commsState );

            // check if communication is valid, and if the communicating location is this agent location
            if ( ( false == commsState->isTrainCommunicationValid() ) ||
                 ( false == ( commsState->getCommunicatingLocation() == m_configuration.getLocationKey() ) ) )
            {
                // if not, return false
                FUNCTION_EXIT;
                return false;
            }

            // release the train state lock
        }

        FUNCTION_EXIT;
        return true;
    }


    void TimsPecManager::invalidatePecForTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "invalidatePecForTrain" );

        // get the train PEC state for the train
        TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

        {
            // acquire the state lock
            TA_IRS_Bus::TrainStateLock lock( pecState );

            // invalidateAllPec
            TA_IRS_Bus::PecTypes::ActivePecPointList activePecPoints = pecState->invalidateAllPec( time( NULL ) );

            // for each PEC in the returned ActivePecPointList
            for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator iter = activePecPoints.begin();
                  iter != activePecPoints.end(); ++iter )
            {
                // create a PecUpdate
                TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

                // populate it with the PEC point, and the other information from the state
                pecUpdate.pec = *iter;
                pecUpdate.sessionId = pecState->getSessionId();
                pecUpdate.trainId = pecState->getTrainId();

                // set its update type to PECUpdated
                pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECUpdated;

                // send the structure using m_messageSender
                m_messageSender.sendClientUpdate( pecUpdate );

                // audit the event using m_auditSender.auditPecCallInvalid
                m_auditSender.auditPecCallInvalid( trainId, time( NULL ), iter->car, iter->pec );
            }

            // release the state lock
        }

        FUNCTION_EXIT;
    }


    void TimsPecManager::endRadioCall( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                       TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                       TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
									   const std::string& sessionId, TA_IRS_Bus::PecTypes::EPecTrainSource pecTrainSource )
    {
        FUNCTION_ENTRY( "give_me_a_name" );

        TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

        try
        {
            // using m_transactionManager.endTrainTimer, end the timer as this is now done
            m_transactionManager.endTrainTimer( trainId,
                                                TA_IRS_Bus::CommonTypes::TrainPecResource );
        }
        // catch the exception in case there was no timer yet
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "Failed to end train timer" );
        }

        unsigned long callId = 0;
        unsigned long consoleId = 0;

        {
            // acquire the PEC state lock
            TA_IRS_Bus::TrainStateLock lock( pecState );

            // get the call and console ID
            callId = pecState->getCallId();
            consoleId = pecState->getConsoleId();

            // call pecAnswered, with 0 for the the call Id and console ID to reset the call details in the state
            pecState->pecAnswered( car, pec, time( NULL ), sessionId, 0, 0, pecTrainSource );

            // release the PEC state lock
        }

        // if there was a call in progress
        if ( 0 != callId )
        {
            try
            {
				unsigned long talkgroupId = -1;

				// get the train comms state for the train
				TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

				//The active TSI to use to end a PEC Call
				std::string activeTsi;
				{
					// acquire the comms state lock
					TA_IRS_Bus::TrainStateLock lock( commsState );

					//The active TSI to use to end a PEC Call
					activeTsi = commsState->getActiveTsi();

					// get talkgroup Id instead
					//talkgroupId = commsState->getTalkgroupId();
					try
					{
						talkgroupId = boost::lexical_cast<unsigned long>(activeTsi);
					}
					catch(...)
					{
						//If it failed to convert set talkgroupId = 0
						talkgroupId = 0;
					}

					// release the comms state lock
				}
                // end the call using m_radioAgent
                m_radioAccess.endPecCall( sessionId, talkgroupId );
            }
            // catch any exceptions in here and log them
            catch ( TA_IRS_Bus::TrainRadioException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainRadioException", e.what() );
            }
        }
    }


    template< typename ExceptionType >
    void TimsPecManager::checkRetries( unsigned long& retries, ExceptionType& e )
    {
        FUNCTION_ENTRY( "checkRetries" );

        // if the retries have not been exceeded
        if ( retries < m_configuration.getTimsRetryLimit() )
        {
            // increment retries
            ++retries;
        }
        else
        {
            // the number of retries has been exceeded
            throw e;
        }

        FUNCTION_EXIT;
    }


    void TimsPecManager::initCarPec2CameraMap()
    {
        FUNCTION_ENTRY( "initCarPec2CameraMap" );

        //TODO: this is hard code
        m_CarPpec2CameraMap[ std::make_pair( TA_IRS_Bus::ProtocolPecTypes::CarOne, TA_IRS_Bus::ProtocolPecTypes::PecOne ) ] = TA_IRS_Bus::ProtocolCctvTypes::CameraThree;
        m_CarPpec2CameraMap[ std::make_pair( TA_IRS_Bus::ProtocolPecTypes::CarOne, TA_IRS_Bus::ProtocolPecTypes::PecTwo ) ] = TA_IRS_Bus::ProtocolCctvTypes::CameraTwo;
        m_CarPpec2CameraMap[ std::make_pair( TA_IRS_Bus::ProtocolPecTypes::CarOne, TA_IRS_Bus::ProtocolPecTypes::PecThree ) ] = TA_IRS_Bus::ProtocolCctvTypes::CameraThree;
        m_CarPpec2CameraMap[ std::make_pair( TA_IRS_Bus::ProtocolPecTypes::CarOne, TA_IRS_Bus::ProtocolPecTypes::PecFour ) ] = TA_IRS_Bus::ProtocolCctvTypes::CameraTwo;
        m_CarPpec2CameraMap[ std::make_pair( TA_IRS_Bus::ProtocolPecTypes::CarTwo, TA_IRS_Bus::ProtocolPecTypes::PecOne ) ] = TA_IRS_Bus::ProtocolCctvTypes::CameraFive;
        m_CarPpec2CameraMap[ std::make_pair( TA_IRS_Bus::ProtocolPecTypes::CarTwo, TA_IRS_Bus::ProtocolPecTypes::PecTwo ) ] = TA_IRS_Bus::ProtocolCctvTypes::CameraFour;
        m_CarPpec2CameraMap[ std::make_pair( TA_IRS_Bus::ProtocolPecTypes::CarTwo, TA_IRS_Bus::ProtocolPecTypes::PecThree ) ] = TA_IRS_Bus::ProtocolCctvTypes::CameraFive;
        m_CarPpec2CameraMap[ std::make_pair( TA_IRS_Bus::ProtocolPecTypes::CarTwo, TA_IRS_Bus::ProtocolPecTypes::PecFour ) ] = TA_IRS_Bus::ProtocolCctvTypes::CameraFour;
        m_CarPpec2CameraMap[ std::make_pair( TA_IRS_Bus::ProtocolPecTypes::CarThree, TA_IRS_Bus::ProtocolPecTypes::PecOne ) ] = TA_IRS_Bus::ProtocolCctvTypes::CameraSix;
        m_CarPpec2CameraMap[ std::make_pair( TA_IRS_Bus::ProtocolPecTypes::CarThree, TA_IRS_Bus::ProtocolPecTypes::PecTwo ) ] = TA_IRS_Bus::ProtocolCctvTypes::CameraSeven;
        m_CarPpec2CameraMap[ std::make_pair( TA_IRS_Bus::ProtocolPecTypes::CarThree, TA_IRS_Bus::ProtocolPecTypes::PecThree ) ] = TA_IRS_Bus::ProtocolCctvTypes::CameraSix;
        m_CarPpec2CameraMap[ std::make_pair( TA_IRS_Bus::ProtocolPecTypes::CarThree, TA_IRS_Bus::ProtocolPecTypes::PecFour ) ] = TA_IRS_Bus::ProtocolCctvTypes::CameraSeven;

        FUNCTION_EXIT;
    }


	
    void TimsPecManager::initTvssManagerNamedObject()
    {
		
        FUNCTION_ENTRY( "initTvssManagerNamedObject" );

        try
        {
            // load the local TVSSAgent entity from the database, and populate the named object with the details.
           TA_Base_Core::CorbaNameList tvssManagerCorbaName =
                TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( TA_Base_Core::TVSSAgentEntityData::getStaticType(),
                m_configuration.getLocationKey(), false );

            if ( tvssManagerCorbaName.size() != 1 )
            {
                std::ostringstream error;

                error << "Incorrect configuration, there are " << tvssManagerCorbaName.size()
                    << " tvss manager corba names at this location";

                TA_THROW( TA_Base_Core::FatalConfigurationException( error.str() ) );
            }

            m_tvssManagerNamedObject.setCorbaName( tvssManagerCorbaName[0] );
        }
        catch ( ... )
        {
            TA_THROW( TA_Base_Core::FatalConfigurationException( "Configuration Error: Can not locate VideoSwitch Agent" ) );
        }
        
        FUNCTION_EXIT;
    }
	


    void TimsPecManager::initConsole2MonitorMap()
    {
        FUNCTION_ENTRY( "initConsole2MonitorMap" );

        try
        {
            TA_Base_Core::IEntityDataList monitorEntities =
                TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( "VideoMonitor", m_configuration.getLocationKey() );

            for ( TA_Base_Core::IEntityDataList::iterator it = monitorEntities.begin(); it != monitorEntities.end(); ++it )
            {
                unsigned long key = (*it)->getKey();
                std::string strAddr = (*it)->getAddress();
                TA_Base_Core::EntityHelper helper( key, "VideoMonitor" );
                std::string strConsoleId = helper.getParameter( "Console" );

                long iMonitorId = 0;
                unsigned long iConsoleId = 0;

                try
                {
                    iMonitorId = boost::lexical_cast<long>( strAddr );
                }
                catch ( ... )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "cannot convert string %s to long", strAddr.c_str() );
                }

                try
                {
                    iConsoleId = boost::lexical_cast<unsigned long>( strConsoleId );
                }
                catch ( ... )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "cannot convert string %s to unsigned long", strConsoleId.c_str() );
                }

                if ( 0 < iMonitorId )
                {
                    m_Console2MonitorMap[ iConsoleId ] = iMonitorId;
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "console to monitor map entry [%d,%d]", iConsoleId, iMonitorId );
                }
            }
        }
        catch (const TA_Base_Core::DatabaseException& e)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what() );
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what() );
        }

        FUNCTION_EXIT;
    }


    long TimsPecManager::getMonitorIdFromSession( const std::string& sessionId )
    {
        FUNCTION_ENTRY( "getMonitorIdFromSession" );

        unsigned long iConsoleId = m_configuration.getConsoleFromSession( sessionId );

        if ( 0 == iConsoleId )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot find console id from session" );

            FUNCTION_EXIT;
            return 0;
        }

        Console2MonitorMapType::iterator findIter = m_Console2MonitorMap.find( iConsoleId );

        if ( findIter != m_Console2MonitorMap.end() )
        {
            FUNCTION_EXIT;
            return findIter->second;
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot find monitor id from console id %d", iConsoleId );

            FUNCTION_EXIT;
            return 0;
        }
    }

	bool TimsPecManager::isAnswerByOperator ( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
		TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
		TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
		const std::string& sessionId ) 
	{
      
		// answeredByOperator = false
		bool answeredByOperator = false;

		unsigned long consoleId = 0;
		unsigned long callId = 0;

		// get the train PEC state
		TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

		TA_IRS_Bus::PecTypes::PecPoint pecToReset;

		pecToReset.car = car;
		pecToReset.pec = pec;

		{
			// acquire the state lock
			TA_IRS_Bus::TrainStateLock lock( pecState );

			// get the call id, console id, and session id
			consoleId = pecState->getConsoleId();
			callId = pecState->getCallId();
			std::string stateSessionId = pecState->getSessionId();

			// get all active PEC from the state
			TA_IRS_Bus::PecTypes::ActivePecPointList activePecPoints = pecState->getActivePecPoints();

			// check if the PEC being reset is in the list of active PEC
			TA_IRS_Bus::PecTypes::ActivePecPointList::iterator findIter = activePecPoints.find( pecToReset );

			if ( findIter != activePecPoints.end() )
			{
				pecToReset = *findIter;

				// Bohong++, for CL-20793, we move the rights check module here
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"pecToReset.valid = %d", pecToReset.valid );
				if ( true == pecToReset.valid )
				{
					if ( false == m_rightsChecker.canAnswerPec( trainId, sessionId ) )
					{
						// TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Cannot reset PEC for this train" ) );
						//setupLiveAnnouncement
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"DVA announcement for trains No Right for resetPec: %d",
							trainId );
					}
				}
				// ++Bohong

				// check if it is answered (if the source is operator)
				if ( TA_IRS_Bus::PecTypes::PecSourceOperator == pecToReset.source )
				{
					// check if the session id is this operators session ID
					if ( stateSessionId != sessionId )
					{
						// throw an AccessDeniedException (Cant reset a PEC in use by another operator)
						TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Cannot reset PEC in use by other operator" ) );
					}

					// then this call will be ended, clear the call
					pecState->clearCall();

					// answeredByOperator = true
					answeredByOperator = true;
				}
			}
			else
			{
				// throw an InvalidParameterException (Given PEC is not active)
				TA_THROW( TA_IRS_Bus::InvalidParameterException( "Given PEC is not active",
					"pec",
					TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pec ) ) );
			}

			return answeredByOperator ;

			// release the state lock
		}

	}

	void TimsPecManager::resetInvalidPec (TA_IRS_Bus::CommonTypes::TrainIdType trainId,
										  std::string sessionId,
										  TA_IRS_Bus::PecTypes::PecPoint pecToReset )
	{
		FUNCTION_ENTRY ("resetInvalidPec");
		
		
		TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );
		
		{
		
			TA_IRS_Bus::TrainStateLock lock( pecState );
		
			typedef TA_IRS_Bus::PecTypes::ActivePecPointList ActPntLstType;
			time_t timestamp;
			
			time(&timestamp);
			
			TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

			// reset the cache for TrainAgent for this train with respect to there car and pec number
			ActPntLstType activePecPoints = pecState->pecReset( pecToReset.car,
																pecToReset.pec,timestamp,
																pecToReset.pecTrainSource);
			
			ActPntLstType::iterator iterActPointList = activePecPoints.begin();

			for (; iterActPointList != activePecPoints.end();++iterActPointList)
			{
				// create a PecUpdate with the PEC Point
				TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

				// set the train and session ID
				pecUpdate.sessionId = sessionId;
				pecUpdate.trainId = trainId;

				pecUpdate.pec = *iterActPointList;

				// set the updateType to PECReset
				pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECReset;

				// send the message using m_messageSender
				m_messageSender.sendClientUpdate( pecUpdate );
			}

			activePecPoints.clear();
			activePecPoints = pecState->getActivePecPoints();
			
			iterActPointList = activePecPoints.begin();
			
			for (; iterActPointList != activePecPoints.end();++iterActPointList)
			{
				// create a PecUpdate with the PEC Point
				TA_IRS_Bus::PecTypes::PecUpdate pecUpdate;

				// set the train and session ID
				pecUpdate.sessionId = pecState->getSessionId();
				pecUpdate.trainId = trainId;

				pecUpdate.pec = *iterActPointList;

				// set the updateType to PECReset
				pecUpdate.updateType = TA_IRS_Bus::PecTypes::PECUpdated;

				// send the message using m_messageSender
				m_messageSender.sendClientUpdate( pecUpdate );
			}
			
			// lock release
		}	

		FUNCTION_EXIT;
	}


	void TimsPecManager::processReleaseOfCtrl( )
	{
		// create a PecUpdateList
		FUNCTION_ENTRY ("processReleaseOfCtrl");

		TA_IRS_Bus::PecTypes::PecUpdateList pecUpdates = getAllActivePec ();

		TA_IRS_Bus::PecTypes::PecUpdateList::iterator iterPecUpdates = pecUpdates.begin();
		
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"locationControlReleased");

		for (; pecUpdates.end() != iterPecUpdates; iterPecUpdates++)
		{
			std::string strSessionId = iterPecUpdates->sessionId;
			long lTrainId = iterPecUpdates->trainId;
			
			bool bIsvalid = m_rightsChecker.canAnswerPec(lTrainId,strSessionId);

			if (!bIsvalid)
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Invalidating out of control PEC with Train ID : %d",lTrainId);
				iterPecUpdates->pec.valid = false;
				m_messageSender.sendClientUpdate(*iterPecUpdates);

				// check if there is on going call
				TrainPecStatePtr pecState = m_statusManager.getTrainPecState( lTrainId );

				if ( 0 != pecState->getCallId())
				{	
					unsigned long talkgroupId;
					try
					{
						//Use the getActiveTsi to get the correct TSI Number.
						talkgroupId = boost::lexical_cast<unsigned long>(m_statusManager.getTrainCommsState(lTrainId)->getActiveTsi());
					}
					catch(...)
					{
						//If it failed to convert set talkgroupId = 0
						talkgroupId = 0;
					}
					
					// shut the on-going call due to lost of rights
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
								"Shutting on-going PEC Call due to duty lost : TG id %d",talkgroupId);

					m_radioAccess.endPecCall( strSessionId, talkgroupId );
				}
			}

		}
		FUNCTION_EXIT
	}


} // TA_IRS_App



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

#include "app/trains/train_agent/src/TimsTisManager.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"
#include "app/trains/train_agent/src/IOperationModeManager.h"
#include "app/trains/train_agent/src/IRightsChecker.h"
#include "app/trains/train_agent/src/IMessageSender.h"
#include "app/trains/train_agent/src/IRadioAgentInterface.h"
#include "app/trains/train_agent/src/RadioMessageProcessor.h"
#include "app/trains/train_agent/src/TisTasks.h"
#include "app/trains/train_agent/src/TrainVoiceCallRequest.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

//#include "bus/managed_agent/src/AgentManager.h"
#include "bus/trains/TrainCommonLibrary/src/TrainAccessDeniedException.h"
#include "bus/trains/TrainCommonLibrary/src/BadCommandException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTransmissionFailureException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTimeoutException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTimerException.h"
#include "bus/trains/TrainCommonLibrary/src/TransactionCancelledException.h"
#include "bus/trains/TrainCommonLibrary/src/MpuChangeoverException.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainRadioException.h"
#include "bus/trains/TrainTransactionModel/src/ITrainTransactionManager.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"
#include "bus/trains/TrainTransactionModel/src/UnsolicitedTrainTimerManager.h"

#include "bus/trains/TrainProtocolLibrary/src/TtisFreeTextMessageCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/TtisPreStoredMessageCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/TtisResetCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/TtisLibraryDownloadCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/TtisScheduleDownloadCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/TtisCommandReceivedEvent.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

    const std::string TimsTisManager::TIS2_TIMER_TEXT = "TIS2";

    TimsTisManager::TimsTisManager( ITrainAgentConfiguration& configuration,
                                    ITrainStatusManager& statusManager,
                                    TA_IRS_Bus::ITrainTransactionManager& transactionManager,
                                    IOperationModeManager& operationModeManager,
                                    IMessageSender& messageSender,
                                    IRightsChecker& rightsChecker,
                                    IRadioAgentInterface& radioAgent,
                                    RadioMessageProcessor& radioMessageProcessor )
          : m_configuration( configuration ),
            m_statusManager( statusManager ),
            m_transactionManager( transactionManager ),
            m_operationModeManager( operationModeManager ),
            m_messageSender( messageSender ),
            m_rightsChecker( rightsChecker ),
            m_radioAgent( radioAgent ),
            m_radioMessageProcessor( radioMessageProcessor ),
            m_threadPool( *TA_Base_Core::ThreadPoolSingletonManager::getInstance() ),
            m_tis2TimerManager( NULL )
    {
        FUNCTION_ENTRY( "TimsTisManager" );

        // this needs to be done here using new
        // it could have been a member by value, however then 'this' would need to
        // be used in the initialiser list, which is not a good idea
        m_tis2TimerManager = new TA_IRS_Bus::UnsolicitedTrainTimerManager( *this );

        // register for train events
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::EndOfTtisDownloadEvent::getStaticType(), this );
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::ChangeoverStatusEvent::getStaticType(), this );

        // subscribe for data call end events
        m_radioMessageProcessor.addDataCallEndObserver( this );

        FUNCTION_EXIT;
    }


    TimsTisManager::~TimsTisManager()
    {
        FUNCTION_ENTRY( "~TimsTisManager" );

        // deregister for everything registered for in the constructor

        // unsubscribe for data call end events
        m_radioMessageProcessor.removeDataCallEndObserver( this );

        // deregister for train events
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::EndOfTtisDownloadEvent::getStaticType(), this );
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::ChangeoverStatusEvent::getStaticType(), this );

        // remove the thread pool instance
        TA_Base_Core::ThreadPoolSingletonManager::removeInstance( &m_threadPool );

        // remove the tis2 timer manager
        delete m_tis2TimerManager;
        m_tis2TimerManager = NULL;

        FUNCTION_EXIT;
    }


    unsigned short TimsTisManager::getFlexibleTimeOutValue()
    {
        FUNCTION_ENTRY( "getFlexibleTimeOutValue" );

        // get the number of retries, and timer tis1
        // the timeout is ( ( (the number of retries + 1) * timer tis 1 ) /1000 ) + 5 (for procesing overhead)
        static unsigned short timeout =
            static_cast< unsigned short >( ( ( ( m_configuration.getTimsRetryLimit() + 1 ) * m_configuration.getTimerTIS1() ) / 1000 ) + 5 );

        FUNCTION_EXIT;
        return timeout;
    }


    TA_IRS_Bus::TtisTypes::TisLibraryVersionList TimsTisManager::getLibraryVersions()
    {
        FUNCTION_ENTRY( "getLibraryVersions" );

        // get all train comms states
        TA_IRS_Bus::TrainIdToStateMap stateMap = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::CoreResource );

        // create a library version list
        TA_IRS_Bus::TtisTypes::TisLibraryVersionList result;

        for ( TA_IRS_Bus::TrainIdToStateMap::iterator stateIter = stateMap.begin();
              stateMap.end() != stateIter; ++stateIter )
        {
            // down cast to the specific type
            TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( stateIter->second );
            TA_ASSERT( NULL != commsState.get(), "Failed to cast to the correct type" );

            TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate trainInfo;
            trainInfo.trainId = stateIter->first;

            {
                // acquire the lock for the train state
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                // get the TTIS library and Schedule version
                trainInfo.libraryVersion = commsState->getTtisLibraryVersion();
                trainInfo.scheduleVersion = commsState->getTtisScheduleVersion();

                // release the lock for the train state
            }

            // add the library version information to the list
            result.push_back( trainInfo );
        }

        // return the list

        FUNCTION_EXIT;
        return result;
    }


    TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate TimsTisManager::getLibraryVersionsForTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "getLibraryVersionsForTrain" );

        TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate result;
        result.trainId = trainId;

        // get the train comms state for the train
        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

        {
            // acquire the lock for the train state
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // get the TTIS library and Schedule version
            result.libraryVersion = commsState->getTtisLibraryVersion();
            result.scheduleVersion = commsState->getTtisScheduleVersion();

            // release the lock for the train state
        }

        // return the library versions

        FUNCTION_EXIT;
        return result;
    }


    void TimsTisManager::displayFreeTextMessage( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
                                                 const TA_IRS_Bus::TtisTypes::TtisFreeTextDisplayParameters& displayParameters,
                                                 time_t timestamp,
                                                 const std::string& sessionId )
    {
        FUNCTION_ENTRY( "displayFreeTextMessage" );

        // check that the PID list size is more than 0
        // if it isnt, throw an InvalidParameterException
        if ( true == displayParameters.pidList.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No PIDS given to display to", "PidList", "" ) );
        }

        // check that the train list size is more than 0
        // if it isnt, throw an InvalidParameterException
        if ( true == trains.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No trains given to display to", "TrainList", "" ) );
        }

        // check the session ID isnt empty
        // if it is, throw an InvalidParameterException
        if ( true == sessionId.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No operator session given", "SessionId", "" ) );
        }

        // check the timestamp is not 0
        // if it is, throw an InvalidParameterException
        if ( 0 == timestamp )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Invalid request timestamp", "timestamp", "0" ) );
        }

        // check the operator has the correct rights to display a ttis free text message
        // if not, throw an AccessDeniedException
        if ( false == m_rightsChecker.canDisplayTtisFreeText( sessionId ) )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not display TTIS Free Text message" ) );
        }

        // get the operators origin
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        // for each train in the train list
        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter = trains.begin();
              trains.end() != trainIter; ++trainIter )
        {
            // create a new FreeTextMessageTask
            // store it in an IWorkItemPtr and then queue it using m_threadPool
            TA_Base_Core::IWorkItemPtr newTask(
                new FreeTextMessageTask( *this,
                                         *trainIter,
                                         origin,
                                         displayParameters,
                                         timestamp,
                                         sessionId ) );

            m_threadPool.queueWorkItem( newTask );
        }

        FUNCTION_EXIT;
    }


    void TimsTisManager::displayStoredMessage( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
                                               const TA_IRS_Bus::TtisTypes::TtisPredefinedDisplayParameters& displayParameters,
                                               time_t timestamp,
                                               const std::string& sessionId )
    {
        FUNCTION_ENTRY( "displayStoredMessage" );

        // check that the PID list size is more than 0
        // if it isnt, throw an InvalidParameterException
        if ( true == displayParameters.pidList.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No PIDS given to display to", "PidList", "" ) );
        }

        // check that the train list size is more than 0
        // if it isnt, throw an InvalidParameterException
        if ( true == trains.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No trains given to display to", "TrainList", "" ) );
        }

        // check the session ID isnt empty
        // if it is, throw an InvalidParameterException
        if ( true == sessionId.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No operator session given", "SessionId", "" ) );
        }

        // check the timestamp is not 0
        // if it is, throw an InvalidParameterException
        if ( 0 == timestamp )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Invalid request timestamp", "timestamp", "0" ) );
        }

        // check the operator has the correct rights to display a ttis predfined message
        // if not, throw an AccessDeniedException
        if ( false == m_rightsChecker.canDisplayTtisPredefined( sessionId ) )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not display TTIS Pre Defined message" ) );
        }

        // get the operators origin
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        // for each train in the train list
        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter = trains.begin();
              trains.end() != trainIter; ++trainIter )
        {
            // create a new StoredMessageTask
            // store it in an IWorkItemPtr and then queue it using m_threadPool
            TA_Base_Core::IWorkItemPtr newTask(
                new StoredMessageTask( *this,
                                       *trainIter,
                                       origin,
                                       displayParameters,
                                       timestamp,
                                       sessionId ) );

            m_threadPool.queueWorkItem( newTask );
        }

        FUNCTION_EXIT;
    }


    void TimsTisManager::resetMessage( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
                                       const TA_IRS_Bus::TtisTypes::TtisResetParameters& displayParameters,
                                       time_t timestamp,
                                       const std::string& sessionId )
    {
        FUNCTION_ENTRY( "resetMessage" );

        // check that the PID list size is more than 0
        // if it isnt, throw an InvalidParameterException
        if ( true == displayParameters.pidList.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No PIDS given to display to", "PidList", "" ) );
        }

        // check that the train list size is more than 0
        // if it isnt, throw an InvalidParameterException
        if ( true == trains.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No trains given to reset", "TrainList", "" ) );
        }

        // check the session ID isnt empty
        // if it is, throw an InvalidParameterException
        if ( true == sessionId.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No operator session given", "SessionId", "" ) );
        }

        // check the timestamp is not 0
        // if it is, throw an InvalidParameterException
        if ( 0 == timestamp )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Invalid request timestamp", "timestamp", "0" ) );
        }

        // check the operator has the correct rights to reset a ttis message
        // if not, throw an AccessDeniedException
        if ( false == m_rightsChecker.canResetTtisMessage( sessionId ) )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not reset TTIS message" ) );
        }

        // get the operators origin
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        // for each train in the train list
        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter = trains.begin();
              trains.end() != trainIter; ++trainIter )
        {
            // create a new ResetMessageTask
            // store it in an IWorkItemPtr and then queue it using m_threadPool
            TA_Base_Core::IWorkItemPtr newTask(
                new ResetMessageTask( *this,
                                      *trainIter,
                                      origin,
                                      displayParameters,
                                      timestamp,
                                      sessionId ) );

            m_threadPool.queueWorkItem( newTask );
        }

        FUNCTION_EXIT;
    }


    void TimsTisManager::libraryDownload( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
                                          const TA_IRS_Bus::TtisTypes::LibraryDataType& libraryData,
                                          unsigned short libraryVersion,
                                          const std::string& sessionId )
    {
        FUNCTION_ENTRY( "libraryDownload" );

        // get the agent location type from m_configuration
        // if it is a station, throw an AccessDeniedException (operation is not supported at station)
        if ( TA_Base_Core::ILocation::STATION == m_configuration.getAgentLocationType() )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not download TTIS library at a station" ) );
        }

        // check that the train list size is more than 0
        // if it isnt, throw an InvalidParameterException
        if ( true == trains.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No trains given to download to", "TrainList", "" ) );
        }

        // check the download data isnt empty
        // if it is, throw an InvalidParameterException
        if ( true == libraryData.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No data to download", "LibraryData", "" ) );
        }

        // check the library version > 0
        // if it isnt, throw an InvalidParameterException
        if ( 0 == libraryVersion )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Invalid library version to download", "LibraryVersion", "0" ) );
        }

        // check the session ID isnt empty
        // if it is, throw an InvalidParameterException
        if ( true == sessionId.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No operator session given", "SessionId", "" ) );
        }

        // check the operator has the correct rights to download a ttis library
        // if not, throw an AccessDeniedException
        if ( false == m_rightsChecker.canDownloadTtisLibrary( sessionId ) )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not download TTIS library" ) );
        }

        // get the operators origin
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        // for each train in the train list
        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter = trains.begin();
              trains.end() != trainIter; ++trainIter )
        {
            // create a new LibraryDownloadTask
            // store it in an IWorkItemPtr and then queue it using m_threadPool
            TA_Base_Core::IWorkItemPtr newTask(
                new LibraryDownloadTask( *this,
                                         *trainIter,
                                         origin,
                                         libraryData,
                                         libraryVersion,
                                         sessionId ) );

            m_threadPool.queueWorkItem( newTask );
        }

        FUNCTION_EXIT;
    }


    void TimsTisManager::libraryUpgrade( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
                                         const std::string& sessionId )
    {
        FUNCTION_ENTRY( "libraryUpgrade" );

        // get the agent location type from m_configuration
        // if it is a station, throw an AccessDeniedException (operation is not supported at station)
        if ( TA_Base_Core::ILocation::STATION == m_configuration.getAgentLocationType() )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not upgrade TTIS library at a station" ) );
        }

        // check that the train list size is more than 0
        // if it isnt, throw an InvalidParameterException
        if ( true == trains.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No trains given to upgrade", "TrainList", "" ) );
        }

        // check the session ID isnt empty
        // if it is, throw an InvalidParameterException
        if ( true == sessionId.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No operator session given", "SessionId", "" ) );
        }

        // check the operator has the correct rights to upgrade a ttis library
        // if not, throw an AccessDeniedException
        if ( false == m_rightsChecker.canDownloadTtisLibrary( sessionId ) )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not upgrade TTIS library" ) );
        }

        // get the operators origin
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        // for each train in the train list
        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter = trains.begin();
              trains.end() != trainIter; ++trainIter )
        {
            // create a new LibraryUpgradeTask
            // store it in an IWorkItemPtr and then queue it using m_threadPool
            TA_Base_Core::IWorkItemPtr newTask(
                new LibraryUpgradeTask( *this,
                                        *trainIter,
                                        origin,
                                        sessionId ) );

            m_threadPool.queueWorkItem( newTask );
        }

        FUNCTION_EXIT;
    }


    void TimsTisManager::scheduleDownload( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
                                           const TA_IRS_Bus::TtisTypes::LibraryDataType& scheduleData,
                                           unsigned short scheduleVersion,
                                           const std::string& sessionId )
    {
        FUNCTION_ENTRY( "scheduleDownload" );

        // get the agent location type from m_configuration
        // if it is a station, throw an AccessDeniedException (operation is not supported at station)
        if ( TA_Base_Core::ILocation::STATION == m_configuration.getAgentLocationType() )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not download TTIS schedule at a station" ) );
        }

        // check that the train list size is more than 0
        // if it isnt, throw an InvalidParameterException
        if ( true == trains.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No trains given to download to", "TrainList", "" ) );
        }

        // check the download data isnt empty
        // if it is, throw an InvalidParameterException
        if ( true == scheduleData.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No data to download", "ScheduleData", "" ) );
        }

        // check the library version > 0
        // if it isnt, throw an InvalidParameterException
        if ( 0 == scheduleVersion )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Invalid library version to download", "ScheduleVersion", "0" ) );
        }

        // check the session ID isnt empty
        // if it is, throw an InvalidParameterException
        if ( true == sessionId.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No operator session given", "SessionId", "" ) );
        }

        // check the operator has the correct rights to download a ttis schedule
        // if not, throw an AccessDeniedException
        if ( false == m_rightsChecker.canDownloadTtisSchedule( sessionId ) )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not download TTIS schedule" ) );
        }

        // get the operators origin
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        // for each train in the train list
        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter = trains.begin();
              trains.end() != trainIter; ++trainIter )
        {
            // create a new ScheduleDownloadTask
            // store it in an IWorkItemPtr and then queue it using m_threadPool
            TA_Base_Core::IWorkItemPtr newTask(
                new ScheduleDownloadTask( *this,
                                          *trainIter,
                                          origin,
                                          scheduleData,
                                          scheduleVersion,
                                          sessionId ) );

            m_threadPool.queueWorkItem( newTask );
        }

        FUNCTION_EXIT;
    }


    void TimsTisManager::scheduleUpgrade( const TA_IRS_Bus::CommonTypes::TrainIdList& trains,
                                          const std::string& sessionId )
    {
        FUNCTION_ENTRY( "scheduleUpgrade" );

        // get the agent location type from m_configuration
        // if it is a station, throw an AccessDeniedException (operation is not supported at station)
        if ( TA_Base_Core::ILocation::STATION == m_configuration.getAgentLocationType() )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not upgrade TTIS schedule at a station" ) );
        }

        // check that the train list size is more than 0
        // if it isnt, throw an InvalidParameterException
        if ( true == trains.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No trains given to upgrade", "TrainList", "" ) );
        }

        // check the session ID isnt empty
        // if it is, throw an InvalidParameterException
        if ( true == sessionId.empty() )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "No operator session given", "SessionId", "" ) );
        }

        // check the operator has the correct rights to upgrade a ttis schedule
        // if not, throw an AccessDeniedException
        if ( false == m_rightsChecker.canDownloadTtisSchedule( sessionId ) )
        {
            TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not upgrade TTIS schedule" ) );
        }

        // get the operators origin
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        // for each train in the train list
        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter = trains.begin();
              trains.end() != trainIter; ++trainIter )
        {
            // create a new ScheduleUpgradeTask
            // store it in an IWorkItemPtr and then queue it using m_threadPool
            TA_Base_Core::IWorkItemPtr newTask(
                new ScheduleUpgradeTask( *this,
                                         *trainIter,
                                         origin,
                                         sessionId ) );

            m_threadPool.queueWorkItem( newTask );
        }

        FUNCTION_EXIT;
    }


    void TimsTisManager::handleTrainTimeout( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "handleTrainTimeout" );

        // This handles a Tis 2 timer expiration
        // a transfer has completed successfully with no response from the train
        // the download that was in progress must now be considered failed
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the current timestamp
        time_t timestamp = time( NULL );

        // get the train tis state for the train
        TrainTisStatePtr tisState = m_statusManager.getTrainTisState( trainId );

        TA_IRS_Bus::TtisTypes::ETtisDownloadType currentDownloadType( TA_IRS_Bus::TtisTypes::TtisNone );
        unsigned short currentDownloadVersion = 0;
        std::string transferTsi( "" );
        std::string sessionId( "" );

        {
            // acquire the train tis state lock
            TA_IRS_Bus::TrainStateLock stateLock( tisState );

            // get the download details
            currentDownloadType = tisState->getCurrentDownloadType();
            currentDownloadVersion = tisState->getCurrentDownloadVersion();
            transferTsi = tisState->getTransferTsi();
            sessionId = tisState->getSessionId();

            // clearDownload
            tisState->clearDownload( timestamp );

            // release the train tis state lock
        }

        // close the transfer channel in case its still open
        if ( false == transferTsi.empty() )
        {
            if ( true == sessionId.empty() )
            {
                sessionId = m_configuration.getAgentSessionId();
            }

            m_radioAgent.cancelDataCall( transferTsi, sessionId );
        }

        // if the current download is not none
        if ( TA_IRS_Bus::TtisTypes::TtisNone != currentDownloadType )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "TIS 2 Timeout [trainId: %d][currentDownloadType: %s][currentDownloadVersion: %d][transferTsi: %s][sessionId: %s]",
                         trainId, TA_IRS_Bus::TtisTypes::ttisDownloadTypeToString( currentDownloadType ).c_str(),
                         currentDownloadVersion, transferTsi.c_str(), sessionId.c_str() );

            // Create a TisLibraryUpdate
            TA_IRS_Bus::TtisTypes::TisLibraryUpdate update;
            update.trainId = trainId;
            update.version = currentDownloadVersion;
            update.updateType = ( TA_IRS_Bus::TtisTypes::TtisLibrary == currentDownloadType ) ?
                                TA_IRS_Bus::TtisTypes::LibraryDownloadComplete : TA_IRS_Bus::TtisTypes::ScheduleDownloadComplete;
            update.commandStatus = TA_IRS_Bus::TtisTypes::TtisFailed;
            update.downloadResult = TA_IRS_Bus::TtisTypes::TimedOut;

            // send the TisLibraryUpdate
            m_messageSender.sendClientUpdate( update );
        }

        FUNCTION_EXIT;
    }


    void TimsTisManager::processDataCallEndEvent( const TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                  bool success,
                                                  const std::string& details )
    {
        FUNCTION_ENTRY( "processDataCallEndEvent" );

        // This handles the end of a transfer to a train
        // It simply indicates the transfer has finished
        // if successful we must now wait for the result from TIMS
        // if unsuccessful we consider the download failed

        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the train tis state for the train
        TrainTisStatePtr tisState = m_statusManager.getTrainTisState( trainId );

        TA_IRS_Bus::TtisTypes::ETtisDownloadType currentDownloadType( TA_IRS_Bus::TtisTypes::TtisNone );
        unsigned short currentDownloadVersion = 0;
        std::string transferTsi( "" );
        std::string sessionId( "" );

        {
            // acquire the train tis state lock
            TA_IRS_Bus::TrainStateLock stateLock( tisState );

            // get the download details
            currentDownloadType = tisState->getCurrentDownloadType();
            currentDownloadVersion = tisState->getCurrentDownloadVersion();
            transferTsi = tisState->getTransferTsi();
            sessionId = tisState->getSessionId();

            // if the transfer was not successful, clear it

            if ( false == success )
            {
                // get the current timestamp
                time_t timestamp = time( NULL );

                // clearDownload
                tisState->clearDownload( timestamp );
            }

            // note do not set transferComplete,
            // as we are now responsible for closing down the transfer

            // release the train tis state lock
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,

                     "Transfer complete for [trainId: %d][currentDownloadType: %s][currentDownloadVersion: %d]"
                     "[transferTsi: %s][sessionId: %s][success: %s][details: %s]",
                     trainId, TA_IRS_Bus::TtisTypes::ttisDownloadTypeToString( currentDownloadType ).c_str(),
                     currentDownloadVersion, transferTsi.c_str(), sessionId.c_str(),
                     ( true == success ) ? "Succeeded" : "Failed", details.c_str() );

        // if the current download is not none
        if ( TA_IRS_Bus::TtisTypes::TtisNone != currentDownloadType )
        {
            // if the transfer was successful
            if ( true == success )
            {
                startTis2Timer( trainId );
            }

            // otherwise if it was unsuccessful
            else
            {
                TA_IRS_Bus::TtisTypes::TisLibraryUpdate update;
                update.trainId = trainId;
                update.version = currentDownloadVersion;
                update.updateType = ( TA_IRS_Bus::TtisTypes::TtisLibrary == currentDownloadType ) ?
                                    TA_IRS_Bus::TtisTypes::LibraryDownloadComplete : TA_IRS_Bus::TtisTypes::ScheduleDownloadComplete;
                update.commandStatus = TA_IRS_Bus::TtisTypes::TtisFailed;
                update.downloadResult = TA_IRS_Bus::TtisTypes::TransferFailed;

                // send the TisLibraryUpdate
                m_messageSender.sendClientUpdate( update );
            }
        }


    }


    void TimsTisManager::processEvent( TA_IRS_Bus::TrainEventPtr event )
    {
        FUNCTION_ENTRY( "processEvent" );

        // if not in control state return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the event type, based on its type, downcast to the correct type and call the appropriate handler function:
        TA_IRS_Bus::TrainMessageType messageType = event->getEventType();

        if ( messageType == TA_IRS_Bus::EndOfTtisDownloadEvent::getStaticType() )
        {
            TA_IRS_Bus::EndOfTtisDownloadEventPtr endOfTtisDownloadEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::EndOfTtisDownloadEvent >( event );

            TA_ASSERT( NULL != endOfTtisDownloadEvent.get(), "cast failed" );

            processEndOfTtisDownloadEvent( endOfTtisDownloadEvent );
        }
        else if ( messageType == TA_IRS_Bus::ChangeoverStatusEvent::getStaticType() )
        {
            TA_IRS_Bus::ChangeoverStatusEventPtr changeoverStatusEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::ChangeoverStatusEvent >( event );

            TA_ASSERT( NULL != changeoverStatusEvent.get(), "cast failed" );

            processChangeoverStatusEvent( changeoverStatusEvent );
        }
        else
        {
            TA_ASSERT( false, "The event type received was not subscribed for" );
        }


    }


    void TimsTisManager::displayFreeTextMessage( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                 unsigned char origin,
                                                 const TA_IRS_Bus::TtisTypes::TtisFreeTextDisplayParameters& displayParameters,
                                                 time_t timestamp,
                                                 const std::string& sessionId )
    {
        FUNCTION_ENTRY( "displayFreeTextMessage" );

        // This simply sends a message to the train to display a free text message
        // the train will respond with a response or an error
        // the result of this command then simply needs to be sent to the client
        // This may spawn multiple commands to the train, depending on the PIDS selected
        // This command will abort when the first failure is encountered

        genericDisplayRequest( trainId,
                               origin,
                               displayParameters,
                               timestamp,
                               sessionId,
                               "TTIS Free Text" );

        FUNCTION_EXIT;
    }


    void TimsTisManager::displayStoredMessage( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                               unsigned char origin,
                                               const TA_IRS_Bus::TtisTypes::TtisPredefinedDisplayParameters& displayParameters,
                                               time_t timestamp,
                                               const std::string& sessionId )
    {
        FUNCTION_ENTRY( "displayStoredMessage" );

        // This simply sends a message to the train to display a predefined text message
        // the train will respond with a response or an error
        // the result of this command then simply needs to be sent to the client
        // This may spawn multiple commands to the train, depending on the PIDS selected
        // This command will abort when the first failure is encountered

        genericDisplayRequest( trainId,
                               origin,
                               displayParameters,
                               timestamp,
                               sessionId,
                               "TTIS Predefined" );

        FUNCTION_EXIT;
    }


    void TimsTisManager::resetMessage( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                       unsigned char origin,
                                       const TA_IRS_Bus::TtisTypes::TtisResetParameters& displayParameters,
                                       time_t timestamp,
                                       const std::string& sessionId )
    {
        FUNCTION_ENTRY( "resetMessage" );

        // This simply sends a message to the train to clear any displayed messages
        // the train will respond with a response or an error
        // the result of this command then simply needs to be sent to the client
        // This may spawn multiple commands to the train, depending on the PIDS selected
        // This command will abort when the first failure is encountered


        genericDisplayRequest( trainId,
                               origin,
                               displayParameters,
                               timestamp,
                               sessionId,
                               "TTIS Reset" );

        FUNCTION_EXIT;
    }


    template <typename DisplayParametersType>
    void TimsTisManager::genericDisplayRequest( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                unsigned char origin,
                                                const DisplayParametersType& displayParameters,
                                                time_t timestamp,
                                                const std::string& sessionId,
                                                const std::string& operationDescription )
    {
        FUNCTION_ENTRY( "genericDisplayRequest" );

        TA_IRS_Bus::TtisTypes::TisReturnStatus tisReturnStatus;
        tisReturnStatus.trainId = trainId;
        tisReturnStatus.sessionId = sessionId;
        tisReturnStatus.timestamp = timestamp;
        tisReturnStatus.originalParameters = displayParameters;
        tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisSuccess;

        try
        {
            // first check this agent is in control of this train
            performPreDisplayTrainChecks( trainId );

            // get the train tis state for this train
            TrainTisStatePtr tisState = m_statusManager.getTrainTisState( trainId );

            bool success = true;

            // a separate command needs to be sent for each named PID (or PID group)
            // for each pid in displayParameters.pidList, and while success is still true
            for ( TA_IRS_Bus::TtisTypes::TtisPidList::const_iterator pidIter = displayParameters.pidList.begin();
                  ( true == success ) && ( displayParameters.pidList.end() != pidIter ); ++pidIter )
            {
                // call the overloaded method to create the correct command for these parameters
                TA_IRS_Bus::AbstractTtisCommandPtr command(
                    createCommand( trainId, origin, displayParameters, *pidIter ) );

                {
                    // get the command lock and acquire it using a threadguard
                    // for some reason it doesnt work all in one line
                    TA_Base_Core::IThreadLockable& commandLock = tisState->getCommandLock();
                    TA_THREADGUARD( commandLock );

                    // send the command
                    m_transactionManager.sendCommand( command,
                                                      m_configuration.getTimerTIS1(),
                                                      m_configuration.getTimsRetryLimit() );

                    // release the command lock
                }

                // get the CommandReceivedResponse, the bad command response, and the changeover status response from the command
                // if none of these are returned, ASSERT - the command should not exit normally without a valid resposne
                TA_IRS_Bus::TtisCommandReceivedEventPtr commandReceived = command->getCommandReceivedResponse();
                TA_IRS_Bus::BadCommandEventPtr badCommand = command->getBadCommandResponse();
                TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = command->getChangeoverStatusEventResponse();

                if ( commandReceived )
                {
                    // get the result of the command
                    TA_IRS_Bus::ProtocolTtisTypes::ETtisResult result = commandReceived->getResult();

                    // based on the result set the appropriate status
                    // again, call on the overloaded method to handle it for this command type
                    success = handleResult( displayParameters, result, tisReturnStatus );

                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                 "%s command for train %d returned a command received with status: %s",
                                 operationDescription.c_str(), trainId,
                                 TA_IRS_Bus::ProtocolTtisTypes::ttisResultToString( result ).c_str() );
                }

                // if the bad command event is returned
                else if ( badCommand )
                {
                    TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError errorCode = badCommand->getErrorCode();

                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                                 "%s command for train %d returned an error: %s",
                                 operationDescription.c_str(), trainId,
                                 TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode ).c_str() );

                    // set TisReturnStatus.result to one of the BadCommand* error codes
                    switch ( errorCode )
                    {

                        case TA_IRS_Bus::ProtocolCommonTypes::BadChecksum:
                            tisReturnStatus.result = TA_IRS_Bus::TtisTypes::BadCommandBadChecksum;
                            break;

                        case TA_IRS_Bus::ProtocolCommonTypes::UnknownCommand:
                            tisReturnStatus.result = TA_IRS_Bus::TtisTypes::BadCommandUnknownCommand;
                            break;

                        case TA_IRS_Bus::ProtocolCommonTypes::BadParameter:
                            tisReturnStatus.result = TA_IRS_Bus::TtisTypes::BadCommandBadParameter;
                            break;

                        case TA_IRS_Bus::ProtocolCommonTypes::UnexpectedCommand:
                            tisReturnStatus.result = TA_IRS_Bus::TtisTypes::BadCommandUnexpectedCommand;
                            break;

                        default:
                            TA_ASSERT( false, "This value must be validated by the protocol library" );
                            break;
                    }

                    success = false;
                }

                // if the mpu changeover event is returned
                else if ( mpuChangeover )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                                 "%s command for train %d returned an MPU changeover",
                                 operationDescription.c_str(), trainId );

                    tisReturnStatus.result = TA_IRS_Bus::TtisTypes::MpuChangeoverError;

                    success = false;
                }
                else
                {
                    // if no valid events were returned,
                    // assert that the command completed without a valid response
                    TA_ASSERT( false, "This command can only exit via exception or a valid response" );
                }
            }
        }

        // catch any of the following exceptions, log the exception, and set the appropriate TisReturnStatus.result
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );

            // set the error status
            tisReturnStatus.result = TA_IRS_Bus::TtisTypes::InvalidTrainError;
        }
        catch ( TA_IRS_Bus::TrainAccessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );

            // set the error status
            tisReturnStatus.result = TA_IRS_Bus::TtisTypes::InvalidTrainError;
        }
        catch ( TA_IRS_Bus::TrainTransmissionFailureException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );

            // set the error status
            tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TransmissionFailureError;
        }
        catch ( TA_IRS_Bus::TrainTimeoutException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimeoutException", e.what() );

            // set the error status
            tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TransactionTimeoutError;
        }
        catch ( TA_IRS_Bus::TransactionCancelledException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactionCancelledException", e.what() );

            // set the error status
            tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TransactionCancelledError;
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_ASSERT( false, "This exception should not leak" );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "" );

            TA_ASSERT( false, "This exception should not leak" );
        }

        // send the result
        m_messageSender.sendClientUpdate( tisReturnStatus );

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::AbstractTtisCommandPtr TimsTisManager::createCommand( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                                      unsigned char origin,
                                                                      const TA_IRS_Bus::TtisTypes::TtisFreeTextDisplayParameters& displayParameters,
                                                                      TA_IRS_Bus::ProtocolTtisTypes::ETtisPid pid ) const
    {
        FUNCTION_ENTRY( "createCommand( TtisFreeTextDisplayParameters )" );

        TA_IRS_Bus::ProtocolTtisTypes::TtisTime startTime;
        startTime.hour = displayParameters.displayPeriod.startHour;
        startTime.minute = displayParameters.displayPeriod.startMin;

        TA_IRS_Bus::ProtocolTtisTypes::TtisTime stopTime;
        stopTime.hour = displayParameters.displayPeriod.stopHour;
        stopTime.minute = displayParameters.displayPeriod.stopMin;

        TA_IRS_Bus::AbstractTtisCommandPtr command(
            new TA_IRS_Bus::TtisFreeTextMessageCommand( trainId,
                                                        origin,
                                                        pid,
                                                        displayParameters.fontSize,
                                                        displayParameters.justification,
                                                        displayParameters.intensity,
                                                        displayParameters.displayMode,
                                                        displayParameters.priority,
                                                        startTime,
                                                        stopTime,
                                                        displayParameters.repeatInterval,
                                                        displayParameters.message ) );

        FUNCTION_EXIT;
        return command;
    }


    TA_IRS_Bus::AbstractTtisCommandPtr TimsTisManager::createCommand( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                                      unsigned char origin,
                                                                      const TA_IRS_Bus::TtisTypes::TtisPredefinedDisplayParameters& displayParameters,
                                                                      TA_IRS_Bus::ProtocolTtisTypes::ETtisPid pid ) const
    {
        FUNCTION_ENTRY( "createCommand( TtisPredefinedDisplayParameters )" );

        TA_IRS_Bus::ProtocolTtisTypes::TtisTime startTime;
        startTime.hour = displayParameters.displayPeriod.startHour;
        startTime.minute = displayParameters.displayPeriod.startMin;

        TA_IRS_Bus::ProtocolTtisTypes::TtisTime stopTime;
        stopTime.hour = displayParameters.displayPeriod.stopHour;
        stopTime.minute = displayParameters.displayPeriod.stopMin;

        TA_IRS_Bus::AbstractTtisCommandPtr command(
            new TA_IRS_Bus::TtisPreStoredMessageCommand( trainId,
                                                         origin,
                                                         pid,
                                                         displayParameters.libraryVersion,
                                                         displayParameters.identifierType,
                                                         displayParameters.identifierOffset,
                                                         displayParameters.priority,
                                                         startTime,
                                                         stopTime ) );

        FUNCTION_EXIT;
        return command;
    }


    TA_IRS_Bus::AbstractTtisCommandPtr TimsTisManager::createCommand( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                                      unsigned char origin,
                                                                      const TA_IRS_Bus::TtisTypes::TtisResetParameters& displayParameters,
                                                                      TA_IRS_Bus::ProtocolTtisTypes::ETtisPid pid ) const
    {
        FUNCTION_ENTRY( "createCommand( TtisResetParameters )" );

        TA_IRS_Bus::AbstractTtisCommandPtr command(
            new TA_IRS_Bus::TtisResetCommand( trainId,
                                              origin,
                                              pid,
                                              displayParameters.messageType ) );

        FUNCTION_EXIT;
        return command;
    }


    bool TimsTisManager::handleResult( const TA_IRS_Bus::TtisTypes::TtisFreeTextDisplayParameters& displayParameters,
                                       TA_IRS_Bus::ProtocolTtisTypes::ETtisResult result,
                                       TA_IRS_Bus::TtisTypes::TisReturnStatus& tisReturnStatus ) const
    {
        FUNCTION_ENTRY( "handleResult( TtisFreeTextDisplayParameters )" );

        bool success = false;

        switch ( result )
        {

            case TA_IRS_Bus::ProtocolTtisTypes::TtisProceeding:
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisSuccess;
                success = true;
                break;

            case TA_IRS_Bus::ProtocolTtisTypes::TtisInUse:
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisInUse;
                success = false;
                break;

            case TA_IRS_Bus::ProtocolTtisTypes::TtisLibraryUpgraded:
                // this is not a valid response for this command
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisFailed;
                success = false;
                break;

            case TA_IRS_Bus::ProtocolTtisTypes::TtisReset:
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisReset;
                success = false;
                break;

            case TA_IRS_Bus::ProtocolTtisTypes::TtisFailed:
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisFailed;
                success = false;
                break;

            case TA_IRS_Bus::ProtocolTtisTypes::TtisDifferentVersion:
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisDifferentVersion;
                success = false;
                break;

            default:
                TA_ASSERT( false, "This value must be validated by the protocol library" );
                break;
        }

        FUNCTION_EXIT;

        return success;
    }


    bool TimsTisManager::handleResult( const TA_IRS_Bus::TtisTypes::TtisPredefinedDisplayParameters& displayParameters,
                                       TA_IRS_Bus::ProtocolTtisTypes::ETtisResult result,
                                       TA_IRS_Bus::TtisTypes::TisReturnStatus& tisReturnStatus ) const
    {
        FUNCTION_ENTRY( "handleResult( TtisPredefinedDisplayParameters )" );

        bool success = false;

        switch ( result )
        {

            case TA_IRS_Bus::ProtocolTtisTypes::TtisProceeding:
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisSuccess;
                success = true;
                break;

            case TA_IRS_Bus::ProtocolTtisTypes::TtisInUse:
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisInUse;
                success = false;
                break;

            case TA_IRS_Bus::ProtocolTtisTypes::TtisLibraryUpgraded:
                // this is not a valid response for this command
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisFailed;
                success = false;
                break;

            case TA_IRS_Bus::ProtocolTtisTypes::TtisReset:
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisReset;
                success = false;
                break;

            case TA_IRS_Bus::ProtocolTtisTypes::TtisFailed:
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisFailed;
                success = false;
                break;

            case TA_IRS_Bus::ProtocolTtisTypes::TtisDifferentVersion:
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisDifferentVersion;
                success = false;
                break;

            default:
                TA_ASSERT( false, "This value must be validated by the protocol library" );
                break;
        }

        FUNCTION_EXIT;

        return success;
    }


    bool TimsTisManager::handleResult( const TA_IRS_Bus::TtisTypes::TtisResetParameters& displayParameters,
                                       TA_IRS_Bus::ProtocolTtisTypes::ETtisResult result,
                                       TA_IRS_Bus::TtisTypes::TisReturnStatus& tisReturnStatus ) const
    {
        FUNCTION_ENTRY( "handleResult( TtisResetParameters )" );

        bool success = false;

        switch ( result )
        {

            case TA_IRS_Bus::ProtocolTtisTypes::TtisProceeding:
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisSuccess;
                success = true;
                break;

            case TA_IRS_Bus::ProtocolTtisTypes::TtisInUse:
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisInUse;
                success = false;
                break;

            case TA_IRS_Bus::ProtocolTtisTypes::TtisLibraryUpgraded:
                // this is not a valid response for this command
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisFailed;
                success = false;
                break;

            case TA_IRS_Bus::ProtocolTtisTypes::TtisReset:
                // consider this success for a reset command
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisSuccess;
                success = true;
                break;

            case TA_IRS_Bus::ProtocolTtisTypes::TtisFailed:
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisFailed;
                success = false;
                break;

            case TA_IRS_Bus::ProtocolTtisTypes::TtisDifferentVersion:
                tisReturnStatus.result = TA_IRS_Bus::TtisTypes::TtisDifferentVersion;
                success = false;
                break;

            default:
                TA_ASSERT( false, "This value must be validated by the protocol library" );
                break;
        }

        FUNCTION_EXIT;

        return success;
    }


    void TimsTisManager::libraryDownload( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                          unsigned char origin,
                                          const TA_IRS_Bus::TtisTypes::LibraryDataType& libraryData,
                                          unsigned short libraryVersion,
                                          const std::string& sessionId )
    {
        FUNCTION_ENTRY( "libraryDownload" );

        // This sends a message to the train to request to start a library download
        // the train will respond with a response or an error
        // if the train accepts, then an actual data transfer is started
        // the result of this command then simply needs to be sent to the client

        TA_IRS_Bus::TtisLibraryDownloadCommandPtr command(
            new TA_IRS_Bus::TtisLibraryDownloadCommand( trainId, origin ) );


        genericDownload( trainId,
                         origin,
                         libraryData,
                         libraryVersion,
                         sessionId,
                         TA_IRS_Bus::TtisTypes::TtisLibrary,
                         TA_IRS_Bus::TtisTypes::LibraryDownloadStart,
                         TA_IRS_Bus::TtdFrame::LibraryData,
                         command,
                         "Library Download" );

        FUNCTION_EXIT;
    }


    void TimsTisManager::libraryUpgrade( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                         unsigned char origin,
                                         const std::string& sessionId )
    {
        FUNCTION_ENTRY( "libraryUpgrade" );

        // This sends a message to the train to request that a previously downloaded library be upgraded
        // the train will respond with a response or an error
        // the result of this command then simply needs to be sent to the client

        TA_IRS_Bus::TtisLibraryUpgradeCommandPtr command(
            new TA_IRS_Bus::TtisLibraryUpgradeCommand( trainId, origin ) );

        // get the train comms state for the train
        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );
        unsigned short currentVersion;
        {
            // acquire the lock for the train state
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // get the ttis library version
            currentVersion = commsState->getTtisLibraryVersion();

            // release the train state lock
        }

        genericUpgrade( trainId,
                        sessionId,
                        TA_IRS_Bus::TtisTypes::TtisLibrary,
                        TA_IRS_Bus::TtisTypes::LibraryUpgradeResult,
                        currentVersion,
                        command,
                        "Library Upgrade" );

        FUNCTION_EXIT;
    }


    void TimsTisManager::scheduleDownload( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                           unsigned char origin,
                                           const TA_IRS_Bus::TtisTypes::LibraryDataType& scheduleData,
                                           unsigned short scheduleVersion,
                                           const std::string& sessionId )
    {
        FUNCTION_ENTRY( "scheduleDownload" );

        // This sends a message to the train to request to start a schedule download
        // the train will respond with a response or an error
        // if the train accepts, then an actual data transfer is started
        // the result of this command then simply needs to be sent to the client

        TA_IRS_Bus::TtisScheduleDownloadCommandPtr command(
            new TA_IRS_Bus::TtisScheduleDownloadCommand( trainId, origin ) );

        genericDownload( trainId,
                         origin,
                         scheduleData,
                         scheduleVersion,
                         sessionId,
                         TA_IRS_Bus::TtisTypes::TtisSchedule,
                         TA_IRS_Bus::TtisTypes::ScheduleDownloadStart,
                         TA_IRS_Bus::TtdFrame::ScheduleData,
                         command,
                         "Schedule Download" );

        FUNCTION_EXIT;
    }


    void TimsTisManager::scheduleUpgrade( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                          unsigned char origin,
                                          const std::string& sessionId )
    {
        FUNCTION_ENTRY( "scheduleUpgrade" );

        // This sends a message to the train to request that a previously downloaded schedule be upgraded
        // the train will respond with a response or an error
        // the result of this command then simply needs to be sent to the client

        TA_IRS_Bus::TtisScheduleUpgradeCommandPtr command(
            new TA_IRS_Bus::TtisScheduleUpgradeCommand( trainId, origin ) );

        // get the train comms state for the train
        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );
        unsigned short currentVersion;
        {
            // acquire the lock for the train state
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // get the ttis schedule version
            currentVersion = commsState->getTtisScheduleVersion();

            // release the train state lock
        }

        genericUpgrade( trainId,
                        sessionId,
                        TA_IRS_Bus::TtisTypes::TtisSchedule,
                        TA_IRS_Bus::TtisTypes::ScheduleUpgradeResult,
                        currentVersion,
                        command,
                        "Schedule Upgrade" );

        FUNCTION_EXIT;
    }


    template <typename CommandTypePtr>
    void TimsTisManager::genericDownload( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                          unsigned char origin,
                                          const TA_IRS_Bus::TtisTypes::LibraryDataType& data,
                                          unsigned short version,
                                          const std::string& sessionId,
                                          TA_IRS_Bus::TtisTypes::ETtisDownloadType downloadType,
                                          TA_IRS_Bus::TtisTypes::ETisUpdateType updateType,
                                          TA_IRS_Bus::TtdFrame::EDownloadDataType dataType,
                                          CommandTypePtr command,
                                          const std::string& operationDescription )
    {
        FUNCTION_ENTRY( "genericDownload" );

        TA_ASSERT( ( TA_IRS_Bus::TtisTypes::TtisLibrary == downloadType ) ||
                   ( TA_IRS_Bus::TtisTypes::TtisSchedule == downloadType ),
                   "Invalid download type" );

        // Create a TisLibraryUpdate
        TA_IRS_Bus::TtisTypes::TisLibraryUpdate libraryUpdate;
        libraryUpdate.trainId = trainId;
        libraryUpdate.version = version;
        libraryUpdate.updateType = updateType;
        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TtisSuccess;
        libraryUpdate.downloadResult = TA_IRS_Bus::TtisTypes::Success;

        bool success = true;
        time_t timestamp = 0;

        // get the train tis state for this train
        TrainTisStatePtr tisState = m_statusManager.getTrainTisState( trainId );

        try
        {
            // first check this agent is in control of this train
            performPreDisplayTrainChecks( trainId );

            {
                // get the command lock and acquire it using a threadguard
                // for some reason it doesnt work all in one line
                TA_Base_Core::IThreadLockable& commandLock = tisState->getCommandLock();
                TA_THREADGUARD( commandLock );

                // send the command
                m_transactionManager.sendCommand( command,
                                                  m_configuration.getTimerTIS1(),
                                                  m_configuration.getTimsRetryLimit() );

                // release the command lock
            }

            // get the CommandReceivedResponse, the bad command response, and the changeover status response from the command
            // if none of these are returned, ASSERT - the command should not exit normally without a valid resposne
            TA_IRS_Bus::TtisCommandReceivedEventPtr commandReceived = command->getCommandReceivedResponse();
            TA_IRS_Bus::BadCommandEventPtr badCommand = command->getBadCommandResponse();
            TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = command->getChangeoverStatusEventResponse();

            if ( commandReceived )
            {
                // get the result of the command
                TA_IRS_Bus::ProtocolTtisTypes::ETtisResult result = commandReceived->getResult();

                switch ( result )
                {

                    case TA_IRS_Bus::ProtocolTtisTypes::TtisProceeding:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TtisSuccess;
                        success = true;
                        break;

                    case TA_IRS_Bus::ProtocolTtisTypes::TtisInUse:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TtisInUse;
                        success = false;
                        break;

                    case TA_IRS_Bus::ProtocolTtisTypes::TtisLibraryUpgraded:
                        // this is not a valid response for this command
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TtisFailed;
                        success = false;
                        break;

                    case TA_IRS_Bus::ProtocolTtisTypes::TtisReset:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TtisReset;
                        success = false;
                        break;

                    case TA_IRS_Bus::ProtocolTtisTypes::TtisFailed:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TtisFailed;
                        success = false;
                        break;

                    case TA_IRS_Bus::ProtocolTtisTypes::TtisDifferentVersion:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TtisDifferentVersion;
                        success = false;
                        break;

                    default:
                        TA_ASSERT( false, "This value must be validated by the protocol library" );
                        break;
                }

                // get the timestamp from the event
                timestamp = commandReceived->getTime();

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "%s command for train %d returned a command received with status: %s",
                             operationDescription.c_str(), trainId,
                             TA_IRS_Bus::ProtocolTtisTypes::ttisResultToString( result ).c_str() );
            }

            // if the bad command event is returned
            else if ( badCommand )
            {
                TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError errorCode = badCommand->getErrorCode();

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "%s command for train %d returned an error: %s",
                             operationDescription.c_str(), trainId,
                             TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode ).c_str() );

                // set TisReturnStatus.result to one of the BadCommand* error codes
                switch ( errorCode )
                {

                    case TA_IRS_Bus::ProtocolCommonTypes::BadChecksum:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::BadCommandBadChecksum;
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::UnknownCommand:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::BadCommandUnknownCommand;
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::BadParameter:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::BadCommandBadParameter;
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::UnexpectedCommand:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::BadCommandUnexpectedCommand;
                        break;

                    default:
                        TA_ASSERT( false, "This value must be validated by the protocol library" );
                        break;
                }

                success = false;
            }

            // if the mpu changeover event is returned
            else if ( mpuChangeover )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "%s command for train %d returned an MPU changeover",
                             operationDescription.c_str(), trainId );

                libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::MpuChangeoverError;

                success = false;
            }
            else
            {
                // if no valid events were returned,
                // assert that the command completed without a valid response
                TA_ASSERT( false, "This command can only exit via exception or a valid response" );
            }

        }
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );

            // set the error status
            libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::InvalidTrainError;
            success = false;
        }
        catch ( TA_IRS_Bus::TrainAccessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );

            // set the error status
            libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::InvalidTrainError;
            success = false;
        }
        catch ( TA_IRS_Bus::TrainTransmissionFailureException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );

            // set the error status
            libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TransmissionFailureError;
            success = false;
        }
        catch ( TA_IRS_Bus::TrainTimeoutException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimeoutException", e.what() );

            // set the error status
            libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TransactionTimeoutError;
            success = false;
        }
        catch ( TA_IRS_Bus::TransactionCancelledException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactionCancelledException", e.what() );

            // set the error status
            libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TransactionCancelledError;
            success = false;
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_ASSERT( false, "This exception should not leak" );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "" );

            TA_ASSERT( false, "This exception should not leak" );
        }

        // if the train accepted the transfer request , the transfer must be started
        if ( true == success )
        {
            try
            {
                // first the data must be framed for the train
                TA_IRS_Bus::TtisTypes::TtisFrameList frames = TA_IRS_Bus::TtdFrame::frameData( data,
                                                                                               trainId,
                                                                                               dataType,
                                                                                               origin );

                // now package the framed data into data to send
                TA_IRS_Bus::TrainMessageData dataToSend;

                for ( TA_IRS_Bus::TtisTypes::TtisFrameList::iterator frameIter = frames.begin();
                      frames.end() != frameIter; ++frameIter )
                {
                    // append its frameData to dataToSend
                    dataToSend.insert( dataToSend.end(),
                                       frameIter->frameData.begin(),
                                       frameIter->frameData.end() );
                }

                // second the transfer must be started
                // get the train comms state for the train
                std::string transferTsi;

                TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

                {
                    // acquire the lock for the train state
                    TA_IRS_Bus::TrainStateLock stateLock( commsState );

                    transferTsi = commsState->getActiveTsi();

                    // release the train state lock
                }

                // stop any running download timers
                stopTis2Timer( trainId );

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "Sending %d frames to train %d",
                             frames.size(), trainId );

                // start the transfer using the radio agent
                m_radioAgent.makeDataCall( transferTsi,
                                           dataToSend,
                                           sessionId );

                // acquire the tis state lock
                TA_IRS_Bus::TrainStateLock tisStateLock( tisState );

                // set the download details in the state
                tisState->setDownload( sessionId,
                                       downloadType,
                                       version,
                                       frames,
                                       transferTsi,
                                       timestamp );

                // release the tis state lock
            }
            catch ( TA_IRS_Bus::TrainRadioException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TrainRadioException", e.what() );

                libraryUpdate.downloadResult = TA_IRS_Bus::TtisTypes::TransferFailed;
            }
        }


        // send the result
        m_messageSender.sendClientUpdate( libraryUpdate );

        FUNCTION_EXIT;
    }


    template <typename CommandTypePtr>
    void TimsTisManager::genericUpgrade( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                         const std::string& sessionId,
                                         TA_IRS_Bus::TtisTypes::ETtisDownloadType upgradeType,
                                         TA_IRS_Bus::TtisTypes::ETisUpdateType updateType,
                                         unsigned short currentVersion,
                                         CommandTypePtr command,
                                         const std::string& operationDescription )
    {
        FUNCTION_ENTRY( "genericUpgrade" );

        TA_ASSERT( ( TA_IRS_Bus::TtisTypes::TtisLibrary == upgradeType ) ||
                   ( TA_IRS_Bus::TtisTypes::TtisSchedule == upgradeType ),
                   "Invalid upgrade type" );

        // Create a TisLibraryUpdate
        TA_IRS_Bus::TtisTypes::TisLibraryUpdate libraryUpdate;
        libraryUpdate.trainId = trainId;
        libraryUpdate.version = currentVersion;
        libraryUpdate.updateType = updateType;
        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TtisSuccess;
        libraryUpdate.downloadResult = TA_IRS_Bus::TtisTypes::Success;

        try
        {
            // first check this agent is in control of this train
            performPreDisplayTrainChecks( trainId );

            // get the train tis state for this train
            TrainTisStatePtr tisState = m_statusManager.getTrainTisState( trainId );

            {
                // get the command lock and acquire it using a threadguard
                // for some reason it doesnt work all in one line
                TA_Base_Core::IThreadLockable& commandLock = tisState->getCommandLock();
                TA_THREADGUARD( commandLock );

                // send the command
                m_transactionManager.sendCommand( command,
                                                  m_configuration.getTimerTIS1(),
                                                  m_configuration.getTimsRetryLimit() );

                // release the command lock
            }

            // get the CommandReceivedResponse, the bad command response, and the changeover status response from the command
            // if none of these are returned, ASSERT - the command should not exit normally without a valid resposne
            TA_IRS_Bus::TtisCommandReceivedEventPtr commandReceived = command->getCommandReceivedResponse();
            TA_IRS_Bus::BadCommandEventPtr badCommand = command->getBadCommandResponse();
            TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = command->getChangeoverStatusEventResponse();

            if ( commandReceived )
            {
                // get the result of the command
                TA_IRS_Bus::ProtocolTtisTypes::ETtisResult result = commandReceived->getResult();

                switch ( result )
                {

                    case TA_IRS_Bus::ProtocolTtisTypes::TtisProceeding:
                        // fall through

                    case TA_IRS_Bus::ProtocolTtisTypes::TtisLibraryUpgraded:
                    {
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TtisSuccess;

                        libraryUpdate.version = commandReceived->getLibraryVersion();

                        // set the library version in the state
                        handleSuccessfulUpgrade( command,
                                                 commandReceived,
                                                 commandReceived->getTime() );
                    }
                    break;

                    case TA_IRS_Bus::ProtocolTtisTypes::TtisInUse:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TtisInUse;
                        break;

                    case TA_IRS_Bus::ProtocolTtisTypes::TtisReset:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TtisReset;
                        break;

                    case TA_IRS_Bus::ProtocolTtisTypes::TtisFailed:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TtisFailed;
                        break;

                    case TA_IRS_Bus::ProtocolTtisTypes::TtisDifferentVersion:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TtisDifferentVersion;
                        break;

                    default:
                        TA_ASSERT( false, "This value must be validated by the protocol library" );
                        break;
                }

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,

                             "%s command for train %d returned a command received with status: %s",
                             operationDescription.c_str(), trainId,
                             TA_IRS_Bus::ProtocolTtisTypes::ttisResultToString( result ).c_str() );
            }

            // if the bad command event is returned
            else if ( badCommand )
            {
                TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError errorCode = badCommand->getErrorCode();

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "%s command for train %d returned an error: %s",
                             operationDescription.c_str(), trainId,
                             TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode ).c_str() );

                // set TisReturnStatus.result to one of the BadCommand* error codes
                switch ( errorCode )
                {

                    case TA_IRS_Bus::ProtocolCommonTypes::BadChecksum:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::BadCommandBadChecksum;
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::UnknownCommand:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::BadCommandUnknownCommand;
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::BadParameter:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::BadCommandBadParameter;
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::UnexpectedCommand:
                        libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::BadCommandUnexpectedCommand;
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
                             "%s command for train %d returned an MPU changeover",
                             operationDescription.c_str(), trainId );

                libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::MpuChangeoverError;
            }
            else
            {
                // if no valid events were returned,
                // assert that the command completed without a valid response
                TA_ASSERT( false, "This command can only exit via exception or a valid response" );
            }
        }
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );

            // set the error status
            libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::InvalidTrainError;
        }
        catch ( TA_IRS_Bus::TrainAccessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );

            // set the error status
            libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::InvalidTrainError;
        }
        catch ( TA_IRS_Bus::TrainTransmissionFailureException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );

            // set the error status
            libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TransmissionFailureError;
        }
        catch ( TA_IRS_Bus::TrainTimeoutException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimeoutException", e.what() );

            // set the error status
            libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TransactionTimeoutError;
        }
        catch ( TA_IRS_Bus::TransactionCancelledException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactionCancelledException", e.what() );

            // set the error status
            libraryUpdate.commandStatus = TA_IRS_Bus::TtisTypes::TransactionCancelledError;
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_ASSERT( false, "This exception should not leak" );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "" );

            TA_ASSERT( false, "This exception should not leak" );
        }

        // send the result
        m_messageSender.sendClientUpdate( libraryUpdate );

        FUNCTION_EXIT;
    }


    void TimsTisManager::handleSuccessfulUpgrade( TA_IRS_Bus::TtisLibraryUpgradeCommandPtr command,
                                                  TA_IRS_Bus::TtisCommandReceivedEventPtr commandReceived,
                                                  time_t timestamp )
    {
        FUNCTION_ENTRY( "handleSuccessfulUpgrade( TtisLibraryUpgradeCommandPtr )" );

        TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate libraryUpdate;
        libraryUpdate.trainId = command->getTrainId();

        // get the train comms state
        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( command->getTrainId() );

        {
            // acquire the comms state lock
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            commsState->setTtisLibraryVersion( commandReceived->getLibraryVersion(), timestamp );
            commsState->setTtisScheduleVersion( commandReceived->getScheduleLibraryVersion(), timestamp );

            libraryUpdate.libraryVersion = commsState->getTtisLibraryVersion();
            libraryUpdate.scheduleVersion = commsState->getTtisScheduleVersion();

            // release the comms state lock
        }

        m_messageSender.sendClientUpdate( libraryUpdate );

        FUNCTION_EXIT;
    }


    void TimsTisManager::handleSuccessfulUpgrade( TA_IRS_Bus::TtisScheduleUpgradeCommandPtr command,
                                                  TA_IRS_Bus::TtisCommandReceivedEventPtr commandReceived,
                                                  time_t timestamp )
    {
        FUNCTION_ENTRY( "handleSuccessfulUpgrade( TtisScheduleUpgradeCommandPtr )" );

        TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate libraryUpdate;
        libraryUpdate.trainId = command->getTrainId();

        // get the train comms state
        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( command->getTrainId() );

        {
            // acquire the comms state lock
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            commsState->setTtisLibraryVersion( commandReceived->getLibraryVersion(), timestamp );
            commsState->setTtisScheduleVersion( commandReceived->getScheduleLibraryVersion(), timestamp );

            libraryUpdate.libraryVersion = commsState->getTtisLibraryVersion();
            libraryUpdate.scheduleVersion = commsState->getTtisScheduleVersion();

            // release the comms state lock
        }

        m_messageSender.sendClientUpdate( libraryUpdate );

        FUNCTION_EXIT;
    }


    void TimsTisManager::processEndOfTtisDownloadEvent( TA_IRS_Bus::EndOfTtisDownloadEventPtr event )
    {
        FUNCTION_ENTRY( "processEndOfTtisDownloadEvent" );

        // This is the download response from TIMS
        // It can indicate success, failure, or a partial retry.
        // The client needs to be informed of success or failure.

        TA_IRS_Bus::CommonTypes::TrainIdType trainId = event->getTrainId();
        TA_IRS_Bus::EndOfTtisDownloadEvent::ETtisDownloadResult downloadResult = event->getResult();

        // get the train tis state for this train
        TrainTisStatePtr tisState = m_statusManager.getTrainTisState( trainId );

        TA_IRS_Bus::TtisTypes::ETtisDownloadType currentDownloadType;
        unsigned short currentDownloadVersion;
        std::string sessionId;
        TA_IRS_Bus::TtisTypes::TtisFrameList currentDownloadData;
        std::string transferTsi;

        {
            // acquire the train tis state lock
            TA_IRS_Bus::TrainStateLock stateLock( tisState );

            // get the details of the current download (if any)
            currentDownloadType = tisState->getCurrentDownloadType();
            currentDownloadVersion = tisState->getCurrentDownloadVersion();
            sessionId = tisState->getSessionId();
            currentDownloadData = tisState->getCurrentDownloadData();
            transferTsi = tisState->getTransferTsi();


            switch ( downloadResult )
            {

                case TA_IRS_Bus::EndOfTtisDownloadEvent::TtisDownloadOk:
                    // fall through

                case TA_IRS_Bus::EndOfTtisDownloadEvent::TtisDownloadFailed:

                    // download is finished
                    tisState->clearDownload( event->getTime() );
                    break;

                case TA_IRS_Bus::EndOfTtisDownloadEvent::TtisDownloadRepetition:

                    // download process is still ongoing, but transfer is not
                    tisState->transferComplete( event->getTime() );
                    break;

                default:
                    TA_ASSERT( false, "This value must be validated by the protocol library" );
                    break;
            }

            // release the train tis state lock
        }

        // if the current download is not none
        if ( TA_IRS_Bus::TtisTypes::TtisNone != currentDownloadType )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "TTIS download complete [TrainId: %d][Type: %s][Version: %d][Status: %d]",
                         trainId,
                         TA_IRS_Bus::TtisTypes::ttisDownloadTypeToString( currentDownloadType ).c_str(),
                         currentDownloadVersion,
                         downloadResult );

            // stop any running timers
            stopTis2Timer( trainId );

            // if there was an ongoing transfer
            if ( false == transferTsi.empty() )
            {
                try
                {
                    // cancel the data call to the train
                    m_radioAgent.cancelDataCall( transferTsi, sessionId );
                }
                catch ( TA_IRS_Bus::TrainRadioException& e )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TrainRadioException", e.what() );
                }
            }

            // if the download result was successful or failed
            if ( TA_IRS_Bus::EndOfTtisDownloadEvent::TtisDownloadRepetition != downloadResult )
            {
                // Create a TisLibraryUpdate
                TA_IRS_Bus::TtisTypes::TisLibraryUpdate update;
                update.trainId = trainId;
                update.version = currentDownloadVersion;
                update.updateType = ( TA_IRS_Bus::TtisTypes::TtisLibrary == currentDownloadType ) ?
                                    TA_IRS_Bus::TtisTypes::LibraryDownloadComplete : TA_IRS_Bus::TtisTypes::ScheduleDownloadComplete;

                // if the result was successful
                if ( TA_IRS_Bus::EndOfTtisDownloadEvent::TtisDownloadOk == downloadResult )
                {
                    update.commandStatus = TA_IRS_Bus::TtisTypes::TtisSuccess;
                    update.downloadResult = TA_IRS_Bus::TtisTypes::Success;
                }

                // otherwise if the result was failed
                else
                {
                    update.commandStatus = TA_IRS_Bus::TtisTypes::TtisFailed;
                    update.downloadResult = TA_IRS_Bus::TtisTypes::TimsFailed;
                }

                // send the update
                m_messageSender.sendClientUpdate( update );
            }
            else
            {
                // this is repetition mode, another transfer must be started

                // first get the frame numbers to repeat from the event
                std::vector<unsigned short> repeatedFrames = event->getFramesToRepeat();

                // second build the dataToSend
                TA_IRS_Bus::TrainMessageData dataToSend;

                for ( std::vector<unsigned short>::iterator frameIter = repeatedFrames.begin();
                      repeatedFrames.end() != frameIter; ++frameIter )
                {
                    // if the frame number is > currentDownloadData.size() or if the frame number is 0
                    // check the frame number is in the valid list of frames
                    if ( ( 0 == *frameIter ) ||
                         ( currentDownloadData.size() < *frameIter ) )
                    {
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                     "TTIS download for train %d requested repetition of invalid frame number %d. Download failed.",
                                     trainId, *frameIter );

                        // we now consider this transfer failed, as there is an error in the process

                        {
                            // acquire the train tis state lock
                            TA_IRS_Bus::TrainStateLock stateLock( tisState );

                            // clear the download
                            tisState->clearDownload( event->getTime() );

                            // release the tis state lock
                        }

                        // send a client update

                        TA_IRS_Bus::TtisTypes::TisLibraryUpdate update;
                        update.trainId = trainId;
                        update.version = currentDownloadVersion;
                        update.updateType = ( TA_IRS_Bus::TtisTypes::TtisLibrary == currentDownloadType ) ?
                                            TA_IRS_Bus::TtisTypes::LibraryDownloadComplete : TA_IRS_Bus::TtisTypes::ScheduleDownloadComplete;
                        update.commandStatus = TA_IRS_Bus::TtisTypes::TtisFailed;
                        update.downloadResult = TA_IRS_Bus::TtisTypes::TransferFailed;

                        // send the update
                        m_messageSender.sendClientUpdate( update );

                        FUNCTION_EXIT;
                        return;
                    }
                    else
                    {
                        // append the frame data to dataToSend
                        dataToSend.insert( dataToSend.end(),
                                           currentDownloadData[ ( *frameIter ) - 1 ].frameData.begin(),
                                           currentDownloadData[ ( *frameIter ) - 1 ].frameData.end() );

                    }
                }

                // third the transfer must be started
                // it can be sent to the same TSI as last time, as ther has been no
                // MPU changeover received (the download would have been failed)

                try
                {
                    // start the transfer using the radio agent
                    m_radioAgent.makeDataCall( transferTsi,
                                               dataToSend,
                                               sessionId );

                    // acquire the tis state lock
                    TA_IRS_Bus::TrainStateLock tisStateLock( tisState );

                    // set the download details in the state
                    tisState->setTransferTsi( transferTsi,
                                              event->getTime() );

                    // release the tis state lock
                }
                catch ( TA_IRS_Bus::TrainRadioException& e )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TrainRadioException", e.what() );

                    {
                        // acquire the train tis state lock
                        TA_IRS_Bus::TrainStateLock stateLock( tisState );

                        // clear the download
                        tisState->clearDownload( event->getTime() );

                        // release the tis state lock
                    }

                    // send a client update

                    TA_IRS_Bus::TtisTypes::TisLibraryUpdate update;
                    update.trainId = trainId;
                    update.version = currentDownloadVersion;
                    update.updateType = ( TA_IRS_Bus::TtisTypes::TtisLibrary == currentDownloadType ) ?
                                        TA_IRS_Bus::TtisTypes::LibraryDownloadComplete : TA_IRS_Bus::TtisTypes::ScheduleDownloadComplete;
                    update.commandStatus = TA_IRS_Bus::TtisTypes::TtisFailed;
                    update.downloadResult = TA_IRS_Bus::TtisTypes::TransferFailed;

                    // send the update
                    m_messageSender.sendClientUpdate( update );
                }
            }
        }

        FUNCTION_EXIT;
    }


    void TimsTisManager::processChangeoverStatusEvent( TA_IRS_Bus::ChangeoverStatusEventPtr event )
    {
        FUNCTION_ENTRY( "processChangeoverStatusEvent" );

        // This handles an MPU changeover
        // if there are any ongoing downloads they are considered failed

        // get the train ID from the event
        TA_IRS_Bus::CommonTypes::TrainIdType trainId = event->getTrainId();

        // get the train tis state for the train
        TrainTisStatePtr tisState = m_statusManager.getTrainTisState( trainId );

        TA_IRS_Bus::TtisTypes::ETtisDownloadType currentDownloadType;
        unsigned short currentDownloadVersion;
        std::string sessionId;
        std::string transferTsi;

        {
            // acquire the train tis state lock
            TA_IRS_Bus::TrainStateLock stateLock( tisState );

            // get the details of the current download (if any)
            currentDownloadType = tisState->getCurrentDownloadType();
            currentDownloadVersion = tisState->getCurrentDownloadVersion();
            sessionId = tisState->getSessionId();
            transferTsi = tisState->getTransferTsi();

            // download is finished
            tisState->clearDownload( event->getTime() );

            // release the train tis state lock
        }

        // if the current download is not none
        if ( TA_IRS_Bus::TtisTypes::TtisNone != currentDownloadType )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "TTIS download complete due to MPU changeover [TrainId: %d][Type: %s][Version: %d]",
                         trainId,
                         TA_IRS_Bus::TtisTypes::ttisDownloadTypeToString( currentDownloadType ).c_str(),
                         currentDownloadVersion );

            // stop any running timers
            stopTis2Timer( trainId );

            // if there was an ongoing transfer
            if ( false == transferTsi.empty() )
            {
                try
                {
                    // cancel the data call to the train
                    m_radioAgent.cancelDataCall( transferTsi, sessionId );
                }
                catch ( TA_IRS_Bus::TrainRadioException& e )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TrainRadioException", e.what() );
                }
            }

            // send a client update

            TA_IRS_Bus::TtisTypes::TisLibraryUpdate update;
            update.trainId = trainId;
            update.version = currentDownloadVersion;
            update.updateType = ( TA_IRS_Bus::TtisTypes::TtisLibrary == currentDownloadType ) ?
                                TA_IRS_Bus::TtisTypes::LibraryDownloadComplete : TA_IRS_Bus::TtisTypes::ScheduleDownloadComplete;
            update.commandStatus = TA_IRS_Bus::TtisTypes::MpuChangeoverError;
            update.downloadResult = TA_IRS_Bus::TtisTypes::MpuChangeover;

            // send the update
            m_messageSender.sendClientUpdate( update );
        }

        FUNCTION_EXIT;
    }


    void TimsTisManager::performPreDisplayTrainChecks( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "performPreDisplayTrainChecks" );

        // check this agent is in control of this train
        // if it is not
        if ( false == m_operationModeManager.isInControlOfTrain( trainId, true, false ) )
        {
            // check if the train is still communicating with this agent:

            // get the train comms state for this train
            TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

            {
                // acquire the lock for the train state
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                //    check if communication is valid, and if the communicating location is this agent location
                if ( ( false == commsState->isTrainCommunicationValid() ) ||
                     ( m_configuration.getLocationKey() != commsState->getCommunicatingLocation() ) )
                {
                    TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Train is not controlled from this location" ) );
                }

                //    release the train state lock
            }

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Operation on train %d will be allowed because it is still communicating with this agent",
                         trainId );
        }

        // if the above tests pass, the operator can perform the action

        FUNCTION_EXIT;
    }


    void TimsTisManager::startTis2Timer( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "startTis2Timer" );

        // start the TIS2 timer for this train
        // get the TIS2 timeout value
        // ensure logging is enabled and use TIS2_TIMER_TEXT for the description
        m_tis2TimerManager->startTimer( trainId,
                                        m_configuration.getTimerTIS2(),
                                        true,
                                        TIS2_TIMER_TEXT );
        FUNCTION_EXIT;
    }


    void TimsTisManager::stopTis2Timer( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "stopTis2Timer" );

        // stop the TIS2 timer for this train
        m_tis2TimerManager->endTimer( trainId );

        FUNCTION_EXIT;
    }
}


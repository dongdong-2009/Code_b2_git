/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date: 2011-01-21 10:00:00$
  * Last modified by:  $Author: Jian Huang $
  *
  */
#ifdef WIN32
// do not use windows define min/max macro
#define NOMINMAX
#endif

#include "app/trains/train_agent/src/TimsPaManager.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"
#include "app/trains/train_agent/src/IOperationModeManager.h"
#include "app/trains/train_agent/src/IMessageSender.h"
#include "app/trains/train_agent/src/IRightsChecker.h"
#include "app/trains/train_agent/src/IRadioAgentInterface.h"
#include "app/trains/train_agent/src/IRadioGroupCoordinator.h"
#include "app/trains/train_agent/src/RadioMessageProcessor.h"
#include "app/trains/train_agent/src/TrainVoiceCallRequest.h"
#include "app/trains/train_agent/src/OperationModeManager.h"
#include "app/trains/train_agent/src/RadioAccess.h"

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
#include "bus/trains/TrainProtocolLibrary/src/PaCommandReceivedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaContinueCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PaCyclicPreRecordedAnnouncementCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PaLiveAnnouncementCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PaPreRecordedAnnouncementCommand.h"
//#include "bus/trains/TrainProtocolLibrary/src/PaResetCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PaDvaResetCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PaLiveResetCommand.h"
//#include "bus/managed_agent/src/AgentManager.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/utilities/src/DebugUtil.h"

#include <boost/lexical_cast.hpp>

namespace TA_IRS_App
{
    const std::string TimsPaManager::PA3_TIMER_TEXT = "PA3";

    TimsPaManager::TimsPaManager( ITrainAgentConfiguration& configuration,
                                  ITrainStatusManager& statusManager,
                                  TA_IRS_Bus::ITrainTransactionManager& transactionManager,
                                  IOperationModeManager& operationModeManager,
                                  IMessageSender& messageSender,
                                  IRightsChecker& rightsChecker,
                                  IRadioAgentInterface& radioAgent,
                                  RadioMessageProcessor& radioMessageProcessor,
                                  IRadioGroupCoordinator& radioGroupCoordinator,
								  RadioAccess& radioAccess
								  )
            : m_configuration( configuration ),
            m_statusManager( statusManager ),
            m_transactionManager( transactionManager ),
            m_operationModeManager( operationModeManager ),
            m_messageSender( messageSender ),
            m_rightsChecker( rightsChecker ),
            m_radioAgent( radioAgent ),
            m_radioMessageProcessor( radioMessageProcessor ),
            m_radioGroupCoordinator( radioGroupCoordinator ),
            m_threadPool( *TA_Base_Core::ThreadPoolSingletonManager::getInstance() ),
            m_pa3TimerManager( NULL ),
			m_radioAccess(radioAccess)
    {
        FUNCTION_ENTRY( "TimsPaManager" );

        // this needs to be done here using new
        // it could have been a member by value, however then 'this' would need to
        // be used in the initialiser list, which is not a good idea
        m_pa3TimerManager = new TA_IRS_Bus::UnsolicitedTrainTimerManager( *this );

        // register for train events
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::PaReadyForLiveAnnouncementEvent::getStaticType(), this );
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::PaRequestForResetEvent::getStaticType(), this );
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::PaResetEvent::getStaticType(), this );
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::PaTrainMessageCompletedEvent::getStaticType(), this );
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::ChangeoverStatusEvent::getStaticType(), this );
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::PaAudioSwitchEvent::getStaticType(), this );
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::AudioCabToCabEvent::getStaticType(), this );

        // subscribe for PA call end events
        m_radioMessageProcessor.addCallEndObserver( this, TA_IRS_Bus::CommonTypes::TrainPaResource );

        FUNCTION_EXIT;
    }


    TimsPaManager::~TimsPaManager()
    {
        FUNCTION_ENTRY( "~TimsPaManager" );

        // deregister for everything registered for in the constructor

        // subscribe for PA call end events
        m_radioMessageProcessor.removeCallEndObserver( this, TA_IRS_Bus::CommonTypes::TrainPaResource );

        // deregister for train events
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::PaReadyForLiveAnnouncementEvent::getStaticType(), this );
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::PaRequestForResetEvent::getStaticType(), this );
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::PaResetEvent::getStaticType(), this );
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::PaTrainMessageCompletedEvent::getStaticType(), this );
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::ChangeoverStatusEvent::getStaticType(), this );
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::PaAudioSwitchEvent::getStaticType(), this );
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::AudioCabToCabEvent::getStaticType(), this );

        // remove the thread pool instance
        TA_Base_Core::ThreadPoolSingletonManager::removeInstance( &m_threadPool );

        // remove the pa3 timer manager
        delete m_pa3TimerManager;
        m_pa3TimerManager = NULL;

        FUNCTION_EXIT;
    }


    unsigned short TimsPaManager::getCommandTimeout()
    {
        FUNCTION_ENTRY( "getCommandTimeout" );

        // get the biggest of the timers
        unsigned long biggestTimer = std::max( m_configuration.getTimerPA1(),
                                               m_configuration.getTimerPA2() );

        biggestTimer = std::max( biggestTimer,
                                 m_configuration.getTimerPA3() );

        biggestTimer = std::max( biggestTimer,
                                 m_configuration.getTimerPA4() );

        // the timeout is ( ( (the number of retries + 1) * timer ) /1000 ) + 5 (for procesing overhead)
        static unsigned short timeout =
            static_cast< unsigned short >( ( ( ( m_configuration.getTimsRetryLimit() + 1 ) * biggestTimer ) / 1000 ) + 5 );

        FUNCTION_EXIT;
        return timeout;
    }


    //unsigned short TimsPaManager::getTrainPaLibraryVersion( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    unsigned long TimsPaManager::getTrainPaLibraryVersion( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "getTrainPaLibraryVersion" );

        // get the train comms state for the train
        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

        //unsigned short paLibraryVersion;
        unsigned long paLibraryVersion;

        {
            // acquire the lock for the train state
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // get the PA library version
            paLibraryVersion = commsState->getPaLibraryVersion();

            // release the lock for the train state
        }

        // return the PA library version

        FUNCTION_EXIT;
        return paLibraryVersion;
    }


    TA_IRS_Bus::PaTypes::TrainLibraryInfoList TimsPaManager::getTrainPaLibraryVersionList()
    {
        FUNCTION_ENTRY( "getTrainPaLibraryVersionList" );

        // get all train comms states
        TA_IRS_Bus::TrainIdToStateMap stateMap = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::CoreResource );

        // create a library version list
        TA_IRS_Bus::PaTypes::TrainLibraryInfoList result;

        for ( TA_IRS_Bus::TrainIdToStateMap::iterator stateIter = stateMap.begin();
              stateMap.end() != stateIter; ++stateIter )
        {
            // down cast to the specific type
            TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( stateIter->second );
            TA_ASSERT( NULL != commsState.get(), "Failed to cast to the correct type" );

            TA_IRS_Bus::PaTypes::TrainLibraryInfo trainInfo;
            trainInfo.trainId = stateIter->first;

            {
                // acquire the lock for the train state
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                // get the PA library version
                trainInfo.libraryVersion = commsState->getPaLibraryVersion();

                // release the lock for the train state
            }

            // add the library version information to the list
            result.push_back( trainInfo );
        }

        // return the list

        FUNCTION_EXIT;
        return result;
    }


    void TimsPaManager::preRecordedAnnouncement( const TA_IRS_Bus::CommonTypes::TrainIdList& trainIdList,
                                                 TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                 //unsigned char messageNumber,
                                                 unsigned short messageNumber,
                                                 //unsigned short libraryVersion,
                                                 unsigned long libraryVersion,
                                                 bool override,
                                                 const std::string& sessionId,
												 TA_Base_Bus::ITrainPaCorba::EDvaMsgPriority messagePriority)
    {
        FUNCTION_ENTRY( "preRecordedAnnouncement" );

        // check the operator has the correct rights to broadcast a DVA message
        // if not, throw an AccessDeniedException
        if ( false == m_rightsChecker.canBroadcastPaDva( sessionId ) )
        {
          //  TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not broadcast train DVA" ) );
			 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "DVA announcement for trains No Rightbfor Broadcast: %s",
                     TA_IRS_Bus::CommonTypes::trainIdListToString( trainIdList ).c_str() );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "DVA announcement for trains: %s",
                     TA_IRS_Bus::CommonTypes::trainIdListToString( trainIdList ).c_str() );

        // get the operators origin:
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        // get the PA request mode, if override is true, use PaOverride, otherwise use PaRequest
        TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode requestMode =
            ( true == override ) ? TA_IRS_Bus::ProtocolPaTypes::PaOverride : TA_IRS_Bus::ProtocolPaTypes::PaRequest;

        // for each train in the train list
        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter = trainIdList.begin();
              trainIdList.end() != trainIter; ++trainIter )
        {
            // create a new PreRecordedAnnouncementTask
            // store it in an IWorkItemPtr and then queue it using m_threadPool
            TA_Base_Core::IWorkItemPtr newTask(
                new PreRecordedAnnouncementTask( *this,
                                                 *trainIter,
                                                 announcementId,
                                                 messageNumber,
                                                 libraryVersion,
                                                 requestMode,
                                                 sessionId,
                                                 origin,
												 messagePriority) );

            m_threadPool.queueWorkItem( newTask );
        }

        FUNCTION_EXIT;
    }


    void TimsPaManager::cyclicAnnouncement( const TA_IRS_Bus::CommonTypes::TrainIdList& trainIdList,
                                            TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                            //unsigned char messageNumber,
                                            unsigned short messageNumber,
                                            //unsigned short libraryVersion,
                                            unsigned long libraryVersion,
                                            const TA_IRS_Bus::PaTypes::MessageSchedule& announcementPeriod,
                                            bool override,
                                            const std::string& sessionId )
    {
        FUNCTION_ENTRY( "cyclicAnnouncement" );

        // check the operator has the correct rights to broadcast a DVA message
        // if not, throw an AccessDeniedException
        if ( false == m_rightsChecker.canBroadcastPaCyclicDva( sessionId ) )
        {
          //  TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not broadcast train DVA" ) );
			 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "DVA announcement for trains No Rightb for cyclicAnnouncement: %s",
                     TA_IRS_Bus::CommonTypes::trainIdListToString( trainIdList ).c_str() );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Cyclic announcement for trains: %s",
                     TA_IRS_Bus::CommonTypes::trainIdListToString( trainIdList ).c_str() );

        // get the operators origin:
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        // get the PA request mode, if override is true, use PaOverride, otherwise use PaRequest
        TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode requestMode =
            ( true == override ) ? TA_IRS_Bus::ProtocolPaTypes::PaOverride : TA_IRS_Bus::ProtocolPaTypes::PaRequest;

        // for each train in the train list
        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter = trainIdList.begin();
              trainIdList.end() != trainIter; ++trainIter )
        {
            // create a new CyclicAnnouncementTask
            // store it in an IWorkItemPtr and then queue it using m_threadPool
            TA_Base_Core::IWorkItemPtr newTask(
                new CyclicAnnouncementTask( *this,
                                            *trainIter,
                                            announcementId,
                                            messageNumber,
                                            libraryVersion,
                                            announcementPeriod,
                                            requestMode,
                                            sessionId,
                                            origin ) );

            m_threadPool.queueWorkItem( newTask );
        }

        FUNCTION_EXIT;
    }


    void TimsPaManager::endBroadcast( const TA_IRS_Bus::CommonTypes::TrainIdList& trainIdList,
                                      TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                      const std::string& sessionId )
    {
        FUNCTION_ENTRY( "endBroadcast" );
        
        // check the operator has the correct rights to broadcast a dva announcement
        // if not, throw an AccessDeniedException
        if ( ( false == m_rightsChecker.canBroadcastPaDva( sessionId ) ) &&
             ( false == m_rightsChecker.canBroadcastPaCyclicDva( sessionId ) ) )
        {
        //    TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not broadcast train DVA" ) );
			 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "DVA announcement for trains No Rightb for canBroadcastPaCyclicDva: %s",
                     TA_IRS_Bus::CommonTypes::trainIdListToString( trainIdList ).c_str() );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Ending broadcast for trains: %s",
                     TA_IRS_Bus::CommonTypes::trainIdListToString( trainIdList ).c_str() );

        // get the operators origin:
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        // for each train in the train list
        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter = trainIdList.begin();
              trainIdList.end() != trainIter; ++trainIter )
        {
            // create a new EndBroadcastTask
            // store it in an IWorkItemPtr and then queue it using m_threadPool
            TA_Base_Core::IWorkItemPtr newTask(
                new EndBroadcastTask( *this,
                                      *trainIter,
                                      announcementId,
                                      sessionId,
                                      origin ) );

            m_threadPool.queueWorkItem( newTask );
        }

        FUNCTION_EXIT;
    }


    void TimsPaManager::setupLiveAnnouncement( const TA_IRS_Bus::CommonTypes::TrainIdList& trainIdList,
                                               TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                               bool override,
                                               const std::string& staticGroup,
                                               const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setupLiveAnnouncement" );

        // check the operator has the correct rights to broadcast a live announcement
        // if not, throw an AccessDeniedException
        if ( false == m_rightsChecker.canBroadcastPaLive( sessionId ) )
        {
        //    TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not broadcast train Live" ) );
			 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "DVA announcement for trains No Rightb for setupLiveAnnouncement: %s",
                     TA_IRS_Bus::CommonTypes::trainIdListToString( trainIdList ).c_str() );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Setting up live announcement for trains: %s",
                     TA_IRS_Bus::CommonTypes::trainIdListToString( trainIdList ).c_str() );

        // get the operators origin:
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        // get the PA request mode, if override is true, use PaOverride, otherwise use PaRequest
        TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode requestMode =
            ( true == override ) ? TA_IRS_Bus::ProtocolPaTypes::PaOverride : TA_IRS_Bus::ProtocolPaTypes::PaRequest;

        // create a SetupLiveAnnouncementMonitorTaskPtr, initially empty
        SetupLiveAnnouncementMonitorTaskPtr monitorTask;

        // if there is a staticGroup given (not empty)
        if ( false == staticGroup.empty() )
        {
            // lock it
            //m_radioGroupCoordinator.lockRadioGroup( staticGroup );

            // create a SetupLiveAnnouncementMonitorTask and assign it to the SetupLiveAnnouncementMonitorTaskPtr
            // then queue it using m_threadPool
            monitorTask.reset(
                new SetupLiveAnnouncementMonitorTask( m_radioGroupCoordinator,
                                                      trainIdList.size(),
                                                      staticGroup ) );
        }

        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter = trainIdList.begin();
              trainIdList.end() != trainIter; ++trainIter )
        {
            // create a new SetupLiveAnnouncementTask.
            // store it in an IWorkItemPtr and then queue it using m_threadPool
            TA_Base_Core::IWorkItemPtr newTask(
                new SetupLiveAnnouncementTask( *this,
                                               *trainIter,
                                               announcementId,
                                               requestMode,
                                               staticGroup,
                                               sessionId,
                                               origin,
                                               monitorTask ) );

            m_threadPool.queueWorkItem( newTask );
        }

        FUNCTION_EXIT;
    }


    void TimsPaManager::beginLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                               const std::string& sessionId )
    {
        FUNCTION_ENTRY( "beginLiveAnnouncement" );

        // This must find all the trains involved in the live announcement and make an audio call to them
        // There is the option of using a static group or a DGNA

        // first get all the train involved in this live announcement
        // if there are no trains, throw an InvalidParameterException ( There are no trains for the given announcement ID )
        TA_IRS_Bus::CommonTypes::TrainIdList trainIdList = m_statusManager.getTrainsInvolvedInLiveAnnouncement( announcementId );

        if ( true == trainIdList.empty() )
        {
            std::ostringstream announcementIdStr;
            announcementIdStr << static_cast< unsigned int >( announcementId );

            TA_THROW( TA_IRS_Bus::InvalidParameterException( "There are no ready trains for this Announcement",
                                                             "AnnouncementId",
                                                             announcementIdStr.str() ) );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Biginning live announcement for trains: %s",
                     TA_IRS_Bus::CommonTypes::trainIdListToString( trainIdList ).c_str() );

        // check if there is a static group used to make the call
        std::string tsi = m_statusManager.getStaticGroupForLiveAnnouncement( announcementId );

        bool groupCall = true;
		
		unsigned long lCallId = 0;
		unsigned long talkgroupId = -1;

		try
		{
			if (trainIdList.size() > 1)
			{
				// DTL-541, wong.peter, talkgroupId is not used
				//talkgroupId = atol(tsi.c_str());
			} 
			else
			{
				TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( *(trainIdList.begin()) );
				TA_IRS_Bus::TrainStateLock stateLock( commsState );
				//The active TSI to use to make a private call
				talkgroupId = atol(commsState->getActiveTsi().c_str());
				lCallId = talkgroupId;
			}
		}
		catch (...)
		{
			talkgroupId = 0;
		}

		if (trainIdList.size() > 1) 
		{
			// DTL-541, wong.peter, RR_ANNGROUP is obsolete in DTL
			m_radioAccess.makePaLiveAnnouncementCall(sessionId,tsi,RadioAccess::SINGLE_TRAIN);
		} 
		else 
		{
			// make private call
			m_radioAccess.makePrivateCall( sessionId,talkgroupId );
		
			// register the call to the state so that if we
			// want to end the call, we could use this as a reference
			
			{
				
				TA_IRS_Bus::CommonTypes::TrainIdList::iterator iterTrainList = trainIdList.begin();
				long lTrainId = *iterTrainList;
				TrainPaStatePtr paState = m_statusManager.getTrainPaState( *(iterTrainList));
				
				time_t timestamp = time( NULL );
				unsigned long iConsoleId = m_configuration.getConsoleFromSession( sessionId );
				
				TA_IRS_Bus::StateUpdateTypes::RadioCallDetails callDetails;

				callDetails.calledTsi = tsi;
				callDetails.callId = lCallId;
				callDetails.consoleId = iConsoleId;
				callDetails.sessionId = sessionId;
				callDetails.resourceType = TA_IRS_Bus::CommonTypes::TrainPaResource;

				m_statusManager.addRadioCall(callDetails);
				paState->setCallDetails(sessionId,iConsoleId,lCallId,timestamp);

			}

		}


        //time_t timestamp = time( NULL );

		////for each train in the live announcement
		//for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter = trainIdList.begin();
		//	trainIdList.end() != trainIter; ++trainIter )
		//{
		//	// get the train pa state for the train
		//	TrainPaStatePtr paState = m_statusManager.getTrainPaState( *trainIter );

		//	// acquire the state lock
		//	TA_IRS_Bus::TrainStateLock stateLock( paState );

		//	// set the call details
		//	paState->setCallDetails( sessionId,
		//		callDetails.consoleId,
		//		callDetails.callId,
		//		timestamp );

		//	// release the state lock
		//}

        FUNCTION_EXIT;
    }


    void TimsPaManager::continueLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                  const std::string& sessionId )
    {
        FUNCTION_ENTRY( "continueLiveAnnouncement" );

        // This must find all the trains involved in the live announcement that require a continue
        // it then needs to send a continue message to all of them
        //
        //first get all the train involved in this live announcement requiring a continue
        TA_IRS_Bus::CommonTypes::TrainIdList trainIdList = m_statusManager.getTrainsInvolvedInLiveAnnouncementRequiringContinue( announcementId );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Continuing live announcement for trains: %s",
                     TA_IRS_Bus::CommonTypes::trainIdListToString( trainIdList ).c_str() );

        // get the operators origin
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter = trainIdList.begin();
              trainIdList.end() != trainIter; ++trainIter )
        {
            // create a new ContinueLiveAnnouncementTask.
            // store it in an IWorkItemPtr and then queue it using m_threadPool
            TA_Base_Core::IWorkItemPtr newTask(
                new ContinueLiveAnnouncementTask( *this,
                                                  *trainIter,
                                                  announcementId,
                                                  sessionId,
                                                  origin ) );

            m_threadPool.queueWorkItem( newTask );
        }

        FUNCTION_EXIT;
    }


    void TimsPaManager::endLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                             const std::string& sessionId )
    {
        FUNCTION_ENTRY( "endLiveAnnouncement" );

        // This must find all the trains involved in the live announcement
        // It must unlock any static group used
        // It must end the audio call
        // Then it must reset each trains PA state and send the end of live announcement command to each train

        // first get all the train involved in this live announcement
        // if there are no trains, throw an InvalidParameterException ( There are no trains for the given announcement ID )
        TA_IRS_Bus::CommonTypes::TrainIdList trainIdList = m_statusManager.getTrainsInvolvedInLiveAnnouncement( announcementId );

        if ( true == trainIdList.empty() )
        {
            std::ostringstream announcementIdStr;
            announcementIdStr << static_cast< unsigned int >( announcementId );

            TA_THROW( TA_IRS_Bus::InvalidParameterException( "There are no ready trains for this Announcement",
                                                             "AnnouncementId",
                                                             announcementIdStr.str() ) );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Ending live announcement for trains: %s",
                     TA_IRS_Bus::CommonTypes::trainIdListToString( trainIdList ).c_str() );

		std::string tsi = m_statusManager.getStaticGroupForLiveAnnouncement( announcementId );

		bool groupCall = true;

		unsigned long talkgroupId = -1;

		try
		{
			if (trainIdList.size() > 1)
			{
				// DTL-541, wong.peter, talkgroupId is not used
				//talkgroupId = atol(tsi.c_str());
			} 
			else
			{
				TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( *(trainIdList.begin()) );
				TA_IRS_Bus::TrainStateLock stateLock( commsState );
				//The active TSI to use to end a PA Call
				talkgroupId = atol(commsState->getActiveTsi().c_str());
			}
		}
		catch (...)
		{
			talkgroupId = 0;
		}

/*
        try
        {
			unsigned long talkgroupId = -1;
			for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter = trainIdList.begin();
				trainIdList.end() != trainIter; ++trainIter )
			{
				TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( *trainIter );

				// acquire the lock for the train state
				TA_IRS_Bus::TrainStateLock stateLock( commsState );

				//// get the primary TSI for the train, and add it to the ItsiList
				//trainItsiList.push_back( commsState->getActiveTsi() );
				//talkgroupId = commsState->getTalkgroupId();
				try
				{
					talkgroupId = boost::lexical_cast<unsigned long>(commsState->getPrimaryTsi());
				}
				catch(...)
				{
					//If it failed to convert set talkgroupId = 0
					talkgroupId = 0;
				}

				/*if (talkgroupId != -1) {
					break;
				}*/

				// release the state lock
			
		try {				

					

			if (trainIdList.size() > 1) {
				// DTL-541, wong.peter, RR_ANNGROUP is obsolete in DTL
				m_radioAccess.endPaLiveAnnouncementCall(sessionId, tsi, RadioAccess::SINGLE_TRAIN);
			} else
			{
				m_radioAccess.endPecCall ( sessionId, talkgroupId );
				
				unsigned long iConsoleId = m_configuration.getConsoleFromSession( sessionId );
				m_statusManager.removeRadioCall(iConsoleId,talkgroupId);
	

			}
        }
        catch ( TA_IRS_Bus::TrainRadioException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainRadioException", e.what() );
        }


        // check if there is a static group used to make the call
        // and unlock it if there was one
        std::string groupTsi = m_statusManager.getStaticGroupForLiveAnnouncement( announcementId );

        if ( false == groupTsi.empty() )
        {
            m_radioGroupCoordinator.unlockRadioGroup( groupTsi );
        }

        // get the operators origin
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter = trainIdList.begin();
              trainIdList.end() != trainIter; ++trainIter )
        {
            // create a new EndLiveAnnouncementTask.
            // store it in an IWorkItemPtr and then queue it using m_threadPool
            TA_Base_Core::IWorkItemPtr newTask(
                new EndLiveAnnouncementTask( *this,
                                             *trainIter,
                                             announcementId,
                                             sessionId,
                                             origin ) );

            m_threadPool.queueWorkItem( newTask );
        }

        FUNCTION_EXIT;
    }








    void TimsPaManager::processCallEndEvent( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetails& callEnd,
                                             const TA_IRS_Bus::CommonTypes::TrainIdList& trainList )
    {
        FUNCTION_ENTRY( "processCallEndEvent" );

        // This means a live announcement has been ended by the call being dropped
        // It is the same as the operator ending the announcement, except:
        // the operator must be notified differently
        // the audio call doesnt need to be ended

        // return if this agent isnt in the control state

        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // unlock the radio group in case this was a call to a group
        m_radioGroupCoordinator.unlockRadioGroup( callEnd.calledTsi );

        // iterate through the train list and find the announcement ID and session ID from the list,
        // exit the loop as soon as a valid announcement ID and session id is found

        TA_IRS_Bus::PaTypes::AnnouncementId announcementId( 0 );
        std::string sessionId( "" );

        TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator trainIter;
        for ( trainIter = trainList.begin();
              ( trainList.end() != trainIter ) && ( 0 == announcementId ) && ( true == sessionId.empty() );
              ++trainIter )
        {
            TrainPaStatePtr paState = m_statusManager.getTrainPaState( *trainIter );

            // acquire the state lock
            TA_IRS_Bus::TrainStateLock stateLock( paState );

            announcementId = paState->getAnnouncementId();
            sessionId = paState->getSessionId();
        }

        // get all the trains that are involved in this announcement to end (some may have not been involved in the call as there is no late entry)
        // merge the 2 sets (the method parameter, and the list returned above)
        // the sets can be merged by inserting the parameter set into the returned set (sets take care of duplicate elements automatically)
        // this is just a precaution for possibly messed up state - to ensure all trains are properly cleaned up
        TA_IRS_Bus::CommonTypes::TrainIdList fullList = m_statusManager.getTrainsInvolvedInLiveAnnouncement( announcementId );

        // well it should be done like this:
        // fullList.insert( trainList.begin(), trainList.end() );
        // but visual c++ 6 sucks ass, it doesnt support it properly
        // so lets do it the shit way
        for ( trainIter = trainList.begin(); trainList.end() != trainIter; ++trainIter )
        {
            fullList.insert( *trainIter );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                     "Audio lost for PA live announcement %d",
                     announcementId );

        // notify the client
        TA_IRS_Bus::PaTypes::PaAnnouncementUpdate clientUpdate;
        clientUpdate.updateType = TA_IRS_Bus::PaTypes::AudioCallLost;
        clientUpdate.announcementId = announcementId;
        m_messageSender.sendClientUpdate( clientUpdate );

        // get the operators origin
        unsigned char origin = m_configuration.getOperatorOrigin( sessionId );

        for ( trainIter = fullList.begin();
              fullList.end() != trainIter; ++trainIter )
        {
            // create a new EndLiveAnnouncementTask.
            // store it in an IWorkItemPtr and then queue it using m_threadPool
            TA_Base_Core::IWorkItemPtr newTask(
                new EndLiveAnnouncementTask( *this,
                                             *trainIter,
                                             announcementId,
                                             sessionId,
                                             origin ) );

            m_threadPool.queueWorkItem( newTask );
        }

        FUNCTION_EXIT;
    }


    void TimsPaManager::handleTrainTimeout( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "handleTrainTimeout" );

        // Simply delegate the call to onPA3Timeout
        onPA3Timeout( trainId );

        FUNCTION_EXIT;
    }


    void TimsPaManager::processEvent( TA_IRS_Bus::TrainEventPtr event )
    {
        FUNCTION_ENTRY( "processEvent" );

        // return if this agent isnt in the control state
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the event type, based on its type, downcast to the appropriate type and call the handler function:
        TA_IRS_Bus::TrainMessageType messageType = event->getEventType();

        if ( messageType == TA_IRS_Bus::PaReadyForLiveAnnouncementEvent::getStaticType() )
        {
            TA_IRS_Bus::PaReadyForLiveAnnouncementEventPtr readyForLiveAnnouncementEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::PaReadyForLiveAnnouncementEvent >( event );

            TA_ASSERT( NULL != readyForLiveAnnouncementEvent.get(), "cast failed" );

            processPaReadyForLiveAnnouncementEvent( readyForLiveAnnouncementEvent );
        }
        else if ( messageType == TA_IRS_Bus::PaRequestForResetEvent::getStaticType() )
        {
            TA_IRS_Bus::PaRequestForResetEventPtr requestForResetEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::PaRequestForResetEvent >( event );

            TA_ASSERT( NULL != requestForResetEvent.get(), "cast failed" );

            processPaRequestForResetEvent( requestForResetEvent );
        }
        else if ( messageType == TA_IRS_Bus::PaResetEvent::getStaticType() )
        {
            TA_IRS_Bus::PaResetEventPtr resetEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::PaResetEvent >( event );

            TA_ASSERT( NULL != resetEvent.get(), "cast failed" );

            processPaResetEvent( resetEvent );
        }
        else if ( messageType == TA_IRS_Bus::PaTrainMessageCompletedEvent::getStaticType() )
        {
            TA_IRS_Bus::PaTrainMessageCompletedEventPtr trainMessageCompletedEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::PaTrainMessageCompletedEvent >( event );

            TA_ASSERT( NULL != trainMessageCompletedEvent.get(), "cast failed" );

            processPaTrainMessageCompletedEvent( trainMessageCompletedEvent );
        }
        else if ( messageType == TA_IRS_Bus::ChangeoverStatusEvent::getStaticType() )
        {
            TA_IRS_Bus::ChangeoverStatusEventPtr changeoverStatusEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::ChangeoverStatusEvent >( event );

            TA_ASSERT( NULL != changeoverStatusEvent.get(), "cast failed" );

            processChangeoverStatusEvent( changeoverStatusEvent );
        }
        else if ( messageType == TA_IRS_Bus::PaAudioSwitchEvent::getStaticType() )
        {
            TA_IRS_Bus::PaAudioSwitchEventPtr audioSwitchEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::PaAudioSwitchEvent >( event );

            TA_ASSERT( NULL != audioSwitchEvent.get(), "cast failed" );

            processPaAudioSwitchEvent( audioSwitchEvent );
        }
        else if ( messageType == TA_IRS_Bus::AudioCabToCabEvent::getStaticType() )
        {
            TA_IRS_Bus::AudioCabToCabEventPtr audioCabToCabEvent =
                boost::dynamic_pointer_cast< TA_IRS_Bus::AudioCabToCabEvent >( event );

            TA_ASSERT( NULL != audioCabToCabEvent.get(), "cast failed" );

            processAudioCabToCabEvent( audioCabToCabEvent );
        }
        else
        {
            TA_ASSERT( false, "The event type received was not subscribed for" );
        }

        FUNCTION_EXIT;
    }


    void TimsPaManager::preRecordedAnnouncementSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                            TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                            //unsigned char messageNumber,
                                                            unsigned short messageNumber,
                                                            //unsigned short libraryVersion,
                                                            unsigned long libraryVersion,
                                                            TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode requestMode,
                                                            const std::string& sessionId,
                                                            unsigned char origin,
															TA_Base_Bus::ITrainPaCorba::EDvaMsgPriority messagePriority )
    {
        FUNCTION_ENTRY( "preRecordedAnnouncementSingleTrain" );

        // This simply sends a message to the train to start a pre recorded announcement
        // the train will respond with a response or an error
        // the result of this command then simply needs to be sent to the client

        // create a PaPreRecordedAnnouncementCommand with all the parameters given
        TA_IRS_Bus::PaPreRecordedAnnouncementCommandPtr command(
            new TA_IRS_Bus::PaPreRecordedAnnouncementCommand( trainId,
                                                              origin,
                                                              requestMode,
                                                              messageNumber,
                                                              libraryVersion,
                                                              announcementId,
															  messagePriority) );

        genericPreRecordedAnnouncementSingleTrain(
            trainId,
            announcementId,
            sessionId,
            "PA Pre Recorded Announcement",
            command );

        FUNCTION_EXIT;
    }


    void TimsPaManager::cyclicAnnouncementSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                       TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                       //unsigned char messageNumber,
                                                       unsigned short messageNumber,
                                                       //unsigned short libraryVersion,
                                                       unsigned long libraryVersion,
                                                       const TA_IRS_Bus::PaTypes::MessageSchedule& announcementPeriod,
                                                       TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode requestMode,
                                                       const std::string& sessionId,
                                                       unsigned char origin )
    {
        FUNCTION_ENTRY( "cyclicAnnouncementSingleTrain" );

        // This simply sends a message to the train to start a cyclic pre recorded announcement
        // the train will respond with a response or an error
        // the result of this command then simply needs to be sent to the client

        // create a PaPreRecordedAnnouncementCommand with all the parameters given
        TA_IRS_Bus::PaCyclicPreRecordedAnnouncementCommandPtr command(
            new TA_IRS_Bus::PaCyclicPreRecordedAnnouncementCommand( trainId,
                                                                    origin,
                                                                    requestMode,
                                                                    messageNumber,
                                                                    libraryVersion,
                                                                    announcementPeriod.period,
                                                                    announcementPeriod.duration,
                                                                    announcementId ) );

        genericPreRecordedAnnouncementSingleTrain(
            trainId,
            announcementId,
            sessionId,
            "PA Cyclic Pre Recorded Announcement",
            command );

        FUNCTION_EXIT;
    }


    template <typename CommandTypePtr>
    void TimsPaManager::genericPreRecordedAnnouncementSingleTrain ( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                                    TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                                    const std::string& sessionId,
                                                                    const std::string& operationDescription,
                                                                    CommandTypePtr command )
    {
        FUNCTION_ENTRY( "genericPreRecordedAnnouncementSingleTrain" );

        // create a PaUpdateEvent - no matter what the outcome the client must be notified
        TA_IRS_Bus::PaTypes::PaUpdateEvent update;
        update.trainId = trainId;
        update.announcementId = announcementId;
        update.sessionId = sessionId;
        update.updateType = TA_IRS_Bus::PaTypes::PaCommandResult;
        update.commandResult = TA_IRS_Bus::PaTypes::PaProceeding;
        update.completionStatus = TA_IRS_Bus::ProtocolPaTypes::Successful;   // not used

        try
        {
            // first check this agent is in control of this train
            performPreBroadcastTrainChecks( trainId );

            // get the train pa state for this train
            TrainPaStatePtr paState = m_statusManager.getTrainPaState( trainId );
            {
                // get the command lock and acquire it using a threadguard
                // for some reason it doesnt work all in one line
                TA_Base_Core::IThreadLockable& commandLock = paState->getCommandLock();
                TA_THREADGUARD( commandLock );

                // send the command
                m_transactionManager.sendCommand( command,
                                                  m_configuration.getTimerPA1(),
                                                  m_configuration.getTimsRetryLimit() );

                // release the command lock
            }

            // get the CommandReceivedResponse, the bad command response, and the changeover status response from the command
            // if none of these are returned, ASSERT - the command should not exit normally without a valid resposne
            TA_IRS_Bus::PaCommandReceivedEventPtr commandReceived = command->getCommandReceivedResponse();
            TA_IRS_Bus::BadCommandEventPtr badCommand = command->getBadCommandResponse();
            TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = command->getChangeoverStatusEventResponse();

            // if it is a PaCommandReceivedEvent
            if ( commandReceived )
            {
                // get the result of the command
                TA_IRS_Bus::ProtocolPaTypes::EPaCommandResult result = commandReceived->getCommandResult();

                // based on the result set the commandResult in the PaUpdateEvent to one of the Pa* error codes
                switch ( result )
                {

                    case TA_IRS_Bus::ProtocolPaTypes::PaProceeding:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaProceeding;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaInUse:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaInUse;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaInUseOptionToOverride:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaInUseOptionToOverride;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaInUseFirstCycleCancelled:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaInUseFirstCycleCancelled;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaReset:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaReset;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaFailed:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaFailed;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaDifferentOrigin:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaDifferentOrigin;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaDifferentId:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaDifferentId;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaDifferentVersion:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaDifferentVersion;
                        break;

                    default:
                        TA_ASSERT( false, "This value must be validated by the protocol library" );
                        break;
                }

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "%s command for train %d returned a command received with status: %s",
                             operationDescription.c_str(), trainId,
                             TA_IRS_Bus::ProtocolPaTypes::paCommandResultToString( result ).c_str() );

                // if the result is PaProceeding or PaInUseFirstCycleCancelled
                // this is a successful broadcast
                //if ( ( TA_IRS_Bus::ProtocolPaTypes::PaProceeding == result ) ||
                //     ( TA_IRS_Bus::ProtocolPaTypes::PaInUseFirstCycleCancelled == result ) )
                if ( TA_IRS_Bus::ProtocolPaTypes::PaProceeding == result )
                {
                    // get the library version from the event
                    if ( 0 != commandReceived->getLibraryVersion() )
                    {
                        // get the train comms state for this train
                        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

                        // acquire the lock for the train state
                        TA_IRS_Bus::TrainStateLock stateLock( commsState );

                        // set the PA library version
                        commsState->setPaLibraryVersion( commandReceived->getLibraryVersion(),
                                                         commandReceived->getTime() );

                        // if it changed, send the version update
                        if ( true == commsState->hasDataChanged() )
                        {
                            TA_IRS_Bus::PaTypes::TrainLibraryInfo libraryUpdate;
                            libraryUpdate.trainId = trainId;
                            libraryUpdate.libraryVersion = commsState->getPaLibraryVersion();

                            m_messageSender.sendClientUpdate( libraryUpdate );
                        }

                        // release the train comms state lock
                    }
                }
            }

            // if the bad command event is returned
            else if ( badCommand )
            {
                TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError errorCode = badCommand->getErrorCode();

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "%s command for train %d returned an error: %s",
                             operationDescription.c_str(), trainId,
                             TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode ).c_str() );

                // set the commandResult in the PaUpdateEvent to one of the BadCommand* error codes
                switch ( errorCode )
                {

                    case TA_IRS_Bus::ProtocolCommonTypes::BadChecksum:
                        update.commandResult = TA_IRS_Bus::PaTypes::BadCommandBadChecksum;
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::UnknownCommand:
                        update.commandResult = TA_IRS_Bus::PaTypes::BadCommandUnknownCommand;
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::BadParameter:
                        update.commandResult = TA_IRS_Bus::PaTypes::BadCommandBadParameter;
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::UnexpectedCommand:
                        update.commandResult = TA_IRS_Bus::PaTypes::BadCommandUnexpectedCommand;
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

                update.commandResult = TA_IRS_Bus::PaTypes::MpuChangeoverError;
            }
            else
            {
                // if no valid events were returned,
                // assert that the command completed without a valid response
                TA_ASSERT( false, "This command can only exit via exception or a valid response" );
            }

        }

        // catch any of the following exceptions, log the exception, and set the appropriate commandResult in the PaUpdateEvent
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );

            // set the error status
            update.commandResult = TA_IRS_Bus::PaTypes::InvalidTrainError;
        }
        catch ( TA_IRS_Bus::TrainAccessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );

            // set the error status
            update.commandResult = TA_IRS_Bus::PaTypes::InvalidTrainError;
        }
        catch ( TA_IRS_Bus::TrainTransmissionFailureException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );

            // set the error status
            update.commandResult = TA_IRS_Bus::PaTypes::TransmissionFailureError;
        }
        catch ( TA_IRS_Bus::TrainTimeoutException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimeoutException", e.what() );

            // set the error status
            update.commandResult = TA_IRS_Bus::PaTypes::TransactionTimeoutError;
        }
        catch ( TA_IRS_Bus::TransactionCancelledException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactionCancelledException", e.what() );

            // set the error status
            update.commandResult = TA_IRS_Bus::PaTypes::TransactionCancelledError;
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

        // send the PaUpdateEvent
        m_messageSender.sendClientUpdate( update );

        FUNCTION_EXIT;
    }


    void TimsPaManager::endBroadcastSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                 TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                 const std::string& sessionId,
                                                 unsigned char origin )
    {
        FUNCTION_ENTRY( "endBroadcastSingleTrain" );

        // This simply sends a reset message to the train
        // the train will respond with a response or an error
        // the result of this command then simply needs to be sent to the client

        TA_IRS_Bus::PaTypes::PaUpdateEvent update;
        update.trainId = trainId;
        update.announcementId = announcementId;
        update.sessionId = sessionId;
        update.updateType = TA_IRS_Bus::PaTypes::PaCommandResult;
        update.commandResult = TA_IRS_Bus::PaTypes::PaProceeding;
        update.completionStatus = TA_IRS_Bus::ProtocolPaTypes::Successful;

        try
        {
            // first check this agent is in control of this train
            performPreBroadcastTrainChecks( trainId );

            //TA_IRS_Bus::PaResetCommandPtr command(
            TA_IRS_Bus::PaDvaResetCommandPtr command(
                //new TA_IRS_Bus::PaResetCommand( trainId,
                new TA_IRS_Bus::PaDvaResetCommand( trainId,
                                                origin,
                                                announcementId ) );

            // get the train pa state for this train
            TrainPaStatePtr paState = m_statusManager.getTrainPaState( trainId );
            {
                // get the command lock and acquire it using a threadguard
                // for some reason it doesnt work all in one line
                TA_Base_Core::IThreadLockable& commandLock = paState->getCommandLock();
                TA_THREADGUARD( commandLock );

                // send the command
                m_transactionManager.sendCommand( command,
                                                  m_configuration.getTimerPA4(),
                                                  m_configuration.getTimsRetryLimit() );

                // release the command lock
            }

            // get the CommandReceivedResponse, the TrainMessageCompletedResponse, the bad command response, and the changeover status response from the command
            // if none of these are returned, ASSERT - the command should not exit normally without a valid resposne
            TA_IRS_Bus::PaCommandReceivedEventPtr commandReceived = command->getCommandReceivedResponse();
            TA_IRS_Bus::PaTrainMessageCompletedEventPtr messageCompleted = command->getTrainMessageCompletedResponse();
            TA_IRS_Bus::BadCommandEventPtr badCommand = command->getBadCommandResponse();
            TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = command->getChangeoverStatusEventResponse();

            // if it is a PaCommandReceivedEvent
            if ( commandReceived )
            {
                // get the result of the command
                TA_IRS_Bus::ProtocolPaTypes::EPaCommandResult result = commandReceived->getCommandResult();

                // based on the result set the commandResult in the PaUpdateEvent to one of the Pa* error codes
                switch ( result )
                {
                    case TA_IRS_Bus::ProtocolPaTypes::PaProceeding:
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Received proceeding acknowledge message when sending Reset command to TIMS" );
                        // DTL-787 modify start
                        // update.commandResult = TA_IRS_Bus::PaTypes::PaFailed;
                        update.commandResult = TA_IRS_Bus::PaTypes::PaProceeding;
                        // DTL-787 modify end
                        break;
                    case TA_IRS_Bus::ProtocolPaTypes::PaReset: // treat as a valid reset
                        update.commandResult = TA_IRS_Bus::PaTypes::PaReset;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaInUse: // invalid status for this operation
                    case TA_IRS_Bus::ProtocolPaTypes::PaInUseOptionToOverride: // invalid status for this operation
                    case TA_IRS_Bus::ProtocolPaTypes::PaInUseFirstCycleCancelled: // invalid status for this operation
                    case TA_IRS_Bus::ProtocolPaTypes::PaFailed:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaFailed;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaDifferentOrigin:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaDifferentOrigin;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaDifferentId:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaDifferentId;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaDifferentVersion:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaDifferentVersion;
                        break;

                    default:
                        TA_ASSERT( false, "This value must be validated by the protocol library" );
                        break;
                }

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "PA Reset command for train %d returned a command received with status: %s",
                             trainId,
                             TA_IRS_Bus::ProtocolPaTypes::paCommandResultToString( result ).c_str() );
            }

            // if the message completed event is returned
            else if ( messageCompleted )
            {
                // get the result of the command
                TA_IRS_Bus::ProtocolPaTypes::EMessageCompletionStatus result = messageCompleted->getCompletionStatus();

                // based on the result set the completionStatus in the PaUpdateEvent to the correct status
                update.completionStatus = result;

                // now, TIMS will reply with this message, the expected status is either InterruptedByOcc or InterruptedByPsc
                // in this case, the response will be treated like a successful reset
                // if the response indicates the message was InterruptedByDriver we need to indicate this correctly
                // and if the response indicates the message actually completed, then this should be indicated also

                switch ( result )
                {
                    // successful reset
                    case TA_IRS_Bus::ProtocolPaTypes::InterruptedByOcc:
                    case TA_IRS_Bus::ProtocolPaTypes::InterruptedByPsc:
                        update.updateType = TA_IRS_Bus::PaTypes::PaCommandResult;
                        //update.commandResult = TA_IRS_Bus::PaTypes::PaProceeding;
                        update.commandResult = TA_IRS_Bus::PaTypes::PaReset;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::Successful:
                    case TA_IRS_Bus::ProtocolPaTypes::InterruptedByDriver:
                        // set the updateType in the PaUpdateEvent to CompletedAnnouncement
                        // the completion status has already been set
                        update.updateType = TA_IRS_Bus::PaTypes::CompletedAnnouncement;
                        break;
					default:;
						// fall through
                }

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "PA Reset command for train %d returned a message completed with status: %s",
                             trainId,
                             TA_IRS_Bus::ProtocolPaTypes::messageCompletionStatusToString( result ).c_str() );
            }

            // if the bad command event is returned
            else if ( badCommand )
            {
                TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError errorCode = badCommand->getErrorCode();

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "PA Reset command for train %d returned an error: %s",
                             trainId,
                             TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode ).c_str() );

                // set the commandResult in the PaUpdateEvent to one of the BadCommand* error codes

                switch ( errorCode )
                {

                    case TA_IRS_Bus::ProtocolCommonTypes::BadChecksum:
                        update.commandResult = TA_IRS_Bus::PaTypes::BadCommandBadChecksum;
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::UnknownCommand:
                        update.commandResult = TA_IRS_Bus::PaTypes::BadCommandUnknownCommand;
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::BadParameter:
                        update.commandResult = TA_IRS_Bus::PaTypes::BadCommandBadParameter;
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::UnexpectedCommand:
                        update.commandResult = TA_IRS_Bus::PaTypes::BadCommandUnexpectedCommand;
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
                             "PA Reset command for train %d returned an MPU changeover",
                             trainId );

                update.commandResult = TA_IRS_Bus::PaTypes::MpuChangeoverError;
            }
            else
            {
                // if no valid events were returned,
                // assert that the command completed without a valid response
                TA_ASSERT( false, "This command can only exit via exception or a valid response" );
            }

        }
        // catch any of the following exceptions, log the exception, and set the appropriate commandResult in the PaUpdateEvent
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );

            // set the error status
            update.commandResult = TA_IRS_Bus::PaTypes::InvalidTrainError;
        }
        catch ( TA_IRS_Bus::TrainAccessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );

            // set the error status
            update.commandResult = TA_IRS_Bus::PaTypes::InvalidTrainError;
        }
        catch ( TA_IRS_Bus::TrainTransmissionFailureException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );

            // set the error status
            update.commandResult = TA_IRS_Bus::PaTypes::TransmissionFailureError;
        }
        catch ( TA_IRS_Bus::TrainTimeoutException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimeoutException", e.what() );

            // set the error status
            update.commandResult = TA_IRS_Bus::PaTypes::TransactionTimeoutError;
        }
        catch ( TA_IRS_Bus::TransactionCancelledException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactionCancelledException", e.what() );

            // set the error status
            update.commandResult = TA_IRS_Bus::PaTypes::TransactionCancelledError;
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

        // send the PaUpdateEvent
        m_messageSender.sendClientUpdate( update );

        FUNCTION_EXIT;
    }


    void TimsPaManager::setupLiveAnnouncementSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                          TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                          TA_IRS_Bus::ProtocolPaTypes::EPaRequestMode requestMode,
                                                          const std::string& staticGroup,
                                                          const std::string& sessionId,
                                                          unsigned char origin,
                                                          SetupLiveAnnouncementMonitorTaskPtr monitorTask )
    {
        FUNCTION_ENTRY( "setupLiveAnnouncementSingleTrain" );

        // This sets up a live announcement
        // The command to set up the live announcement must be sent to the train
        // Then the ready for live announcement must be received, or the process will retry
        // When this process is complete, either a ready for live announcement will be sent to the client, or a failure update

        TA_IRS_Bus::PaTypes::PaUpdateEvent update;
        update.trainId = trainId;
        update.announcementId = announcementId;
        update.sessionId = sessionId;
        update.updateType = TA_IRS_Bus::PaTypes::PaCommandResult;
        update.commandResult = TA_IRS_Bus::PaTypes::PaProceeding;
        update.completionStatus = TA_IRS_Bus::ProtocolPaTypes::Successful;

        bool retry = true;
        bool success = false;

        // first check this agent is in control of this train
        try
        {
            performPreBroadcastTrainChecks( trainId );
        }
        catch ( TA_IRS_Bus::TrainAccessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TrainAccessDeniedException", e.what() );

            update.commandResult = TA_IRS_Bus::PaTypes::InvalidTrainError;
            retry = false;
        }

        // get the train PA state
        TrainPaStatePtr paState = m_statusManager.getTrainPaState( trainId );

        // create a PaLiveAnnouncementCommand
        TA_IRS_Bus::PaLiveAnnouncementCommandPtr command(
            new TA_IRS_Bus::PaLiveAnnouncementCommand( trainId,
                                                       origin,
                                                       requestMode,
                                                       announcementId ) );

        unsigned int retries = 0;

        while ( true == retry )
        {
            try
            {
                {
                    // get the command lock and acquire it using a threadguard
                    // for some reason it doesnt work all in one line
                    TA_Base_Core::IThreadLockable& commandLock = paState->getCommandLock();
                    TA_THREADGUARD( commandLock );

                    // send the command
                    m_transactionManager.sendCommand( command,
                                                      m_configuration.getTimerPA1(),
                                                      0 ); // note PA setup live announcement retry logic needs to be done here

                    // release the command lock
                }

                // get the CommandReceivedResponse, the bad command response, and the changeover status response from the command
                // if none of these are returned, ASSERT - the command should not exit normally without a valid resposne
                TA_IRS_Bus::PaCommandReceivedEventPtr commandReceived = command->getCommandReceivedResponse();
                TA_IRS_Bus::BadCommandEventPtr badCommand = command->getBadCommandResponse();
                TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = command->getChangeoverStatusEventResponse();

                // if it is a PaCommandReceivedEvent
                if ( commandReceived )
                {
                    // get the result of the command
                    TA_IRS_Bus::ProtocolPaTypes::EPaCommandResult result = commandReceived->getCommandResult();

                    // based on the result set the commandResult in the PaUpdateEvent to one of the Pa* error codes
                    switch ( result )
                    {

                        case TA_IRS_Bus::ProtocolPaTypes::PaProceeding:
                            update.commandResult = TA_IRS_Bus::PaTypes::PaProceeding;
                            break;

                        case TA_IRS_Bus::ProtocolPaTypes::PaInUse:
                            update.commandResult = TA_IRS_Bus::PaTypes::PaInUse;
                            break;

                        case TA_IRS_Bus::ProtocolPaTypes::PaInUseOptionToOverride:
                            update.commandResult = TA_IRS_Bus::PaTypes::PaInUseOptionToOverride;
                            break;

                        case TA_IRS_Bus::ProtocolPaTypes::PaInUseFirstCycleCancelled:
                            update.commandResult = TA_IRS_Bus::PaTypes::PaInUseFirstCycleCancelled;
                            break;

                        case TA_IRS_Bus::ProtocolPaTypes::PaReset:
                            update.commandResult = TA_IRS_Bus::PaTypes::PaReset;
                            break;

                        case TA_IRS_Bus::ProtocolPaTypes::PaFailed:
                            update.commandResult = TA_IRS_Bus::PaTypes::PaFailed;
                            break;

                        case TA_IRS_Bus::ProtocolPaTypes::PaDifferentOrigin:
                            update.commandResult = TA_IRS_Bus::PaTypes::PaDifferentOrigin;
                            break;

                        case TA_IRS_Bus::ProtocolPaTypes::PaDifferentId:
                            update.commandResult = TA_IRS_Bus::PaTypes::PaDifferentId;
                            break;

                        case TA_IRS_Bus::ProtocolPaTypes::PaDifferentVersion:
                            update.commandResult = TA_IRS_Bus::PaTypes::PaDifferentVersion;
                            break;

                        default:
                            TA_ASSERT( false, "This value must be validated by the protocol library" );
                            break;
                    }

                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                 "Live Announcement command for train %d returned a command received with status: %s",
                                 trainId,
                                 TA_IRS_Bus::ProtocolPaTypes::paCommandResultToString( result ).c_str() );

                    // if the result is PaProceeding
                    if ( TA_IRS_Bus::ProtocolPaTypes::PaProceeding == result )
                    {
                        // if here, it means a valid PA command received with a proceeding status was received
                        // but it is not finished yet, the PA2 timer must be started

                        // first, set the announcement details in the train state
                        {
                            // acquire the train PA state lock
                            TA_IRS_Bus::TrainStateLock stateLock( paState );

                            // set the announcement details (the timestamp can come from the PaCommandReceivedEvent)
                            paState->setAnnouncementDetails( announcementId,
                                                             sessionId,
                                                             commandReceived->getTime(),
                                                             staticGroup );

                            // clear any ongoing call, as it is no longer valid
                            paState->clearCall();

                            // release the train PA state lock
                        }

                        // start the PA2 timer
                        m_transactionManager.startTrainTimer( trainId,
                                                              TA_IRS_Bus::CommonTypes::TrainPaResource,
                                                              m_configuration.getTimerPA2() );

                        // wait on the timer to be signalled or expire
                        m_transactionManager.waitTrainTimer( trainId, TA_IRS_Bus::CommonTypes::TrainPaResource );

                        // end the timer as this is now done
                        m_transactionManager.endTrainTimer( trainId, TA_IRS_Bus::CommonTypes::TrainPaResource );

                        TA_IRS_Bus::PaTypes::AnnouncementId stateAnnouncementId = 0;

                        {
                            // acquire the train PA state lock
                            TA_IRS_Bus::TrainStateLock stateLock( paState );

                            // get the announcement ID from the state
                            stateAnnouncementId = paState->getAnnouncementId();

                            // release the train PA state lock
                        }

                        // if the announcement ID in the state doesnt match the announcement ID we set before,
                        // then this has been externally cancelled
                        if ( stateAnnouncementId != announcementId )
                        {
                            // set the commandResult to PaReset
                            update.commandResult = TA_IRS_Bus::PaTypes::PaReset;
                        }
                        else
                        {
                            // everything is now set up

                            // set the updateType to LiveAnnouncementReady
                            update.updateType = TA_IRS_Bus::PaTypes::LiveAnnouncementReady;

                            // set the commandResult to PaProceeding
                            update.commandResult = TA_IRS_Bus::PaTypes::PaProceeding;

                            // start the PA3 timer for this train
                            startPA3Timer( trainId );

                            success = true;
                        }

                        // signal the loop should exit
                        retry = false;
                    }
                    else
                    {
                        // exit while loop
                        retry = false;
                    }
                }

                // if the bad command event is returned
                else if ( badCommand )
                {
                    // get the error code from the event
                    TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError errorCode = badCommand->getErrorCode();

                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                                 "Live Announcement command for train %d returned an error: %s",
                                 trainId,
                                 TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode ).c_str() );

                    // set the commandResult in the PaUpdateEvent to one of the BadCommand* error codes
                    switch ( errorCode )
                    {

                        case TA_IRS_Bus::ProtocolCommonTypes::BadChecksum:
                            update.commandResult = TA_IRS_Bus::PaTypes::BadCommandBadChecksum;
                            break;

                        case TA_IRS_Bus::ProtocolCommonTypes::UnknownCommand:
                            update.commandResult = TA_IRS_Bus::PaTypes::BadCommandUnknownCommand;
                            break;

                        case TA_IRS_Bus::ProtocolCommonTypes::BadParameter:
                            update.commandResult = TA_IRS_Bus::PaTypes::BadCommandBadParameter;
                            break;

                        case TA_IRS_Bus::ProtocolCommonTypes::UnexpectedCommand:
                            update.commandResult = TA_IRS_Bus::PaTypes::BadCommandUnexpectedCommand;
                            break;

                        default:
                            TA_ASSERT( false, "This value must be validated by the protocol library" );
                            break;
                    }

                    // exit while loop
                    retry = false;
                }

                // if the mpu changeover event is returned
                else if ( mpuChangeover )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                                 "Live Announcement command for train %d returned an MPU changeover",
                                 trainId );

                    update.commandResult = TA_IRS_Bus::PaTypes::MpuChangeoverError;

                    // exit while loop
                    retry = false;
                }
                else
                {
                    // if no valid events were returned,
                    // assert that the command completed without a valid response
                    TA_ASSERT( false, "This command can only exit via exception or a valid response" );
                }

            }
            // catch any of the following exceptions, these will allow for retry:
            catch ( TA_IRS_Bus::TrainTimerException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimerException", e.what() );

                // TA_IRS_Bus::CommonTypes::TrainIdTypeend the timer as this is now done
                m_transactionManager.endTrainTimer( trainId, TA_IRS_Bus::CommonTypes::TrainPaResource );

                if ( retries < m_configuration.getTimsRetryLimit() )
                {
                    // increment retries
                    ++retries;

                    // try again
                }
                else
                {
                    // the number of retries has been exceeded
                    // set the commandResult in the PaUpdateEvent to TransactionTimeoutError
                    update.commandResult = TA_IRS_Bus::PaTypes::TransactionTimeoutError;

                    // signal the loop should exit
                    retry = false;
                }
            }
            catch ( TA_IRS_Bus::TrainTimeoutException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimeoutException", e.what() );

                // a timer has not be started at the time this exception can be triggered

                if ( retries < m_configuration.getTimsRetryLimit() )
                {
                    // increment retries
                    ++retries;

                    // try again
                }
                else
                {
                    // the number of retries has been exceeded
                    // set the commandResult in the PaUpdateEvent to TransactionTimeoutError
                    update.commandResult = TA_IRS_Bus::PaTypes::TransactionTimeoutError;

                    // signal the loop should exit
                    retry = false;
                }
            }
            // catch any of these exceptions, these wont retry
            catch ( TA_IRS_Bus::InvalidTrainException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );

                // set the appropriate commandResult in the PaUpdateEvent:
                update.commandResult = TA_IRS_Bus::PaTypes::InvalidTrainError;

                // signal the loop should exit
                retry = false;
            }
            catch ( TA_IRS_Bus::TrainTransmissionFailureException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );

                // set the appropriate commandResult in the PaUpdateEvent:
                update.commandResult = TA_IRS_Bus::PaTypes::TransmissionFailureError;

                // signal the loop should exit
                retry = false;
            }
            catch ( TA_IRS_Bus::TransactionCancelledException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactionCancelledException", e.what() );

                // set the appropriate commandResult in the PaUpdateEvent:
                update.commandResult = TA_IRS_Bus::PaTypes::TransactionCancelledError;

                // signal the loop should exit
                retry = false;
            }
        }

        // send the PaUpdateEvent
        m_messageSender.sendClientUpdate( update );

        // notify the monitor task of the result
        if ( monitorTask )
        {
            if ( true == success )
            {
                monitorTask->signalSetupLiveAnnouncementSuccess();
            }
            else
            {
                monitorTask->signalSetupLiveAnnouncementFailure();

                // also ensure the state is clear
                {
                    // acquire the train PA state lock
                    TA_IRS_Bus::TrainStateLock stateLock( paState );

                    // if the state contains this announcement...
                    if ( paState->getAnnouncementId() == announcementId )
                    {
                        paState->resetAnnouncementDetails();
                    }

                    // release the train PA state lock
                }
            }
        }

        FUNCTION_EXIT;
    }


    void TimsPaManager::continueLiveAnnouncementSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                             TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                             const std::string& sessionId,
                                                             unsigned char origin )
    {
        FUNCTION_ENTRY( "continueLiveAnnouncementSingleTrain" );

        // This sends a PA continue to a train requiring continue

        // get the train PA state
        TrainPaStatePtr paState = m_statusManager.getTrainPaState( trainId );

        {
            // acquire the train PA state lock
            TA_IRS_Bus::TrainStateLock stateLock( paState );

            // get the announcement ID from the state
            TA_IRS_Bus::PaTypes::AnnouncementId stateAnnouncementId = paState->getAnnouncementId();

            // if it does not match the given announcement ID
            if ( announcementId != stateAnnouncementId )
            {
                // this can happen due to timing - in the case the announcement
                // has been ended at the same time the operator hits continue

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                             "PA Continue button pressed for train %d, the announcement on that train has ended, ignoring.",
                             trainId );

                FUNCTION_EXIT;
                return;
            }

            // reset the auto continue count
            paState->resetAutoContinueCount();

            // release the train PA state lock
        }

        try
        {
            // create a PaContinueCommand
            TA_IRS_Bus::PaContinueCommandPtr command(
                new TA_IRS_Bus::PaContinueCommand( trainId, origin, announcementId ) );

            // send the command (use the default timeout and retries as this command has no response)
            m_transactionManager.sendCommand( command );

            // start the PA3 timer for this train
            startPA3Timer( trainId );
        }
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );
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

        FUNCTION_EXIT;
    }


    void TimsPaManager::endLiveAnnouncementSingleTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                        TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                        const std::string& sessionId,
                                                        unsigned char origin )
    {
        FUNCTION_ENTRY( "endLiveAnnouncementSingleTrain" );

        // This ends a live announcement in progress
        // It assumes the call has just been ended
        // the announcement details must be reset in the train state
        // the reset command must be sent to the train, the result then sent out to clients
        //
        // stop the PA3 timer for this train
        stopPA3Timer( trainId );

        // get the PA state for the train
        TrainPaStatePtr paState = m_statusManager.getTrainPaState( trainId );

        TA_IRS_Bus::PaTypes::AnnouncementId stateAnnouncementId = 0;
        std::string staticGroup( "" );
        std::string stateSessionId( "" );
        unsigned long consoleId = 0;
        unsigned long callId = 0;

        {
            // acquire the train PA state lock
            TA_IRS_Bus::TrainStateLock stateLock( paState );

            // get the announcement ID from the state
            stateAnnouncementId = paState->getAnnouncementId();

            // get the static group, session id, console id, call id
            staticGroup = paState->getAnnouncementStaticGroup();
            stateSessionId = paState->getSessionId();
            consoleId = paState->getConsoleId();
            callId = paState->getCallId();

            // reset the announcement details
            paState->resetAnnouncementDetails();

            // clear the call
            paState->clearCall();

            // release the train PA state lock
        }

        // if the announcement ID in the state is 0
        // there is no need to do anything, this has been reset by some other means
        if ( 0 == stateAnnouncementId )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "PA live announcement for train %d has already ended.",
                         trainId );

            FUNCTION_EXIT;
            return;
        }

        TA_IRS_Bus::PaTypes::PaUpdateEvent update;
        update.trainId = trainId;
        update.announcementId = announcementId;
        update.sessionId = sessionId;
        update.updateType = TA_IRS_Bus::PaTypes::PaCommandResult;
        update.commandResult = TA_IRS_Bus::PaTypes::PaProceeding;
        update.completionStatus = TA_IRS_Bus::ProtocolPaTypes::Successful;

        try
        {
            // first check this agent is in control of this train
            performPreBroadcastTrainChecks( trainId );

            //TA_IRS_Bus::PaResetCommandPtr command(
            TA_IRS_Bus::PaLiveResetCommandPtr command(
                //new TA_IRS_Bus::PaResetCommand( trainId,
                new TA_IRS_Bus::PaLiveResetCommand( trainId,
                                                origin,
                                                announcementId ) );

            // get the train pa state for this train
            TrainPaStatePtr paState = m_statusManager.getTrainPaState( trainId );
            {
                // get the command lock and acquire it using a threadguard
                // for some reason it doesnt work all in one line
                TA_Base_Core::IThreadLockable& commandLock = paState->getCommandLock();
                TA_THREADGUARD( commandLock );

                // send the command
                m_transactionManager.sendCommand( command,
                                                  m_configuration.getTimerPA4(),
                                                  m_configuration.getTimsRetryLimit() );

                // release the command lock
            }

            // get the CommandReceivedResponse, the TrainMessageCompletedResponse, the bad command response, and the changeover status response from the command
            // if none of these are returned, ASSERT - the command should not exit normally without a valid resposne
            TA_IRS_Bus::PaCommandReceivedEventPtr commandReceived = command->getCommandReceivedResponse();
            TA_IRS_Bus::PaTrainMessageCompletedEventPtr messageCompleted = command->getTrainMessageCompletedResponse();
            TA_IRS_Bus::BadCommandEventPtr badCommand = command->getBadCommandResponse();
            TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover = command->getChangeoverStatusEventResponse();

            // if it is a PaCommandReceivedEvent
            if ( commandReceived )
            {
                // get the result of the command
                TA_IRS_Bus::ProtocolPaTypes::EPaCommandResult result = commandReceived->getCommandResult();

                // based on the result set the commandResult in the PaUpdateEvent to one of the Pa* error codes
                switch ( result )
                {

                    case TA_IRS_Bus::ProtocolPaTypes::PaProceeding:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaProceeding;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaInUse:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaInUse;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaInUseOptionToOverride:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaInUseOptionToOverride;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaInUseFirstCycleCancelled:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaInUseFirstCycleCancelled;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaReset:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaReset;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaFailed:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaFailed;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaDifferentOrigin:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaDifferentOrigin;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaDifferentId:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaDifferentId;
                        break;

                    case TA_IRS_Bus::ProtocolPaTypes::PaDifferentVersion:
                        update.commandResult = TA_IRS_Bus::PaTypes::PaDifferentVersion;
                        break;

                    default:
                        TA_ASSERT( false, "This value must be validated by the protocol library" );
                        break;
                }

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "PA Reset command for train %d returned a command received with status: %s",
                             trainId,
                             TA_IRS_Bus::ProtocolPaTypes::paCommandResultToString( result ).c_str() );
            }

            // if the message completed event is returned
            else if ( messageCompleted )
            {
                // get the result of the command
                TA_IRS_Bus::ProtocolPaTypes::EMessageCompletionStatus result = messageCompleted->getCompletionStatus();

                // set the updateType in the PaUpdateEvent to AnnouncementReset
                update.updateType = TA_IRS_Bus::PaTypes::AnnouncementReset;

                // based on the result set the completionStatus in the PaUpdateEvent to the correct status
                update.completionStatus = result;

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "PA Reset command for train %d returned a message completed with status: %s",
                             trainId,
                             TA_IRS_Bus::ProtocolPaTypes::messageCompletionStatusToString( result ).c_str() );
            }

            // if the bad command event is returned
            else if ( badCommand )
            {
                TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError errorCode = badCommand->getErrorCode();

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "PA Reset command for train %d returned an error: %s",
                             trainId,
                             TA_IRS_Bus::ProtocolCommonTypes::badCommandErrorToString( errorCode ).c_str() );

                // set the commandResult in the PaUpdateEvent to one of the BadCommand* error codes
                switch ( errorCode )
                {

                    case TA_IRS_Bus::ProtocolCommonTypes::BadChecksum:
                        update.commandResult = TA_IRS_Bus::PaTypes::BadCommandBadChecksum;
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::UnknownCommand:
                        update.commandResult = TA_IRS_Bus::PaTypes::BadCommandUnknownCommand;
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::BadParameter:
                        update.commandResult = TA_IRS_Bus::PaTypes::BadCommandBadParameter;
                        break;

                    case TA_IRS_Bus::ProtocolCommonTypes::UnexpectedCommand:
                        update.commandResult = TA_IRS_Bus::PaTypes::BadCommandUnexpectedCommand;
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
                             "PA Reset command for train %d returned an MPU changeover",
                             trainId );

                update.commandResult = TA_IRS_Bus::PaTypes::MpuChangeoverError;
            }
            else
            {
                // if no valid events were returned,
                // assert that the command completed without a valid response
                TA_ASSERT( false, "This command can only exit via exception or a valid response" );
            }

        }
        // catch any of the following exceptions, log the exception, and set the appropriate commandResult in the PaUpdateEvent
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );

            // set the error status
            update.commandResult = TA_IRS_Bus::PaTypes::InvalidTrainError;
        }
        catch ( TA_IRS_Bus::TrainAccessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );

            // set the error status
            update.commandResult = TA_IRS_Bus::PaTypes::InvalidTrainError;
        }
        catch ( TA_IRS_Bus::TrainTransmissionFailureException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );

            // set the error status
            update.commandResult = TA_IRS_Bus::PaTypes::TransmissionFailureError;
        }
        catch ( TA_IRS_Bus::TrainTimeoutException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimeoutException", e.what() );

            // set the error status
            update.commandResult = TA_IRS_Bus::PaTypes::TransactionTimeoutError;
        }
        catch ( TA_IRS_Bus::TransactionCancelledException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactionCancelledException", e.what() );

            // set the error status
            update.commandResult = TA_IRS_Bus::PaTypes::TransactionCancelledError;
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

        // send the PaUpdateEvent
        m_messageSender.sendClientUpdate( update );

        FUNCTION_EXIT;
    }


    void TimsPaManager::performPreBroadcastTrainChecks( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "performPreBroadcastTrainChecks" );

        // check this agent is in control of this train
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

        // if the above tests pass, the operator can start a train broadcast

        FUNCTION_EXIT;
    }


    void TimsPaManager::processPaTrainMessageCompletedEvent( TA_IRS_Bus::PaTrainMessageCompletedEventPtr event )
    {
        FUNCTION_ENTRY( "processPaTrainMessageCompletedEvent" );

        // This is simply to notify ISCS that a DVA or cyclic PA broadcast on a train has completed
        // Based on the completion status all that needs to be done is to notify clients of the completion

        // Create a PaUpdateEvent
        TA_IRS_Bus::PaTypes::PaUpdateEvent update;
        update.trainId = event->getTrainId();
        update.announcementId = static_cast<TA_IRS_Bus::PaTypes::AnnouncementId>( event->getAnnouncementId() );
        update.sessionId = "";
        update.updateType = TA_IRS_Bus::PaTypes::CompletedAnnouncement;
        update.commandResult = TA_IRS_Bus::PaTypes::PaProceeding;   // not used
        update.completionStatus = event->getCompletionStatus();

        // send the PaUpdateEvent
        m_messageSender.sendClientUpdate( update );

        FUNCTION_EXIT;
    }


    void TimsPaManager::processPaReadyForLiveAnnouncementEvent( TA_IRS_Bus::PaReadyForLiveAnnouncementEventPtr event )
    {
        FUNCTION_ENTRY( "processPaReadyForLiveAnnouncementEvent" );

        // get the PA state for the train
        TrainPaStatePtr paState = m_statusManager.getTrainPaState( event->getTrainId() );

        TA_IRS_Bus::PaTypes::AnnouncementId stateAnnouncementId = 0;

        {
            // acquire the train PA state lock
            TA_IRS_Bus::TrainStateLock stateLock( paState );

            // get the announcement ID from the state
            stateAnnouncementId = paState->getAnnouncementId();

            // release the train PA state lock
        }

        // if the announcement ID in the event and the announcement ID in the state are both 0
        if ( ( 0 == event->getAnnouncementId() ) &&
             ( 0 == stateAnnouncementId ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "Ready for live announcement received from train %d with no ongoing live announcement",
                         event->getTrainId() );

            FUNCTION_EXIT;
            return;
        }

        // if the announcement ID in the event doesnt match the announcement ID in the state
        if ( event->getAnnouncementId() != stateAnnouncementId )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "Ready for live announcement received from train %d with announcement ID %d when train has announcement ID %d",
                         event->getTrainId(), event->getAnnouncementId(), stateAnnouncementId );

            FUNCTION_EXIT;
            return;
        }

        // otherwise this ready is for this announcement,
        // using m_transactionManager.signalTrainTimer signal the timer for this train for TrainPaResource
        try
        {
            m_transactionManager.signalTrainTimer( event->getTrainId(), TA_IRS_Bus::CommonTypes::TrainPaResource );
        }
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );
        }

        FUNCTION_EXIT;
    }


    void TimsPaManager::processPaResetEvent( TA_IRS_Bus::PaResetEventPtr event )
    {
        FUNCTION_ENTRY( "processPaResetEvent" );

        // This is sent when there was a PA live announcement ongoing on the train, and it has been reset by TIMS
        // Any PA live broadcast in progress must be reported as reset
        // no commands are sent to the train
        // if there is a call active, and this is the only train in the call, the call must be terminated

        // stop the PA3 timer for this train
        stopPA3Timer( event->getTrainId() );

        // get the PA state for the train
        TrainPaStatePtr paState = m_statusManager.getTrainPaState( event->getTrainId() );

        TA_IRS_Bus::PaTypes::AnnouncementId stateAnnouncementId = 0;
        std::string staticGroup( "" );
        std::string stateSessionId( "" );
        unsigned long consoleId = 0;
        unsigned long callId = 0;

        {
            // acquire the train PA state lock
            TA_IRS_Bus::TrainStateLock stateLock( paState );

            // get the announcement ID from the state
            stateAnnouncementId = paState->getAnnouncementId();

            // get the static group, session id, console id, call id
            staticGroup = paState->getAnnouncementStaticGroup();
            stateSessionId = paState->getSessionId();
            consoleId = paState->getConsoleId();
            callId = paState->getCallId();

            // reset the announcement details
            paState->resetAnnouncementDetails();

            // clear the call
            paState->clearCall();

            // if there is an ongoing announcement
            if ( 0 != stateAnnouncementId )
            {
                // notify the client this announcement has been reset
                TA_IRS_Bus::PaTypes::PaUpdateEvent update;
                update.trainId = event->getTrainId();
                update.announcementId = stateAnnouncementId;
                update.sessionId = stateSessionId;
                update.updateType = TA_IRS_Bus::PaTypes::AnnouncementReset;
				if(event->getStatus() == TA_IRS_Bus::PaTypes::PaFailed)
				{
	                update.commandResult = TA_IRS_Bus::PaTypes::PaFailed;
				}
				else
				{
	                update.commandResult = TA_IRS_Bus::PaTypes::PaReset;
				}
                update.completionStatus = TA_IRS_Bus::ProtocolPaTypes::Successful; // not used

                // send the PaUpdateEvent
                m_messageSender.sendClientUpdate( update );
            }

            // release the train PA state lock
        }



        // if the announcement ID in the PA state differs from the announcement ID received in the event
        // and the announcement in the event is not 0
        // then a second message needs to be sent to the clients
        if ( ( event->getAnnouncementId() != stateAnnouncementId ) &&
             ( event->getAnnouncementId() != 0 ) )
        {

            // log a warning message to indicate the PA state and the event had different announcement IDs
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "PA Reset received from train %d with announcement ID %d when train has announcement ID %d",
                         event->getTrainId(), event->getAnnouncementId(), stateAnnouncementId );

            TA_IRS_Bus::PaTypes::PaUpdateEvent update;
            update.trainId = event->getTrainId();
            update.announcementId = static_cast<TA_IRS_Bus::PaTypes::AnnouncementId>( event->getAnnouncementId() );
            update.sessionId = stateSessionId;
            update.updateType = TA_IRS_Bus::PaTypes::AnnouncementReset;
			if(event->getStatus() == TA_IRS_Bus::PaTypes::PaFailed)
			{
                update.commandResult = TA_IRS_Bus::PaTypes::PaFailed;
			}
			else
			{
                update.commandResult = TA_IRS_Bus::PaTypes::PaReset;
			}
            update.completionStatus = TA_IRS_Bus::ProtocolPaTypes::Successful; // not used

            // send the PaUpdateEvent
            m_messageSender.sendClientUpdate( update );
        }

        // in case there was a setup in progress, signal the train timer using m_transactionManager.signalTrainTimer for the TrainPaResource
        // catch the exception that can possibly be thrown and continue
        try
        {
            m_transactionManager.signalTrainTimer( event->getTrainId(), TA_IRS_Bus::CommonTypes::TrainPaResource );
        }
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );
        }

        // if there was a call ID set, we must now see if it must be ended
        if ( ( 0 != callId ) &&
             ( 0 != consoleId ) )
        {
            try
            {
                // get the details for the call id and console ID
                // if there is nothing found an exception will be thrown
                TA_IRS_Bus::StateUpdateTypes::RadioCallDetails callDetails = m_statusManager.getDetailsForRadioCall( consoleId, callId );

                // get the trains involved in the call
                TA_IRS_Bus::CommonTypes::TrainIdList trainList = m_statusManager.getTrainsInvolvedInRadioCall( callDetails );

                // if there are none then this train was the last one and has just been removed
                if ( true == trainList.empty() )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                 "PA Reset received from last train (%d) in announcement %d ending call",
                                 event->getTrainId(), stateAnnouncementId );

                    // the call must be ended
                    m_radioAgent.endCall( callId, stateSessionId );
                }
            }
            catch ( TA_IRS_Bus::TrainRadioException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainRadioException", e.what() );
            }
        }

        // If there was a locked radio group, this must also be unlocked if it is no longer in use
        if ( false == staticGroup.empty() )
        {
            // get the static group for the announcement ID that was in the train state
            std::string foundGroup = m_statusManager.getStaticGroupForLiveAnnouncement( stateAnnouncementId );

            if ( foundGroup != staticGroup )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "PA Reset received from last train (%d) in announcement %d unlocking radio group %s",
                             event->getTrainId(), stateAnnouncementId, staticGroup.c_str() );

                // unlock the group
                m_radioGroupCoordinator.unlockRadioGroup( staticGroup );
            }
        }

        FUNCTION_EXIT;
    }


    void TimsPaManager::processPaRequestForResetEvent( TA_IRS_Bus::PaRequestForResetEventPtr event )
    {
        FUNCTION_ENTRY( "processPaRequestForResetEvent" );

        // get the PA state for the train
        TrainPaStatePtr paState = m_statusManager.getTrainPaState( event->getTrainId() );

        TA_IRS_Bus::PaTypes::AnnouncementId stateAnnouncementId = 0;
        bool continueRequired = false;
        std::string sessionId;

        {
            // acquire the train PA state lock
            TA_IRS_Bus::TrainStateLock stateLock( paState );

            // get the announcement ID from the state
            stateAnnouncementId = paState->getAnnouncementId();

            // if the announcement ID in the event and the announcement ID in the state are both 0
            if ( ( 0 == event->getAnnouncementId() ) &&
                 ( 0 == stateAnnouncementId ) )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "Request for reset received from train %d with no ongoing live announcement",
                             event->getTrainId() );

                FUNCTION_EXIT;
                return;
            }

            // if the announcement ID in the event doesnt match the announcement ID in the state
            if ( event->getAnnouncementId() != stateAnnouncementId )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                             "Request for reset received from train %d with announcement ID %d when train has announcement ID %d",
                             event->getTrainId(), event->getAnnouncementId(), stateAnnouncementId );

                FUNCTION_EXIT;
                //return; //limin CL-20750
            }

            // get the session id
            std::string sessionId = paState->getSessionId();

            // gets the auto continue ack count
            // if it is less than the auto continue ack limit, continue is not required, otherwise it is
            continueRequired = ( paState->getAutoContinueCount() >= m_configuration.getTimsAutoContinueAckLimit() ) ;

            // prepare the update
            TA_IRS_Bus::PaTypes::PaUpdateEvent update;
            update.trainId = event->getTrainId();
            update.announcementId = stateAnnouncementId;
            update.sessionId = sessionId;
            update.updateType = TA_IRS_Bus::PaTypes::LiveAnnouncementContinueRequired;
            update.commandResult = TA_IRS_Bus::PaTypes::PaProceeding;
            update.completionStatus = TA_IRS_Bus::ProtocolPaTypes::Successful; // not used

            // if continue is not required
            if ( false == continueRequired )
            {
                // increment the auto continue count
                paState->incrementAutoContinueCount();

                // signal the continue request should be reset
                update.commandResult = TA_IRS_Bus::PaTypes::PaReset;
            }

            // otherwise if it is
            else
            {
                // set that continue is required
                paState->setContinueRequired( event->getTime() );
            }


            // send the PaUpdateEvent
            m_messageSender.sendClientUpdate( update );

            // release the train PA state lock
        }

        // stop the PA3 timer for the train
        stopPA3Timer( event->getTrainId() );

        // if continue is not required
        // then an automatic continue needs to be sent
        if ( false == continueRequired )
        {
            try
            {
                // create a PaContinueCommand with the announcement id from the state
                TA_IRS_Bus::PaContinueCommandPtr command(
                    new TA_IRS_Bus::PaContinueCommand( event->getTrainId(),
                                                       m_configuration.getOperatorOrigin( sessionId ),
                                                       stateAnnouncementId ) );

                // send the command (use the default timeout and retries as this command has no response)
                m_transactionManager.sendCommand( command );

                // start the PA3 timer for this train
                startPA3Timer( event->getTrainId() );
            }
            catch ( TA_IRS_Bus::TrainException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainException", e.what() );
            }
        }

        FUNCTION_EXIT;
    }


    void TimsPaManager::processChangeoverStatusEvent( TA_IRS_Bus::ChangeoverStatusEventPtr event )
    {
        FUNCTION_ENTRY( "processChangeoverStatusEvent" );

        // This is when there is an MPU failure for a train
        // Any PA live broadcast in progress must be reported as failed due to MPU failure
        // no commands are sent to the train
        // if there is a call active, and this is the only train in the call, the call must be terminated

        // stop the PA3 timer for this train
        stopPA3Timer( event->getTrainId() );

		

        // get the PA state for the train
        TrainPaStatePtr paState = m_statusManager.getTrainPaState( event->getTrainId() );

        TA_IRS_Bus::PaTypes::AnnouncementId stateAnnouncementId = 0;
        std::string staticGroup( "" );
        std::string stateSessionId( "" );
        unsigned long consoleId = 0;
        unsigned long callId = 0;
		
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					 "Start MPU Changeover received from last train (%d) in announcement %d ending call",
				     event->getTrainId(), stateAnnouncementId );

        {
            // acquire the state lock
            TA_IRS_Bus::TrainStateLock stateLock( paState );

            // check if there is a PA announcement in progress (get the announcement ID)
            stateAnnouncementId = paState->getAnnouncementId();

            // if the announcement ID is 0, return since there is no broadcast on this train
            // remove this if MPU changeover should cause DVA to be aborted
            // also see in PA Agent TrainAgentSubscriber.cpp
            /*
            if ( 0 == stateAnnouncementId )
            {
                FUNCTION_EXIT;
                return;
            }
            */

            // get the static group, session id, console id, call id
            staticGroup = paState->getAnnouncementStaticGroup();
            stateSessionId = paState->getSessionId();
            consoleId = paState->getConsoleId();
            callId = paState->getCallId();

            // reset the announcement details
            paState->resetAnnouncementDetails();

            TA_IRS_Bus::PaTypes::PaUpdateEvent update;
            update.trainId = event->getTrainId();
            update.announcementId = stateAnnouncementId;
            update.sessionId = stateSessionId;
            update.updateType = TA_IRS_Bus::PaTypes::AnnouncementReset;
            update.commandResult = TA_IRS_Bus::PaTypes::MpuChangeoverError;
            update.completionStatus = TA_IRS_Bus::ProtocolPaTypes::Successful; // not used

            // send the PaUpdateEvent
            m_messageSender.sendClientUpdate( update );

            // release the state lock
        }

        // in case there was a setup in progress, signal the train timer
        // catch the exception that can possibly be thrown and continue
        try
        {
            m_transactionManager.signalTrainTimer( event->getTrainId(), TA_IRS_Bus::CommonTypes::TrainPaResource );
        }
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );
        }

        // if there was a call ID set, we must now see if it must be ended:
        if ( ( 0 != callId ) &&
             ( 0 != consoleId ) )
        {
            try
            {
                // get the details for the call id and console ID
                // if there is nothing found an exception will be thrown
                TA_IRS_Bus::StateUpdateTypes::RadioCallDetails callDetails = m_statusManager.getDetailsForRadioCall( consoleId, callId );

                // get the trains involved in the call
                TA_IRS_Bus::CommonTypes::TrainIdList trainList = m_statusManager.getTrainsInvolvedInRadioCall( callDetails );


                // if there are none then this train was the last one and has just been removed

                if ( true == trainList.empty())
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                                 "MPU Changeover received from last train (%d) in announcement %d ending call",
                                 event->getTrainId(), stateAnnouncementId );

					TA_IRS_Bus::ChangeoverStatusEventPtr mpuChangeover (event);

					// reset pec call when the status failure reason is due to ACSU failure
					if (mpuChangeover->getStatus ())
					{
						LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"ChangeOver Due to ACSU Failure-- Ending Current Call");
						m_radioAccess.endPecCall(stateSessionId,callId);
					}

					
                }
				m_statusManager.removeRadioCall (consoleId,callId);
            }
            catch ( TA_IRS_Bus::TrainRadioException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainRadioException", e.what() );
            }
        }

        // If there was a locked radio group, this must also be unlocked if it is no longer in use
        if ( false == staticGroup.empty() )
        {
            // get the static group for the announcement ID that was in the train state
            std::string foundGroup = m_statusManager.getStaticGroupForLiveAnnouncement( stateAnnouncementId );

            if ( foundGroup != staticGroup )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "PA Reset received from last train (%d) in announcement %d unlocking radio group %s",
                             event->getTrainId(), stateAnnouncementId, staticGroup.c_str() );

                // unlock the group
                m_radioGroupCoordinator.unlockRadioGroup( staticGroup );
            }
        }
		
		// clear the call
		paState->clearCall();

        FUNCTION_EXIT;
    }


    void TimsPaManager::processAudioCabToCabEvent( TA_IRS_Bus::AudioCabToCabEventPtr event )
    {
        FUNCTION_ENTRY( "processAudioCabToCabEvent" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Audio Cab to Cab received for train %d",
                     event->getTrainId() );

        FUNCTION_EXIT;
    }


    void TimsPaManager::processPaAudioSwitchEvent( TA_IRS_Bus::PaAudioSwitchEventPtr event )
    {
        FUNCTION_ENTRY( "processPaAudioSwitchEvent" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "PA Audio Switch received for train %d",
                     event->getTrainId() );

        FUNCTION_EXIT;
    }


    void TimsPaManager::startPA3Timer( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "startPA3Timer" );

        // start the PA3 timer for this train
        m_pa3TimerManager->startTimer( trainId,
                                       m_configuration.getTimerPA3(),
                                       true,
                                       PA3_TIMER_TEXT );

        FUNCTION_EXIT;
    }


    void TimsPaManager::stopPA3Timer( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "stopPA3Timer" );

        // stop the PA3 timer for this train
        m_pa3TimerManager->endTimer( trainId );

        FUNCTION_EXIT;
    }


    void TimsPaManager::onPA3Timeout( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "onPA3Timeout" );

        // This is called when an expected PA request for reset is not received from TIMS during a live announcement
        // It doesnt react in any way, other than notifying any observers (PA Agent) that the train communication MAY be interrupted

        // return if this agent isnt in the control state
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // This is only applicable if in fact there is an ongoing live announcement for the train.

        // get the train PA state for the given train
        TrainPaStatePtr paState = m_statusManager.getTrainPaState( trainId );

        // acquire the state lock
        TA_IRS_Bus::TrainStateLock stateLock( paState );

        // get the announcement ID
        TA_IRS_Bus::PaTypes::AnnouncementId announcementId = paState->getAnnouncementId();

        // if there is no announcement ID (its 0) return
        if ( 0 == announcementId )
        {
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                     "Expected PA Request for reset not received for train %d. PA Live announcement on this train may not be operating correctly",
                     trainId );

        // create a PaUpdateEvent
        TA_IRS_Bus::PaTypes::PaUpdateEvent update;
        update.trainId = trainId;
        update.announcementId = announcementId;
        update.sessionId = paState->getSessionId();
        update.updateType = TA_IRS_Bus::PaTypes::LiveAnnouncementContinueTimeout;
        update.commandResult = TA_IRS_Bus::PaTypes::PaProceeding;   // not used
        update.completionStatus = TA_IRS_Bus::ProtocolPaTypes::Successful;   // not used

        // send the PaUpdateEvent
        m_messageSender.sendClientUpdate( update );

        // release the train state lock

        FUNCTION_EXIT;
    }
}


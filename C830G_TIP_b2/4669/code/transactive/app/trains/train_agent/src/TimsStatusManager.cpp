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

#include "app/trains/train_agent/src/TimsStatusManager.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"
#include "app/trains/train_agent/src/IOperationModeManager.h"
#include "app/trains/train_agent/src/ITimsModeManagerInternal.h"
#include "app/trains/train_agent/src/IAlarmUtility.h"
#include "app/trains/train_agent/src/IMessageSender.h"
#include "app/trains/train_agent/src/IStateUpdateMessageSender.h"
#include "app/trains/train_agent/src/AtsMessageProcessor.h"
#include "app/trains/train_agent/src/IAtsAgentInterface.h"
#include "app/trains/train_agent/src/TrainActivityMonitor.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

#include "bus/trains/TrainTransactionModel/src/ITrainTransactionManager.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"
#include "bus/trains/TrainProtocolLibrary/src/TestCallResultCommand.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include <ctime>
#include<map>
using std::map;
namespace TA_IRS_App
{

    TimsStatusManager::TimsStatusManager( ITrainAgentConfiguration& configuration,
                                          ITrainStatusManager& statusManager,
                                          TA_IRS_Bus::ITrainTransactionManager& transactionManager,
                                          IOperationModeManager& operationModeManager,
                                          ITimsModeManagerInternal& modeManager,
                                          IAlarmUtility& alarms,
                                          IMessageSender& messageSender,
                                          IStateUpdateMessageSender& stateUpdateSender,
                                          AtsMessageProcessor& atsMessageProcessor,
                                          IAtsAgentInterface& atsAgent )
            : m_configuration( configuration ),
              m_statusManager( statusManager ),
              m_transactionManager( transactionManager ),
              m_operationModeManager( operationModeManager ),
              m_modeManager( modeManager ),
              m_alarms( alarms ),
              m_messageSender( messageSender ),
              m_stateUpdateSender( stateUpdateSender ),
              m_atsMessageProcessor( atsMessageProcessor ),
              m_atsAgent( atsAgent ),
              m_activityMonitor( NULL ),
              m_atsAgentCommsOk( true )
    {
        FUNCTION_ENTRY( "TimsStatusManager" );

        // create the activity monitor
        // note this could be created in the initialiser list by value,
        // however its not good to use 'this' in the initialser list,
        // so its best to manage creation manually
        m_activityMonitor = new TrainActivityMonitor( configuration.getOa1Timeout(),
                                                      configuration.getAtcTimeout(),
                                                      *this );

        // register for operaton mode changes
        m_operationModeManager.registerForOperationModeChanges( this );

        // register for ats changes
        m_atsMessageProcessor.addObserver( this );

        // register for the following train events
        // TestCallEvent::getStaticType()
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::TestCallEvent::getStaticType(), this );
        // AtcEvent::getStaticType()
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::AtcEvent::getStaticType(), this );
        // Oa1Event::getStaticType()
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::Oa1Event::getStaticType(), this );
        // ChangeoverStatusEvent::getStaticType()
        m_transactionManager.registerForTrainEvents( TA_IRS_Bus::ChangeoverStatusEvent::getStaticType(), this );

        // register for operation state callbacks
        //TA_Base_Bus::AgentManager::registerForStateChangeCallbacks( this );

        FUNCTION_EXIT;
    }


    TimsStatusManager::~TimsStatusManager()
    {
        FUNCTION_ENTRY( "~TimsStatusManager" );

        // deregister for everything registered for in the constructor

        // deregister for operation state callbacks
        //TA_Base_Bus::AgentManager::deregisterForStateChangeCallbacks( this );

        // deregister for the following train events
        // TestCallEvent
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::TestCallEvent::getStaticType(), this );
        // AtcEvent
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::AtcEvent::getStaticType(), this );
        // Oa1Event
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::Oa1Event::getStaticType(), this );
        // ChangeoverStatusEvent
        m_transactionManager.deregisterForTrainEvents( TA_IRS_Bus::ChangeoverStatusEvent::getStaticType(), this );

        // deregister for ats changes
        m_atsMessageProcessor.removeObserver( this );

        // deregister for operaton mode changes
        m_operationModeManager.deregisterForOperationModeChanges( this );

        // clean up the activity monitor
        delete m_activityMonitor;
        m_activityMonitor = NULL;

        FUNCTION_EXIT;
    }

    void TimsStatusManager::notifyOperationStateChange( TA_Base_Core::EOperationMode operationState )
    {
        FUNCTION_ENTRY( "notifyOperationStateChange" );

        // if the mode is not control, return
        if ( TA_Base_Core::Control != operationState )
        {
            FUNCTION_EXIT;
            return;
        }

        // get all train comms states
        TA_IRS_Bus::TrainIdToStateMap stateMap = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::CoreResource );

        for ( TA_IRS_Bus::TrainIdToStateMap::iterator stateIter = stateMap.begin();
              stateMap.end() != stateIter; ++stateIter )
        {
            // if this agent is in control of the train, add it for monitoring
            if ( true == m_operationModeManager.isInControlOfTrain( stateIter->first, true, false ) )
            {
                m_activityMonitor->addTrain( stateIter->first );
            }
        }

        // close any open ATS agent comms alarms
        m_atsAgentCommsOk = true;
        m_alarms.closeAtsAgentCommsAlarm();

        FUNCTION_EXIT;
    }


    void TimsStatusManager::processAtsUpdate( const AtsTrainInformation& oldInformation,
                                              const AtsTrainInformation& newInformation )
    {
        FUNCTION_ENTRY( "processAtsUpdate" );

        // check the agent is in the control state using AgentManager, if not, return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        bool wasInArea = false;
        bool isInArea = false;

        // for the oldInformation
        // if it is valid, and this agent is in control of the location
        // wasInArea = true
        wasInArea = ( true == oldInformation.trainValid ) &&
                    ( true == m_operationModeManager.isInControlOfLocation( oldInformation.localisation.currentLocation ) );

        // for the newInformation
        // if it is valid (true == trainValid), and this agent is in control of the location (use m_operationModeManager.isInControlOfLocation)
        // isInArea = true
        isInArea = ( true == newInformation.trainValid ) &&
                   ( true == m_operationModeManager.isInControlOfLocation( newInformation.localisation.currentLocation ) );

        // if the train was in this agents area and it now isnt
        // then all alarms should be closed for the train

        if ( ( true == wasInArea ) &&
             ( false == isInArea ) )
        {
            closeAlarmsForTrain( newInformation.physicalTrainNumber );
        }

        // check the trains validity - get the train comms state
        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( newInformation.physicalTrainNumber );

        bool trainValid = false;

        {
            // acquire the lock for the train state
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // get the overall validity
            trainValid = commsState->isValid();

            // release the lock for the train state
        }

        // if the validity is false then this train has either been shut down, or crossed the zone boundary
        // its state must be reset if it is no longer communicating
        if ( false == trainValid )
        {
            resetTrainState( commsState );
        }

        // if this train was not in this trains control area (false == wasInArea) and it now is (true == isInArea)
        // schedule its timers using m_activityMonitor
        if ( ( false == wasInArea ) &&
             ( true == isInArea ) )
        {
            m_activityMonitor->addTrain( newInformation.physicalTrainNumber );
        }

        FUNCTION_EXIT;
    }


    void TimsStatusManager::processOa1Timeout( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "processOa1Timeout" );

        // check the agent is in the control state using AgentManager, if not, return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the train comms state
        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

        bool oa1Valid = false;

        unsigned long communicatingLocation = 0;

        {
            // acquire the lock for the train state
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // get the train's OA1 validity
            oa1Valid = commsState->isOa1Valid();

            // get the trains communicating location
            communicatingLocation = commsState->getCommunicatingLocation();

            // release the lock for the train state
        }

        // if the OA1 validity is already false, return
        if ( false == oa1Valid )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the current timestamp
        time_t timestamp = time( NULL );

        // check if this agent should be in control of the train
        if ( true == m_operationModeManager.isInControlOfTrain( trainId, true, true ) )
        {
            bool trainValid = true;

            {
                // acquire the lock for the train state
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                // set the OA1 validity to false
                commsState->setOa1Validity( false, timestamp );

                // get the overall vadity
                trainValid = commsState->isValid();

                // release the lock for the train state
            }

            // if the train is still valid
            if ( true == trainValid )
            {
                // get the alarm state
                TrainAlarmStatePtr alarmState = m_statusManager.getTrainAlarmState( trainId );

                // acquire the lock for the train state
                TA_IRS_Bus::TrainStateLock stateLock( alarmState );

                // get the OA1 alarm ID
                // if it is empty, raise an OA1 alarm and set the OA1 alarm ID
                if ( true == alarmState->getOa1AlarmId().empty() )
                {
                    alarmState->setOa1AlarmId( m_alarms.raiseOa1AtcTimerAlarm( trainId, "OA1", timestamp ) );
                }

                // release the lock for the train state
            }

            //    otherwise if it is not valid
            else
            {
                resetTrainState( commsState );
            }
        }

        // otherwise, if the communicating location is this agent's location
        else if ( m_configuration.getLocationKey() == communicatingLocation )
        {
            bool trainValid = false;

            {
                // acquire the lock for the train state
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                // set the OA1 validity to false
                commsState->setOa1Validity( false, timestamp );

                // get the overall validity
                trainValid = commsState->isValid();

                // release the lock for the train state
            }

            // if the validity is false then the train is gone from ATS and from radio
            // its state must be reset if it is no longer communicating
            if ( false == trainValid )
            {
                resetTrainState( commsState );
            }
        }

        // forward a blank OA1 info (empty vector) to the ATS agent
        TA_IRS_Bus::TrainMessageData emptyVector;

        try
        {
            m_atsAgent.sendOa1TableToAts( trainId, emptyVector );

            if ( false == m_atsAgentCommsOk )
            {
                m_alarms.closeAtsAgentCommsAlarm();
                m_atsAgentCommsOk = true;
            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "While forwarding data to ATS" );

            if ( true == m_atsAgentCommsOk )
            {
                m_alarms.raiseAtsAgentCommsAlarm( m_atsAgent.getAtsAgentName() );
                m_atsAgentCommsOk = true;
            }
        }

        FUNCTION_EXIT;
    }


    void TimsStatusManager::processAtcTimeout( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "processAtcTimeout" );

        // check the agent is in the control state using AgentManager, if not, return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the train comms state
        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

        bool atsValid = false;

        unsigned long communicatingLocation = 0;

        {
            // acquire the lock for the train state
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // get the train's ATC validity
            atsValid = commsState->isAtcValid();

            // get the trains communicating location
            communicatingLocation = commsState->getCommunicatingLocation();

            // release the lock for the train state
        }

        // if the ATC validity is already false, return
        if ( false == atsValid )
        {
            FUNCTION_EXIT;
            return;
        }

        // get the current timestamp
        time_t timestamp = time( NULL );

        // check if this agent should be in control of the train
        if ( true == m_operationModeManager.isInControlOfTrain( trainId, true, true ) )
        {
            bool trainValid = true;

            {
                // acquire the lock for the train state
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                // set the ATC validity to false
                commsState->setAtcValidity( false, timestamp );

                // get the overall vadity
                trainValid = commsState->isValid();

                // release the lock for the train state
            }

            // if the train is still valid
            if ( true == trainValid )
            {
                // get the alarm state
                TrainAlarmStatePtr alarmState = m_statusManager.getTrainAlarmState( trainId );

                // acquire the lock for the train state
                TA_IRS_Bus::TrainStateLock stateLock( alarmState );

                // get the ATC alarm ID
                // if it is empty, raise an ATC alarm and set the ATC alarm ID
                if ( true == alarmState->getAtcAlarmId().empty() )
                {
                    alarmState->setAtcAlarmId( m_alarms.raiseOa1AtcTimerAlarm( trainId, "ATC", timestamp ) );
                }

                // release the lock for the train state
            }

            //    otherwise if it is not valid
            else
            {
                resetTrainState( commsState );
            }
        }

        // otherwise, if the communicating location is this agent's location
        else if ( m_configuration.getLocationKey() == communicatingLocation )
        {
            bool trainValid = false;

            {
                // acquire the lock for the train state
                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                // set the ATC validity to false
                commsState->setAtcValidity( false, timestamp );

                // get the overall validity
                trainValid = commsState->isValid();

                // release the lock for the train state
            }

            // if the validity is false then the train is gone from ATS and from radio
            // its state must be reset if it is no longer communicating
            if ( false == trainValid )
            {
                resetTrainState( commsState );
            }
        }

        // forward a blank ATC info (empty vector) to the ATS agent
        TA_IRS_Bus::TrainMessageData emptyVector;

        try
        {
            m_atsAgent.sendAtcTrainStatusReportToAts( trainId, emptyVector );

            if ( false == m_atsAgentCommsOk )
            {
                m_alarms.closeAtsAgentCommsAlarm();
                m_atsAgentCommsOk = true;
            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "While forwarding data to ATS" );

            if ( true == m_atsAgentCommsOk )
            {
                m_alarms.raiseAtsAgentCommsAlarm( m_atsAgent.getAtsAgentName() );
                m_atsAgentCommsOk = true;
            }
        }

        FUNCTION_EXIT;
    }


    void TimsStatusManager::locationControlAcquired( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "locationControlAcquired" );

        // check the agent is in the control state using AgentManager, if not, return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // using m_statusManager get all train comms states
        TA_IRS_Bus::TrainIdToStateMap stateMap = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::CoreResource );

        TA_IRS_Bus::TrainInformationTypes::TrainDetailsList trainList;

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

            // if the ATS is valid, and
            // if the location matches locationKey (ignore localisation validity)
            if ( ( true == atsValid ) &&
                 ( trainLocation == locationKey ) )
            {
                // add the train for monitoring
                m_activityMonitor->addTrain( commsState->getTrainId() );
            }
        }

        FUNCTION_EXIT;
    }


    void TimsStatusManager::locationControlReleased( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "locationControlReleased" );

        // check the agent is in the control state using AgentManager, if not, return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // using m_statusManager get all train comms states
        TA_IRS_Bus::TrainIdToStateMap stateMap = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::CoreResource );

        TA_IRS_Bus::TrainInformationTypes::TrainDetailsList trainList;

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

            // if the ATS is valid, and
            // if the location matches locationKey (ignore localisation validity)
            if ( ( true == atsValid ) &&
                 ( trainLocation == locationKey ) )
            {
                // add the train for monitoring
                closeAlarmsForTrain( commsState->getTrainId() );
            }
        }

        FUNCTION_EXIT;
    }


    void TimsStatusManager::processEvent( TA_IRS_Bus::TrainEventPtr event )
    {
        FUNCTION_ENTRY( "processEvent" );

        // return if this agent isnt in the control state
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        TA_IRS_Bus::TimsEventPtr timsEvent = boost::dynamic_pointer_cast< TA_IRS_Bus::TimsEvent >( event );
        TA_ASSERT( NULL != timsEvent.get(), "Everything is a TimsEvent!" );

        TA_IRS_Bus::CommonTypes::TrainIdType trainId = event->getTrainId();
        TA_IRS_Bus::TrainMessageType messageType = timsEvent->getEventType();

        // get the train comms state for this train
        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

        bool setTsiAsPrimary = true;
        bool messageHandled = false;

        if ( messageType == TA_IRS_Bus::ChangeoverStatusEvent::getStaticType() )
        {
            TA_IRS_Bus::ChangeoverStatusEventPtr changeoverStatusEvent = boost::dynamic_pointer_cast< TA_IRS_Bus::ChangeoverStatusEvent >( event );
            TA_ASSERT( NULL != changeoverStatusEvent.get(), "Event received identified as a changeover status event but did not cast to it" );

            setTsiAsPrimary = processChangeoverStatusEvent( changeoverStatusEvent, commsState );
            messageHandled = true;
        }

        unsigned long trainLocation = 0;

        {
            // acquire the lock
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // set the tsi for the MPU this message comes from (getOrigin), using setTrainTsi (set this as the primary TSI)
            commsState->setTrainTsi( timsEvent->getOrigin(),
                                     timsEvent->getTsi(),
                                     timsEvent->getTime(),
                                     setTsiAsPrimary );

            trainLocation = commsState->getLocation();

            // release the lock
        }

        // check the event type, and handle it if it is one of the following:
        // TestCallEvent
        // AtcEvent
        // Oa1Event
        // ChangeoverStatusEvent
        // if it is any other type, assert that an unsubscribed event type was received.

        if ( messageType == TA_IRS_Bus::TestCallEvent::getStaticType() )
        {
            TA_IRS_Bus::TestCallEventPtr testCallEvent = boost::dynamic_pointer_cast< TA_IRS_Bus::TestCallEvent >( event );
            TA_ASSERT( NULL != testCallEvent.get(), "Event received identified as a test call but did not cast to it" );

            processTestCallEvent( testCallEvent, commsState );
        }
        else if ( messageType == TA_IRS_Bus::AtcEvent::getStaticType() )
        {
            TA_IRS_Bus::AtcEventPtr atcEvent = boost::dynamic_pointer_cast< TA_IRS_Bus::AtcEvent >( event );
            TA_ASSERT( NULL != atcEvent.get(), "Event received identified as an ATC event but did not cast to it" );

            processAtcEvent( atcEvent, commsState );
        }
        else if ( messageType == TA_IRS_Bus::Oa1Event::getStaticType() )
        {
            TA_IRS_Bus::Oa1EventPtr oa1Event = boost::dynamic_pointer_cast< TA_IRS_Bus::Oa1Event >( event );
            TA_ASSERT( NULL != oa1Event.get(), "Event received identified as an OA1 event but did not cast to it" );

            processOa1Event( oa1Event, commsState );
        }
        else if ( false == messageHandled )
        {
            TA_ASSERT( false, "The event type received was not subscribed for" );
        }

        // check m_operationModeManager if this agent should be in control of the given train
        // if it shouldnt be, then using m_stateUpdateSender, send out a NonDutyTrainTSI
        if ( false == m_operationModeManager.isInControlOfTrain( trainId, true, true ) )
        {
            TA_IRS_Bus::StateUpdateTypes::NonDutyTrainTSI update;
            update.trainId = trainId;
            update.trainLocation = trainLocation;
            update.trainTsi = timsEvent->getTsi();

            m_stateUpdateSender.sendStateUpdate( update );
        }

        FUNCTION_EXIT;
    }
bool TimsStatusManager::isMessageSentPreviously(TA_IRS_Bus::PaTypes::TrainLibraryInfo &pa_info)
{
if(m_trainidtoPALibraryMap.find(pa_info.trainId)==m_trainidtoPALibraryMap.end() || m_trainidtoPALibraryMap[pa_info.trainId].libraryVersion!=pa_info.libraryVersion)
{
	m_trainidtoPALibraryMap[pa_info.trainId]=pa_info;
	return false;
}
return true;
}
bool TimsStatusManager::isMessageSentPreviously(TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate &tis_update)
{
	if(m_trainidtoTisLibraryUpdateMap.find(tis_update.trainId)==m_trainidtoTisLibraryUpdateMap.end() || m_trainidtoTisLibraryUpdateMap[tis_update.trainId].libraryVersion!=tis_update.libraryVersion || m_trainidtoTisLibraryUpdateMap[tis_update.trainId].scheduleVersion!=tis_update.scheduleVersion)
	{
		m_trainidtoTisLibraryUpdateMap[tis_update.trainId]=tis_update;
		return false;
	}
	return true;
}
    void TimsStatusManager::processTestCallEvent( TA_IRS_Bus::TestCallEventPtr testCallEvent,
                                                  TrainCommsStatePtr commsState )
    {
        FUNCTION_ENTRY( "processTestCallEvent" );

        TA_IRS_Bus::CommonTypes::TrainIdType trainId = testCallEvent->getTrainId();

        {
            // acquire the lock
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // set library version info (PA, TTIS, TTIS Schedule)
            commsState->setLibraryVersions( testCallEvent->getPaVersion(),
                                            testCallEvent->getTtisVersion(),
                                            testCallEvent->getScheduleVersion(),
                                            testCallEvent->getTime() + 1 ); // the +1 is to force a DB update
                                                                            // its possible setTrainTsi was called earlier,
                                                                            // so this ensures its newer than the time in the DB

            // send out messages for library info (pa, ttis, ttis version)
            TA_IRS_Bus::PaTypes::TrainLibraryInfo paInfo;
            paInfo.trainId = trainId;
            paInfo.libraryVersion = commsState->getPaLibraryVersion();
			if(!isMessageSentPreviously(paInfo))
            m_messageSender.sendClientUpdate( paInfo );

            TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate tisInfo;
            tisInfo.trainId = trainId;
            tisInfo.libraryVersion = commsState->getTtisLibraryVersion();
            tisInfo.scheduleVersion = commsState->getTtisScheduleVersion();
			tisInfo.nextlibraryVersion = commsState->getNextTtisLibraryVersion();
			tisInfo.nextscheduleVersion = commsState->getNextTtisScheduleVersion();
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Set nextTtisLibraryVersion \
					of Train Id %d to %d", trainId, tisInfo.nextlibraryVersion);
			if(!isMessageSentPreviously(tisInfo))
            m_messageSender.sendClientUpdate( tisInfo );

            // get the ats validity, localisation validity, and location
            // if the ats is not valid
            if ( false == commsState->isAtsValid() )
            {
                // then the test call should not be answered
                // log a message to indicate this train does not have valid ats data and the test call will not be answered and return

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "Test Call from train %d, where ATS data is not valid will not be answered",
                             trainId );

                FUNCTION_EXIT;
                return;
            }

            // if the localisation is valid, and the location is not controlled by this agent
            if ( ( true == commsState->isLocalisationValid() ) &&
                 ( false == m_operationModeManager.isInControlOfLocation( commsState->getLocation() ) ) )
            {
                // then another agent should answer this test call
                // log a message to indicate this train should be answered by another agent and return

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "Test Call from train %d, where the train is at location %d, a location not under the control of this agent, will not be answered",
                             trainId,
                             commsState->getLocation() );

                FUNCTION_EXIT;
                return;
            }

            // log a message indicating the ats validity, localistation validity and location, and that this agent will answer the test call
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Answering Test Call from train %d, where the train is %s at location %d",
                         trainId,
                         commsState->isLocalisationValid() ? "localised" : "delocalised",
                         commsState->getLocation() );

            // initialise OA1/ATC to valid ( so on timeout the alarms will be raised )
            commsState->setAtcValidity( true,
                                        testCallEvent->getTime(),
                                        m_configuration.getLocationKey() );

            commsState->setOa1Validity( true,
                                        testCallEvent->getTime(),
                                        m_configuration.getLocationKey() );

            // release the lock
        }

		//Bohong++, for CL-20889
        //resetTrainState( commsState );
        TA_IRS_Bus::CommonTypes::TrainIdType tempTrainId = commsState->getTrainId();
        closeAlarmsForTrain( tempTrainId );
		closeTrainPec(tempTrainId);
		closeIncommingCall(tempTrainId);		
		//++Bohong

        // send a reply message explicitly to the MPU this message came from, catch and log any exceptions
        // Use the agent origin (from configuration)
        // if this is an OCC or station location, the ETrainArea is mainline, if it is a depot agent, ETrainArea is depot.

        TA_IRS_Bus::ETrainArea area = TA_IRS_Bus::MainlineArea;

        if ( TA_Base_Core::ILocation::DPT == m_configuration.getAgentLocationType() )
        {
            area = TA_IRS_Bus::DepotArea;
        }

        TA_IRS_Bus::TrainCommandPtr response( new TA_IRS_Bus::TestCallResultCommand( trainId,
                                              m_configuration.getAgentOrigin(),
                                              area ) );

        m_transactionManager.sendCommand( response, 0, -1, true, testCallEvent->getTsi() );

        // note this is not really necessary
        // also use m_modeManager to trigger a mode change command
        // m_modeManager.signalChangeMode( trainId );

        FUNCTION_EXIT;
    }


    void TimsStatusManager::processOa1Event( TA_IRS_Bus::Oa1EventPtr oa1Event,
                                             TrainCommsStatePtr commsState )
    {
        FUNCTION_ENTRY( "processOa1Event" );

        // signal m_activityMonitor an OA1 was received
        m_activityMonitor->oa1Received( oa1Event->getTrainId() );

        // get the train comms state
        {
            // acquire the lock
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // set the oa1 validity
            commsState->setOa1Validity( true,
                                        oa1Event->getTime(),
                                        m_configuration.getLocationKey() );

            // release the lock
        }

        {
            // get the train alarms state
            TrainAlarmStatePtr alarmState = m_statusManager.getTrainAlarmState( oa1Event->getTrainId() );

            // acquire the lock
            TA_IRS_Bus::TrainStateLock stateLock( alarmState );

            // Get the OA1 alarm ID
            std::string alarmId = alarmState->getOa1AlarmId();

            // close the OA1 alarm if open
            if ( false == alarmId.empty() )
            {
                m_alarms.closeAlarm( alarmId );

                // clear the alarm ID
                alarmState->setOa1AlarmId();
            }

            // release the lock
        }

        try
        {
            // forward OA1 info to the ATS agent
            m_atsAgent.sendOa1TableToAts( oa1Event->getTrainId(),
                                          oa1Event->getTableData() );

            if ( false == m_atsAgentCommsOk )
            {
                m_alarms.closeAtsAgentCommsAlarm();
                m_atsAgentCommsOk = true;
            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "While forwarding data to ATS" );

            if ( true == m_atsAgentCommsOk )
            {
                m_alarms.raiseAtsAgentCommsAlarm( m_atsAgent.getAtsAgentName() );
                m_atsAgentCommsOk = true;
            }
        }

        FUNCTION_EXIT;
    }


    void TimsStatusManager::processAtcEvent( TA_IRS_Bus::AtcEventPtr atcEvent,
                                             TrainCommsStatePtr commsState )
    {
        FUNCTION_ENTRY( "processAtcEvent" );

        // signal m_activityMonitor an ATC was received
        m_activityMonitor->atcReceived( atcEvent->getTrainId() );

        // get the train comms state
        {
            // acquire the lock
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // set the atc validity
            commsState->setAtcValidity( true,
                                        atcEvent->getTime(),
                                        m_configuration.getLocationKey() );

            // release the lock
        }

        {
            // get the train alarms state
            TrainAlarmStatePtr alarmState = m_statusManager.getTrainAlarmState( atcEvent->getTrainId() );

            // acquire the lock
            TA_IRS_Bus::TrainStateLock stateLock( alarmState );

            // Get the ATC alarm ID
            std::string alarmId = alarmState->getAtcAlarmId();

            // close the ATC alarm if open
            if ( false == alarmId.empty() )
            {
                m_alarms.closeAlarm( alarmId );

                // clear the alarm ID
                alarmState->setAtcAlarmId();
            }

            // release the lock
        }

        // forward atc info to the ATS agent
        m_atsAgent.sendAtcTrainStatusReportToAts( atcEvent->getTrainId(),
                atcEvent->getTableData() );

        try
        {
            // forward atc info to the ATS agent
            m_atsAgent.sendAtcTrainStatusReportToAts( atcEvent->getTrainId(),
                                                      atcEvent->getTableData() );

            if ( false == m_atsAgentCommsOk )
            {
                m_alarms.closeAtsAgentCommsAlarm();
                m_atsAgentCommsOk = true;
            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "While forwarding data to ATS" );

            if ( true == m_atsAgentCommsOk )
            {
                m_alarms.raiseAtsAgentCommsAlarm( m_atsAgent.getAtsAgentName() );
                m_atsAgentCommsOk = true;
            }
        }

        FUNCTION_EXIT;
    }


    bool TimsStatusManager::processChangeoverStatusEvent( TA_IRS_Bus::ChangeoverStatusEventPtr changeoverEvent,
                                                          TrainCommsStatePtr commsState )
    {
        FUNCTION_ENTRY( "processChangeoverStatusEvent" );

        {
            // get the train comms state for this train
            // acquire the lock
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // initialise OA1/ATC to valid
            commsState->setAtcValidity( true,
                                        changeoverEvent->getTime(),
                                        m_configuration.getLocationKey() );

            commsState->setOa1Validity( true,
                                        changeoverEvent->getTime(),
                                        m_configuration.getLocationKey() );

            // release the lock
        }

        // raise an MPU changeover alarm
        m_alarms.raiseMpuChangeoverAlarm( changeoverEvent->getTrainId(),
                                          changeoverEvent->getTime() );

        FUNCTION_EXIT;
        return ( TA_IRS_Bus::ChangeoverStatusEvent::MpuMaster == changeoverEvent->getMpuMode() );
    }


    void TimsStatusManager::closeAlarmsForTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "closeAlarmsForTrain" );

        // get the alarm state for the train
        TrainAlarmStatePtr alarmState = m_statusManager.getTrainAlarmState( trainId );

        // acquire the lock for the alarm state
        TA_IRS_Bus::TrainStateLock stateLock( alarmState );

        // get the alarm IDs for:
        // OA1
        // ATC
        // Radio Mode
        // Pre Conflict

        // for each alarm ID, if it is not empty use the appropriate method on m_alarms to close the alarm
        // Then clear each in the alarm state (set to "")

        if ( false == alarmState->getOa1AlarmId().empty() )
        {
            m_alarms.closeAlarm( alarmState->getOa1AlarmId() );
            alarmState->setOa1AlarmId();
        }

        if ( false == alarmState->getAtcAlarmId().empty() )
        {
            m_alarms.closeAlarm( alarmState->getAtcAlarmId() );
            alarmState->setAtcAlarmId();
        }

        if ( false == alarmState->getCctvPreConflictAlarmId().empty() )
        {
            m_alarms.closeAlarm( alarmState->getCctvPreConflictAlarmId() );
            alarmState->setCctvPreConflictAlarmId();
        }

        if ( false == alarmState->getRadioModeAlarmId().empty() )
        {
            m_alarms.closeAlarm( alarmState->getRadioModeAlarmId() );
            alarmState->setRadioModeAlarmId();
        }

        // release the lock for the alarm state

        FUNCTION_EXIT;
    }


    void TimsStatusManager::resetTrainState( TrainCommsStatePtr commsState )
    {
        FUNCTION_ENTRY( "resetTrainState" );

        TA_IRS_Bus::CommonTypes::TrainIdType trainId = commsState->getTrainId();

        // close all alarms for the train
        closeAlarmsForTrain( trainId );

		/*
        time_t timestamp = time( NULL );

        {
            // get its CCTV state
            TrainCctvStatePtr cctvState = m_statusManager.getTrainCctvState( trainId );

            // acquire the lock
            TA_IRS_Bus::TrainStateLock stateLock( cctvState );

            // set cctv inactive
            // (message will be sent from state change handler in TIMS CCTV Manager)
            cctvState->setActivated( false, timestamp );

            // release the lock			
        }

        {
            // get its PEC state
            TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );

            // acquire the lock
            TA_IRS_Bus::TrainStateLock stateLock( pecState );

            // invalidate all the PEC
            TA_IRS_Bus::PecTypes::ActivePecPointList invalidatedPec = pecState->invalidateAllPec( timestamp );

            // for each pec point, send a PEC update
            for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator pecIter = invalidatedPec.begin();
                  invalidatedPec.end() != pecIter; ++pecIter )
            {
                TA_IRS_Bus::PecTypes::PecUpdate update;
                update.trainId = trainId;
                update.updateType = TA_IRS_Bus::PecTypes::PECUpdated;
                update.pec = *pecIter;
                update.sessionId = pecState->getSessionId();

                m_messageSender.sendClientUpdate( update );
            }

            // release the lock
        }

        {
            // get its Call state
            TrainCallStatePtr callState = m_statusManager.getTrainCallState( trainId );

            // acquire the lock
            TA_IRS_Bus::TrainStateLock stateLock( callState );

            // if there was an incoming call
            if ( true == callState->hasIncomingCall() )
            {
                // clear incoming call
                callState->clearIncomingCall( timestamp );

                // send the OccCallReset message
                TA_IRS_Bus::CallTypes::OccCallReset update;
                update.trainId = trainId;

                m_messageSender.sendClientUpdate( update );
            }

            // release the lock
        }
		*/

		//Bohong++ to replace code commented above
		closeTrainCctv(trainId);
		closeTrainPec(trainId);
		closeIncommingCall(trainId);
		//++Bohong

        FUNCTION_EXIT;
    }

	//Bohong++
	void TimsStatusManager::closeTrainCctv( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
	{
		FUNCTION_ENTRY( "closeTrainCctv" );
		// get its CCTV state
		TrainCctvStatePtr cctvState = m_statusManager.getTrainCctvState( trainId );
		time_t timestamp = time( NULL );
		
		// acquire the lock
		TA_IRS_Bus::TrainStateLock stateLock( cctvState );
		
		// set cctv inactive
		// (message will be sent from state change handler in TIMS CCTV Manager)
		cctvState->setActivated( false, timestamp );
		
		FUNCTION_EXIT;
	}
	
	//Bohong++
	void TimsStatusManager::closeTrainPec( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
	{
		FUNCTION_ENTRY( "closeTrainPec" );

		// get its PEC state
		TrainPecStatePtr pecState = m_statusManager.getTrainPecState( trainId );
		time_t timestamp = time( NULL );
		
		// acquire the lock
		TA_IRS_Bus::TrainStateLock stateLock( pecState );
		
		// invalidate all the PEC
		TA_IRS_Bus::PecTypes::ActivePecPointList invalidatedPec = pecState->invalidateAllPec( timestamp );
		
		// for each pec point, send a PEC update
		for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator pecIter = invalidatedPec.begin();
		invalidatedPec.end() != pecIter; ++pecIter )
		{
			TA_IRS_Bus::PecTypes::PecUpdate update;
			update.trainId = trainId;
			update.updateType = TA_IRS_Bus::PecTypes::PECUpdated;
			update.pec = *pecIter;
			update.sessionId = pecState->getSessionId();
			
			m_messageSender.sendClientUpdate( update );
        }

		FUNCTION_EXIT;
	}
	
	//Bohong++
	void TimsStatusManager::closeIncommingCall( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
	{
		FUNCTION_ENTRY( "closeIncommingCall" );

		// get its Call state
		TrainCallStatePtr callState = m_statusManager.getTrainCallState( trainId );
		time_t timestamp = time( NULL );
		
		// acquire the lock
		TA_IRS_Bus::TrainStateLock stateLock( callState );
		
		// if there was an incoming call
		if ( true == callState->hasIncomingCall() )
		{
			// clear incoming call
			callState->clearIncomingCall( timestamp );
			
			// send the OccCallReset message
			TA_IRS_Bus::CallTypes::OccCallReset update;
			update.trainId = trainId;
			
			m_messageSender.sendClientUpdate( update );
        }

		FUNCTION_EXIT;
	}

} // TA_IRS_App


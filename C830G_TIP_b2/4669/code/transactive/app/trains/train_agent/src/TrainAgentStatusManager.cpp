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

#include "app/trains/train_agent/src/TrainAgentStatusManager.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/IStateUpdateMessageSender.h"
#include "app/trains/train_agent/src/IMessageSender.h"
#include "app/trains/train_agent/src/TrainStateHelper.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

#include "bus/trains/TrainCommonLibrary/src/InvalidSelectionListException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainRadioException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
//#include "bus/managed_agent/src/AgentManager.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

#include <algorithm>

namespace
{
    static const TA_IRS_Bus::CommonTypes::TrainIdType MAX_TRAINS = 99;
}

namespace TA_IRS_App
{

    TrainAgentStatusManager::TrainAgentStatusManager( ITrainAgentConfiguration& configuration,
                                                      IStateUpdateMessageSender& stateUpdateSender,
                                                      IMessageSender& clientUpdateSender )
            : TrainStatusManager(),
              m_configuration( configuration ),
              m_stateUpdateSender( stateUpdateSender ),
              m_clientUpdateSender( clientUpdateSender )
    {
        FUNCTION_ENTRY( "TrainAgentStatusManager" );
        FUNCTION_EXIT;
    }


    TrainAgentStatusManager::~TrainAgentStatusManager()
    {
        FUNCTION_ENTRY( "~TrainAgentStatusManager" );

        TA_THREADGUARD( m_trainSelectionMapLock );
        TA_THREADGUARD( m_radioCallLock );

        m_trainSelectionMap.clear();
        m_radioCalls.clear();

        FUNCTION_EXIT;
    }


    void TrainAgentStatusManager::initialise()
    {
        FUNCTION_ENTRY( "initialise" );

        // create train states for 99 trains
        setUpInitialStates();

        // load persisted configuration from the database
        loadPersistedStates();

        FUNCTION_EXIT;
    }


    void TrainAgentStatusManager::registerForStateChanges( TA_IRS_Bus::CommonTypes::ETrainResource resourceType,
                                                           TA_IRS_Bus::ITrainStateUpdateListener& listener )
    {
        FUNCTION_ENTRY( "registerForStateChanges" );

        TrainStatusManager::registerForStateChanges( resourceType, listener );

        FUNCTION_EXIT;
    }


    void TrainAgentStatusManager::deregisterForStateChanges( TA_IRS_Bus::CommonTypes::ETrainResource resourceType,
                                                             TA_IRS_Bus::ITrainStateUpdateListener& listener )
    {
        FUNCTION_ENTRY( "deregisterForStateChanges" );

        TrainStatusManager::deregisterForStateChanges( resourceType, listener );

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::TrainIdToStateMap TrainAgentStatusManager::getAllTrainStates( TA_IRS_Bus::CommonTypes::ETrainResource resource ) const
    {
        FUNCTION_ENTRY( "getAllTrainStates" );
        FUNCTION_EXIT;
        return TrainStatusManager::getAllTrainStates( resource );
    }


    TrainCommsStatePtr TrainAgentStatusManager::getTrainCommsState( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) const
    {
        FUNCTION_ENTRY( "getTrainCommsState" );

        // get the train state for the given trainId and CoreResource from the parent object
        // cast to the correct type and return it.
        TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( getTrainState( trainId, TA_IRS_Bus::CommonTypes::CoreResource ) );

        // ASSERT if the cast fails
        TA_ASSERT( NULL != commsState.get(), "Cannot cast to correct state" );

        FUNCTION_EXIT;
        return commsState;
    }


    TrainCctvStatePtr TrainAgentStatusManager::getTrainCctvState( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) const
    {
        FUNCTION_ENTRY( "getTrainCctvState" );

        // get the train state for the given trainId and CoreResource from the parent object
        // cast to the correct type and return it.
        TrainCctvStatePtr cctvState = boost::dynamic_pointer_cast< TrainCctvState >( getTrainState( trainId, TA_IRS_Bus::CommonTypes::TrainCctvResource ) );

        // ASSERT if the cast fails
        TA_ASSERT( NULL != cctvState.get(), "Cannot cast to correct state" );

        FUNCTION_EXIT;
        return cctvState;
    }


    TrainAlarmStatePtr TrainAgentStatusManager::getTrainAlarmState( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) const
    {
        FUNCTION_ENTRY( "getTrainAlarmState" );

        // get the train state for the given trainId and CoreResource from the parent object
        // cast to the correct type and return it.
        TrainAlarmStatePtr alarmState = boost::dynamic_pointer_cast< TrainAlarmState >( getTrainState( trainId, TA_IRS_Bus::CommonTypes::TrainAlarmResource ) );

        // ASSERT if the cast fails
        TA_ASSERT( NULL != alarmState.get(), "Cannot cast to correct state" );

        FUNCTION_EXIT;
        return alarmState;
    }


    TrainCallStatePtr TrainAgentStatusManager::getTrainCallState( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) const
    {
        FUNCTION_ENTRY( "getTrainCallState" );

        // get the train state for the given trainId and CoreResource from the parent object
        // cast to the correct type and return it.
        TrainCallStatePtr callState = boost::dynamic_pointer_cast< TrainCallState >( getTrainState( trainId, TA_IRS_Bus::CommonTypes::TrainCallResource ) );

        // ASSERT if the cast fails
        TA_ASSERT( NULL != callState.get(), "Cannot cast to correct state" );

        FUNCTION_EXIT;
        return callState;
    }


    TrainPaStatePtr TrainAgentStatusManager::getTrainPaState( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) const
    {
        FUNCTION_ENTRY( "getTrainPaState" );

        // get the train state for the given trainId and CoreResource from the parent object
        // cast to the correct type and return it.
        TrainPaStatePtr paState = boost::dynamic_pointer_cast< TrainPaState >( getTrainState( trainId, TA_IRS_Bus::CommonTypes::TrainPaResource ) );

        // ASSERT if the cast fails
        TA_ASSERT( NULL != paState.get(), "Cannot cast to correct state" );

        FUNCTION_EXIT;
        return paState;
    }


    TrainPecStatePtr TrainAgentStatusManager::getTrainPecState( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) const
    {
        FUNCTION_ENTRY( "getTrainPecState" );

        // get the train state for the given trainId and CoreResource from the parent object
        // cast to the correct type and return it.
        TrainPecStatePtr pecState = boost::dynamic_pointer_cast< TrainPecState >( getTrainState( trainId, TA_IRS_Bus::CommonTypes::TrainPecResource ) );

        // ASSERT if the cast fails
        TA_ASSERT( NULL != pecState.get(), "Cannot cast to correct state" );

        FUNCTION_EXIT;
        return pecState;
    }


    TrainTisStatePtr TrainAgentStatusManager::getTrainTisState( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) const
    {
        FUNCTION_ENTRY( "getTrainTisState" );

        // get the train state for the given trainId and CoreResource from the parent object
        // cast to the correct type and return it.
        TrainTisStatePtr tisState = boost::dynamic_pointer_cast< TrainTisState >( getTrainState( trainId, TA_IRS_Bus::CommonTypes::TrainTisResource ) );

        // ASSERT if the cast fails
        TA_ASSERT( NULL != tisState.get(), "Cannot cast to correct state" );

        FUNCTION_EXIT;
        return tisState;
    }


    std::vector<std::string> TrainAgentStatusManager::getTrainSelectionNames() const
    {
        FUNCTION_ENTRY( "getTrainSelectionNames" );

        // go through the m_traninSelectionMap and return all the names in a vector.

        std::vector<std::string> trainSelectionNames;

        using TA_IRS_Bus::StateUpdateTypes::TrainSelectionMap;

        TA_THREADGUARD( m_trainSelectionMapLock );

        for ( TrainSelectionMap::const_iterator mapIter = m_trainSelectionMap.begin();
              m_trainSelectionMap.end() != mapIter; ++mapIter )
        {
            trainSelectionNames.push_back( mapIter->first );
        }

        FUNCTION_EXIT;
        return trainSelectionNames;
    }


    TA_IRS_Bus::CommonTypes::TrainIdList TrainAgentStatusManager::getTrainSelection( const std::string& selectionName ) const
    {
        FUNCTION_ENTRY( "getTrainSelection" );

        using TA_IRS_Bus::StateUpdateTypes::TrainSelectionMap;

        // aquire the map lock
        TA_THREADGUARD( m_trainSelectionMapLock );

        // find the entry in m_trainSelectionMap and return it.
        TrainSelectionMap::const_iterator findIter = m_trainSelectionMap.find( selectionName );

        // throw an exception if it is not found.
        if ( m_trainSelectionMap.end() == findIter )
        {
            TA_THROW( TA_IRS_Bus::InvalidSelectionListException( "Train selection does not exist" ) );
        }

        FUNCTION_EXIT;
        return findIter->second;
    }


    void TrainAgentStatusManager::saveTrainSelection( const std::string& selectionName,
                                                      const TA_IRS_Bus::CommonTypes::TrainIdList& trainList )
    {
        FUNCTION_ENTRY( "saveTrainSelection" );

        using TA_IRS_Bus::StateUpdateTypes::TrainSelectionMap;

        // saves the list of trains in the given map.

        // acquire the map lock
        TA_THREADGUARD( m_trainSelectionMapLock );

        // If the entry is not in the map, add it
        // if it is, overwrite it
        m_trainSelectionMap[ selectionName ] = trainList;

        // send the state update before returning
        m_stateUpdateSender.sendStateUpdate( m_trainSelectionMap );

        FUNCTION_EXIT;
    }


    void TrainAgentStatusManager::deleteTrainSelection( const std::string& selectionName )
    {
        FUNCTION_ENTRY( "deleteTrainSelection" );

        using TA_IRS_Bus::StateUpdateTypes::TrainSelectionMap;

        // aquire the map lock
        TA_THREADGUARD( m_trainSelectionMapLock );

        // find the entry in m_trainSelectionMap
        TrainSelectionMap::iterator findIter = m_trainSelectionMap.find( selectionName );

        // throw an exception if it is not found.
        if ( m_trainSelectionMap.end() == findIter )
        {
            TA_THROW( TA_IRS_Bus::InvalidSelectionListException( "Train selection does not exist" ) );
        }

        // delete it from the map
        m_trainSelectionMap.erase( findIter );

        // send the state update
        m_stateUpdateSender.sendStateUpdate( m_trainSelectionMap );

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::CommonTypes::TrainIdType TrainAgentStatusManager::getTrainIdFromTsi( const std::string& tsi ) const
    {
        FUNCTION_ENTRY( "getTrainIdFromTsi" );
        FUNCTION_EXIT;
        return TrainStateHelper::getTrainIdFromTsi( tsi, *this );
    }


    void TrainAgentStatusManager::addRadioCall( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetails& callDetails )
    {
        FUNCTION_ENTRY( "addRadioCall" );

        using TA_IRS_Bus::StateUpdateTypes::RadioCallDetailsSet;

        // acquire the m_radioCallLock
        TA_THREADGUARD( m_radioCallLock );

        // Try to find the call details in the map
        RadioCallDetailsSet::iterator findIter = m_radioCalls.find( callDetails );

        // if it is found, overwrite it with the current call details - and log an error to say the call already existed (use getLogString from the call details)

        if ( m_radioCalls.end() != findIter )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "Existing radio call %s replaced with new radio call %s",
                         findIter->getLogString().c_str(), callDetails.getLogString().c_str() );

            m_radioCalls.erase( findIter );
            m_radioCalls.insert( callDetails );
        }
        else // if it is not found, then add it to the map, log a message to indicate the call was added to the list of ongoing calls.
        {
            m_radioCalls.insert( callDetails );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "Radio call details added %s",
                         callDetails.getLogString().c_str() );
        }

        // send a sync message
        m_stateUpdateSender.sendStateUpdate( m_radioCalls );

        FUNCTION_EXIT;
    }


    void TrainAgentStatusManager::removeRadioCall( unsigned long consoleId, unsigned long callId )
    {
        FUNCTION_ENTRY( "removeRadioCall" );

        using TA_IRS_Bus::StateUpdateTypes::RadioCallDetailsSet;

        TA_IRS_Bus::StateUpdateTypes::RadioCallDetails callDetails;
        callDetails.consoleId = consoleId;
        callDetails.callId = callId;

        // acquire the m_radioCallLock
        TA_THREADGUARD( m_radioCallLock );

        // Try to find the call details in the map
        RadioCallDetailsSet::iterator findIter = m_radioCalls.find( callDetails );

        // if it is found, remove the call details
        if ( m_radioCalls.end() != findIter )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Existing radio call %s removed",
                         findIter->getLogString().c_str() );

            m_radioCalls.erase( findIter );
        }

        // send a sync message
        m_stateUpdateSender.sendStateUpdate( m_radioCalls );

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::StateUpdateTypes::RadioCallDetails TrainAgentStatusManager::getDetailsForRadioCall( unsigned long consoleId, unsigned long callId ) const
    {
        FUNCTION_ENTRY( "getDetailsForRadioCall" );

        using TA_IRS_Bus::StateUpdateTypes::RadioCallDetailsSet;

        TA_IRS_Bus::StateUpdateTypes::RadioCallDetails callDetails;
        callDetails.consoleId = consoleId;
        callDetails.callId = callId;

        // acquire the m_radioCallLock
        TA_THREADGUARD( m_radioCallLock );

        // Try to find the call details in the map
        RadioCallDetailsSet::const_iterator findIter = m_radioCalls.find( callDetails );

        // if it is not found, throw a TrainRadioException - with an invalid call id error
        if ( m_radioCalls.end() == findIter )
        {
            TA_THROW( TA_IRS_Bus::TrainRadioException( "Call not found", TA_IRS_Bus::CommonTypes::RadioCallInvalid ) );
        }

        // if it is found, return the details

        FUNCTION_EXIT;
        return ( *findIter );
    }


    TA_IRS_Bus::CommonTypes::TrainIdList TrainAgentStatusManager::getTrainsInvolvedInRadioCall( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetails& callDetails ) const
    {
        FUNCTION_ENTRY( "getTrainsInvolvedInRadioCall" );

        // Call TrainStateHelper::getTrainsInCall with the call details

        FUNCTION_EXIT;
        return TrainStateHelper::getTrainsInCall( callDetails, *this );
    }


    TA_IRS_Bus::CommonTypes::TrainIdList TrainAgentStatusManager::getTrainsInvolvedInLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId ) const
    {
        FUNCTION_ENTRY( "getTrainsInvolvedInLiveAnnouncement" );

        // Call TrainStateHelper::getTrainsInAnnouncement with the appropriate announcement ID, and continueRequired = false

        FUNCTION_EXIT;
        return TrainStateHelper::getTrainsInAnnouncement( announcementId, false, *this );
    }


    TA_IRS_Bus::CommonTypes::TrainIdList TrainAgentStatusManager::getTrainsInvolvedInLiveAnnouncementRequiringContinue( TA_IRS_Bus::PaTypes::AnnouncementId announcementId ) const
    {
        FUNCTION_ENTRY( "getTrainsInvolvedInLiveAnnouncementRequiringContinue" );

        // Call TrainStateHelper::getTrainsInAnnouncement with the appropriate announcement ID, and continueRequired = true

        FUNCTION_EXIT;
        return TrainStateHelper::getTrainsInAnnouncement( announcementId, true, *this );
    }


    TA_IRS_Bus::StateUpdateTypes::RadioCallDetails TrainAgentStatusManager::getCallDetailsForLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId ) const
    {
        FUNCTION_ENTRY( "getCallDetailsForLiveAnnouncement" );
        FUNCTION_EXIT;
        return TrainStateHelper::getCallDetailsForLiveAnnouncement( announcementId, *this );
    }


    std::string TrainAgentStatusManager::getStaticGroupForLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId ) const
    {
        FUNCTION_ENTRY( "getStaticGroupForLiveAnnouncement" );
        FUNCTION_EXIT;
        return TrainStateHelper::getStaticGroupForLiveAnnouncement( announcementId, *this );
    }


    TA_IRS_Bus::StateUpdateTypes::RadioCallDetailsSet TrainAgentStatusManager::getRadioCallDetails() const
    {
        FUNCTION_ENTRY( "getRadioCallDetails" );

        // acquire the m_radioCallLock
        TA_THREADGUARD( m_radioCallLock );

        FUNCTION_EXIT;
        return m_radioCalls;
    }


    void TrainAgentStatusManager::syncRadioCallDetails( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetailsSet& syncState )
    {
        FUNCTION_ENTRY( "syncRadioCallDetails" );

        // If this agent is in control mode (AgentManager), return.
//        if ( true == OperationModeManager::isRunningControl() )		hongzhi
		if ( OperationModeManager::isRunningControl()==true )
        {
            FUNCTION_EXIT;
            return;
        }

        // acquire the m_radioCallLock
        TA_THREADGUARD( m_radioCallLock );

        m_radioCalls.clear();

        m_radioCalls = syncState;

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::StateUpdateTypes::TrainSelectionMap TrainAgentStatusManager::getTrainSelectionMap() const
    {
        FUNCTION_ENTRY( "getTrainSelectionMap" );

        // aquire the map lock
        TA_THREADGUARD( m_trainSelectionMapLock );

        FUNCTION_EXIT;
        return m_trainSelectionMap;
    }


    void TrainAgentStatusManager::syncTrainSelection( const TA_IRS_Bus::StateUpdateTypes::TrainSelectionMap& syncState )
    {
        FUNCTION_ENTRY( "syncTrainSelection" );

        // If this agent is in control mode (AgentManager), return.
        //if ( true == OperationModeManager::isRunningControl() )		hongzhi
		if ( OperationModeManager::isRunningControl()==true )
        {
            FUNCTION_EXIT;
            return;
        }

        // otherwise acquire m_trainSelectionMapLock and set m_trainSelectionMap
        TA_THREADGUARD( m_trainSelectionMapLock );

        m_trainSelectionMap.clear();

        m_trainSelectionMap = syncState;

        FUNCTION_EXIT;
    }


    void TrainAgentStatusManager::setUpInitialStates()
    {
        FUNCTION_ENTRY( "setUpInitialStates" );

        // for trains 1 to MAX_TRAINS (99)

        for ( TA_IRS_Bus::CommonTypes::TrainIdType trainId = 1;
              trainId <= MAX_TRAINS; ++trainId )
        {
            // create new train states

            TrainCommsStatePtr trainCommsState( new TrainCommsState( trainId ) );
            TrainAlarmStatePtr trainAlarmState( new TrainAlarmState( trainId ) );
            TrainCctvStatePtr trainCctvState( new TrainCctvState( trainId ) );
            TrainPaStatePtr trainPaState( new TrainPaState( trainId ) );
            TrainCallStatePtr trainCallState( new TrainCallState( trainId ) );
            TrainPecStatePtr trainPecState( new TrainPecState( trainId ) );
            TrainTisStatePtr trainTisState( new TrainTisState( trainId ) );

            // add each new train state

            addTrain( trainCommsState );
            addTrainState( trainAlarmState );
            addTrainState( trainCctvState );
            addTrainState( trainPaState );
            addTrainState( trainCallState );
            addTrainState( trainPecState );
            addTrainState( trainTisState );
        }

        FUNCTION_EXIT;
    }


    void TrainAgentStatusManager::loadPersistedStates()
    {
        FUNCTION_ENTRY( "loadPersistedStates" );

        // from m_configuration, get the persisted train states.
        TA_IRS_Core::IPersistedTrainStateMap persistedTrainStates = m_configuration.getPersistedTrainStates();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Loaded %d persisted train states from the database",
                     persistedTrainStates.size() );

        // for each state
        for ( TA_IRS_Core::IPersistedTrainStateMap::iterator stateIter = persistedTrainStates.begin();
              persistedTrainStates.end() != stateIter; ++stateIter )
        {
            try
            {
                // get the train comms state
                TrainCommsStatePtr trainCommsState = getTrainCommsState( static_cast< TA_IRS_Bus::CommonTypes::TrainIdType >( stateIter->first ) );

                time_t timestamp = stateIter->second->getTimestamp();

                // there is no need to acquire the state lock at this point since the agent is not running yet
                // set the persisted state information into this train state object
                trainCommsState->setTrainTsi( TA_IRS_Bus::ProtocolCommonTypes::MpuBackup,
                                              stateIter->second->getSecondaryTsi(),
                                              timestamp,
                                              false );

                trainCommsState->setTrainTsi( TA_IRS_Bus::ProtocolCommonTypes::MpuMain,
                                              stateIter->second->getPrimaryTsi(),
                                              timestamp,
                                              true );

                trainCommsState->setLibraryVersions( stateIter->second->getPaLibraryVersion(),
                                                     stateIter->second->getTtisLibraryVersion(),
                                                     stateIter->second->getTtisScheduleVersion(),					 
													 timestamp );

             trainCommsState->setNextTtisLibraryVersion( stateIter->second->getNextTtisLibraryVersion() ,timestamp ) ;
             trainCommsState->setNextTtisScheduleVersion(  stateIter->second->getNextTtisScheduleVersion() ,timestamp ) ;
            }
            catch ( const TA_IRS_Bus::InvalidTrainException& e )
            {
                std::ostringstream error;
                error << "Error while applying persisted train state for train "
                << static_cast< unsigned int >( stateIter->first )
                << ": " << e.what();

                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", error.str().c_str() );
            }
        }

        FUNCTION_EXIT;
    }

} // TA_IRS_App


/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $Date: 2018/03/14 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include "app/trains/train_agent/src/TrainInformation.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"
#include "app/trains/train_agent/src/IRightsChecker.h"
#include "app/trains/train_agent/src/IAuditSender.h"
#include "app/trains/train_agent/src/IMessageSender.h"

#include "bus/trains/TrainCommonLibrary/src/TrainAccessDeniedException.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

    TrainInformation::TrainInformation( IRightsChecker& rightsChecker,
                                        IMessageSender& messageSender,
                                        IAuditSender& auditSender,
                                        ITrainStatusManager& statusManager )
            : m_rightsChecker( rightsChecker ),
              m_messageSender( messageSender ),
              m_auditSender( auditSender ),
              m_statusManager( statusManager )
    {
        FUNCTION_ENTRY( "TrainInformation" );

        // using m_statusManager subscribe to changes in CoreResource (TrainCommsState)
        m_statusManager.registerForStateChanges( TA_IRS_Bus::CommonTypes::CoreResource, *this );

        // register for operation state callbacks
        //TA_Base_Bus::AgentManager::registerForStateChangeCallbacks( this );

        FUNCTION_EXIT;
    }


    TrainInformation::~TrainInformation()
    {
        FUNCTION_ENTRY( "~TrainInformation" );

        // deregister for operation state callbacks
        //TA_Base_Bus::AgentManager::deregisterForStateChangeCallbacks( this );

        // using m_statusManager unsubscribe to changes in CoreResource (TrainCommsState)
        m_statusManager.deregisterForStateChanges( TA_IRS_Bus::CommonTypes::CoreResource, *this );

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::TrainInformationTypes::TrainDetailsList TrainInformation::getTrainList()
    {
        FUNCTION_ENTRY( "getTrainList" );

        // Get all train comms states from m_statusManager
        TA_IRS_Bus::TrainIdToStateMap stateMap = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::CoreResource );

        TA_IRS_Bus::TrainInformationTypes::TrainDetailsList trainList;

        for ( TA_IRS_Bus::TrainIdToStateMap::iterator stateIter = stateMap.begin();
              stateMap.end() != stateIter; ++stateIter )
        {
            // down cast to the specific type
            TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( stateIter->second );
            TA_ASSERT( NULL != commsState.get(), "Failed to cast to the correct type" );

            // acquire the state lock
            TA_IRS_Bus::TrainStateLock stateLock( commsState );

            // get the TrainDetails and add it to the list
            trainList.push_back( commsState->getTrainDetails() );

            // release the state lock
        }

        // return the complete list

        FUNCTION_EXIT;
        return trainList;
    }


    TA_IRS_Bus::TrainInformationTypes::TrainDetails TrainInformation::getTrainDetails( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "getTrainDetails" );

        // get the TrainCommsState from m_statusManager
        TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );

        // acquire the state lock
        TA_IRS_Bus::TrainStateLock stateLock( commsState );

        FUNCTION_EXIT;
        return commsState->getTrainDetails();
    }


    TA_IRS_Bus::TrainInformationTypes::TrainSelectionList TrainInformation::getTrainSelectionList()
    {
        FUNCTION_ENTRY( "getTrainSelectionList" );

        // delegate the call on to m_statusManager to getTrainSelectionNames

        FUNCTION_EXIT;
        return m_statusManager.getTrainSelectionNames();
    }


    TA_IRS_Bus::CommonTypes::TrainIdList TrainInformation::getTrainSelection( const std::string& selectionName )
    {
        FUNCTION_ENTRY( "getTrainSelection" );

        // delegate the call on to m_statusManager to getTrainSelection

        FUNCTION_EXIT;
        return m_statusManager.getTrainSelection( selectionName );
    }


    void TrainInformation::saveTrainSelection( const std::string& selectionName,
                                               const TA_IRS_Bus::CommonTypes::TrainIdList& trainIDs,
                                               const std::string& sessionId )
    {
        FUNCTION_ENTRY( "saveTrainSelection" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Request to save train selection [name: %s][trains: %s][sessionId: %s]",
                     selectionName.c_str(),
                     TA_IRS_Bus::CommonTypes::trainIdListToString( trainIDs ).c_str(),
                     sessionId.c_str() );

        // check rights using m_rightsChecker.canModifySelectionList
        // if the operator cannot perform the action, throw an AccessDeniedException

        if ( false == m_rightsChecker.canModifySelectionList( sessionId ) )
        {
        //    TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not save train selection" ) );
			 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "DVA announcement for trains No Rightb for canModifySelectionList: %s",
					 sessionId.c_str ()  );
        }

        // delegate the call on to m_statusManager to saveTrainSelection
        m_statusManager.saveTrainSelection( selectionName, trainIDs );

        // send the gui update - use m_messageSender to send a TrainSelectionUpdate
        TA_IRS_Bus::TrainInformationTypes::TrainSelectionUpdate update;
        update.change = TA_IRS_Bus::TrainInformationTypes::SelectionListUpdate;
        update.selectionName = selectionName;
        m_messageSender.sendClientUpdate( update );

        // audit the action using m_auditSender.auditSaveTrainSelection
        m_auditSender.auditSaveTrainSelection( sessionId, selectionName, trainIDs );

        FUNCTION_EXIT;
    }


    void TrainInformation::deleteTrainSelection( const std::string& selectionName,
                                                 const std::string& sessionId )
    {
        FUNCTION_ENTRY( "deleteTrainSelection" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Request to delete train selection [name: %s][sessionId: %s]",
                     selectionName.c_str(),
                     sessionId.c_str() );

        // check rights using m_rightsChecker.canModifySelectionList
        // if the operator cannot perform the action, throw an AccessDeniedException

        if ( false == m_rightsChecker.canModifySelectionList( sessionId ) )
        {
   //         TA_THROW( TA_IRS_Bus::TrainAccessDeniedException( "Can not delete saved train selection" ) );

             LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "DVA announcement for trains No Rightb for canModifySelectionList: %s",
					 sessionId.c_str ()  );
        }

        // delegate the call on to m_statusManager to deleteTrainSelection
        m_statusManager.deleteTrainSelection( selectionName );

        // send the gui update - use m_messageSender to send a TrainSelectionUpdate
        TA_IRS_Bus::TrainInformationTypes::TrainSelectionUpdate update;
        update.change = TA_IRS_Bus::TrainInformationTypes::SelectionListDeletion;
        update.selectionName = selectionName;
        m_messageSender.sendClientUpdate( update );

        // audit the action using m_auditSender.auditDeleteTrainSelection
        m_auditSender.auditDeleteTrainSelection( sessionId, selectionName );

        FUNCTION_EXIT;
    }


    void TrainInformation::processTrainStateChanged( TA_IRS_Bus::TrainStatePtr state )
    {
        FUNCTION_ENTRY( "processTrainStateChanged" );

        // down cast the state to a TrainCommsState, assert if it fails
        TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( state );
        TA_ASSERT( NULL != commsState.get(), "Failed to cast to the correct type" );

        TA_IRS_Bus::TrainStateLock stateLock( commsState );

        // get the TrainDetails from the train state
        // send the details using m_messageSender
        m_messageSender.sendClientUpdate( commsState->getTrainDetails() );

        FUNCTION_EXIT;
    }


    void TrainInformation::notifyOperationStateChange( TA_Base_Core::EOperationMode operationState )
    {
        FUNCTION_ENTRY( "notifyOperationStateChange" );

        // if the mode is control
        if ( TA_Base_Core::Control == operationState )
        {
            // deregister for operation state callbacks
            //TA_Base_Bus::AgentManager::deregisterForStateChangeCallbacks( this );

            // get all train comms states
            TA_IRS_Bus::TrainIdToStateMap stateMap = m_statusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::CoreResource );

            for ( TA_IRS_Bus::TrainIdToStateMap::iterator stateIter = stateMap.begin();
                  stateMap.end() != stateIter; ++stateIter )
            {
                TrainCommsStatePtr commsState = boost::dynamic_pointer_cast< TrainCommsState >( stateIter->second );
                TA_ASSERT( NULL != commsState.get(), "Failed to cast to the correct type" );

                TA_IRS_Bus::TrainStateLock stateLock( commsState );

                // get the TrainDetails from the train state
                // send the details using m_messageSender
                m_messageSender.sendClientUpdate( commsState->getTrainDetails() );

                TA_IRS_Bus::PaTypes::TrainLibraryInfo paInfo;
                paInfo.trainId = stateIter->first;
                paInfo.libraryVersion = commsState->getPaLibraryVersion();

                m_messageSender.sendClientUpdate( paInfo );

                TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate tisInfo;
                tisInfo.trainId = stateIter->first;
                tisInfo.libraryVersion = commsState->getTtisLibraryVersion();
                tisInfo.scheduleVersion = commsState->getTtisScheduleVersion();

                m_messageSender.sendClientUpdate( tisInfo );
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

} // TA_IRS_App

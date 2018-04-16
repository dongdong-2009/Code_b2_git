/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/COEBVSStage.cpp $
  * @author:   Robert Young
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/17 14:59:54 $
  * Last modified by:  $Author: builder $
  *
  * Handles the protocol dependent sections of the general functions for a BVSStage
  * using the COE protocol.
  * Inherits from the IPDBVSStage class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the BVSStage and
  * that the BVSStage and the protocol support the required features.
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/COEBVSStage.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/TrainCameraAlarmClearCommand.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"

#include "bus/trains/TrainCommonLibrary/src/CctvTypes.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/TrainAgentComms_MessageTypes.h"
#include "core/message/types/VisualComms_MessageTypes.h"
#include "core/naming/src/NamedObject.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    //
    // COEBVSStage
    //
    COEBVSStage::COEBVSStage( TA_Base_Core::BVSStagePtr BVSStageData )
    : m_BVSStageData( BVSStageData )
	{
        FUNCTION_ENTRY( "COEBVSStage" );
        m_trainCctvCorbaProxy = TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainCctvCorbaProxy();
        m_trainCctvCorbaProxy->addObserver( this );
		TA_Base_Core::ThreadGuard stateGuard( m_stateLock );
		m_messageSender = TA_Base_Core::MessagePublicationManager::getInstance()
        .getCommsMessageSender( TA_Base_Core::VisualComms::BvsStageStateUpdate );

		m_bvsStageState = getBvsDetails();
		
        COESwitchManager::getInstance().addBVSStage( *this );
        FUNCTION_EXIT;
    }


    COEBVSStage::~COEBVSStage()
    {
        FUNCTION_ENTRY( "~COEBVSStage" );

        m_trainCctvCorbaProxy->removeObserver( this );

        FUNCTION_EXIT;
    }
    unsigned long COEBVSStage::getKey()
    {
        FUNCTION_ENTRY( "getKey" );
        FUNCTION_EXIT;
        return m_BVSStageData->getKey();
    }

    
    std::string COEBVSStage::getAddress()
    {
        FUNCTION_ENTRY( "getAddress" );
        FUNCTION_EXIT;
        return m_BVSStageData->getAddress();
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // IPDBVSStage Methods
    //
    ///////////////////////////////////////////////////////////////////////

    //
    // getBVSStageState
    //
	TA_Base_Bus::BVSStageCorbaDef::BVSStageState COEBVSStage::getBVSStageState()
    {
        FUNCTION_ENTRY( "getBVSStageState" );
        TA_Base_Core::ThreadGuard stateGuard( m_stateLock );

		TA_Base_Bus::BVSStageCorbaDef::BVSStageState state;
		
		state.activeTrainID=m_bvsStageState.activeTrainID;
		state.numActiveMonitors=m_bvsStageState.numActiveMonitors;
		state.switchMode=m_bvsStageState.switchMode;
		state.preConflict=m_bvsStageState.preConflict;
		state.item1=m_bvsStageState.item1;
		state.item2=m_bvsStageState.item2;
		state.item3=m_bvsStageState.item3;
		state.item4=m_bvsStageState.item4;

        FUNCTION_EXIT;
        return state;
    }


    //
    // updateBVSStageState
    //
    void COEBVSStage::updateBVSStageState(TA_Base_Bus::BVSStageCorbaDef::BVSStageState state)
    {
        FUNCTION_ENTRY( "updateBVSStageState" );
        TA_Base_Core::ThreadGuard stateGuard( m_stateLock );

		m_bvsStageState.activeTrainID = state.activeTrainID;
		m_bvsStageState.numActiveMonitors = state.numActiveMonitors;
		m_bvsStageState.switchMode = state.switchMode;
		m_bvsStageState.preConflict = state.preConflict;
		m_bvsStageState.item1 = state.item1;
		m_bvsStageState.item2 = state.item2;
		m_bvsStageState.item3 = state.item3;
		m_bvsStageState.item4 = state.item4;

		notifyStateChange();

        FUNCTION_EXIT;
    }


    //
    // setActiveTrain
    //
    void  COEBVSStage::setActiveTrain( CORBA::Octet activeTrainID )
    {
        FUNCTION_ENTRY( "setActiveTrain" );

        LOG_GENERIC( SourceInfo,
                     TA_Base_Core::DebugUtil::DebugDebug,
                     "Active Train for stage %s set to Train %d by display manager",
                     m_BVSStageData->getAddress().c_str(),
                     activeTrainID );
        {
			TA_Base_Core::ThreadGuard stateGuard( m_stateLock );
			m_bvsStageState.activeTrainID = activeTrainID;
		}

		if(activeTrainID==0)
		{
			parkAllStageMonitors();
		}

		notifyStateChange();
        FUNCTION_EXIT;
	}

	
	TA_Base_Bus::BVSStageCorbaDef::BVSStageState COEBVSStage::getBvsDetails()
	{
        FUNCTION_ENTRY( "getBvsDetails" );
		TA_Base_Bus::BVSStageCorbaDef::BVSStageState stageState;

        // initialise to blank
        stageState.activeTrainID = 0;
		stageState.numActiveMonitors = 0;
		stageState.preConflict = false;
		stageState.switchMode = TA_Base_Bus::BVSStageCorbaDef::Camera;
		stageState.item1 = 0;
        stageState.item2 = 0;
        stageState.item3 = 0;
        stageState.item4 = 0;

        try
        {
            TA_IRS_Bus::CctvTypes::CctvSwitchState trainCctvState =
                m_trainCctvCorbaProxy->getActiveTrainForStage( atol( m_BVSStageData->getAddress().c_str() ) );

            stageState.activeTrainID = trainCctvState.trainId;

            stageState.preConflict = ( trainCctvState.preConflictTrain != 0 );
            stageState.numActiveMonitors = COESwitchManager::getInstance().getAllVideoOutputsWithAssignment( m_BVSStageData->getKey() ).size();

            switch ( trainCctvState.cctvState )
            {

                case TA_IRS_Bus::CctvTypes::CctvCamera:
                    stageState.switchMode = TA_Base_Bus::BVSStageCorbaDef::Camera;
                    break;

                case TA_IRS_Bus::CctvTypes::CctvSequence:
                    stageState.switchMode = TA_Base_Bus::BVSStageCorbaDef::Seq;
                    break;

                case TA_IRS_Bus::CctvTypes::CctvQuad:
                    stageState.switchMode = TA_Base_Bus::BVSStageCorbaDef::Quad;
                    break;
            }

            stageState.item1 = trainCctvState.item1;
            stageState.item2 = trainCctvState.item2;
            stageState.item3 = trainCctvState.item3;
            stageState.item4 = trainCctvState.item4;
        }
        catch ( TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
        }

        FUNCTION_EXIT;
		return stageState;

	}


    void COEBVSStage::processCctvSwitchState( const TA_IRS_Bus::CctvTypes::CctvSwitchState& event )
    {
        FUNCTION_ENTRY( "processCctvSwitchState" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
          "Received %s", event.getLogString().c_str() );
        
        
        if ( event.stageId == atoi( m_BVSStageData->getAddress().c_str() ) )
        {
            bool carrierOn = ( TA_IRS_Bus::CctvTypes::CctvInactive != event.cctvState );

            if ( ( true == carrierOn ) ||   // the train is in this stage and active, OR
                 ( ( false == carrierOn ) && // the train is not active, AND
                   ( m_bvsStageState.activeTrainID == event.trainId ) ) ) // was the active train for this stage
            {
                {
                    LOG_GENERIC( SourceInfo,
                                 TA_Base_Core::DebugUtil::DebugInfo,
                                 "Train CCTV message matches stage %s",
                                 m_BVSStageData->getAddress().c_str() );

                    TA_Base_Core::ThreadGuard stateGuard( m_stateLock );

                    if ( true == carrierOn )
                    {
                        m_bvsStageState.activeTrainID = event.trainId;

                        LOG_GENERIC( SourceInfo,
                                     TA_Base_Core::DebugUtil::DebugDebug,
                                     "Active Train for stage %d set to Train %d",
                                     event.stageId,
                                     event.trainId );
                    }
                    else
                    {
                        m_bvsStageState.activeTrainID = 0;

                        LOG_GENERIC( SourceInfo,
                                     TA_Base_Core::DebugUtil::DebugDebug,
                                     "Active Train for stage %d set to 0",
                                     event.stageId );
                    }

                    m_bvsStageState.preConflict = ( event.preConflictTrain != 0 );

                    switch ( event.cctvState )
                    {

                        case TA_IRS_Bus::CctvTypes::CctvCamera:
                            m_bvsStageState.switchMode = TA_Base_Bus::BVSStageCorbaDef::Camera;
                            break;

                        case TA_IRS_Bus::CctvTypes::CctvSequence:
                            m_bvsStageState.switchMode = TA_Base_Bus::BVSStageCorbaDef::Seq;
                            break;

                        case TA_IRS_Bus::CctvTypes::CctvQuad:
                            m_bvsStageState.switchMode = TA_Base_Bus::BVSStageCorbaDef::Quad;
                            break;
                    }

                    m_bvsStageState.item1 = event.item1;
                    m_bvsStageState.item2 = event.item2;
                    m_bvsStageState.item3 = event.item3;
                    m_bvsStageState.item4 = event.item4;

                    notifyStateChange();
                }
            }

            if ( m_bvsStageState.activeTrainID == 0 )
            {
                //only want to park if actively been deactivated, if the train moved then we must wait for the switch
                //to switch to next stage
                parkAllStageMonitors();
            }
        }
        else if ( m_bvsStageState.activeTrainID == event.trainId )
        {
            LOG_GENERIC( SourceInfo,
                         TA_Base_Core::DebugUtil::DebugDebug,
                         "Active Train for stage %d is no longer active",
                         event.stageId );
						//train must have moved out of our stage without us realising
						TA_Base_Core::ThreadGuard stateGuard( m_stateLock );
						m_bvsStageState.activeTrainID = 0;
						m_bvsStageState.preConflict = false;
						m_bvsStageState.switchMode = TA_Base_Bus::BVSStageCorbaDef::Seq;
						m_bvsStageState.item1 = 1;
						m_bvsStageState.item2 = 0;
						m_bvsStageState.item3 = 0;
						m_bvsStageState.item4 = 0;

						notifyStateChange();
        }

        FUNCTION_EXIT;
	}

	void COEBVSStage::notifyStateChange()
	{
        FUNCTION_ENTRY( "notifyStateChange" );
		try
		{
			TA_Base_Core::ThreadGuard stateGuard( m_stateLock );
		    StateUpdateBroadcaster::getInstance().notifyBVSStateUpdate( getKey() );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo,
                                   "DatabaseException",
                                   "Failed to notify the agent's clients that a VideoInput has changed state." );
        }
        catch ( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo,
                                   "DataException",
                "Failed to notify the agent's clients that a VideoInput has changed state." );
        }

        FUNCTION_EXIT;
	}


	void COEBVSStage::addObserver()
	{
        FUNCTION_ENTRY( "addObserver" );
		m_bvsStageState.numActiveMonitors++;
        FUNCTION_EXIT;
	}

	void COEBVSStage::removeObserver()
	{
        FUNCTION_ENTRY( "removeObserver" );
		m_bvsStageState.numActiveMonitors--;
        FUNCTION_EXIT;
	}

	unsigned long COEBVSStage::getObserverCount()
	{
        FUNCTION_ENTRY( "getObserverCount" );
        FUNCTION_EXIT;
		return m_bvsStageState.numActiveMonitors;
	}

	void COEBVSStage::parkAllStageMonitors()
	{
        FUNCTION_ENTRY( "parkAllStageMonitors" );
		std::vector<COEVideoOutput*>::iterator it;
		std::vector<COEVideoOutput*> outputs = COESwitchManager::getInstance().getAllVideoOutputsWithAssignment(m_BVSStageData->getKey());
		for(it = outputs.begin(); it!=outputs.end(); it++)
		{
			if(!(*it)->isInAlarmMode())
			{
				(*it)->requestSwitchOfInput(0, true);
			}
			else
			{
                COECommunicationsHandler::RequestCommandSharedPtr command   = COECommunicationsHandler::RequestCommandSharedPtr( new TrainCameraAlarmClearCommand( atoi( m_BVSStageData->getAddress().c_str() ),
                                                                              atoi( ( *it )->getAddress().c_str() ) ) );
				COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand(command, false);

			}
		}
        FUNCTION_EXIT;
	}

	bool COEBVSStage::isInAnAlarmStack()
	{
        FUNCTION_ENTRY( "isInAnAlarmStack" );
		std::vector<COEVideoOutput*>::iterator it;
		std::vector<COEVideoOutput*> outputs = COESwitchManager::getInstance().getAllVideoOutputsWithAssignment(m_BVSStageData->getKey());
		for(it = outputs.begin(); it!=outputs.end(); it++)
		{
            if ( ( *it )->isInAlarmMode() )
            {
                FUNCTION_EXIT;
                return true;
            }
        }

        FUNCTION_EXIT;
		return false;
	}



} // namespace TA_IRS_App

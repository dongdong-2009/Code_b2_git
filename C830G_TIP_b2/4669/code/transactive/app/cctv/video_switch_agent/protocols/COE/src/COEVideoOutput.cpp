/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/COEVideoOutput.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/17 14:59:54 $
  * Last modified by:  $Author: builder $
  *
  * Handles the protocol dependent sections of the operations for a VideoOutput
  * using the COE protocol. 
  * Inherits from IPDVideoOutput. Before the methods
  * in this interface are called, the user privileges have been checked and
  * that the protocol supports the required features.
  */

#include <sstream>

#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/StateValidationCycle.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SwitchInputToOutputCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/ControlSequencePlaybackCommand.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "bus/cctv/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/video_switch_agent/src/IVideoTriggeringEvent.h"
#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEventAccessFactory.h"


namespace TA_IRS_App
{
    //
    // COEVideoOutput
    //
    COEVideoOutput::COEVideoOutput( TA_Base_Core::VideoOutputPtr videoOutputData )
        : m_videoOutputData( videoOutputData ),
          m_currentVideoInputKey( 0 ),
		  m_isInAlarmMode( false )
    {
        COESwitchManager::getInstance().addVideoOutput( *this );
        COESwitchManager::getInstance().getStateValidationCycle().addValidationItem( *this, false );

		// Commented for TD16066
/*		TA_IRS_Core::IVideoTriggeringEvents triggeringData = TA_IRS_Core::VideoTriggeringEventAccessFactory::getInstance().getAllVideoTriggeringEvents();
		TA_IRS_Core::IVideoTriggeringEvents::iterator itr;
		for(itr = triggeringData.begin(); itr != triggeringData.end(); itr++)
		{
			TA_COE::AlarmState state;
			state.eventId = (*itr)->getEventId();
			state.stationId = (*itr)->getStationId();
			unsigned long pkey = (*itr)->getKey();

			m_alarmStateToId[state] = pkey;
			m_idToAlarmState[pkey] = state;
		}*/
    }


    //
    // getKey
    //
    unsigned long COEVideoOutput::getKey()
    {
        return m_videoOutputData->getKey();
    }

    
    //
    // getAddress
    //
    std::string COEVideoOutput::getAddress()
    {
        try
        {
		    return m_videoOutputData->getAddress();
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }	
        return 0;
    }
 

    ///////////////////////////////////////////////////////////////////////
    //
    // State Update Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // notifyInputSwitchedToOutput
    //
    void COEVideoOutput::notifyInputSwitchedToOutput( unsigned long videoInputKey )
    {
		if(m_isInAlarmMode || m_currentVideoInputKey!=videoInputKey)
		{
			
			m_isInAlarmMode = false;
			m_alarmStack.clear();
			//if we were showing a BVS, and now are obviously changing, we need to tell the BVS to decrement
			if(m_currentVideoInputKey!=0)
			{
				if(COESwitchManager::getInstance().getInputType( m_currentVideoInputKey ) == COESwitchManager::BVS_STAGE)
				{
					COEBVSStage* bvsInput = COESwitchManager::getInstance().getBVSStage( m_currentVideoInputKey );
					if(bvsInput!=NULL)
					{
						bvsInput->removeObserver();
						StateUpdateBroadcaster::getInstance().notifyBVSStateUpdate(m_currentVideoInputKey);
					}
				}
			}

			m_currentVideoInputKey = videoInputKey;

			//if we have been switched to a BVS we need to tell it so that it can keep track
			if(videoInputKey != 0)
			{
				if(COESwitchManager::getInstance().getInputType( videoInputKey ) == COESwitchManager::BVS_STAGE)
				{
					COEBVSStage* bvsInput = COESwitchManager::getInstance().getBVSStage( videoInputKey );
					if(bvsInput!=NULL)
					{
						bvsInput->addObserver();
						StateUpdateBroadcaster::getInstance().notifyBVSStateUpdate(videoInputKey);
					}
				}
			}
			try
			{
				StateUpdateBroadcaster::getInstance().notifyVideoOutputStateUpdate( m_videoOutputData->getKey() );
			}
			catch( const TA_Base_Core::DatabaseException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
					"Failed to notify the agent's clients that a switch of input has occurred." );
			}
			catch( const TA_Base_Core::DataException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
					"Failed to notify the agent's clients that a switch of input has occurred." );
			}	
		}
    }


    /**
	  *
	  */
	void COEVideoOutput::notifyOutputShowingAlarmStack( TA_COE::AlarmStack alarmStack)
	{
		if(!m_isInAlarmMode || m_alarmStack!=alarmStack)
		{
			m_isInAlarmMode=true;
			m_alarmStack = alarmStack;
			m_currentVideoInputKey = 0;
			try
			{
				StateUpdateBroadcaster::getInstance().notifyVideoOutputStateUpdate( m_videoOutputData->getKey() );
			}
			catch( const TA_Base_Core::DatabaseException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
					"Failed to notify the agent's clients that a switch of input has occurred." );
			}
			catch( const TA_Base_Core::DataException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
					"Failed to notify the agent's clients that a switch of input has occurred." );
			}
		}
	}

	//
    // updateState
    //
    void COEVideoOutput::updateState( const TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState& state )
    {
        m_currentVideoInputKey = state.currentVideoInputKey;
		m_isInAlarmMode = state.isInAlarmMode;
		
		m_alarmStack = alarmStateSequenceToAlarmStack(state.alarmStack);
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IPDVideoOutput Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getCurrentVideoInputName
    //
    unsigned long COEVideoOutput::getCurrentVideoInputKey()
    {
        return m_currentVideoInputKey;
    }


    //
    // getVideoOutputState
    //
    TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState* COEVideoOutput::getVideoOutputState()
	{
		TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState* state = new TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState();
		state->currentVideoInputKey=m_currentVideoInputKey;
		state->alarmStack=alarmStateSequenceFromAlarmStack(m_alarmStack);
		state->isInAlarmMode=m_isInAlarmMode;

		return state;
	}

	//
    // requestSwitchOfInput
    //
    void COEVideoOutput::requestSwitchOfInput( unsigned long videoInputEntityKey, 
                               bool isDemand )
    {
		if(!m_isInAlarmMode)
		{
			try
			{
				COECommunicationsHandler::RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr( new SwitchInputToOutputCommand( 
					videoInputEntityKey, *this ) );
				if ( 0 == command.get() )
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
						"Failed to create SwitchInputToOutputCommand object" );
					LOGMORE( SourceInfo, "Failed to send Switch command." );
					throw TA_Base_Bus::VideoSwitchAgentException( "Failed to send Switch command." );
				}

				COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

				command->waitForCompletion();

				if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
				{
					std::string error;
					if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
					{
						error = "UE-090047"; 
					}
					else
					{
						error = "UE-090048";
					}
                
					throw TA_Base_Bus::VideoSwitchAgentException( error.c_str() );
				}
			}
			catch( const TA_Base_Core::DatabaseException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Switch command." );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
				throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
			}
			catch( const TA_Base_Core::DataException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Switch command." );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
				throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
			}
			catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Switch command." );
				LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
				throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
			}
		}
		else
		{
			if(videoInputEntityKey==0)
			{
				throw TA_Base_Bus::VideoSwitchAgentException( "IDS_UE_090089" );
			}
			else
			{
				throw TA_Base_Bus::VideoSwitchAgentException( "IDS_UE_090090" );
			}
		}
    }

    //
    // cycleToNextVideoInput
    //
    void COEVideoOutput::cycleToNextVideoInput()
    {
        try
        {     
            COECommunicationsHandler::RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr( new ControlSequencePlaybackCommand( 
                ControlSequencePlaybackCommand::SEQUENCE_SKIP_FORWARD, m_videoOutputData->getKey()) );

            if ( 0 == command.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to create ControlSequencePlaybackCommand object" );
                LOGMORE( SourceInfo, "Failed to send Sequence Playback command." );
                throw TA_Base_Bus::VideoSwitchAgentException( "Failed to send Sequence Playback command." );
            }

            COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

            command->waitForCompletion();

            if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
            {
                std::string error;
                if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
                {
                    error = "UE-090047"; 
                }
                else
                {
                    error = "UE-090048";
                } 
                throw TA_Base_Bus::VideoSwitchAgentException( error.c_str() );
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }         
    }


    //
    // cycleToPreviousVideoInput
    //
    void COEVideoOutput::cycleToPreviousVideoInput()
    {
        try
        {     
            COECommunicationsHandler::RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr( new ControlSequencePlaybackCommand( 
                ControlSequencePlaybackCommand::SEQUENCE_SKIP_BACKWARD, m_videoOutputData->getKey()) );

            if ( 0 == command.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to create ControlSequencePlaybackCommand object" );
                LOGMORE( SourceInfo, "Failed to send Sequence Playback command." );
                throw TA_Base_Bus::VideoSwitchAgentException( "Failed to send Sequence Playback command." );
            }

            COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

            command->waitForCompletion();

            if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
            {
                std::string error;
                if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
                {
                    error = "UE-090047"; 
                }
                else
                {
                    error = "UE-090048";
                }
                throw TA_Base_Bus::VideoSwitchAgentException( error.c_str() );
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        } 
    }


    //
    // pause
    //
    void COEVideoOutput::pause()
    {
        try
        {     
            COECommunicationsHandler::RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr( new ControlSequencePlaybackCommand( 
                ControlSequencePlaybackCommand::SEQUENCE_HOLD, m_videoOutputData->getKey() ) );

            if ( 0 == command.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to create ControlSequencePlaybackCommand object" );
                LOGMORE( SourceInfo, "Failed to send Sequence Playback command." );
                throw TA_Base_Bus::VideoSwitchAgentException( "Failed to send Sequence Playback command." );
            }

            COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

            command->waitForCompletion();

            if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
            {
                std::string error;
                if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
                {
                    error = "UE-090047"; 
                }
                else
                {
                    error = "UE-090048";
                } 
                throw TA_Base_Bus::VideoSwitchAgentException( error.c_str() );
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        } 
    }

    
    //
    // play
    //
    void COEVideoOutput::play()
    {
        try
        {     
            COECommunicationsHandler::RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr( new ControlSequencePlaybackCommand( 
                ControlSequencePlaybackCommand::SEQUENCE_PLAY, m_videoOutputData->getKey() ) );

            if ( 0 == command.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to create ControlSequencePlaybackCommand object" );
                LOGMORE( SourceInfo, "Failed to send Sequence Playback command." );
                throw TA_Base_Bus::VideoSwitchAgentException( "Failed to send Sequence Playback command." );
            }

            COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

            command->waitForCompletion();

            if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
            {
                std::string error;
                if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
                {
                    error = "UE-090047"; 
                }
                else
                {
                    error = "UE-090048";
                }
                throw TA_Base_Bus::VideoSwitchAgentException( error.c_str() );
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        } 
    }


    //
    // stop
    //
    void COEVideoOutput::stop()
    {
        try
        {     
            COECommunicationsHandler::RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr( new ControlSequencePlaybackCommand( 
                ControlSequencePlaybackCommand::SEQUENCE_HOLD, m_videoOutputData->getKey() ) );

            if ( 0 == command.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to create ControlSequencePlaybackCommand object" );
                LOGMORE( SourceInfo, "Failed to send Sequence Playback command." );
                throw TA_Base_Bus::VideoSwitchAgentException( "Failed to send Sequence Playback command." );
            }

            COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

            command->waitForCompletion();

            if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
            {
                std::string error;
                if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
                {
                    error = "UE-090047"; 
                }
                else
                {
                    error = "UE-090048";
                } 
                throw TA_Base_Bus::VideoSwitchAgentException( error.c_str() );
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Sequence Playback command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
    }




    ///////////////////////////////////////////////////////////////////////
    //
    // IStateValidationItem Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // validateState
    //
    void COEVideoOutput::validateState()
    {
        COECommunicationsHandler::RequestCommandSharedPtr statusCmd = 
            COECommunicationsHandler::RequestCommandSharedPtr( TA_COE::createRequestMonitorStatusCommand( *this ) );
        COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( statusCmd, false );
        statusCmd->waitForCompletion();
    }

	TA_COE::AlarmStack COEVideoOutput::alarmStateSequenceToAlarmStack(TA_Base_Bus::VideoOutputCorbaDef::MonitorAlarmStack alarmStateSequence)
	{
		unsigned long length= alarmStateSequence.length();
		TA_COE::AlarmStack alarmStack;
		for (unsigned long i=0; i< length; i++)
		{
			TA_COE::AlarmState state;
			
			state.eventId = alarmStateSequence[i].eventId;
			if(state.eventId == 0)
			{
				COEBVSStage* bvsStage = COESwitchManager::getInstance().getBVSStage( alarmStateSequence[i].stationId );
				if(bvsStage != 0)
				{
					state.stationId = atoi(( *bvsStage ).getAddress().c_str());
				}
				else
				{
					state.stationId = 0;
				}
			}
			else
			{
				state.stationId = alarmStateSequence[i].stationId;
			}
			alarmStack.push_back(state);
		}
		return alarmStack;
	}
	
	TA_Base_Bus::VideoOutputCorbaDef::MonitorAlarmStack COEVideoOutput::alarmStateSequenceFromAlarmStack(TA_COE::AlarmStack alarmStack)
	{
		TA_Base_Bus::VideoOutputCorbaDef::MonitorAlarmStack stateSequence;
		stateSequence.length(alarmStack.size());
		TA_COE::AlarmStack::iterator itr;
		unsigned long i=0;
		for (itr = alarmStack.begin(); itr!=alarmStack.end() && i< stateSequence.length(); itr++, i++)
		{
			stateSequence[i].eventId = (*itr).eventId;
			if(stateSequence[i].eventId == 0)
			{
				COEBVSStage* bvsStage = COESwitchManager::getInstance().getBVSStageFromId(static_cast<unsigned short>((*itr).stationId));
				stateSequence[i].stationId = bvsStage->getKey();
			}
			else
			{
				stateSequence[i].stationId = (*itr).stationId;
			}
		}
		return stateSequence;
	}

	bool COEVideoOutput::isInAlarmMode()
	{
		return m_isInAlarmMode;
	}

	bool COEVideoOutput::isBVSStageInAlarmStack(unsigned long bvsEntityKey)
	{
		if(!m_isInAlarmMode)
		{
			return false;
		}
		TA_COE::AlarmStack::iterator itr;
		for(itr = m_alarmStack.begin(); itr!=m_alarmStack.end(); itr++)
		{
			if((*itr).eventId==0)
			{
				COEBVSStage* bvsStage = COESwitchManager::getInstance().getBVSStageFromId(static_cast<unsigned short>((*itr).stationId));
				if(bvsEntityKey == bvsStage->getKey())
				{
					return true;
				}
			}
		}

		return false;
	}

} // TA_IRS_App


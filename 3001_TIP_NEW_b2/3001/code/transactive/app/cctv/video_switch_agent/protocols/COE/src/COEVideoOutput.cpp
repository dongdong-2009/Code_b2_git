/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/COEVideoOutput.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #11 $
  *
  * Last modification: $DateTime: 2013/11/08 11:10:48 $
  * Last modified by:  $Author: huang.wenbo $
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
#include "app/cctv/video_switch_agent/protocols/COE/src/SwitchQuadInputsCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/MappingTrainBorneMonitorCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/GlobalConstants.h"
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
		  m_isInAlarmMode( false ),
		  m_isShowingAckAlarm(false),
		  m_bQuadInput(false)
    {
		COESwitchManager::getInstance().addVideoOutput(*this);
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
		if(m_isInAlarmMode || m_currentVideoInputKey!=videoInputKey || m_bQuadInput)
		{
			m_isInAlarmMode = false;
			m_isShowingAckAlarm = false;
			m_alarmStack.clear();
			m_currentVideoInputKey = videoInputKey;
			m_bQuadInput=false;
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
	// notifyInputSwitchedToOutput
	//
	void COEVideoOutput::notifyInputsSwitchedToQuad( unsigned long topLeftVideoInput, unsigned long topRightVideoInput,
					unsigned long bottomLeftVideoInput, unsigned long bottomRightVideoInput )
	{
		if(m_isInAlarmMode || m_quadInputs.cameraW !=topLeftVideoInput || m_quadInputs.cameraX !=topRightVideoInput 
			|| m_quadInputs.cameraY !=bottomLeftVideoInput || m_quadInputs.cameraZ !=bottomRightVideoInput)
		{

			m_isInAlarmMode = false;
			m_isShowingAckAlarm = false;
			m_alarmStack.clear();

			m_quadInputs.cameraW =topLeftVideoInput;
			m_quadInputs.cameraX =topRightVideoInput; 
			m_quadInputs.cameraY =bottomLeftVideoInput;
			m_quadInputs.cameraZ =bottomRightVideoInput;
			m_bQuadInput=true;
			//if we have been switched to a BVS we need to tell it so that it can keep track
			
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


	void COEVideoOutput::notifyOutputAlarm(const TA_COE::AlarmStack& alarmStack)
	{
		m_alarmStack = alarmStack;
		if(!m_alarmStack.empty())
		{
			m_isInAlarmMode = true;
			m_isShowingAckAlarm = false;
			m_bQuadInput = false;
			m_currentVideoInputKey = 0;
		}
		else
		{
			m_isInAlarmMode = false;
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

	/*
	void COEVideoOutput::notifyOutputAlarm(unsigned long eventId,unsigned long stationId,bool isOpenAlarm)
	{
		TA_COE::AlarmState state;
		state.eventId = eventId;
		state.stationId = stationId;
		if(isOpenAlarm)
		{
			m_isInAlarmMode=true;
			m_isShowingAckAlarm = false;
			m_bQuadInput = false;
			m_alarmStack.insert(state);
			m_currentVideoInputKey = 0;
		}
		else
		{
			m_alarmStack.erase(state);
			if(m_alarmStack.empty())
			{
				m_isInAlarmMode = false;
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
	}*/

    /**
	  *
	  */
	void COEVideoOutput::notifyShowingAckAlarm(unsigned long eventId,unsigned long stationId,bool bAlarmMode)
	{
		TA_COE::AlarmState state;
		state.eventId = eventId;
		state.stationId = stationId;
		m_isShowingAckAlarm=bAlarmMode;
		if(bAlarmMode)
		{
			m_isInAlarmMode = false;
			m_bQuadInput = false;
			if(m_alarmStack.find(state) == m_alarmStack.end())
			{
				m_alarmStack.clear();
				m_alarmStack.insert(state);
			}
			m_currentVideoInputKey = 0;

		}
		else
		{
			m_alarmStack.erase(state);
			if(m_alarmStack.empty())
			{
				m_isInAlarmMode = false;
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

	void COEVideoOutput::notifyOutputShowingAlarmStack( TA_COE::AlarmStack alarmStack)
	{
		if(!m_isInAlarmMode || m_alarmStack!=alarmStack)
		{
			m_isInAlarmMode=true;
			m_bQuadInput = false;
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
	
	void COEVideoOutput::notifyMappingTrainBorneMonitor()
	{
		m_isInAlarmMode=false;
		m_isShowingAckAlarm = false;
		m_currentVideoInputKey = NULL;
		this->m_bQuadInput = false;
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
	//
    // updateState
    //
    void COEVideoOutput::updateState( const TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState& state )
    {
        m_currentVideoInputKey = state.currentVideoInputKey;
		m_isInAlarmMode = state.isInAlarmMode;
		m_isShowingAckAlarm = state.isShowingAckAlarm ;
		m_alarmStack = alarmStateSequenceToAlarmStack(state.alarmStack);
		m_bQuadInput = state.isQuadInput;
		m_quadInputs = state.cameras;
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
		state->isShowingAckAlarm = m_isShowingAckAlarm;
		state->isQuadInput=m_bQuadInput;
		state->cameras=m_quadInputs;
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
				COECommunicationsHandler::RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr(TA_COE::createSwitchInputToOutputCommand(videoInputEntityKey, *this ) );

				COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

				command->waitForCompletion();

				if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
				{
					if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
					{
						throw EXCEPTION_CONNECTION_WITH_VSS;
					}
					throw EXCEPTION_PERFORMING_REQUEST;
				}
			}
			catch( const TA_Base_Core::DatabaseException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Switch command." );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
				throw EXCEPTION_PERFORMING_REQUEST;
			}
			catch( const TA_Base_Core::DataException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Switch command." );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
				throw EXCEPTION_PERFORMING_REQUEST;
			}
			catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Switch command." );
				LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
				throw EXCEPTION_PERFORMING_REQUEST;
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "The monitor is in alarm mode");
			throw EXCEPTION_PERFORMING_REQUEST;
		}
    }

	//
	// requestSwitchOfInput
	//
	void COEVideoOutput::assignQuad( unsigned long cameraW,unsigned long cameraX,unsigned long cameraY,unsigned long cameraZ, 
		bool isDemand )
	{
		if(!m_isInAlarmMode)
		{
			try
			{
				COECommunicationsHandler::RequestCommandSharedPtr command;
				try
				{
					command = COECommunicationsHandler::RequestCommandSharedPtr( new SwitchQuadInputsCommand( 
						cameraW,cameraX,cameraY,cameraZ, *this ) );
				}
				catch (...)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create SwitchInputToOutputCommand object" );
					throw EXCEPTION_PERFORMING_REQUEST;
				}
				COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

				command->waitForCompletion();

				if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
				{
					if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
					{
						throw EXCEPTION_CONNECTION_WITH_VSS;
					}
					throw EXCEPTION_PERFORMING_REQUEST;
				}
			}
			catch( const TA_Base_Core::DatabaseException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Switch command." );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
				throw EXCEPTION_PERFORMING_REQUEST;
			}
			catch( const TA_Base_Core::DataException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Switch command." );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
				throw EXCEPTION_PERFORMING_REQUEST;
			}
			catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Switch command." );
				LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
				throw EXCEPTION_PERFORMING_REQUEST;
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "The monitor is in alarm mode");
			throw EXCEPTION_PERFORMING_REQUEST;
		}
	}

	bool COEVideoOutput::mappingTrainBorneMonitor(bool isDemand)
	{
		if(!m_isInAlarmMode)
		{
			try
			{
				COECommunicationsHandler::RequestCommandSharedPtr command;
				try
				{
					command = COECommunicationsHandler::RequestCommandSharedPtr( new MappingTrainBorneMonitorCommand( *this ) );
				}
				catch (...)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create SwitchInputToOutputCommand object" );
					throw EXCEPTION_PERFORMING_REQUEST;
				}
				COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

				command->waitForCompletion();

				if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
				{
					if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
					{
						throw EXCEPTION_CONNECTION_WITH_VSS;
					}
					throw EXCEPTION_PERFORMING_REQUEST;
				}
			}
			catch( const TA_Base_Core::DatabaseException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Switch command." );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
				throw EXCEPTION_PERFORMING_REQUEST;
			}
			catch( const TA_Base_Core::DataException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Switch command." );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
				throw EXCEPTION_PERFORMING_REQUEST;
			}
			catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Switch command." );
				LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
				throw EXCEPTION_PERFORMING_REQUEST;
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "The monitor is in alarm mode");
			throw EXCEPTION_PERFORMING_REQUEST;
		}
		notifyMappingTrainBorneMonitor();
		return true;
	}
    ///////////////////////////////////////////////////////////////////////
    //
    // IStateValidationItem Methods
    //
    ///////////////////////////////////////////////////////////////////////


	TA_COE::AlarmStack COEVideoOutput::alarmStateSequenceToAlarmStack(TA_Base_Bus::VideoOutputCorbaDef::MonitorAlarmStack alarmStateSequence)
	{
		unsigned long length= alarmStateSequence.length();
		TA_COE::AlarmStack alarmStack;
		for (unsigned long i=0; i< length; i++)
		{
			TA_COE::AlarmState state;
			
			state.eventId = alarmStateSequence[i].eventId;
			state.stationId = alarmStateSequence[i].stationId;
			alarmStack.insert(state);
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
			stateSequence[i].stationId = (*itr).stationId;
		}
		return stateSequence;
	}

	bool COEVideoOutput::isInAlarmMode()
	{
		return m_isInAlarmMode;
	}

	bool COEVideoOutput::isShowingAckAlarm()
	{
		return m_isShowingAckAlarm;
	}

	bool COEVideoOutput::isQuadInput()
	{
		return m_bQuadInput;
	}

	TA_Base_Bus::VideoOutputCorbaDef::QuadCameras COEVideoOutput::getQuadCameras()
	{
		return m_quadInputs;
	}

} // TA_IRS_App


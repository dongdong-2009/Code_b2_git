/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/IMOS/src/IMOSVideoOutput.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/08/21 08:52:08 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the protocol dependent sections of the operations for a VideoOutput
  * using the IMOS protocol. 
  * Inherits from IPDVideoOutput. Before the methods
  * in this interface are called, the user privileges have been checked and
  * that the protocol supports the required features.
  */

#include <sstream>
#include <iostream>
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSCommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSSwitchManager.h"
//#include "app/cctv/video_switch_agent/protocols/IMOS/src/SetSequenceConfigCommand.h"
//#include "app/cctv/video_switch_agent/protocols/IMOS/src/StateValidationCycle.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSHelper.h"
/*
#include "app/cctv/video_switch_agent/protocols/IMOS/src/SwitchInputToOutputCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/PTZCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/ActiveVideoInputCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/ControlSequencePlaybackCommand.h"
*/
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/video_switch_agent/src/IVideoTriggeringEvent.h"
#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEventAccessFactory.h"
//#include "app/cctv/video_switch_agent/protocols/IMOS/src/CommandPackage.h"

#include "core/data_access_interface/video_switch_agent/src/SequenceHelper.h"
using namespace std;
namespace TA_IRS_App
{
    //
    // IMOSVideoOutput
    //
	namespace
    {
        static const int MAX_SOURCES_PER_SEQUENCE = 64;
    }

    IMOSVideoOutput::IMOSVideoOutput( TA_Base_Core::VideoOutputPtr videoOutputData )
        : m_videoOutputData( videoOutputData ),
          m_currentVideoInputKey( 0 ),
		  m_isInAlarmMode( false )
    {
        IMOSSwitchManager::getInstance().addVideoOutput( *this );
//        IMOSSwitchManager::getInstance().getStateValidationCycle().addValidationItem( *this, false );

		// Commented for TD16066
/*		TA_IRS_Core::IVideoTriggeringEvents triggeringData = TA_IRS_Core::VideoTriggeringEventAccessFactory::getInstance().getAllVideoTriggeringEvents();
		TA_IRS_Core::IVideoTriggeringEvents::iterator itr;
		for(itr = triggeringData.begin(); itr != triggeringData.end(); itr++)
		{
			TA_IMOS::AlarmState state;
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
    unsigned long IMOSVideoOutput::getKey()
    {
        return m_videoOutputData->getKey();
    }

    
    //
    // getAddress
    //
    std::string IMOSVideoOutput::getAddress()
    {
        try
        {
		    return m_videoOutputData->getAddress();
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
        }	
    }
 

    ///////////////////////////////////////////////////////////////////////
    //
    // State Update Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // notifyInputSwitchedToOutput
    //
    void IMOSVideoOutput::notifyInputSwitchedToOutput( unsigned long videoInputKey )
    {
		if(m_isInAlarmMode || m_currentVideoInputKey!=videoInputKey)
		{
			
			m_isInAlarmMode = false;
			m_alarmStack.clear();
			m_currentVideoInputKey = videoInputKey;
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
	void IMOSVideoOutput::notifyOutputShowingAlarmStack( TA_IMOS::AlarmStack alarmStack)
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
    void IMOSVideoOutput::updateState( const TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState& state )
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
    unsigned long IMOSVideoOutput::getCurrentVideoInputKey()
    {
        return m_currentVideoInputKey;
    }


    //
    // getVideoOutputState
    //
    TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState* IMOSVideoOutput::getVideoOutputState()
	{
		TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState* state = new TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState();
		state->currentVideoInputKey=m_currentVideoInputKey;
		state->alarmStack=alarmStateSequenceFromAlarmStack(m_alarmStack);
		state->isInAlarmMode=m_isInAlarmMode;

		return state;
	}

	// IMOS
	void IMOSVideoOutput::requestSingleCamToOutput(unsigned long camKey)
	{
		std::string outPutAddress = m_videoOutputData->getAddress();
		IMOSCamera *cam = IMOSSwitchManager::getInstance().getCamera(camKey);
		if(cam == 0)
		{
			std::stringstream ss;
			ss << "Not find coecamera entity " << camKey;
			throw TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
				ss.str());
		}
		std::string camAddress = cam->getAddress();
		IMOSSwitchManager::getInstance().getCommunicationsHandler()->requestSingleCamToOutput(camAddress, outPutAddress);
	}

	void IMOSVideoOutput::requestSequenceToOutput(unsigned long seqKey)
	{
		std::string outPutAddress = m_videoOutputData->getAddress();
		IMOSSequence *seq = IMOSSwitchManager::getInstance().getSequence(seqKey);
		if(seq == 0)
		{
			std::stringstream ss;
			ss << "Not find coesequence object" << seqKey;
			throw TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
				ss.str());
		}
		std::string seqAddress = seq->getAddress();
		IMOSSwitchManager::getInstance().getCommunicationsHandler()->requestSequenceToOutput(seqAddress, outPutAddress);

	}

	void IMOSVideoOutput::deactiveSequence(unsigned long seqKey)
	{
		std::string outPutAddress = m_videoOutputData->getAddress();
		IMOSSequence *seq = IMOSSwitchManager::getInstance().getSequence(seqKey);
		if(seq == 0)
		{
			std::stringstream ss;
			ss << "Not find coesequence object" << seqKey;
			throw TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
				ss.str());
		}
		std::string seqAddress = seq->getAddress();
		IMOSSwitchManager::getInstance().getCommunicationsHandler()->deactiveSequence(seqAddress, outPutAddress);
	}

	void IMOSVideoOutput::requestQuadToOutput(unsigned long quadKey)
	{
		std::string outPutAddress = m_videoOutputData->getAddress();
		IMOSQuad* quad = IMOSSwitchManager::getInstance().getQuad(quadKey);
		if(quad == 0)
		{
			std::stringstream ss;
			ss << "Not find coequad object" << quadKey;
			throw TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
				ss.str());
		}
		std::string quadAddress = quad->getAddress();
		IMOSSwitchManager::getInstance().getCommunicationsHandler()->requestQuadToOutput(quadAddress, outPutAddress);
	}

	void IMOSVideoOutput::stopMonitor()
	{
		std::string outPutAddress = m_videoOutputData->getAddress();
		IMOSSwitchManager::getInstance().getCommunicationsHandler()->stopMonitor(outPutAddress);
	}
	//
    // requestSwitchOfInput
    //
    void IMOSVideoOutput::requestSwitchOfInput( unsigned long videoInputEntityKey, 
                               bool isDemand )
    {
		// the input should not be quad
		if(!m_isInAlarmMode)
		{
			try
			{
				if(m_currentVideoInputKey == videoInputEntityKey) // if not change, just return
				{
					return;
				}
				if(videoInputEntityKey == 0) // if zero, clear the monitor
				{
		            switch ( IMOSSwitchManager::getInstance().getInputType( m_currentVideoInputKey ) )
		            {
		            case IMOSSwitchManager::CAMERA:
						stopMonitor();
		                break;
						/*
		            case IMOSSwitchManager::QUAD:
						requestQuadToOutput(videoInputEntityKey);
		                break;
						*/
		            case IMOSSwitchManager::SEQUENCE:
						deactiveSequence(m_currentVideoInputKey);
		                break;
		            default:
						TA_ASSERT(false, "Unknow input type");
		            }
				}
				else
				{
		            switch ( IMOSSwitchManager::getInstance().getInputType( videoInputEntityKey ) )
		            {
		            case IMOSSwitchManager::CAMERA:
						requestSingleCamToOutput(videoInputEntityKey);
		                break;
						/*
		            case IMOSSwitchManager::QUAD:
						requestQuadToOutput(videoInputEntityKey);
		                break;
						*/
		            case IMOSSwitchManager::SEQUENCE:
						requestSequenceToOutput(videoInputEntityKey);
		                break;
		            default:
						TA_ASSERT(false, "Unknow input type");
		            }
				}

				// broadcast the input update message
				notifyInputSwitchedToOutput(videoInputEntityKey);

				/*
				TA_IRS_Core::SequenceHelper sequenceHelper;
				if(videoInputEntityKey != 0)
				{
					sequenceHelper.setRelationShipOfParentKey( this->getKey(),videoInputEntityKey,"", RL_MONITOR_CAMERA_NORMAL);
				}
				else
				{
					sequenceHelper.delRelationShipOfParentKey(this->getKey());
				}
				*/
			}
			catch( const TA_Base_Core::DatabaseException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
				throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
			}
			catch( const TA_Base_Core::DataException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
				throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
			}
			catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
				throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
			}
		}
		else
		{
			if(videoInputEntityKey==0)
			{
				throw TA_Base_Bus_GZL9::VideoSwitchAgentException( "IDS_UE_090089" );
			}
			else
			{
				throw TA_Base_Bus_GZL9::VideoSwitchAgentException( "IDS_UE_090090" );
			}
		}
    }

    //
    // connectSequence
    //
    void IMOSVideoOutput::connectSequence( const TA_Base_Bus_GZL9::VideoOutputCorbaDef::UserDefinedSequenceConfig& newConfig )
    {
    }

    //
    // connectSequence
    //
    void IMOSVideoOutput::disconnectSequence(  )
    {
    }

    //
    // cycleToNextVideoInput
    //
    void IMOSVideoOutput::cycleToNextVideoInput()
    {
    }


    //
    // cycleToPreviousVideoInput
    //
    void IMOSVideoOutput::cycleToPreviousVideoInput()
    {
    }


    //
    // pause
    //
    void IMOSVideoOutput::pause()
    {
    }

    
    //
    // play
    //
    void IMOSVideoOutput::play()
    {
    }


    //
    // stop
    //
    void IMOSVideoOutput::stop()
    {
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // IStateValidationItem Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // validateState
    //
    void IMOSVideoOutput::validateState()
    {
    }

	TA_IMOS::AlarmStack IMOSVideoOutput::alarmStateSequenceToAlarmStack(TA_Base_Bus_GZL9::VideoOutputCorbaDef::MonitorAlarmStack alarmStateSequence)
	{
		unsigned long length= alarmStateSequence.length();
		TA_IMOS::AlarmStack alarmStack;
		for (unsigned long i=0; i< length; i++)
		{
			TA_IMOS::AlarmState state;
			
			state.eventId = alarmStateSequence[i].eventId;
			if(state.eventId == 0)
			{
				state.stationId = 0;
			}
			else
			{
				state.stationId = alarmStateSequence[i].stationId;
			}
			alarmStack.push_back(state);
		}
		return alarmStack;
	}
	
	TA_Base_Bus_GZL9::VideoOutputCorbaDef::MonitorAlarmStack IMOSVideoOutput::alarmStateSequenceFromAlarmStack(TA_IMOS::AlarmStack alarmStack)
	{
		TA_Base_Bus_GZL9::VideoOutputCorbaDef::MonitorAlarmStack stateSequence;
		stateSequence.length(alarmStack.size());
		TA_IMOS::AlarmStack::iterator itr;
		unsigned long i=0;
		for (itr = alarmStack.begin(); itr!=alarmStack.end() && i< stateSequence.length(); itr++, i++)
		{
			stateSequence[i].eventId = (*itr).eventId;
			stateSequence[i].stationId = (*itr).stationId;
		}
		return stateSequence;
	}

	bool IMOSVideoOutput::isInAlarmMode()
	{
		return m_isInAlarmMode;
	}

	void IMOSVideoOutput::activeVideoInput(::CORBA::ULong videoInputEntityKey, ::CORBA::Boolean active,  ::CORBA::Boolean isDemand)
	{
		/*
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"CIMOSVideoOutput::activeVideoInput be called.--(%d)(%d)",videoInputEntityKey,active);
		try
		{
			if(IMOSSwitchManager::SEQUENCE != IMOSSwitchManager::getInstance().getInputType( videoInputEntityKey ))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "activeVideoInput only valid to sequence");
				return;
			}
			if(active)
			{
				requestSequenceToOutput(videoInputEntityKey);
			}
			else
			{
				deactiveSequence(videoInputEntityKey);
			}

			// broadcast the input update message
			notifyInputSwitchedToOutput(videoInputEntityKey);

			TA_IRS_Core::SequenceHelper sequenceHelper;
			if(active != 0)
			{
				sequenceHelper.setRelationShipOfParentKey( this->getKey(),videoInputEntityKey,RL_MONITOR_CAMERA_NORMAL);
			}
			else
			{
				sequenceHelper.delRelationShipOfParentKey(this->getKey());
			}
			
		}
		catch (const TA_Base_Core::DatabaseException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
		}
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
		}
		catch (const TA_Base_Core::VideoSwitchAgentInternalException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
		}
		*/
	}
} // TA_IRS_App


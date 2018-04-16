/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSRecordingUnit.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/08/21 08:52:08 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the protocol dependent sections of the general functions for a Quad
  * using the MODBUS protocol.
  * Inherits from the IPDRecordingUnit class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Quad and
  * that the Quad and the protocol support the required features.
  */

#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSRecordingUnit.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSSwitchManager.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/StateValidationCycle.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSHelper.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSCommunicationsHandler.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/SetRecordingUnitModeCommand.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"

#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    const unsigned short NUM_RECORDING_SPEEDS = 3;
    const unsigned short NOT_RECORDING_INDEX = 0;
    const unsigned short REAL_TIME_INDEX = 1;
    const unsigned short TIME_LAPSE_INDEX = 2;
    static const char* NOT_RECORDING = "Not Recording";
    static const char* REAL_TIME_RECORDING_SPEED = "Real-Time Recording";
    static const char* TIME_LAPSE_RECORDING_SPEED = "Time-Lapse Recording";

    //
    // MODBUSRecordingUnit
    //
    MODBUSRecordingUnit::MODBUSRecordingUnit( TA_Base_Core::RecordingUnitPtr recordingUnitData )
    : m_recordingUnitData( recordingUnitData )
    {
        m_recordingUnitState.currentRecordingSpeedIndex = TIME_LAPSE_INDEX;
        MODBUSSwitchManager::getInstance().addRecordingUnit( *this );
//        MODBUSSwitchManager::getInstance().getStateValidationCycle().addValidationItem( *this, false );
    }


    unsigned long MODBUSRecordingUnit::getKey()
    {
        return m_recordingUnitData->getKey();
    }

    
    std::string MODBUSRecordingUnit::getAddress()
    {
        FUNCTION_ENTRY( "getAddress" );

        return m_recordingUnitData->getAddress();

        FUNCTION_EXIT;
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // IPDRecordingUnit Methods
    //
    ///////////////////////////////////////////////////////////////////////

    //
    // getSupportedFeatures
    //
    TA_Base_Bus_GZL9::RecordingUnitCorbaDef::SupportedRecordingUnitFeatures MODBUSRecordingUnit::getSupportedFeatures()
    {
        FUNCTION_ENTRY( "getSupportedFeatures" );

        TA_Base_Bus_GZL9::RecordingUnitCorbaDef::SupportedRecordingUnitFeatures recordingUnitFeatures;
        recordingUnitFeatures.isPlaybackControllable = false;
        recordingUnitFeatures.isRecordingControllable = false;
        recordingUnitFeatures.isRecordingSpeedControllable = false;
        try
        {
            recordingUnitFeatures.isPlaybackControllable = m_recordingUnitData->isPlaybackSupported();
            recordingUnitFeatures.isRecordingControllable = m_recordingUnitData->isRecordingSupported();
            recordingUnitFeatures.isRecordingSpeedControllable = m_recordingUnitData->isRecordingSpeedSupported();
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            std::stringstream details;
            details << "Assuming all recording unit features are not supported.";
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", details.str().c_str() );
        }        
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            std::stringstream details;
            details << "Assuming all recording unit features are not supported.";
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", details.str().c_str() );
        }

        FUNCTION_EXIT;
        return recordingUnitFeatures;
    }


    //
    // getRecordingUnitState
    //
    TA_Base_Bus_GZL9::RecordingUnitCorbaDef::RecordingUnitState MODBUSRecordingUnit::getRecordingUnitState()
    {
        return m_recordingUnitState;
    }


    //
    // getRecordingSpeeds
    //
	TA_Base_Bus_GZL9::RecordingUnitCorbaDef::recordingSpeedSequence* MODBUSRecordingUnit::getRecordingSpeeds()
    {
        FUNCTION_ENTRY( "getRecordingSpeeds" );

        TA_Base_Bus_GZL9::RecordingUnitCorbaDef::recordingSpeedSequence* recordingSpeeds = new TA_Base_Bus_GZL9::RecordingUnitCorbaDef::recordingSpeedSequence;
        recordingSpeeds->length( NUM_RECORDING_SPEEDS );
        ( *recordingSpeeds )[ NOT_RECORDING_INDEX ] = NOT_RECORDING;
        ( *recordingSpeeds )[ REAL_TIME_INDEX ] = REAL_TIME_RECORDING_SPEED;
        ( *recordingSpeeds )[ TIME_LAPSE_INDEX ] = TIME_LAPSE_RECORDING_SPEED;
        
        FUNCTION_EXIT;
        return recordingSpeeds;
    }


	//
    // play
    //
    void MODBUSRecordingUnit::play()
    {
        // TODO
    }


	//
    // fastForward
    //
	void MODBUSRecordingUnit::fastForward()
    {
        // TODO
    }


	//
    // rewind
    //
	void MODBUSRecordingUnit::rewind()
    {
        // TODO
    }


    //
    // pause
    //
	void MODBUSRecordingUnit::pause()
    {
        // TODO
    }


	//
    // stopPlayback
    //
	void MODBUSRecordingUnit::stopPlayback()
    {
        // TODO
    }


	//
    // skipToNextChapter
    //
	void MODBUSRecordingUnit::skipToNextChapter()
    {
        // TODO
    }


	//
    // skipToPreviousChapter
    //
	void MODBUSRecordingUnit::skipToPreviousChapter()
    {
    }


	//
    // record
    //
	void MODBUSRecordingUnit::record()
    {
    }


	//
    // stopRecording
    //
	void MODBUSRecordingUnit::stopRecording()
    {
    }


	//
    // setRecordingSpeed
    //
	void MODBUSRecordingUnit::setRecordingSpeed( unsigned short recordingSpeedIndex )
    {
		/*
        try
        {
            MODBUSCommunicationsHandler::RequestCommandSharedPtr command = MODBUSCommunicationsHandler::RequestCommandSharedPtr( new 
                SetRecordingUnitModeCommand( recordingSpeedIndex, *this ) );

            if ( 0 == command.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to create SwitchQuadInputsCommand object" );
                LOGMORE( SourceInfo, "Failed to send Switch command." );
                throw TA_Base_Bus_GZL9::VideoSwitchAgentException( "Failed to send RecordingUnit Mode command." );
            }

            MODBUSSwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

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
                throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.c_str() );
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send RecordingUnit Mode command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send RecordingUnit Mode command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send RecordingUnit Mode command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
        }  
		*/
    }


    //
    // notifyRecordingSpeedSet
    //
    void MODBUSRecordingUnit::notifyRecordingSpeedSet( unsigned short recordingSpeedIndex )
    {
        m_recordingUnitState.currentRecordingSpeedIndex = recordingSpeedIndex;

        try
        {
            StateUpdateBroadcaster::getInstance().notifyRecordingUnitStateUpdate( m_recordingUnitData->getKey() );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
                "Failed to notify the agent's clients that a recording units recording speed has been set." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
                "Failed to notify the agent's clients that a recording units recording speed has been set." );
        }	
    }


    //
    // updateState
    //
    void MODBUSRecordingUnit::updateState( const TA_Base_Bus_GZL9::RecordingUnitCorbaDef::RecordingUnitState& state )
    {
        m_recordingUnitState.currentRecordingSpeedIndex = state.currentRecordingSpeedIndex;
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IStateValidationItem Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // validateState
    //
    void MODBUSRecordingUnit::validateState()
    {
		/*
        MODBUSCommunicationsHandler::RequestCommandSharedPtr statusCmd = 
            MODBUSCommunicationsHandler::RequestCommandSharedPtr( TA_MODBUS::createRequestRecordingUnitStatusCommand( *this ) );
        MODBUSSwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( statusCmd, false );
        statusCmd->waitForCompletion();
		*/
    }

} // namespace TA_IRS_App

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/COERecordingUnit.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/17 14:59:54 $
  * Last modified by:  $Author: builder $
  *
  * Handles the protocol dependent sections of the general functions for a Quad
  * using the COE protocol.
  * Inherits from the IPDRecordingUnit class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Quad and
  * that the Quad and the protocol support the required features.
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/COERecordingUnit.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/StateValidationCycle.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SetRecordingUnitModeCommand.h"
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
    const char* NOT_RECORDING = "Not Recording";
    const char* REAL_TIME_RECORDING_SPEED = "Real-Time Recording";
    const char* TIME_LAPSE_RECORDING_SPEED = "Time-Lapse Recording";

    //
    // COERecordingUnit
    //
    COERecordingUnit::COERecordingUnit( TA_Base_Core::RecordingUnitPtr recordingUnitData )
    : m_recordingUnitData( recordingUnitData )
    {
        m_recordingUnitState.currentRecordingSpeedIndex = TIME_LAPSE_INDEX;
        COESwitchManager::getInstance().addRecordingUnit( *this );
        COESwitchManager::getInstance().getStateValidationCycle().addValidationItem( *this, false );
    }


    unsigned long COERecordingUnit::getKey()
    {
        return m_recordingUnitData->getKey();
    }

    
    std::string COERecordingUnit::getAddress()
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
    TA_Base_Bus::RecordingUnitCorbaDef::SupportedRecordingUnitFeatures COERecordingUnit::getSupportedFeatures()
    {
        FUNCTION_ENTRY( "getSupportedFeatures" );

        TA_Base_Bus::RecordingUnitCorbaDef::SupportedRecordingUnitFeatures recordingUnitFeatures;
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
    TA_Base_Bus::RecordingUnitCorbaDef::RecordingUnitState COERecordingUnit::getRecordingUnitState()
    {
        return m_recordingUnitState;
    }


    //
    // getRecordingSpeeds
    //
	TA_Base_Bus::RecordingUnitCorbaDef::recordingSpeedSequence* COERecordingUnit::getRecordingSpeeds()
    {
        FUNCTION_ENTRY( "getRecordingSpeeds" );

        TA_Base_Bus::RecordingUnitCorbaDef::recordingSpeedSequence* recordingSpeeds = new TA_Base_Bus::RecordingUnitCorbaDef::recordingSpeedSequence;
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
    void COERecordingUnit::play()
    {
        // TODO
    }


	//
    // fastForward
    //
	void COERecordingUnit::fastForward()
    {
        // TODO
    }


	//
    // rewind
    //
	void COERecordingUnit::rewind()
    {
        // TODO
    }


    //
    // pause
    //
	void COERecordingUnit::pause()
    {
        // TODO
    }


	//
    // stopPlayback
    //
	void COERecordingUnit::stopPlayback()
    {
        // TODO
    }


	//
    // skipToNextChapter
    //
	void COERecordingUnit::skipToNextChapter()
    {
        // TODO
    }


	//
    // skipToPreviousChapter
    //
	void COERecordingUnit::skipToPreviousChapter()
    {
    }


	//
    // record
    //
	void COERecordingUnit::record()
    {
    }


	//
    // stopRecording
    //
	void COERecordingUnit::stopRecording()
    {
    }


	//
    // setRecordingSpeed
    //
	void COERecordingUnit::setRecordingSpeed( unsigned short recordingSpeedIndex )
    {
        try
        {
            COECommunicationsHandler::RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr( new 
                SetRecordingUnitModeCommand( recordingSpeedIndex, *this ) );

            if ( 0 == command.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to create SwitchQuadInputsCommand object" );
                LOGMORE( SourceInfo, "Failed to send Switch command." );
                throw TA_Base_Bus::VideoSwitchAgentException( "Failed to send RecordingUnit Mode command." );
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
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send RecordingUnit Mode command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send RecordingUnit Mode command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send RecordingUnit Mode command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }  
    }


    //
    // notifyRecordingSpeedSet
    //
    void COERecordingUnit::notifyRecordingSpeedSet( unsigned short recordingSpeedIndex )
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
    void COERecordingUnit::updateState( const TA_Base_Bus::RecordingUnitCorbaDef::RecordingUnitState& state )
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
    void COERecordingUnit::validateState()
    {
        COECommunicationsHandler::RequestCommandSharedPtr statusCmd = 
            COECommunicationsHandler::RequestCommandSharedPtr( TA_COE::createRequestRecordingUnitStatusCommand( *this ) );
        COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( statusCmd, false );
        statusCmd->waitForCompletion();
    }

} // namespace TA_IRS_App

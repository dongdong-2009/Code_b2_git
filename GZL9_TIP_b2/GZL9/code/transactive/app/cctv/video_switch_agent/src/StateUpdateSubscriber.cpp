/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/src/StateUpdateSubscriber.cpp $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Subscribes to and processes VisualStateUpdate messages from the 
  * Video Switch Agent that is in Control mode.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/src/StateUpdateSubscriber.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/Camera.h"
#include "app/cctv/video_switch_agent/src/Quad.h"
#include "app/cctv/video_switch_agent/src/Sequence.h"
#include "app/cctv/video_switch_agent/src/RecordingUnit.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/src/VideoInput.h"
#include "app/cctv/video_switch_agent/src/VideoOutput.h"
//#include "app/cctv/video_switch_agent/src/BVSStage.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/message/types/VisualComms_MessageTypes.h"
#include "core/message/types/VisualStateUpdate_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

namespace
{
    //
    // Log messages.
    //
    const std::string PRESET_NAME_UPDATE_FAILED_LOG_MSG = 
        "Failed to update the name of preset %d for Camera %ul to %s.";
    const std::string CAMERA_STATE_UPDATE_FAILED_LOG_MSG = 
        "Failed to update the state of Camera %ul.";
    const std::string VIDEO_INPUT_STATE_UPDATE_FAILED_LOG_MSG = 
        "Failed to update the state of VideoInput %ul.";
    const std::string VIDEO_OUTPUT_STATE_UPDATE_FAILED_LOG_MSG = 
        "Failed to update the state of VideoOutput %ul.";
    const std::string SEQUENCE_STATE_UPDATE_FAILED_LOG_MSG =
        "Failed to update the state of Sequence %ul.";
    const std::string SEQUENCE_CONFIG_UPDATE_FAILED_LOG_MSG =
        "Failed to update the sequence config of Sequence %ul.";
    const std::string QUAD_STATE_UPDATE_FAILED_LOG_MSG = 
        "Failed to update the state of Quad %ul.";
    const std::string RECORDING_UNIT_STATE_UPDATE_FAILED_LOG_MSG = 
        "Failed to update the state of RecordingUnit %ul.";
//    const std::string BVS_STATE_UPDATE_FAILED_LOG_MSG = 
//        "Failed to update the state of BVSStage %ul.";
    const std::string FAILED_TO_GET_OBJECT_LOG_MSG =
        "Failed to get the %s object with the key %ul.";
}

namespace TA_IRS_App
{

    ///////////////////////////////////////////////////////////////////////
    //
    // SpecialisedMessageSubscriber Method
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // receiveSpecialisedMessage
    //
    void StateUpdateSubscriber::receiveSpecialisedMessage( const TA_Base_Core::StateUpdateMessageCorbaDef& message )
    {
	    // Extract the stateUpdateInfo from the State Update Message

        // Check the mode that the agent is in.  
        // In Control mode, we should only receive VideoSwitchAgentUpdateRequest messages.  
        // In Monitor mode, we should only receive VideoSwitchAgentUpdate messages.  
        
        if ( AgentModeMonitor::getInstance().isInControlMode() )
        {
            if ( 0 == TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdate.getTypeKey().compare( 
                message.messageTypeKey ) )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Received VideoSwitchAgentUpdate message while in Control mode." );
                return;
            }
            else if ( 0 != TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdateRequest.getTypeKey().compare( 
                message.messageTypeKey ) )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Received an unknown message." );
                return;
            }
            else
            {
                StateUpdateBroadcaster::getInstance().notifyAgentState();
                return;
            }
        }
        else 
        {
            if ( 0 == TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdateRequest.getTypeKey().compare( 
                message.messageTypeKey ) )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Received VideoSwitchAgentUpdateRequest message while in Monitor mode." );
                return;
            }
            else if ( 0 != TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdate.getTypeKey().compare( 
                message.messageTypeKey ) )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Received an unknown message." );
                return;
            }
        
            // Now try and extract it to a recognised type.
            
            TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState* videoOutputState = 0;
            if ( 0 != ( message.stateUpdateInfo >>= videoOutputState ) )   // Is this a VideoOutput state update message?
		    {
			    updateVideoOutputState( message.assocEntityKey, *videoOutputState );
                return;
		    }

            TA_Base_Bus_GZL9::QuadCorbaDef::QuadState* quadState = 0;
            if ( 0 != ( message.stateUpdateInfo >>= quadState ) )   // Is this a Quad state update message?
		    {
			    updateQuadState( message.assocEntityKey, *quadState );
                return;
		    }

            TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig* sequenceConfig = 0;
            if ( 0 != ( message.stateUpdateInfo >>= sequenceConfig ) )   // Is this a Sequence SequenceConfig update message?
		    {
			    updateSequenceConfig( message.assocEntityKey, *sequenceConfig );
                return;
		    }

            TA_Base_Bus_GZL9::RecordingUnitCorbaDef::RecordingUnitState* recordingUnitState = 0;
            if ( 0 != ( message.stateUpdateInfo >>= recordingUnitState ) )   // Is this a ReocrdingUnit state update message?
		    {
			    updateRecordingUnitState( message.assocEntityKey, *recordingUnitState );
                return;
		    }

            TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceState* sequenceState = 0;
            if ( 0 != ( message.stateUpdateInfo >>= sequenceState ) )   // Is this a VideoOutput state update message?
		    {
			    updateSequenceState( message.assocEntityKey, *sequenceState );
                return;
		    }

            TA_Base_Bus_GZL9::MovementControlCorbaDef::CameraPreset* preset = 0;
            if ( 0 != ( message.stateUpdateInfo >>= preset ) )		    // Is this a CameraPreset name change update message?
		    {
			    updatePresetName( message.assocEntityKey, *preset );
                return;
		    }
            
            TA_Base_Bus_GZL9::CameraCorbaDef::CameraState* cameraState = 0;
            if ( 0 != ( message.stateUpdateInfo >>= cameraState ) )  // Is this a Camera state update message?
		    {
			    updateCameraState( message.assocEntityKey, *cameraState );
                return;
		    }

            /*
			TA_Base_Bus_GZL9::BVSStageCorbaDef::BVSStageState* bvsStageState = 0;
            if ( 0 != ( message.stateUpdateInfo >>= bvsStageState ) )    // Is this a BVS state update message?
		    {
			    updateBVSStageState( message.assocEntityKey, *bvsStageState );
                return;
		    }*/

           TA_Base_Bus_GZL9::VideoInputCorbaDef::VideoInputState* videoInputState = 0;
            if ( 0 != ( message.stateUpdateInfo >>= videoInputState ) )    // Is this a VideoInput state update message?
		    {
			    updateVideoInputState( message.assocEntityKey, *videoInputState );
                return;
		    }
			TA_Base_Bus_GZL9::SwitchManagerCorbaDef::AlarmStack* alarmStack = 0;
			if ( 0 != ( message.stateUpdateInfo >>= alarmStack ) )    // Is this an AlarmStack state update message?
		    {
			    updateAlarmStack( *alarmStack );
                return;
		    }
			

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "Unrecognised state update message received." );
        }
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Update Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // updatePresetName
    //
    void StateUpdateSubscriber::updatePresetName( unsigned long cameraKey, 
        const TA_Base_Bus_GZL9::MovementControlCorbaDef::CameraPreset& preset )
    {
        try
        {
            Camera& camera = SwitchManager::getInstance().getCameraInternal( cameraKey );
            if( !camera.isMovementControllable() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Camera %lu is not movement controllable.", cameraKey );
                LOGMORE( SourceInfo, PRESET_NAME_UPDATE_FAILED_LOG_MSG.c_str(), preset.index, cameraKey, preset.name.in() );
                return;
            }
            MovementControl& control = camera.getMovementControlInternal();
            control.updatePresetName( preset.index, std::string( preset.name ) );
        }
        catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
            LOGMORE( SourceInfo, PRESET_NAME_UPDATE_FAILED_LOG_MSG.c_str(), preset.index, cameraKey, preset.name.in() );
            return;
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOGMORE( SourceInfo, PRESET_NAME_UPDATE_FAILED_LOG_MSG.c_str(), preset.index, cameraKey, preset.name.in() );
            return;
        }
    }

    
    //
    // updateCameraState
    //
    void StateUpdateSubscriber::updateCameraState( unsigned long cameraKey, 
        const TA_Base_Bus_GZL9::CameraCorbaDef::CameraState& state )
    {
        try
        {
            Camera& camera = SwitchManager::getInstance().getCameraInternal( cameraKey );
            camera.updateState( state );
        }
        catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
	        LOGMORE( SourceInfo, CAMERA_STATE_UPDATE_FAILED_LOG_MSG.c_str(), cameraKey );
            return;
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
	        LOGMORE( SourceInfo, CAMERA_STATE_UPDATE_FAILED_LOG_MSG.c_str(), cameraKey );
            return;
        }
    }


    //
    // updateCameraState
    //
	/*
    void StateUpdateSubscriber::updateBVSStageState( unsigned long bvsKey, 
        const TA_Base_Bus_GZL9::BVSStageCorbaDef::BVSStageState& state )
    {
        try
        {
            BVSStage& stage = SwitchManager::getInstance().getBVSStageInternal( bvsKey );
            stage.updateBVSStageState( state );
        }
        catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
	        LOGMORE( SourceInfo, BVS_STATE_UPDATE_FAILED_LOG_MSG.c_str(), bvsKey );
            return;
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
	        LOGMORE( SourceInfo, BVS_STATE_UPDATE_FAILED_LOG_MSG.c_str(), bvsKey );
            return;
        }
    }
*/

    //
    // updateVideoInputState
    //
    void StateUpdateSubscriber::updateVideoInputState( unsigned long videoInputKey,
        const TA_Base_Bus_GZL9::VideoInputCorbaDef::VideoInputState& state )
    {
        try
        {
            VideoInput& videoInput = SwitchManager::getInstance().getVideoInputInternal( videoInputKey );
            videoInput.updateState( state );
        }
        catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
	        LOGMORE( SourceInfo, VIDEO_INPUT_STATE_UPDATE_FAILED_LOG_MSG.c_str(), videoInputKey );
            return;
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
	        LOGMORE( SourceInfo, VIDEO_INPUT_STATE_UPDATE_FAILED_LOG_MSG.c_str(), videoInputKey );
            return;
        }
    }


    //
    // updateVideoOutputState
    //
    void StateUpdateSubscriber::updateVideoOutputState( unsigned long videoOutputKey,
        const TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState& state )
    {
        try
        {
            // This VideoOutput* does not have to be cleaned up.

            VideoOutput& videoOutput = SwitchManager::getInstance().getVideoOutputInternal( videoOutputKey );
            videoOutput.updateState( state );
        }
        catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
	        LOGMORE( SourceInfo, VIDEO_OUTPUT_STATE_UPDATE_FAILED_LOG_MSG.c_str(), videoOutputKey );
            return;
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
	        LOGMORE( SourceInfo, VIDEO_OUTPUT_STATE_UPDATE_FAILED_LOG_MSG.c_str(), videoOutputKey );
            return;
        }

    }


    //
    // updateQuadState
    //
    void StateUpdateSubscriber::updateQuadState( unsigned long quadKey,
        const TA_Base_Bus_GZL9::QuadCorbaDef::QuadState& state )
    {
        try
        {
            Quad& quad = SwitchManager::getInstance().getQuadInternal( quadKey );
            quad.updateState( state );
        }
        catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
	        LOGMORE( SourceInfo, VIDEO_OUTPUT_STATE_UPDATE_FAILED_LOG_MSG.c_str(), quadKey );
            return;
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
	        LOGMORE( SourceInfo, VIDEO_OUTPUT_STATE_UPDATE_FAILED_LOG_MSG.c_str(), quadKey );
            return;
        }

    }


    //
    // updateSequenceState
    //
    void StateUpdateSubscriber::updateSequenceState( unsigned long sequenceKey, 
                                                     const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceState& state )
    {
        try
        {
            Sequence& sequence = SwitchManager::getInstance().getSequenceInternal( sequenceKey );
            sequence.updateState( state );
        }
        catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
            LOG_GENERIC( SourceInfo,
                         TA_Base_Core::DebugUtil::DebugError,
                         SEQUENCE_STATE_UPDATE_FAILED_LOG_MSG.c_str(),
                         sequenceKey );
            return;
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_GENERIC( SourceInfo,
                         TA_Base_Core::DebugUtil::DebugError,
                         SEQUENCE_STATE_UPDATE_FAILED_LOG_MSG.c_str(),
                         sequenceKey );
            return;
        }
    }


    //
    // updateSequenceConfig
    //
    void StateUpdateSubscriber::updateSequenceConfig( unsigned long sequenceKey, 
                                                     const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig& sequenceConfig )
    {
        try
        {
            Sequence& sequence = SwitchManager::getInstance().getSequenceInternal( sequenceKey );
            sequence.updateSequenceConfig( sequenceConfig );
        }
        catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
            LOG_GENERIC( SourceInfo,
                         TA_Base_Core::DebugUtil::DebugError,
                         SEQUENCE_CONFIG_UPDATE_FAILED_LOG_MSG.c_str(),
                         sequenceKey );
            return;
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_GENERIC( SourceInfo,
                         TA_Base_Core::DebugUtil::DebugError,
                         SEQUENCE_CONFIG_UPDATE_FAILED_LOG_MSG.c_str(),
                         sequenceKey );
            return;
        }
    }


    //
    // updateRecordingUnitState
    //
    void StateUpdateSubscriber::updateRecordingUnitState( unsigned long recordingUnitKey,
        const TA_Base_Bus_GZL9::RecordingUnitCorbaDef::RecordingUnitState& state )
    {
        try
        {
            RecordingUnit& recordingUnit = SwitchManager::getInstance().getRecordingUnitInternal( recordingUnitKey );
            recordingUnit.updateState( state );
        }
        catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
	        LOGMORE( SourceInfo, RECORDING_UNIT_STATE_UPDATE_FAILED_LOG_MSG.c_str(), recordingUnitKey );
            return;
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
	        LOGMORE( SourceInfo, RECORDING_UNIT_STATE_UPDATE_FAILED_LOG_MSG.c_str(), recordingUnitKey );
            return;
        }

    }

	//
	// updateAlarmStack
	//
	void StateUpdateSubscriber::updateAlarmStack( TA_Base_Bus_GZL9::SwitchManagerCorbaDef::AlarmStack& stack)
	{
		IPDAlarmInterface* alarmInterface = SwitchManager::getInstance().getAlarmInterface();
		if(alarmInterface != NULL)
		{
			alarmInterface->updateAlarmStack(stack);
		}
	}

} // TA_IRS_App

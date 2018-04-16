/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Katherine Thomson
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
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
#include "app/cctv/video_switch_agent/src/Sequence.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/src/VideoInput.h"
#include "app/cctv/video_switch_agent/src/VideoOutput.h"
#include "bus/cctv/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"

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
    const std::string BVS_STATE_UPDATE_FAILED_LOG_MSG =
        "Failed to update the state of BVSStage %ul.";
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
    void StateUpdateSubscriber::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
    {
        // Extract the stateUpdateInfo from the State Update Message
        // Check the mode that the agent is in.
        // In Control mode, we should only receive VideoSwitchAgentUpdateRequest messages.
        // In Monitor mode, we should only receive VideoSwitchAgentUpdate messages.
        if(AgentModeMonitor::getInstance().isInControlMode())
        {
            if(0 == TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdate.getTypeKey().compare(
                        message.messageTypeKey))
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Received VideoSwitchAgentUpdate message while in Control mode.");
                return;
            }
            else if(0 != TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdateRequest.getTypeKey().compare(
                        message.messageTypeKey))
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Received an unknown message.");
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
            if(0 == TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdateRequest.getTypeKey().compare(
                        message.messageTypeKey))
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Received VideoSwitchAgentUpdateRequest message while in Monitor mode.");
                return;
            }
            else if(0 != TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdate.getTypeKey().compare(
                        message.messageTypeKey))
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Received an unknown message.");
                return;
            }
            // Now try and extract it to a recognised type.
            TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState* videoOutputState = 0;
            if(0 != (message.stateUpdateInfo >>= videoOutputState))        // Is this a VideoOutput state update message?
            {
                updateVideoOutputState(message.assocEntityKey, *videoOutputState);
                return;
            }
            TA_Base_Bus::SequenceCorbaDef::SequenceConfig* sequenceConfig = 0;
            if(0 != (message.stateUpdateInfo >>= sequenceConfig))        // Is this a Sequence SequenceConfig update message?
            {
                updateSequenceConfig(message.assocEntityKey, *sequenceConfig);
                return;
            }
            TA_Base_Bus::SequenceCorbaDef::SequenceState* sequenceState = 0;
            if(0 != (message.stateUpdateInfo >>= sequenceState))        // Is this a VideoOutput state update message?
            {
                updateSequenceState(message.assocEntityKey, *sequenceState);
                return;
            }
            TA_Base_Bus::CameraCorbaDef::CameraState* cameraState = 0;
            if(0 != (message.stateUpdateInfo >>= cameraState))       // Is this a Camera state update message?
            {
                updateCameraState(message.assocEntityKey, *cameraState);
                return;
            }
            TA_Base_Bus::VideoInputCorbaDef::VideoInputState* videoInputState = 0;
            if(0 != (message.stateUpdateInfo >>= videoInputState))         // Is this a VideoInput state update message?
            {
                updateVideoInputState(message.assocEntityKey, *videoInputState);
                return;
            }
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unrecognised state update message received.");
        }
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Update Methods
    //
    ///////////////////////////////////////////////////////////////////////
    //
    // updateCameraState
    //
    void StateUpdateSubscriber::updateCameraState(unsigned long cameraKey,
                                                  const TA_Base_Bus::CameraCorbaDef::CameraState& state)
    {
        try
        {
            Camera& camera = SwitchManager::getInstance().getCameraInternal(cameraKey);
            camera.updateState(state);
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            const char* what_the_ = e.what;  // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentException", what_the_);
            LOGMORE(SourceInfo, CAMERA_STATE_UPDATE_FAILED_LOG_MSG.c_str(), cameraKey);
            return;
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            LOGMORE(SourceInfo, CAMERA_STATE_UPDATE_FAILED_LOG_MSG.c_str(), cameraKey);
            return;
        }
    }


    //
    // updateVideoInputState
    //
    void StateUpdateSubscriber::updateVideoInputState(unsigned long videoInputKey,
                                                      const TA_Base_Bus::VideoInputCorbaDef::VideoInputState& state)
    {
        try
        {
            VideoInput& videoInput = SwitchManager::getInstance().getVideoInputInternal(videoInputKey);
            videoInput.updateState(state);
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            const char* what_the_ = e.what;  // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentException", what_the_);
            LOGMORE(SourceInfo, VIDEO_INPUT_STATE_UPDATE_FAILED_LOG_MSG.c_str(), videoInputKey);
            return;
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            LOGMORE(SourceInfo, VIDEO_INPUT_STATE_UPDATE_FAILED_LOG_MSG.c_str(), videoInputKey);
            return;
        }
    }


    //
    // updateVideoOutputState
    //
    void StateUpdateSubscriber::updateVideoOutputState(unsigned long videoOutputKey,
                                                       const TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState& state)
    {
        try
        {
            // This VideoOutput* does not have to be cleaned up.
            VideoOutput& videoOutput = SwitchManager::getInstance().getVideoOutputInternal(videoOutputKey);
            videoOutput.updateState(state);
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            const char* what_the_ = e.what;  // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentException", what_the_);
            LOGMORE(SourceInfo, VIDEO_OUTPUT_STATE_UPDATE_FAILED_LOG_MSG.c_str(), videoOutputKey);
            return;
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            LOGMORE(SourceInfo, VIDEO_OUTPUT_STATE_UPDATE_FAILED_LOG_MSG.c_str(), videoOutputKey);
            return;
        }
    }


    //
    // updateSequenceState
    //
    void StateUpdateSubscriber::updateSequenceState(unsigned long sequenceKey,
                                                    const TA_Base_Bus::SequenceCorbaDef::SequenceState& state)
    {
        try
        {
            Sequence& sequence = SwitchManager::getInstance().getSequenceInternal(sequenceKey);
            sequence.updateState(state);
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            const char* what_the_ = e.what;  // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentException", what_the_);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, SEQUENCE_STATE_UPDATE_FAILED_LOG_MSG.c_str(), sequenceKey);
            return;
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, SEQUENCE_STATE_UPDATE_FAILED_LOG_MSG.c_str(), sequenceKey);
            return;
        }
    }


    //
    // updateSequenceConfig
    //
    void StateUpdateSubscriber::updateSequenceConfig(unsigned long sequenceKey,
                                                     const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& sequenceConfig)
    {
        try
        {
            Sequence& sequence = SwitchManager::getInstance().getSequenceInternal(sequenceKey);
            sequence.updateSequenceConfig(sequenceConfig);
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            const char* what_the_ = e.what;  // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentException", what_the_);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, SEQUENCE_CONFIG_UPDATE_FAILED_LOG_MSG.c_str(), sequenceKey);
            return;
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, SEQUENCE_CONFIG_UPDATE_FAILED_LOG_MSG.c_str(), sequenceKey);
            return;
        }
    }
} // TA_IRS_App

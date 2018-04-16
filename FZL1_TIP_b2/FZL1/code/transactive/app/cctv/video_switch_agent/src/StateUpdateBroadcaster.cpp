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
  * Broadcasts messages to the GUIs and any other Video Switch Agents
  * notifying them of state updates.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "app/cctv/video_switch_agent/src/Camera.h"
#include "app/cctv/video_switch_agent/src/Sequence.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/src/VideoInput.h"
#include "app/cctv/video_switch_agent/src/VideoOutput.h"
#include "app/cctv/video_switch_agent/src/VideoMonitor.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/types/VisualComms_MessageTypes.h"
#include "core/message/types/VisualStateUpdate_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include <boost/foreach.hpp>


namespace TA_IRS_App
{
    //
    // getInstance
    //
    StateUpdateBroadcaster& StateUpdateBroadcaster::getInstance()
    {
		return *(ACE_Singleton<StateUpdateBroadcaster, ACE_Recursive_Thread_Mutex>::instance());
    }


    //
    // ~StateUpdateBroadcaster
    //
    StateUpdateBroadcaster::~StateUpdateBroadcaster()
    {
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Request Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // requestAgentState
    //
    void StateUpdateBroadcaster::requestAgentState()
    {
        // Request the current state from the Video Switch Agent in Control mode.
        CORBA::Any emptyData;
        m_stateSender->sendStateUpdateMessage(
            TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdateRequest,     // Message Type
            0,                                                                  // Unused
            TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME),           // Agent Name
            emptyData);                                                         // Unused
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Notification Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // notifyAgentState
    //
    void StateUpdateBroadcaster::notifyAgentState()
    {
        unsigned long key = 0;
		// Cameras
		std::vector<Camera*> cameras = SwitchManager::getInstance().getInternalCameras();
		BOOST_FOREACH(Camera* i, cameras)
        {
            key = i->getKey();
            notifyCameraStateUpdate(key, true);
            notifyVideoInputStateUpdate(key, true);
        }
		// Monitors
		std::vector<VideoMonitor*> monitors = SwitchManager::getInstance().getInternalMonitors();
		BOOST_FOREACH(VideoMonitor* i, monitors)
        {
            key = i->getKey();
            notifyVideoOutputStateUpdate(key, true);
        }
		// Sequences
		std::vector<Sequence*> sequences = SwitchManager::getInstance().getInternalSequences();
		BOOST_FOREACH(Sequence* i, sequences)
        {
            key = i->getKey();
            notifySequenceStateUpdate(key, true);
        }
    }


    //
    // notifyCameraStateUpdate
    //
    void StateUpdateBroadcaster::notifyCameraStateUpdate(unsigned long cameraKey, bool sendStateUpdateMessageOnly)
    {
        // Get the state of the Camera and the subsystem and location keys.
        CORBA::Any stateData;
        unsigned long subsystemKey = 0;
        unsigned long locationKey = 0;
        try
        {
            Camera& camera = SwitchManager::getInstance().getCameraInternal(cameraKey);
            stateData <<= camera.getState();
            subsystemKey = camera.getSubsystemKey();
            locationKey = camera.getLocationKey();
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            const char* what_the_ = e.what;  // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentException", what_the_);
            std::stringstream details;
            details << "Failed to notify clients of the state update for Camera " << cameraKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the state update for Camera " << cameraKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the state update for Camera " << cameraKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the state update for Camera " << cameraKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", details.str().c_str());
            return;
        }
        // Send a state update message to update the monitoring agent.
        m_stateSender->sendStateUpdateMessage(
            TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdate,    // Message Type
            cameraKey,                                                  // Entity Key
            TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME),   // Agent Name
            stateData);                                                 // Update Info (Message Data)
        if(!sendStateUpdateMessageOnly)
        {
            // Send a comms message to update any clients (e.g. Camera Controllers, Display Managers).
            m_commsSender->sendCommsMessage(
                TA_Base_Core::VisualComms::CameraStateUpdate,                    // Message Type
                cameraKey,                                                  // Entity Key
                stateData,                                                  // Message Data
                subsystemKey,                                               // Subsystem Key
                locationKey);                                               // Location Key
        }
    }


    //
    // notifyVideoInputStateUpdate
    //
    void StateUpdateBroadcaster::notifyVideoInputStateUpdate(unsigned long videoInputKey, bool sendStateUpdateMessageOnly)
    {
        // Get the state of the VideoInput and the subsystem and location keys.
        CORBA::Any stateData;
        unsigned long subsystemKey = 0;
        unsigned long locationKey = 0;
        try
        {
            VideoInput& videoInput = SwitchManager::getInstance().getVideoInputInternal(videoInputKey);
            stateData <<= videoInput.getVideoInputState();
            subsystemKey = videoInput.getSubsystemKey();
            locationKey = videoInput.getLocationKey();
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            const char* what_the_ = e.what;  // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentException", what_the_);
            std::stringstream details;
            details << "Failed to notify clients of the state update for VideoInput " << videoInputKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the state update for VideoInput " << videoInputKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the state update for VideoInput " << videoInputKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the state update for VideoInput " << videoInputKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", details.str().c_str());
            return;
        }
        // Send a state update message to update the monitoring agent.
        m_stateSender->sendStateUpdateMessage(
            TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdate,    // Message Type
            videoInputKey,                                              // Entity Key
            TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME),   // Agent Name
            stateData);                                                 // Update Info (Message Data)
        if(!sendStateUpdateMessageOnly)
        {
            // Send a comms message to update any clients (e.g. Camera Controllers, Display Managers).
            m_commsSender->sendCommsMessage(
                TA_Base_Core::VisualComms::VideoInputStateUpdate,                // Message Type
                videoInputKey,                                              // Entity Key
                stateData,                                                  // Message Data
                subsystemKey,                                               // Subsystem Key
                locationKey);                                               // Location Key
        }
    }


    //
    // notifyVideoOutputStateUpdate
    //
    void StateUpdateBroadcaster::notifyVideoOutputStateUpdate(unsigned long videoOutputKey, bool sendStateUpdateMessageOnly)
    {
        // Get the state of the VideoOutput and the subsystem and location keys.
        CORBA::Any stateData;
        unsigned long subsystemKey = 0;
        unsigned long locationKey = 0;
        try
        {
            VideoOutput& videoOutput = SwitchManager::getInstance().getVideoOutputInternal(videoOutputKey);
            stateData <<= videoOutput.getVideoOutputState();
            subsystemKey = videoOutput.getSubsystemKey();
            locationKey = videoOutput.getLocationKey();
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            const char* what_the_ = e.what;  // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentException", what_the_);
            std::stringstream details;
            details << "Failed to notify clients of the state update for VideoOutput " << videoOutputKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the state update for VideoOutput " << videoOutputKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the state update for VideoOutput " << videoOutputKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the state update for VideoOutput " << videoOutputKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", details.str().c_str());
            return;
        }
        // Send a state update message to update the monitoring agent.
        m_stateSender->sendStateUpdateMessage(
            TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdate,    // Message Type
            videoOutputKey,                                             // Entity Key
            TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME),   // Agent Name
            stateData);                                                 // Update Info (Message Data)
        if(!sendStateUpdateMessageOnly)
        {
            // Send a comms message to update any clients (e.g. Camera Controllers, Display Managers).
            m_commsSender->sendCommsMessage(
                TA_Base_Core::VisualComms::VideoOutputStateUpdate,               // Message Type
                videoOutputKey,                                             // Entity Key
                stateData,                                                  // Message Data
                subsystemKey,                                               // Subsystem Key
                locationKey);                                               // Location Key
        }
    }


    //
    // notifyQuadStateUpdate
    //
    /*void StateUpdateBroadcaster::notifyQuadStateUpdate( unsigned long quadKey, bool sendStateUpdateMessageOnly )
    {
        // Get the state of the Quad and the subsystem and location keys.

        CORBA::Any stateData;
        unsigned long subsystemKey = 0;
        unsigned long locationKey = 0;

        try
        {
            Quad& quad = SwitchManager::getInstance().getQuadInternal( quadKey );
            stateData <<= quad.getQuadState();
            subsystemKey = quad.getSubsystemKey();
            locationKey = quad.getLocationKey();
        }
        catch( const TA_Base_Bus::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
            std::stringstream details;
            details << "Failed to notify clients of the state update for Quad " << quadKey << ".";
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentException", details.str().c_str() );
            return;
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            std::stringstream details;
            details << "Failed to notify clients of the state update for Quad " << quadKey << ".";
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", details.str().c_str() );
            return;
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            std::stringstream details;
            details << "Failed to notify clients of the state update for Quad " << quadKey << ".";
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", details.str().c_str() );
            return;
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            std::stringstream details;
            details << "Failed to notify clients of the state update for Quad " << quadKey << ".";
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", details.str().c_str() );
            return;
        }

        // Send a state update message to update the monitoring agent.

        m_stateSender->sendStateUpdateMessage(
                TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdate,    // Message Type
                quadKey,                                                    // Entity Key
                TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ), // Agent Name
                stateData );                                                // Update Info (Message Data)

        if ( !sendStateUpdateMessageOnly )
        {
            // Send a comms message to update any clients (e.g. Camera Controllers, Display Managers).

            m_commsSender->sendCommsMessage(
                    TA_Base_Core::VisualComms::QuadStateUpdate,                  // Message Type
                    quadKey,                                                // Entity Key
                    stateData,                                              // Message Data
                    subsystemKey,                                           // Subsystem Key
                    locationKey );                                          // Location Key

            // TODO Audit message ?
        }
    }*/


    //
    // notifySequenceStateUpdate
    //
    void StateUpdateBroadcaster::notifySequenceStateUpdate(unsigned long sequenceKey,
                                                           unsigned long videoOutputKey,
                                                           bool sendStateUpdateMessageOnly)
    {
        // Get the state of the Sequence and the subsystem and location keys.
        CORBA::Any stateData;
        unsigned long subsystemKey = 0;
        unsigned long locationKey = 0;
        try
        {
            Sequence& sequence = SwitchManager::getInstance().getSequenceInternal(sequenceKey);
            stateData <<= sequence.getSequenceState(videoOutputKey);
            subsystemKey = sequence.getSubsystemKey();
            locationKey = sequence.getLocationKey();
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            const char* what_the_ = e.what;  // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentException", what_the_);
            std::stringstream details;
            details << "Failed to notify clients of the state update for Sequence " << sequenceKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the state update for Sequence " << sequenceKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the state update for Sequence " << sequenceKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the state update for Sequence " << sequenceKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", details.str().c_str());
            return;
        }
        // Send a state update message to update the monitoring agent.
        m_stateSender->sendStateUpdateMessage(
            TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdate,    // Message Type
            sequenceKey,                                                // Entity Key
            TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME),   // Agent Name
            stateData);                                                 // Update Info (Message Data)
        if(!sendStateUpdateMessageOnly)
        {
            // Send a comms message to update any clients (e.g. Display Managers).
            m_commsSender->sendCommsMessage(
                TA_Base_Core::VisualComms::SequenceStateUpdate,              // Message Type
                sequenceKey,                                            // Entity Key
                stateData,                                              // Message Data
                subsystemKey,                                           // Subsystem Key
                locationKey);                                           // Location Key
        }
    }


    //
    // notifySequenceConfigUpdate
    //
    void StateUpdateBroadcaster::notifySequenceConfigUpdate(unsigned long sequenceKey,
                                                            bool sendStateUpdateMessageOnly)
    {
        // Get the Sequence Config of the Sequence and the subsystem and location keys.
        CORBA::Any stateData;
        unsigned long subsystemKey = 0;
        unsigned long locationKey = 0;
        try
        {
            Sequence& sequence = SwitchManager::getInstance().getSequenceInternal(sequenceKey);
            stateData <<= sequence.getSequenceConfig(); // Note "<<=" insertion consumes the pointers memory so no leak.
            subsystemKey = sequence.getSubsystemKey();
            locationKey = sequence.getLocationKey();
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            const char* what_the_ = e.what;  // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentException", what_the_);
            std::stringstream details;
            details << "Failed to notify clients of the sequence config update for Sequence " << sequenceKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the sequence config update for Sequence " << sequenceKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the sequence config update for Sequence " << sequenceKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the sequence config for Sequence " << sequenceKey << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", details.str().c_str());
            return;
        }
        // Send a state update message to update the monitoring agent.
        m_stateSender->sendStateUpdateMessage(
            TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdate,    // Message Type
            sequenceKey,                                                // Entity Key
            TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME),   // Agent Name
            stateData);                                                 // Update Info (Message Data)
        if(!sendStateUpdateMessageOnly)
        {
            // Send a comms message to update any clients (e.g. Display Managers).
            m_commsSender->sendCommsMessage(
                TA_Base_Core::VisualComms::SequenceConfigUpdate,             // Message Type
                sequenceKey,                                            // Entity Key
                stateData,                                              // Message Data
                subsystemKey,                                           // Subsystem Key
                locationKey);                                           // Location Key
        }
    }


    //
    // notifySequenceConfigUpdate
    //
    void StateUpdateBroadcaster::notifySequenceConfigUpdate(Sequence& sequence,
                                                            bool sendStateUpdateMessageOnly)
    {
        CORBA::Any stateData;
        unsigned long subsystemKey = 0;
        unsigned long locationKey = 0;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Testingnotify");
        try
        {
            TA_Base_Bus::SequenceCorbaDef::SequenceConfig* seqConf = sequence.getSequenceConfig();
            seqConf->description = CORBA::string_dup(sequence.getSequenceDescription());
            stateData <<= seqConf; // Note "<<=" insertion consumes the pointers memory so no leak.
            subsystemKey = sequence.getSubsystemKey();
            locationKey = sequence.getLocationKey();
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            const char* what_the_ = e.what;  // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentException", what_the_);
            std::stringstream details;
            details << "Failed to notify clients of the sequence config update for Sequence " << sequence.getKey() << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the sequence config update for Sequence " << sequence.getKey() << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the sequence config update for Sequence " << sequence.getKey() << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", details.str().c_str());
            return;
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            std::stringstream details;
            details << "Failed to notify clients of the sequence config for Sequence " << sequence.getKey() << ".";
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", details.str().c_str());
            return;
        }
        // Send a state update message to update the monitoring agent.
        m_stateSender->sendStateUpdateMessage(
            TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdate,    // Message Type
            sequence.getKey(),                                                // Entity Key
            TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME),   // Agent Name
            stateData);                                                 // Update Info (Message Data)
        if(!sendStateUpdateMessageOnly)
        {
            // Send a comms message to update any clients (e.g. Display Managers).
            m_commsSender->sendCommsMessage(
                TA_Base_Core::VisualComms::SequenceConfigUpdate,             // Message Type
                sequence.getKey(),                                            // Entity Key
                stateData,                                              // Message Data
                subsystemKey,                                           // Subsystem Key
                locationKey);                                           // Location Key
        }
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // Private Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // StateUpdateBroadcaster
    //
    StateUpdateBroadcaster::StateUpdateBroadcaster()
        : m_commsSender(std::auto_ptr< TA_Base_Core::CommsMessageSender >(
                            TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender(
                                TA_Base_Core::VisualComms::Context))),
        m_stateSender(std::auto_ptr< TA_Base_Core::StateUpdateMessageSender >(
                          TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender(
                              TA_Base_Core::VisualStateUpdate::Context)))
    {
        // Note: Publish by context as the MessageSender will be sending multiple MessageTypes.
    }

} // TA_IRS_App


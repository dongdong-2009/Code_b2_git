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
  * Handles all the common functions of video outputs.
  * Implements the IVideoOutput interface.
  * Uses the IPDVideoOutput interface to perform the protocol
  * dependent sections of the control functions.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/src/AllSupportedFeatures.h"
#include "app/cctv/video_switch_agent/src/VideoOutput.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/src/VideoInput.h"
#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"
#include "app/cctv/video_switch_agent/src/VisualAuditMessageSender.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include <sstream>

namespace TA_IRS_App
{

    //
    // VideoOutput
    //
    VideoOutput::VideoOutput(TA_Base_Core::IEntityDataPtr entityData)
        :   VideoSwitchResource(entityData),
            m_videoOutputData(boost::dynamic_pointer_cast<TA_Base_Core::VideoOutput>(entityData)),
            m_pdVideoOutput(0)
    {
        // Store the database data.
        if(0 == m_videoOutputData.get())
        {
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::INVALID_CONFIG, "The provided entity data is not VideoOutput data."));
        }
        // Create an object to handle the protocol dependent work for the VideoOutput.
        m_pdVideoOutput = std::auto_ptr< IPDVideoOutput >(PDInterfaceFactory::createVideoOutput(m_videoOutputData));
        if(0 == m_pdVideoOutput.get())
        {
            // m_videoOutputData is cleaned up by GenericAgent, so we don't have to clean it up here.
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDVideoOutput."));
        }
    }


    //
    // ~VideoOutput
    //
    VideoOutput::~VideoOutput()
    {
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // State Update Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // updateState
    //
    void VideoOutput::updateState(const TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState& state)
    {
        FUNCTION_ENTRY("updateState");
        TA_ASSERT(0 != m_pdVideoOutput.get(), "The object handling the protocol dependent video output tasks is NULL");
        m_pdVideoOutput->updateState(state);
        FUNCTION_EXIT;
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // VideoOutputCorbaDef Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getVideoOutputState
    //
    TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState* VideoOutput::getVideoOutputState()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        try
        {
            return m_pdVideoOutput->getVideoOutputState();
        }
        catch(TA_Base_Core::VideoSwitchAgentInternalException& vsiex)
        {
            throw TA_Base_Bus::VideoSwitchAgentException(vsiex.what());
        }
    }


    //
    // getVideoOutputGroupName
    //
    char* VideoOutput::getVideoOutputGroupName()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        TA_ASSERT(0 != m_videoOutputData.get(), "The video output database object has not been set.");
        try
        {
            return CORBA::string_dup(m_videoOutputData->getVideoOutputGroupName().c_str());
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus::VideoSwitchAgentException(e.what());
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw TA_Base_Bus::VideoSwitchAgentException(e.what());
        }
    }


    //
    // getCurrentVideoInputKey
    //
    unsigned long VideoOutput::getCurrentVideoInputKey()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        TA_ASSERT(0 != m_pdVideoOutput.get(), "The object handling the protocol dependent video output tasks is NULL");
        try
        {
            return m_pdVideoOutput->getCurrentVideoInputKey();
        }
        catch(TA_Base_Core::VideoSwitchAgentInternalException& vsiex)
        {
            throw TA_Base_Bus::VideoSwitchAgentException(vsiex.what());
        }
    }

    //
    // requestSwitchOfInput
    //
    void VideoOutput::requestSwitchOfInput(unsigned long videoInputEntityKey,
                                           const char* sessionId, bool isDemand)
    {
        FUNCTION_ENTRY("requestSwitchOfInput");
        TA_ASSERT(0 != m_pdVideoOutput.get(), "The object handling the protocol dependent video output tasks is NULL");
        AgentModeMonitor::getInstance().verifyInControlMode(TA_Visual::REQUEST_SWITCH_OF_INPUT);
        if(sessionId != NULL) //NULL means called by self
        {
            m_userPrivilegeVerifier.verifyRights(sessionId, m_videoOutputData, TA_Base_Bus::aca_CONTROL_MONITOR_VIDEO_ASSIGNMENT, "Switch Monitor Video Input");
        }
        m_pdVideoOutput->requestSwitchOfInput(videoInputEntityKey, isDemand);
        FUNCTION_EXIT;
    }

    //
    // requestSwitchOfInput
    //
    void VideoOutput::assignQuad(unsigned long cameraW, unsigned long cameraX, unsigned long cameraY, unsigned long cameraZ,
                                 const char* sessionId, bool isDemand)
    {
        FUNCTION_ENTRY("assignQuad");
        TA_ASSERT(0 != m_pdVideoOutput.get(), "The object handling the protocol dependent video output tasks is NULL");
        AgentModeMonitor::getInstance().verifyInControlMode(TA_Visual::REQUEST_SWITCH_OF_INPUT);
        m_userPrivilegeVerifier.verifyRights(sessionId, m_videoOutputData, TA_Base_Bus::aca_CONTROL_MONITOR_VIDEO_ASSIGNMENT, "Switch Monitor Video Input");
        m_pdVideoOutput->assignQuad(cameraW, cameraX, cameraY, cameraZ, isDemand);
        FUNCTION_EXIT;
    }

    bool VideoOutput::mappingTrainBorneMonitor(const char* sessionId, bool isDemand)
    {
        return true;
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // IEntity Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // isValid
    //
    bool VideoOutput::isValid()
    {
        return true;
    }

    void VideoOutput::cycleToNextVideoInput(const char* sessionId)
    {
    }
    void VideoOutput::cycleToPreviousVideoInput(const char* sessionId)
    {
    }
    void VideoOutput::pause(const char* sessionId)
    {
    }
    void VideoOutput::play(const char* sessionId)
    {
    }
    void VideoOutput::stop(const char* sessionId)
    {
    }

    void VideoOutput::requestRecordingPlayBack(::CORBA::ULong cameraKey,
                                               const ::TA_Base_Bus::VideoOutputCorbaDef::Time& startTime,
                                               const ::TA_Base_Bus::VideoOutputCorbaDef::Time& endTime,
                                               const char* sessionId)
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        TA_ASSERT(0 != m_pdVideoOutput.get(), "The object handling the protocol dependent video output tasks is NULL");
		m_pdVideoOutput->requestRecordingPlayBack(cameraKey, startTime, endTime);
    }

    void VideoOutput::playBackCtrl(TA_Base_Bus::VideoOutputCorbaDef::PlayBackCtrlCmd cmd, const char* sessionId)
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        TA_ASSERT(0 != m_pdVideoOutput.get(), "The object handling the protocol dependent video output tasks is NULL");
		m_pdVideoOutput->playBackCtrl(cmd);
    }
    //
    // update
    //
    void VideoOutput::update(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        TA_ASSERT(0 != m_videoOutputData.get(), "The database object for the VideoOutput has not been initialised.");
        m_videoOutputData->invalidate();
        // FUTURE TODO When adding online updates - may need to notify PD object.
        // FUTURE TODO If you update your corba name you have to reregister and update m_myCorbaName.
    }
} // TA_IRS_App


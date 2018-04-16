/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Rob Young
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
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
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SwitchInputToOutputCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SwitchQuadInputsCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/GlobalConstants.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/CommandCreateHelper.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "bus/cctv/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputCorbaDef.h"
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
    COEVideoOutput::COEVideoOutput(TA_Base_Core::VideoOutputPtr videoOutputData)
        : m_videoOutputData(videoOutputData),
          m_currentVideoInputKey(0)
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


    ///////////////////////////////////////////////////////////////////////
    //
    // State Update Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // notifyInputSwitchedToOutput
    //
    void COEVideoOutput::notifyInputSwitchedToOutput(unsigned long videoInputKey)
    {
        if(m_currentVideoInputKey != videoInputKey)
        {
            m_currentVideoInputKey = videoInputKey;
            try
            {
                StateUpdateBroadcaster::getInstance().notifyVideoOutputStateUpdate(m_videoOutputData->getKey());
            }
            catch(const TA_Base_Core::DatabaseException& e)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
                LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException",
                                      "Failed to notify the agent's clients that a switch of input has occurred.");
            }
            catch(const TA_Base_Core::DataException& e)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
                LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException",
                                      "Failed to notify the agent's clients that a switch of input has occurred.");
            }
        }
    }

    //
    // updateState
    //
    void COEVideoOutput::updateState(const TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState& state)
    {
        m_currentVideoInputKey = state.currentVideoInputKey;
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
        state->currentVideoInputKey = m_currentVideoInputKey;
        return state;
    }

    //
    // requestSwitchOfInput
    //
    void COEVideoOutput::requestSwitchOfInput(unsigned long videoInputEntityKey,
                                              bool isDemand)
    {
        try
        {
            COECommunicationsHandler::RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr(TA_COE::createSwitchInputToOutputCommand(videoInputEntityKey, *this));
            COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand(command, true);
            command->waitForCompletion();
            if(IAgentRequestCommand::SUCCEEDED != command->getCompletionState())
            {
                if(IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState())
                {
                    throw EXCEPTION_CONNECTION_WITH_VSS;
                }
                throw EXCEPTION_PERFORMING_REQUEST;
            }
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Failed to send Switch command.");
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw EXCEPTION_PERFORMING_REQUEST;
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Failed to send Switch command.");
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw EXCEPTION_PERFORMING_REQUEST;
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Switch command.");
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw EXCEPTION_PERFORMING_REQUEST;
        }
    }

    //
    // requestSwitchOfInput
    //
    void COEVideoOutput::assignQuad(unsigned long cameraW,
                                    unsigned long cameraX,
                                    unsigned long cameraY,
                                    unsigned long cameraZ,
                                    bool isDemand)
    {
        try
        {
            COECommunicationsHandler::RequestCommandSharedPtr command;
            try
            {
                command = COECommunicationsHandler::RequestCommandSharedPtr(new SwitchQuadInputsCommand(cameraW, cameraX, cameraY, cameraZ, *this));
            }
            catch(...)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create SwitchInputToOutputCommand object");
                throw EXCEPTION_PERFORMING_REQUEST;
            }
            COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand(command, true);
            command->waitForCompletion();
            if(IAgentRequestCommand::SUCCEEDED != command->getCompletionState())
            {
                if(IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState())
                {
                    throw EXCEPTION_CONNECTION_WITH_VSS;
                }
                throw EXCEPTION_PERFORMING_REQUEST;
            }
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Failed to send Switch command.");
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw EXCEPTION_PERFORMING_REQUEST;
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Failed to send Switch command.");
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw EXCEPTION_PERFORMING_REQUEST;
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Switch command.");
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw EXCEPTION_PERFORMING_REQUEST;
        }
    }

    void COEVideoOutput::requestRecordingPlayBack(::CORBA::ULong cameraKey, const ::TA_Base_Bus::VideoOutputCorbaDef::Time& startTime, const ::TA_Base_Bus::VideoOutputCorbaDef::Time& endTime)
    {
        try
        {
            COECommunicationsHandler::RequestCommandSharedPtr command =
                COECommunicationsHandler::RequestCommandSharedPtr(TA_COE::createRecordingPlayBackCommand(cameraKey,
                                                                  TA_COE::convertCorbaTimeToRecordingComdTime(startTime),
                                                                  TA_COE::convertCorbaTimeToRecordingComdTime(endTime),
                                                                  *this));
            COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand(command, true);
            command->waitForCompletion();
            if(IAgentRequestCommand::SUCCEEDED != command->getCompletionState())
            {
                if(IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState())
                {
                    throw EXCEPTION_CONNECTION_WITH_VSS;
                }
                throw EXCEPTION_PERFORMING_REQUEST;
            }
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Failed to send Switch command.");
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw EXCEPTION_PERFORMING_REQUEST;
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Failed to send Switch command.");
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw EXCEPTION_PERFORMING_REQUEST;
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Switch command.");
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw EXCEPTION_PERFORMING_REQUEST;
        }
    }

    void COEVideoOutput::playBackCtrl(TA_Base_Bus::VideoOutputCorbaDef::PlayBackCtrlCmd cmd)
    {
        try
        {
            COECommunicationsHandler::RequestCommandSharedPtr command =
                COECommunicationsHandler::RequestCommandSharedPtr(TA_COE::createPlayBackCtrlCommand(TA_COE::convertCorbaCmdToPlaybackCtrlCmdType(cmd), *this));
            COESwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand(command, true);
            command->waitForCompletion();
            if(IAgentRequestCommand::SUCCEEDED != command->getCompletionState())
            {
                if(IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState())
                {
                    throw EXCEPTION_CONNECTION_WITH_VSS;
                }
                throw EXCEPTION_PERFORMING_REQUEST;
            }
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Failed to send Switch command.");
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw EXCEPTION_PERFORMING_REQUEST;
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Failed to send Switch command.");
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw EXCEPTION_PERFORMING_REQUEST;
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Switch command.");
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException");
            throw EXCEPTION_PERFORMING_REQUEST;
        }
    }

} // TA_IRS_App


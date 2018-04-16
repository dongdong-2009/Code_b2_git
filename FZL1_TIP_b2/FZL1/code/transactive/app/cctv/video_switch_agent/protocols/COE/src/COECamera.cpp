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
  * Handles the protocol dependent sections of the general functions for a Camera
  * using the COE protocol.
  * Inherits from the IPDCamera class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Camera and
  * that the Camera and the protocol support the required features.
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/COECamera.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/CommandCreateHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/GlobalConstants.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    //
    // COECamera
    //
    COECamera::COECamera(TA_Base_Core::CameraPtr cameraData)
        : m_cameraData(cameraData)
    {
        COESwitchManager::getInstance().addCamera(*this);
    }


    COECamera::~COECamera()
    {
    }
    //
    // getKey
    //
    unsigned long COECamera::getKey()
    {
        return m_cameraData->getKey();
    }


    //
    // getParsedAddress
    //
    std::pair< unsigned short, unsigned short > COECamera::getParsedAddress()
    {
		return TA_COE::getParsedAddress(m_cameraData->getAddress());
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // State Update Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // notifyIsInAlarmState
    //
    void COECamera::notifyIsInAlarmState(bool isInAlarmState)
    {
    }


    //
    // updateState
    //
    void COECamera::updateState(const TA_Base_Bus::CameraCorbaDef::CameraState& state)
    {
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IPDCamera Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // isInAlarmState
    //
    bool COECamera::isInAlarmState()
    {
        return false;
    }

    void COECamera::ptzRequest(TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd cmd)
	{
        try
        {
            COECommunicationsHandler::RequestCommandSharedPtr command =
                COECommunicationsHandler::RequestCommandSharedPtr(TA_COE::createPtzCtrlCommand(TA_COE::convertCorbaCmdToPtzCtrlCmdType(cmd), *this));
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
} // namespace TA_IRS_App

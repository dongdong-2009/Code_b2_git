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
  * Provides access to the Camera state, supported features and Camera control.
  * Inherits from the VideoInput class and implements the CameraCorbaDef interface.
  */

#include <sstream>
#include "app/cctv/video_switch_agent/src/Camera.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/naming/src/Naming.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
    const std::string Camera::GET_SUPPORTED_FEATURES            = "GetSupportedFeatures";
    const std::string Camera::GET_STATE                         = "GetState";
    const std::string Camera::IS_MOVEMENT_CONTROLLABLE          = "IsMovementControllable";
    const std::string Camera::GET_MOVEMENT_CONTROL              = "GetMovementControl";
    const std::string Camera::IS_FOCUS_IRIS_CONTROLLABLE        = "IsFocusIrisControllable";
    const std::string Camera::GET_FOCUS_IRIS_CONTROL            = "GetFocusIrisControl";
    const std::string Camera::IS_WASH_WIPE_CONTROLLABLE         = "IsWashWipeControllable";
    const std::string Camera::GET_WASH_WIPE_CONTROL             = "GetWashWipeControl";
    const std::string Camera::GET_MOVEMENT_CONTROL_INTERNAL     = "GetMovementControlInternal";
    const std::string Camera::GET_FOCUS_IRIS_CONTROL_INTERNAL   = "GetFocusIrisControlInternal";
    const std::string Camera::GET_WASH_WIPE_CONTROL_INTERNAL    = "GetWashWipeControlInternal";

    //
    // Camera
    //
    Camera::Camera(TA_Base_Core::IEntityDataPtr entityData)
        : VideoInput(entityData),
          m_cameraData(boost::dynamic_pointer_cast<TA_Base_Core::Camera>(entityData)),
          m_pdCamera(0),
          m_entityKey(entityData->getKey())
    {
		TA_ASSERT(m_cameraData.get() != NULL, "Camera data config error");
        // Create an object to handle the protocol dependent work for the Camera.
        // It's okay for m_pdCamera to be 0 (we may not be interested in the alarm state of a Camera).
        m_pdCamera = std::auto_ptr< IPDCamera >(PDInterfaceFactory::createCamera(m_cameraData));
        activateServantAtInit();
    }


    //
    // ~Camera
    //
    Camera::~Camera()
    {
        // If an exception is thrown here, there is nothing we can do,
        // but we should make sure no exceptions are thrown out of this destructor.
        try
        {
            deactivateServantAtClose();
        }
        catch(const TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Reason unknown");
        }
    }


    //
    // getCameraData
    //
    TA_Base_Core::CameraPtr& Camera::getCameraData()
    {
        return m_cameraData;
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // State Update Methods
    //
    ///////////////////////////////////////////////////////////////////////

    //
    // updateState
    //

    void Camera::updateState(const TA_Base_Bus::CameraCorbaDef::CameraState& state)
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "updateState");
        if(0 != m_pdCamera.get())
        {
            m_pdCamera->updateState(state);
        }
        LOG_FUNCTION_EXIT(SourceInfo, "updateState");
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // CameraCorbaDef Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getState
    //
    TA_Base_Bus::CameraCorbaDef::CameraState Camera::getState()
    {
        AgentModeMonitor::getInstance().verifyInControlMode(GET_STATE);
        // There are a number of ways that these can be 0 or false, so
        // set them to their default values here and then set them to their
        // real values if everything goes well.
        TA_Base_Bus::CameraCorbaDef::CameraState state;
        state.isAutoFocusOn = false;
        state.isAutoIrisOn = false;
        state.isIrisPeakWhiteInversionOn = false;
        state.activePresetIndex = 0;
        state.isInAlarmState = false;
        return state;
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IEntity Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // start
    //
    void Camera::start()
    {
        // Create the interface objects that are used to control the Camera.
        VideoInput::start();
    }


    //
    // update
    //
    void Camera::update(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        TA_ASSERT(0 != m_cameraData.get(), "The database object for the Camera has not been initialised.");
        m_cameraData->invalidate();
        VideoInput::update(updateEvent);
        // FUTURE TODO When adding online updates - may need to notify PD object.
        // FUTURE TODO If you update your corba name you have to reregister and update m_myCorbaName.
        // FUTURE TODO If they no longer support one of the controls, it should be removed.
    }


    //
    // remove
    //
    void Camera::remove()
    {
        // Remove the object from the SwitchManager object.
        SwitchManager::getInstance().removeCamera(m_entityKey);
        deactivateCorbaServants();
        stop();
        VideoInput::remove();
    }


    //
    // setToControlMode
    //
    void Camera::activateServantAtInit()
    {
        // The state of this object is requested from the hardware after a
        // hardware connection has been achieved.
        // Activate the Corba servants so they accept incoming requests.
        activateCorbaServants();
    }


    //
    // setToMonitorMode
    //
    void Camera::deactivateServantAtClose()
    {
        // Deactivate and delete the Corba servants so they no longer accept incoming requests.
        deactivateCorbaServants();
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Internal Helper Methods
    //
    ///////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////
    //
    // Private Methods
    //
    ///////////////////////////////////////////////////////////////////////
    //
    // activateCorbaServants
    //
    void Camera::activateCorbaServants()
    {
        //TA_ASSERT( AgentModeMonitor::getInstance().isInControlMode(), "The agent is not in Control mode." );
        // Activate the CORBA servants, so they accept incoming calls.
        try
        {
            activateServantWithName(m_cameraData->getName());
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            // There is nothing we can do to recover.
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "A Camera or its controls cannot be activated.");
        }
        catch(const TA_Base_Core::DataException& e)
        {
            // There is nothing we can do to recover.
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "A Camera or its controls cannot be activated.");
        }
        catch(const TA_Base_Core::TransactiveException& e)
        {
            // There is nothing we can do to recover.
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "TransactiveException", "A Camera or its controls cannot be activated.");
        }
        catch(...)
        {
            // There is nothing we can do to recover.
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "A Camera or its controls cannot be activated.");
        }
    }


    //
    // deactivateCorbaServants
    //
    void Camera::deactivateCorbaServants()
    {
        // Deactivate the CORBA servants, so they no longer accepts incoming calls.
        deactivateServant();    // Don't call deactivateAndDelete() here because Generic Agent deletes this object.
    }

    void Camera::ptzRequest(::TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd cmd, const char* sessionId)
    {
		TA_Base_Bus::GenericAgent::ensureControlMode();
        TA_ASSERT(0 != m_pdCamera.get(), "The object handling the protocol dependent video input tasks is NULL");
        m_userPrivilegeVerifier.verifyRights(sessionId, m_cameraData, TA_Base_Bus::aca_CONTROL_CAMERA, "Control Camera");
		m_pdCamera->ptzRequest(cmd);
    }

	TA_Base_Bus::CameraCorbaDef::SupportedCameraFeatures Camera::getSupportedFeatures()
	{
		TA_Base_Bus::CameraCorbaDef::SupportedCameraFeatures a = {false, false, false, false, false,
			false, false, false, false, false};
		return a;
	}

    ::CORBA::Boolean Camera::isMovementControllable()
	{
		return false;
	}

    TA_Base_Bus::MovementControlCorbaDef_ptr Camera::getMovementControl()
	{
		return NULL;
	}

    ::CORBA::Boolean Camera::isFocusIrisControllable()
	{
		return false;
	}

    TA_Base_Bus::FocusIrisControlCorbaDef_ptr Camera::getFocusIrisControl()
	{
		return false;
	}

    ::CORBA::Boolean Camera::isWashWipeControllable()
	{
		return false;
	}

    TA_Base_Bus::WashWipeControlCorbaDef_ptr Camera::getWashWipeControl()
	{
		return NULL;
	}

    ::CORBA::Boolean Camera::isWiperOn(const char* sessionId)
	{
		return false;
	}

    ::CORBA::Boolean Camera::isWasherOn(const char* sessionId)
	{
		return false;
	}
} // namespace TA_IRS_App


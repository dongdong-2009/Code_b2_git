/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/src/Camera.cpp $
  * @author:   Katherine Thomson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/08/13 14:55:34 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Provides access to the Camera state, supported features and Camera control.
  * Inherits from the VideoInput class and implements the CameraCorbaDef interface.
  */

#include <sstream>

#include "app/cctv/video_switch_agent/src/Camera.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/AllSupportedFeatures.h"
#include "app/cctv/video_switch_agent/src/SuperimposedTextControl.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"

#include "bus/generic_agent/src/GenericAgent.h" //limin

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
    Camera::Camera( TA_Base_Core::IEntityDataPtr entityData )
        : VideoInput( entityData ),
        m_cameraData( boost::dynamic_pointer_cast<TA_Base_Core::Camera>(entityData) ),
          m_pdCamera( 0 ),
          m_movementControl( 0 ),
          m_focusIrisControl( 0 ),
          m_washWipeControl( 0 ),
          m_entityKey( entityData->getKey() )
    {
		if ( 0 == m_cameraData.get() )
		{
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::INVALID_CONFIG,
                "The provided entity data could not be converted to Camera data." ) );
		}

        // Create an object to handle the protocol dependent work for the Camera.
        // It's okay for m_pdCamera to be 0 (we may not be interested in the alarm state of a Camera).

        m_pdCamera = std::auto_ptr< IPDCamera >( getPDInterfaceFactory().createCamera( m_cameraData ) );

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
            //stop();
			deactivateServantAtClose();
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        }
        catch( ... )
        {
             LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Reason unknown" );           
        }
    }


    //
    // getCameraData
    //
    TA_Base_Core::CameraPtr& Camera::getCameraData()
    {
        TA_ASSERT( 0 != m_cameraData.get(), "The database object for this Camera has not been initialised." );
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

    void Camera::updateState( const TA_Base_Bus_GZL9::CameraCorbaDef::CameraState& state )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "updateState" );

        if ( 0 != m_pdCamera.get() )
        {
            m_pdCamera->updateState( state );
        }

		/*
        if ( 0 != m_focusIrisControl.get() )
        {
            m_focusIrisControl->updateState( state );
        }

        if ( 0 != m_movementControl.get() )
        {
            m_movementControl->updateState( state );
        }
		*/

        LOG_FUNCTION_EXIT( SourceInfo, "updateState" );
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // CameraCorbaDef Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // getSupportedFeatures
    //
    TA_Base_Bus_GZL9::CameraCorbaDef::SupportedCameraFeatures Camera::getSupportedFeatures()
    {
        AgentModeMonitor::getInstance().verifyInControlMode( GET_SUPPORTED_FEATURES );
        TA_Base_Bus_GZL9::CameraCorbaDef::SupportedCameraFeatures features;

        // Get the movement features.

        if ( 0 != m_movementControl.get() )
        {
            features.isMovementSupported = 
                m_movementControl->isFeatureSupported( TA_Visual::Pan ) ||
                m_movementControl->isFeatureSupported( TA_Visual::Tilt ) ||
                m_movementControl->isFeatureSupported( TA_Visual::Zoom );
            features.isProportionalMovementSupported = 
                m_movementControl->isFeatureSupported( TA_Visual::ProportionalMovement );
	        features.arePresetsSupported =
                m_movementControl->isFeatureSupported( TA_Visual::Presets );
        }
        else
        {
            features.isMovementSupported = false;
            features.isProportionalMovementSupported = false;
            features.arePresetsSupported = false;
        }

        // Get the focus/iris features.

        if ( 0 != m_focusIrisControl.get() )
        {
	        features.isAutoFocusSupported = 
                m_focusIrisControl->isFeatureSupported( TA_Visual::AutoFocus );
	        features.isManualFocusSupported =
                m_focusIrisControl->isFeatureSupported( TA_Visual::ManualFocus );
	        features.isAutoIrisSupported =
                m_focusIrisControl->isFeatureSupported( TA_Visual::AutoIris );
	        features.isManualIrisSupported =
                m_focusIrisControl->isFeatureSupported( TA_Visual::ManualIris );
	        features.isIrisPeakWhiteInversionSupported =
                m_focusIrisControl->isFeatureSupported( TA_Visual::IrisPeakWhiteInversion );
        }
        else
        {
	        features.isAutoFocusSupported = false;
            features.isManualFocusSupported = false;
            features.isAutoIrisSupported = false;
            features.isManualIrisSupported = false;
            features.isIrisPeakWhiteInversionSupported = false;
        }

        // Get the wash/wipe features.

        if ( 0 != m_washWipeControl.get() )
        {
	        features.isWashSupported =
                m_washWipeControl->isFeatureSupported( TA_Visual::Wash );
            features.isWipeSupported =
                m_washWipeControl->isFeatureSupported( TA_Visual::Wipe );        
        }
        else
        {
            features.isWashSupported = false;
            features.isWipeSupported = false;
        }

        return features;
    }


    //
    // getState
    //
    TA_Base_Bus_GZL9::CameraCorbaDef::CameraState Camera::getState()
    {
        AgentModeMonitor::getInstance().verifyInControlMode( GET_STATE );

        // There are a number of ways that these can be 0 or false, so 
        // set them to their default values here and then set them to their
        // real values if everything goes well.

		/*
        TA_Base_Bus_GZL9::CameraCorbaDef::CameraState state;
        state.isAutoFocusOn = false;
        state.isAutoIrisOn = false;
        state.isIrisPeakWhiteInversionOn = false;
        state.activePresetIndex = 0;
        state.isInAlarmState = false;
		*/

        try
        {
			/*
            if ( 0 != m_focusIrisControl.get() )
            {
                if ( m_focusIrisControl->isFeatureSupported( TA_Visual::AutoFocus ) )
                {
	                state.isAutoFocusOn = m_focusIrisControl->isAutoFocusOn();
                }

                if ( m_focusIrisControl->isFeatureSupported( TA_Visual::AutoIris ) )
                {
                    state.isAutoIrisOn = m_focusIrisControl->isAutoIrisOn();
                }
                
                if ( m_focusIrisControl->isFeatureSupported( TA_Visual::IrisPeakWhiteInversion ) )
                {
                    state.isIrisPeakWhiteInversionOn = m_focusIrisControl->isIrisPeakWhiteInversionOn();
                }
            } 
            if ( 0 != m_movementControl.get() )
            {
                if ( m_movementControl->isFeatureSupported( TA_Visual::Presets ) )
                {
                    TA_Base_Bus_GZL9::MovementControlCorbaDef::CameraPreset* preset = m_movementControl->getActivePreset();
                    if ( 0 != preset )
                    {
                        state.activePresetIndex = preset->index;
                    }
                }
            }
			*/
			TA_ASSERT(m_pdCamera.get() != 0, "PD camera should not be null");
			return m_pdCamera->getCameraState();
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


    //
    // isMovementControllable
    //
    bool Camera::isMovementControllable()
    {
        AgentModeMonitor::getInstance().verifyInControlMode( IS_MOVEMENT_CONTROLLABLE );
		return ( 0 != m_movementControl.get() );
    }


    //
    // getMovementControl
    //
    TA_Base_Bus_GZL9::MovementControlCorbaDef_ptr Camera::getMovementControl()
    {
        AgentModeMonitor::getInstance().verifyInControlMode( GET_MOVEMENT_CONTROL );
        if( 0 == m_movementControl.get() )
        {
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( "Camera does not support movement control" );
        }
		return m_movementControl->_this();
    }


    //
    // isFocusIrisControllable
    //
    bool Camera::isFocusIrisControllable()
    {
        AgentModeMonitor::getInstance().verifyInControlMode( IS_FOCUS_IRIS_CONTROLLABLE );
		return ( 0 != m_focusIrisControl.get() );
    }


    //
    // getFocusIrisControl
    //
    TA_Base_Bus_GZL9::FocusIrisControlCorbaDef_ptr Camera::getFocusIrisControl()
    {
        AgentModeMonitor::getInstance().verifyInControlMode( GET_FOCUS_IRIS_CONTROL );
        if( 0 == m_focusIrisControl.get() )
        {
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( "Camera does not support focus or iris control" );
        }
		return m_focusIrisControl->_this();
    }


    //
    // isWashWipeControllable
    //
    bool Camera::isWashWipeControllable()
    {
        AgentModeMonitor::getInstance().verifyInControlMode( IS_WASH_WIPE_CONTROLLABLE );
 		return ( 0 != m_washWipeControl.get() );
    }


    //
    // getWashWipeControl
    //
    TA_Base_Bus_GZL9::WashWipeControlCorbaDef_ptr Camera::getWashWipeControl()
    {
        AgentModeMonitor::getInstance().verifyInControlMode( GET_WASH_WIPE_CONTROL );
        if( 0 == m_washWipeControl.get() )
        {
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( "Camera does not support wash or wipe control" );
        }
		return m_washWipeControl->_this();
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

        createControls();

        VideoInput::start();
    }


    //
    // update
    //
    void Camera::update( const TA_Base_Core::ConfigUpdateDetails& updateEvent )
    {
        TA_ASSERT( 0 != m_cameraData.get(), "The database object for the Camera has not been initialised." );
        m_cameraData->invalidate();

        VideoInput::update( updateEvent );

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

        try
        {
            SwitchManager::getInstance().removeCamera( m_entityKey );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to remove Camera from SwitchManager." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to remove Camera from SwitchManager." );
        }

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

 
    //
    // getMovementControlInternal
    //
    MovementControl& Camera::getMovementControlInternal()
    {
        AgentModeMonitor::getInstance().verifyInControlMode( GET_MOVEMENT_CONTROL_INTERNAL );
        TA_ASSERT( 0 != m_movementControl.get(), "Camera does not support movement control" );
		return *m_movementControl;
    }

 
    //
    // getFocusIrisControlInternal
    //
    FocusIrisControl& Camera::getFocusIrisControlInternal()
    {
        AgentModeMonitor::getInstance().verifyInControlMode( GET_FOCUS_IRIS_CONTROL_INTERNAL );
        TA_ASSERT( 0 != m_focusIrisControl.get(), "Camera does not support focus or iris control" );
		return *m_focusIrisControl;
    }

    
    //
    // getWashWipeControlInternal
    //
    WashWipeControl& Camera::getWashWipeControlInternal()
    {
        AgentModeMonitor::getInstance().verifyInControlMode( GET_WASH_WIPE_CONTROL_INTERNAL );
        TA_ASSERT( 0 != m_washWipeControl.get(), "Camera does not support wash or wipe control" );
		return *m_washWipeControl;
    }


    //
    // isWiperOn
    //
    bool Camera::isWiperOn( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "isWiperOn" );
        
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        LOG_FUNCTION_EXIT( SourceInfo, "isWiperOn" );
        return m_washWipeControl->isWiperOn( sessionId );
    }


    //
    // isWasherOn
    //
    bool Camera::isWasherOn( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "isWasherOn" );

        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        LOG_FUNCTION_EXIT( SourceInfo, "isWasherOn" );
        return m_washWipeControl->isWasherOn( sessionId );
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Private Methods
    //
    ///////////////////////////////////////////////////////////////////////

        
    //
    // createControls
    //
    void Camera::createControls()
    {
        TA_ASSERT( 0 != m_cameraData.get(), "The Camera data has not been initialised." );

        try
        {
            // Create the movement control if required.
        
            // Check that the protocol supports movement control.

            if ( getPDInterfaceFactory().isMovementControlSupported() )
            {
                // Check that this Camera supports movement control.

                if ( m_cameraData->isMovementSupported() || 
                     m_cameraData->isProportionalMovementSupported() ||
                     m_cameraData->arePresetsSupported() )
                {
                    m_movementControl = std::auto_ptr< MovementControl >( new MovementControl( *this ) );
                    if ( 0 == m_movementControl.get() )
                    {
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                            "Failed to create the MovementControl" );
                    }
                }
            }


            // Create the focus and iris control if required.
                
            // Check that the protocol supports focus and iris control.

            if ( getPDInterfaceFactory().isFocusIrisControlSupported() )
            {
                // Check that this Camera supports focus and iris control.

                if ( m_cameraData->isAutoFocusSupported()   || 
                     m_cameraData->isManualFocusSupported() ||
                     m_cameraData->isAutoIrisSupported()    ||
                     m_cameraData->isManualIrisSupported()  ||
                     m_cameraData->isIrisPeakWhiteInversionSupported() )
                {
                    m_focusIrisControl = std::auto_ptr< FocusIrisControl >( new FocusIrisControl( *this ) );
                    if ( 0 == m_focusIrisControl.get() )
                    {
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                            "Failed to create the FocusIrisControl" );
                    }
                }
            }


            // Create the wash and wipe control if required.
       
            // Check that the protocol supports wash and wipe control.

            if ( getPDInterfaceFactory().isWashWipeControlSupported() )
            {
                // Check that this Camera supports focus and iris control.

                if ( m_cameraData->isWashSupported() ||
                     m_cameraData->isWipeSupported() )
                {
                    m_washWipeControl = std::auto_ptr< WashWipeControl >( new WashWipeControl( *this ) );
                    if ( 0 == m_washWipeControl.get() )
                    {
                        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                            "Failed to create the WashWipeControl" );
                    }
                }
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to create the Camera controls." );
            m_washWipeControl = std::auto_ptr< WashWipeControl >( 0 );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to create the Camera controls." );
            m_washWipeControl = std::auto_ptr< WashWipeControl >( 0 );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to create the Camera controls." );
            m_washWipeControl = std::auto_ptr< WashWipeControl >( 0 );
        }
    }


    //
    // activateCorbaServants
    //
    void Camera::activateCorbaServants()
    {
        //TA_ASSERT( AgentModeMonitor::getInstance().isInControlMode(), "The agent is not in Control mode." );

        // Activate the CORBA servants, so they accept incoming calls.

        try
        {
            activateServantWithName( m_cameraData->getName() );
        
			if ( 0 != m_movementControl.get() )
			{
				m_movementControl->activateServant();
			}

			if ( 0 != m_focusIrisControl.get() )
			{
				m_focusIrisControl->activateServant();
			}

            if ( 0 != m_washWipeControl.get() )
			{
				m_washWipeControl->activateServant();
			}
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
			// There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "A Camera or its controls cannot be activated." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
			// There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "A Camera or its controls cannot be activated." );
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
			// There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "TransactiveException", "A Camera or its controls cannot be activated." );
        }
        catch( ... )
        {
			// There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "A Camera or its controls cannot be activated." );
        }
    }
  

    //
    // deactivateCorbaServants
    //
    void Camera::deactivateCorbaServants()
    {
        // Deactivate the CORBA servants, so they no longer accepts incoming calls.

        deactivateServant();    // Don't call deactivateAndDelete() here because Generic Agent deletes this object.

        // Don't call deactivateAndDelete() here because we may not have finished with the objects
        // (ie if we're just going into Monitor mode).
        // These objects are auto_ptrs, so they will be automatically deleted at the right time.

		try
		{
			if ( 0 != m_movementControl.get() )
			{
				m_movementControl->deactivateServant();
			}
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
			// There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "TransactiveException", "MovementControl CORBA servant deactivation failed." );
        }
        catch( ... )
        {
			// There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
                "MovementControl CORBA servant deactivation failed." );
        }

        try
		{
			if ( 0 != m_focusIrisControl.get() )
			{
				m_focusIrisControl->deactivateServant();
			}
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
			// There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "TransactiveException", "FocusIrisControl CORBA servant deactivation failed." );
        }
        catch( ... )
        {
			// There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
                "FocusIrisControl CORBA servant deactivation failed." );
        }

        try
		{
			if ( 0 != m_washWipeControl.get() )
			{
				m_washWipeControl->deactivateServant();
			}
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
			// There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "TransactiveException", "WashWipeControl CORBA servant deactivation failed." );
        }
        catch( ... )
        {
			// There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
                "WashWipeControl CORBA servant deactivation failed." );
        }
    }  



	void Camera::CameraPan(::CORBA::UShort movement, ::CORBA::UShort speed)
	{
		try
		{
			LOG2(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Camera::CameraTilt will be called.--(%d)(%d)",movement ,speed);
			m_pdCamera->CameraPan(  movement,speed );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
	}

	void Camera::CameraTilt( ::CORBA::UShort movement, ::CORBA::UShort speed)
	{
		try
		{
			LOG2(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Camera::CameraTilt will be called.--(%d)(%d)",movement ,speed);
			m_pdCamera->CameraTilt( movement,speed );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
	}

	void Camera::CameraZoom(::CORBA::UShort movement)
	{
		try
		{
			LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Camera::CameraTilt will be called.--(%d)",movement );
			m_pdCamera->CameraZoom( movement);
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
	}

	void Camera::CameraPanAndTilt(::CORBA::UShort pan, ::CORBA::UShort panspeed, ::CORBA::UShort tilt, ::CORBA::UShort tiltspeed)
	{
		try
		{
			LOG2(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Camera::CameraTilt will be called.--(%d)(%d)",pan,tilt );
			m_pdCamera->CameraPanAndTilt(pan, panspeed,  tilt,  tiltspeed);;
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
	}

} // namespace TA_IRS_App


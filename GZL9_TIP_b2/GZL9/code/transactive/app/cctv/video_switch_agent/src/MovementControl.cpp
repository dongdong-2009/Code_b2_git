/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/src/MovementControl.cpp $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Controls pan, tilt, zoom and presets for a Camera.
  * Implements the IMovementControl interface. 
  * Uses the IPDMovementControl interface to perform the protocol
  * dependent sections of the control functions.
  */

#include <sstream>

#include "app/cctv/video_switch_agent/src/MovementControl.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/AllSupportedFeatures.h"
#include "app/cctv/video_switch_agent/src/Camera.h"
#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/data_access_interface/entity_access/src/Camera.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
    const std::string MovementControl::GET_ACTIVE_PRESET            = "GetActivePreset";
    const std::string MovementControl::GET_ALL_PRESETS              = "GetAllPresets";
    const std::string MovementControl::GET_FULL_PRESET_RANGE        = "GetFullPresetRange";
    const std::string MovementControl::GET_MODIFIABLE_PRESET_RANGE  = "GetModifiablePresetRange";

    //
    // MovementControl
    //
    MovementControl::MovementControl( Camera& camera )
        : m_camera( camera )
    {
        m_pdMovementControl = std::auto_ptr< IPDMovementControl >( 
            getPDInterfaceFactory().createMovementControl( m_camera.getCameraData() ) );
        if ( 0 == m_pdMovementControl.get() )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDMovementControl" ) );
        }
    }
        
    
    //
    // isFeatureSupported
    //
    bool MovementControl::isFeatureSupported( TA_Visual::EMovementFeature feature )
    {       
        // Check if it is supported by the protocol.

        if ( !m_pdMovementControl->isFeatureSupported( feature ) )
        {
            return false;
        }

        // Check if it is supported by this Camera.
        
        std::string featureStr = "Unknown";
        try
        {
            switch( feature )
            {
                case TA_Visual::Pan:
                    featureStr = TA_Visual::PAN;
                    return m_camera.getCameraData()->isMovementSupported();
                case TA_Visual::Tilt:
                    featureStr = TA_Visual::TILT;
                    return m_camera.getCameraData()->isMovementSupported();
                case TA_Visual::PanAndTilt:
                    featureStr = TA_Visual::PAN_AND_TILT;
                    return m_camera.getCameraData()->isMovementSupported();
                case TA_Visual::Zoom:
                    featureStr = TA_Visual::ZOOM;
                    return m_camera.getCameraData()->isMovementSupported();
                case TA_Visual::ProportionalMovement:
                    featureStr = TA_Visual::PROPORTIONAL_MOVEMENT;
                    return m_camera.getCameraData()->isProportionalMovementSupported();
                case TA_Visual::Presets:
                    featureStr = TA_Visual::PRESETS;
                    return m_camera.getCameraData()->arePresetsSupported();
                default:
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unknown feature (%s). Assuming feature is not supported", featureStr.c_str() );
                    return false;
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            std::stringstream details;
            details << "Assuming feature (" << featureStr << ") is not supported";
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", details.str().c_str() );
            return false;
        }        
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            std::stringstream details;
            details << "Assuming feature (" << featureStr << ") is not supported";
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", details.str().c_str() );
            return false;
        }
        return false;
    }


  	///////////////////////////////////////////////////////////////////////
	//
	// State Update Methods
	//
	///////////////////////////////////////////////////////////////////////


    //
    // updatePresetName
    //
    void MovementControl::updatePresetName( unsigned short index, const std::string& name )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "updatePresetName" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );
		try
		{
        	m_pdMovementControl->updatePreset( index, name );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}

        LOG_FUNCTION_EXIT( SourceInfo, "updatePresetName" );
    }


    //
    // updateState
    //
    void MovementControl::updateState( const TA_Base_Bus_GZL9::CameraCorbaDef::CameraState& state )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "updateState" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );
        m_pdMovementControl->updateState( state );

        LOG_FUNCTION_EXIT( SourceInfo, "updateState" );
    }

  	///////////////////////////////////////////////////////////////////////
	//
	// MovementControlCorbaDef Methods
	//
	///////////////////////////////////////////////////////////////////////

    void MovementControl::panAndTilt( TA_Base_Bus_GZL9::MovementControlCorbaDef::EPanDirection panDirection,
                                      unsigned short panSpeed,
                                      TA_Base_Bus_GZL9::MovementControlCorbaDef::ETiltDirection tiltDirection,
                                      unsigned short tiltSpeed,
                                      const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "panAndTilt" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::PAN_AND_TILT );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::PAN_AND_TILT );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::PanAndTilt );
        m_camera.verifyInService();

        // Set the speed to zero if proportional movement is not supported.

        if ( !isFeatureSupported( TA_Visual::ProportionalMovement ) )
        {
            if ( panSpeed != 0 )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "Pan Speed set to zero because proportional movement is not supported." );
                panSpeed = 1;
            }

            if ( tiltSpeed != 0 )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "Tilt Speed set to zero because proportional movement is not supported." );
                tiltSpeed = 1;
            }
        }

        try
        {
            m_pdMovementControl->panAndTilt( panDirection, panSpeed, tiltDirection, tiltSpeed );
        }
        catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
        {
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
        }
        
        LOG_FUNCTION_EXIT( SourceInfo, "panAndTilts" );
    }

	///////////////////////////////////////////////////////////////////////
	//
	// Pan Controls
	//
	///////////////////////////////////////////////////////////////////////


	//
    // panLeft
    //
	void MovementControl::panLeft( unsigned short speed, const char* sessionId, bool isContinuous )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "panLeft" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::PAN_LEFT );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::PAN_LEFT );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Pan );
        m_camera.verifyInService();

        // Set the speed to zero if proportional movement is not supported.

        if ( ( 0 != speed ) && ( !isFeatureSupported( TA_Visual::ProportionalMovement ) ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Speed set to zero because proportional movement is not supported." );
            speed = 0;
        }

		try
		{
        	m_pdMovementControl->panLeft( speed, isContinuous );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "panLeft" );
    }


	//
    // panRight
	// 	
    void MovementControl::panRight( unsigned short speed, const char* sessionId, bool isContinuous )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "panRight" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::PAN_RIGHT );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::PAN_RIGHT );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Pan );
        m_camera.verifyInService();

        // Set the speed to zero if proportional movement is not supported.

        if ( ( 0 != speed ) && ( !isFeatureSupported( TA_Visual::ProportionalMovement ) ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Speed set to zero because proportional movement is not supported." );
            speed = 0;
        }

		try
		{
        	m_pdMovementControl->panRight( speed, isContinuous );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "panRight" );
    }


    //
    // panHalt
    //
	void MovementControl::panHalt( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "panHalt" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::PAN_HALT );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::PAN_HALT );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Pan );
        m_camera.verifyInService();
		
		try
		{
        	m_pdMovementControl->panHalt();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
		
        LOG_FUNCTION_EXIT( SourceInfo, "panHalt" );
    }


	///////////////////////////////////////////////////////////////////////
	//
	// Tilt Controls
	//
	///////////////////////////////////////////////////////////////////////


    //
    // tiltUp
    //
	void MovementControl::tiltUp( unsigned short speed, const char* sessionId, bool isContinuous )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "tiltUp" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::TILT_UP );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::TILT_UP );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Tilt );
        m_camera.verifyInService();

        // Set the speed to zero if proportional movement is not supported.

        if ( ( 0 != speed ) && ( !isFeatureSupported( TA_Visual::ProportionalMovement ) ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Speed set to zero because proportional movement is not supported." );
            speed = 0;
        }

		try
		{
        	m_pdMovementControl->tiltUp( speed, isContinuous );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "tiltUp" );
    }

	
    //
    // tiltDown
    //
	void MovementControl::tiltDown( unsigned short speed, const char* sessionId, bool isContinuous )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "tiltDown" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::TILT_DOWN );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::TILT_DOWN );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Tilt );
        m_camera.verifyInService();

        // Set the speed to zero if proportional movement is not supported.

        if ( ( 0 != speed ) && ( !isFeatureSupported( TA_Visual::ProportionalMovement ) ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Speed set to zero because proportional movement is not supported." );
            speed = 0;
        }

		try
		{
        	m_pdMovementControl->tiltDown( speed, isContinuous );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
	
        
        LOG_FUNCTION_EXIT( SourceInfo, "tiltDown" );
    }


    //
    // tiltHalt
    //
	void MovementControl::tiltHalt( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "tiltHalt" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::TILT_HALT );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::TILT_HALT );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Tilt );
        m_camera.verifyInService();
	
		try
		{
        	m_pdMovementControl->tiltHalt( );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "tiltHalt" );
    }


	///////////////////////////////////////////////////////////////////////
	//
	// Zoom Controls
	//
	///////////////////////////////////////////////////////////////////////


    //
    // zoomIn
    //
	void MovementControl::zoomIn( unsigned short speed, const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "zoomIn" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::ZOOM_IN );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::ZOOM_IN );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Zoom );
        m_camera.verifyInService();

        // Set the speed to zero if proportional movement is not supported.

        if ( ( 0 != speed ) && ( !isFeatureSupported( TA_Visual::ProportionalMovement ) ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Speed set to zero because proportional movement is not supported." );
            speed = 0;
        }

		try
		{
        	m_pdMovementControl->zoomIn( speed );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "zoomIn" );
    }


    //
    // zoomOut
    //
	void MovementControl::zoomOut( unsigned short speed, const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "zoomOut" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::ZOOM_OUT );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::ZOOM_OUT );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Zoom );
        m_camera.verifyInService();

        // Set the speed to zero if proportional movement is not supported.

        if ( ( 0 != speed ) && ( !isFeatureSupported( TA_Visual::ProportionalMovement ) ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Speed set to zero because proportional movement is not supported." );
            speed = 0;
        }

		try
		{
        	m_pdMovementControl->zoomOut( speed );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "zoomOut" );
    }


    //
    // zoomHalt
    //
	void MovementControl::zoomHalt( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "zoomHalt" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::ZOOM_HALT );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::ZOOM_HALT );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Zoom );
        m_camera.verifyInService();

		try
		{
        	m_pdMovementControl->zoomHalt();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "zoomHalt" );
    }


	///////////////////////////////////////////////////////////////////////
	//
	// Preset Controls
	//
	///////////////////////////////////////////////////////////////////////
    

    //
    // activatePreset
    //
    void MovementControl::activatePreset( unsigned short index, const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "activatePreset" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::PRESET_ACTIVATE );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::PRESET_ACTIVATE );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Presets );
        m_camera.verifyInService();

		// Check that the preset is in the valid range.

		unsigned short minimumIndex = 0;
		unsigned short maximumIndex = 0;
		getFullPresetRange( minimumIndex, maximumIndex );
		if( ( index < minimumIndex ) || ( maximumIndex < index ) )
		{
			std::stringstream error;
			error << "The preset index " << index << " is invalid. ";
			error << "It must be in the range " << minimumIndex;
			error << " to " << maximumIndex << " inclusive.";
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
		}

		try
		{
        	m_pdMovementControl->activatePreset( index );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "activatePreset" );
    }

 
    //
    // getActivePreset
    //
    TA_Base_Bus_GZL9::MovementControlCorbaDef::CameraPreset* MovementControl::getActivePreset()
    {
        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( GET_ACTIVE_PRESET );

		try
		{
        	return m_pdMovementControl->getActivePreset();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
    }


    //
    // getAllPresets
    //
    TA_Base_Bus_GZL9::MovementControlCorbaDef::CameraPresetSequence* MovementControl::getAllPresets()
    {
        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( GET_ALL_PRESETS );

		try
		{
        	return m_pdMovementControl->getAllPresets();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
    }

 
    //
    // storePreset
    //
    void MovementControl::storePreset( const TA_Base_Bus_GZL9::MovementControlCorbaDef::CameraPreset& preset, const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "storePreset" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::PRESET_STORE );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::PRESET_STORE );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Presets );
        m_camera.verifyInService();

		try
		{
        	m_pdMovementControl->storePreset( preset );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "storePreset" );
    }


    //
    // getFullPresetRange
    //
    void MovementControl::getFullPresetRange( unsigned short& minimumIndex, unsigned short& maximumIndex )
    {
        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( GET_FULL_PRESET_RANGE );
		
		try
		{
        	m_pdMovementControl->getFullPresetRange( minimumIndex, maximumIndex );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
    }


    //
    // getModifiablePresetRange
    //
    void MovementControl::getModifiablePresetRange( unsigned short& minimumIndex, unsigned short& maximumIndex )
    {
        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( GET_MODIFIABLE_PRESET_RANGE );
		
		try
		{
        	m_pdMovementControl->getModifiablePresetRange( minimumIndex, maximumIndex );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
    }


    //
    // setPresetName
    //
    void MovementControl::setPresetName( unsigned short index, const char* name, 
        const char* sessionId )    
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "setPresetName" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera movement control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::PRESET_SET_NAME );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::PRESET_SET_NAME );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Presets );
        m_camera.verifyInService();

		// Check that the preset index is in the valid range.

		unsigned short minimumIndex = 0;
		unsigned short maximumIndex = 0;
		getModifiablePresetRange( minimumIndex, maximumIndex );
		if( ( index < minimumIndex ) || ( maximumIndex < index ) )
		{
			std::stringstream error;
			error << "The preset index " << index << " is invalid. ";
			error << "It must be in the range " << minimumIndex;
			error << " to " << maximumIndex << " inclusive.";
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
		}

		try
		{
        	m_pdMovementControl->setPresetName( index, name );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "setPresetName" );
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Private Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // hasInitialised
    //
    bool MovementControl::hasInitialised() const
    {
        return ( 0 != m_pdMovementControl.get() );
    }

    
    //
    // verifyFeatureIsSupported
    //
    void MovementControl::verifyFeatureIsSupported( TA_Visual::EMovementFeature feature )
    {
        if ( !isFeatureSupported( feature ) )
        {
            std::stringstream error;
            switch( feature )
            {
                case TA_Visual::Pan:
                    error << TA_Visual::PAN;
                case TA_Visual::Tilt:
                    error << TA_Visual::TILT;
                case TA_Visual::PanAndTilt:
                    error << TA_Visual::PAN_AND_TILT;
                case TA_Visual::Zoom:
                    error << TA_Visual::ZOOM;
                case TA_Visual::ProportionalMovement:
                    error << TA_Visual::PROPORTIONAL_MOVEMENT;
                case TA_Visual::Presets:
                    error << TA_Visual::PRESETS;
                default:
                    error << TA_Visual::UNKNOWN;
            }        
            error << " feature is not supported.";
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
        }
    }

}	// TA_IRS_App
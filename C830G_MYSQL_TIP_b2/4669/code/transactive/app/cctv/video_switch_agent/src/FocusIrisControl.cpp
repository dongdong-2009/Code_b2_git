/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/src/FocusIrisControl.cpp $
  * @author:   Katherine Thomson
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/01/25 17:58:20 $
  * Last modified by:  $Author: builder $
  *
  * Handles the focus and iris control for a Camera.
  * Implements the IFocusIrisControl interface.
  * Uses the IPDFocusIrisControl interface to perform the protocol
  * dependent sections of the control functions.
  */

#include <sstream>

#include "app/cctv/video_switch_agent/src/FocusIrisControl.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/AllSupportedFeatures.h"
#include "app/cctv/video_switch_agent/src/Camera.h"
#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"
#include "bus/cctv/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/data_access_interface/entity_access/src/Camera.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_App
{
    const std::string FocusIrisControl::IS_AUTO_FOCUS_ON            = "IsAutoFocusOn";
    const std::string FocusIrisControl::SET_AUTO_FOCUS              = "SetAutoFocus";
    const std::string FocusIrisControl::IS_AUTO_IRIS_ON             = "IsAutoIrisOn";
    const std::string FocusIrisControl::SET_AUTO_IRIS               = "SetAutoIris";
    const std::string FocusIrisControl::IS_PEAK_WHITE_INVERTER_ON   = "IsIrisPeakWhiteInversionOn";
    const std::string FocusIrisControl::SET_PEAK_WHITE_INVERTER     = "SetIrisPeakWhiteInversion";

    //
    // FocusIrisControl
    //
    FocusIrisControl::FocusIrisControl( Camera& camera )
        : m_camera( camera ),
        m_pdFocusIrisControl(), // HuangQi++
        m_userPrivilegeVerifier( UserPrivilegeVerifier::getInstance() ) // HuangQi++
    {
        m_pdFocusIrisControl = std::auto_ptr< IPDFocusIrisControl >( 
            PDInterfaceFactory::createFocusIrisControl( m_camera.getCameraData() ) );
        if ( 0 == m_pdFocusIrisControl.get() )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDFocusIrisControl" ) );
        }
    }

    
    //
    // isFeatureSupported
    //
    bool FocusIrisControl::isFeatureSupported( TA_Visual::EFocusIrisFeature feature )
    {
        // Check if it is supported by the protocol.

        if ( !m_pdFocusIrisControl->isFeatureSupported( feature ) )
        {
            return false;
        }

        // Check if it is supported by this Camera.

        std::string featureStr = "Unknown";
        try
        {
            switch( feature )
            {
                case TA_Visual::AutoFocus:
                    featureStr = TA_Visual::AUTO_FOCUS;
                    return m_camera.getCameraData()->isAutoFocusSupported();
                case TA_Visual::ManualFocus:
                    featureStr = TA_Visual::MANUAL_FOCUS;
                    return m_camera.getCameraData()->isManualFocusSupported();
                case TA_Visual::AutoIris:
                    featureStr = TA_Visual::AUTO_IRIS;
                    return m_camera.getCameraData()->isAutoIrisSupported();
                case TA_Visual::ManualIris:
                    featureStr = TA_Visual::MANUAL_IRIS;
                    return m_camera.getCameraData()->isManualIrisSupported();
                case TA_Visual::IrisPeakWhiteInversion:
                    featureStr = TA_Visual::IRIS_PEAK_WHITE_INVERSION;
                    return m_camera.getCameraData()->isIrisPeakWhiteInversionSupported();
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
    // updateState
    //
    void FocusIrisControl::updateState( const TA_Base_Bus::CameraCorbaDef::CameraState& state )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "updateState" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera iris control is NULL" );
        m_pdFocusIrisControl->updateState( state );

        LOG_FUNCTION_EXIT( SourceInfo, "updateState" );
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // FocusIrisControlCorbaDef Methods
    //
    ///////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////
    //
    // Focus Controls
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // isAutoFocusOn
    //
	bool FocusIrisControl::isAutoFocusOn()
    {
        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera iris control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( IS_AUTO_FOCUS_ON );
		try
		{
        	return m_pdFocusIrisControl->isAutoFocusOn();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
    }


    //
    // setAutoFocusOn
    //
	void FocusIrisControl::setAutoFocusOn( bool isAutoFocusOn, const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "setAutoFocusOn" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera focus control is NULL" );
       
        AgentModeMonitor::getInstance().verifyInControlMode( SET_AUTO_FOCUS );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, SET_AUTO_FOCUS );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::AutoFocus );
        m_camera.verifyInService();

		try
		{
        	m_pdFocusIrisControl->setAutoFocusOn( isAutoFocusOn );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "setAutoFocusOn" );
    }


    //
    // focusNear
    //
	void FocusIrisControl::focusNear( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "focusNear" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera focus control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::FOCUS_NEAR );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::FOCUS_NEAR );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::ManualFocus );
        m_camera.verifyInService();
		try
		{
        	m_pdFocusIrisControl->focusNear();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
		
        
        LOG_FUNCTION_EXIT( SourceInfo, "focusNear" );
    }


    //
    // focusFar
    //
	void FocusIrisControl::focusFar( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "focusFar" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera focus control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::FOCUS_FAR );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::FOCUS_FAR );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::ManualFocus );
        m_camera.verifyInService();
		try
		{
        	m_pdFocusIrisControl->focusFar();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "focusFar" );
    }


    //
    // focusHalt
    //
	void FocusIrisControl::focusHalt( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "focusHalt" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera focus control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::FOCUS_HALT );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::FOCUS_HALT );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::ManualFocus );
        m_camera.verifyInService();

		try
		{
        	m_pdFocusIrisControl->focusHalt();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "focusHalt" );
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Iris Controls
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // isAutoIrisOn
    //
	bool FocusIrisControl::isAutoIrisOn()
    {
        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera iris control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( IS_AUTO_IRIS_ON );
		try
		{
        	return m_pdFocusIrisControl->isAutoIrisOn();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
    }


    //
    // setAutoIrisOn
    //
	void FocusIrisControl::setAutoIrisOn( bool isAutoIrisOn, const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "setAutoIrisOn" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera iris control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( SET_AUTO_IRIS );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, SET_AUTO_IRIS );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::AutoIris );
        m_camera.verifyInService();

		try
		{
        	m_pdFocusIrisControl->setAutoIrisOn( isAutoIrisOn );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "setAutoIrisOncusHalt" );
    }


    //
    // openIris
    //
	void FocusIrisControl::openIris( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "openIris" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera iris control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::IRIS_OPEN );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::IRIS_OPEN );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::ManualIris );
        m_camera.verifyInService();
		
		try
		{
        	m_pdFocusIrisControl->openIris();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "openIris" );
    }


    //
    // closeIris
    //
	void FocusIrisControl::closeIris( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "closeIris" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera iris control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::IRIS_CLOSE );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::IRIS_CLOSE );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::ManualIris );
        m_camera.verifyInService();
	
		try
		{
        	m_pdFocusIrisControl->closeIris();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "closeIris" );
    }


    //
    // irisHalt
    //
	void FocusIrisControl::irisHalt( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "irisHalt" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera iris control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::IRIS_HALT );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::IRIS_HALT );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::ManualIris );
        m_camera.verifyInService();

		try
		{
        	m_pdFocusIrisControl->irisHalt();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "irisHalt" );
    }

  
    //
    // isIrisPeakWhiteInversionOn
    //
	bool FocusIrisControl::isIrisPeakWhiteInversionOn()
    {
        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera iris control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( IS_PEAK_WHITE_INVERTER_ON );

		try
		{
        	return m_pdFocusIrisControl->isIrisPeakWhiteInversionOn();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
		
    }


    //
    // setIrisPeakWhiteInversionOn
    //
	void FocusIrisControl::setIrisPeakWhiteInversionOn( bool isIrisPeakWhiteInversionOn, const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "setIrisPeakWhiteInversionOn" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera iris control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( SET_PEAK_WHITE_INVERTER );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, SET_PEAK_WHITE_INVERTER );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::IrisPeakWhiteInversion );
        m_camera.verifyInService();
	
		try
		{
        	m_pdFocusIrisControl->setIrisPeakWhiteInversionOn( isIrisPeakWhiteInversionOn );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "setIrisPeakWhiteInversionOn" );
    }


    ///////////////////////////////////////////////////////////////////////////
    //
    // Private Methods
    //
    ///////////////////////////////////////////////////////////////////////////


    //
    // hasInitialised
    //
    bool FocusIrisControl::hasInitialised() const
    {
        return ( 0 != m_pdFocusIrisControl.get() );
    }

    
    //
    // verifyFeatureIsSupported
    //
    void FocusIrisControl::verifyFeatureIsSupported( TA_Visual::EFocusIrisFeature feature )
    {
        if ( !isFeatureSupported( feature ) )
        {
            std::stringstream error;
            switch( feature )
            {
                case TA_Visual::AutoFocus:
                    error << TA_Visual::AUTO_FOCUS;
                case TA_Visual::ManualFocus:
                    error << TA_Visual::MANUAL_FOCUS;
                case TA_Visual::AutoIris:
                    error << TA_Visual::AUTO_IRIS;
                case TA_Visual::ManualIris:
                    error << TA_Visual::MANUAL_IRIS;
                case TA_Visual::IrisPeakWhiteInversion:
                    error << TA_Visual::IRIS_PEAK_WHITE_INVERSION;
                default:
                    error << TA_Visual::UNKNOWN;
            }        
            error << " feature is not supported.";
            throw TA_Base_Bus::VideoSwitchAgentException( error.str().c_str() );
        }
    }

} // TA_IRS_App


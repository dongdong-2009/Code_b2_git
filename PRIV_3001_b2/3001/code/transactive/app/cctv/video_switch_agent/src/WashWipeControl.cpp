/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/src/WashWipeControl.cpp $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Handles washing and wiping control for a Camera.
  * Implements the IWashWipeControl interface.
  * Uses the IPDWashWipeControl interface to perform the protocol
  * dependent sections of the control functions.
  */

#include <sstream>

#include "app/cctv/video_switch_agent/src/WashWipeControl.h"
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

    const std::string WashWipeControl::IS_WIPER_ON  = "IsWiperOn";
    const std::string WashWipeControl::IS_WASHER_ON = "IsWasherOn";

    //
    // WashWipeControl
    //
    WashWipeControl::WashWipeControl( Camera& camera )
        : m_camera( camera )
    {
        m_pdWashWipeControl = std::auto_ptr< IPDWashWipeControl >( 
            PDInterfaceFactory::createWashWipeControl( m_camera.getCameraData() ) );
        if ( 0 == m_pdWashWipeControl.get() )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDWashWipeControl" ) );
        }
    }


    //
    // isFeatureSupported
    //
    bool WashWipeControl::isFeatureSupported( TA_Visual::EWashWipeFeature feature )
    {       
        // Check if it is supported by the protocol.

        if ( !m_pdWashWipeControl->isFeatureSupported( feature ) )
        {
            return false;
        }

        // Check if it is supported by this Camera.
        
        std::string featureStr = "Unknown";
        try
        {
            switch( feature )
            {
                case TA_Visual::Wash:
                    featureStr = TA_Visual::WASH;
                    return m_camera.getCameraData()->isWashSupported();
                case TA_Visual::Wipe:
                    featureStr = TA_Visual::WIPE;
                    return m_camera.getCameraData()->isWipeSupported();
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
    // IWashWipeControl Methods
    //
    ///////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////
    //
    // Wash Controls
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // startWashing
    //
	void WashWipeControl::startWashing( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "startWashing" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera wash control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::WASH_START );
        
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::WASH_START );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Wash );
        m_camera.verifyInService();

		try
		{
        	m_pdWashWipeControl->startWashing();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "startWashing" );
    }


    //
    // stopWashing
    //
	void WashWipeControl::stopWashing( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "stopWashing" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera wash control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::WASH_STOP );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::WASH_STOP );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Wash );
        m_camera.verifyInService();
		try
		{
        	m_pdWashWipeControl->stopWashing();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "stopWashing" );
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Wipe Controls
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // startWiping
    //
	void WashWipeControl::startWiping( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "startWiping" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera wipe control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::WIPE_START );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::WIPE_START );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Wipe );
        m_camera.verifyInService();

		try
		{
        	m_pdWashWipeControl->startWiping();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "startWiping" );
    }


    //
    // stopWiping
    //
	void WashWipeControl::stopWiping( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "stopWiping" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera wipe control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::WIPE_STOP );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, TA_Visual::WIPE_STOP );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Wipe );
        m_camera.verifyInService();

		try
		{
        	m_pdWashWipeControl->stopWiping();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "stopWiping" );
    }


    //
    // isWiperOn
    //
    bool WashWipeControl::isWiperOn( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "isWiperOn" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera wipe control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( IS_WIPER_ON );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, IS_WIPER_ON );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Wipe );
        m_camera.verifyInService();

		try
		{
        	return m_pdWashWipeControl->isWiperOn();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "isWiperOn" );
        return false;
    }


    //
    // isWasherOn
    //
    bool WashWipeControl::isWasherOn( const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "isWasherOn" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent Camera wash control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( IS_WASHER_ON );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_camera.getCameraData(), TA_Base_Bus::aca_CONTROL_CAMERA, IS_WASHER_ON );
        m_camera.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::Wash );
        m_camera.verifyInService();

		try
		{
        	return m_pdWashWipeControl->isWiperOn();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "isWasherOn" );
        return false;
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Private Methods
    //
    ///////////////////////////////////////////////////////////////////////

    
    //
    // hasInitialised
    //
    bool WashWipeControl::hasInitialised() const
    {
        return ( 0 != m_pdWashWipeControl.get() );
    }

    
    //
    // verifyFeatureIsSupported
    //
    void WashWipeControl::verifyFeatureIsSupported( TA_Visual::EWashWipeFeature feature )
    {
        if ( !isFeatureSupported( feature ) )
        {
            std::stringstream error;
            switch( feature )
            {
                case TA_Visual::Wash:
                    error << TA_Visual::WASH;
                case TA_Visual::Wipe:
                    error << TA_Visual::WIPE;
                default:
                    error << TA_Visual::UNKNOWN;
            }        
            error << " feature is not supported.";
            throw TA_Base_Bus::VideoSwitchAgentException( error.str().c_str() );
        }
    }
} // TA_IRS_App


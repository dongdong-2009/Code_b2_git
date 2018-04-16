/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/src/SuperimposedTextControl.cpp $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Handles all the functions involving superimposing text on a video input.
  * Implements the ISuperimposedTextControl interface. 
  * Uses the IPDSuperimposedTextControl interface to perform the protocol
  * dependent sections of the control functions.
  */

#include <sstream>

#include "app/cctv/video_switch_agent/src/SuperimposedTextControl.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/AllSupportedFeatures.h"
#include "app/cctv/video_switch_agent/src/VideoInput.h"
#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"
#include "bus/cctv/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/data_access_interface/entity_access/src/VideoInput.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    //
    // SuperimposedTextControl
    //
    SuperimposedTextControl::SuperimposedTextControl( VideoInput& videoInput )
        : m_videoInput( videoInput )
    {
        m_pdSuperimposedTextControl = std::auto_ptr< IPDSuperimposedTextControl >( 
            PDInterfaceFactory::createSuperimposedTextControl( m_videoInput.getVideoInputData() ) );
        if ( 0 == m_pdSuperimposedTextControl.get() )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDSuperimposedTextControl." ) );
        }
    }


    //
    // isFeatureSupported
    //
    bool SuperimposedTextControl::isFeatureSupported( TA_Visual::ESuperimposedTextFeature feature )
    {
        // Check if it is supported by the protocol.

        if ( !m_pdSuperimposedTextControl->isFeatureSupported( feature ) )
        {
            return false;
        }

        // Check if it is supported by this Camera.

        std::string featureStr = "Unknown";
        try
        {
            switch( feature )
            {
                case TA_Visual::SuperimposeDate:
                    featureStr = TA_Visual::SUPERIMPOSE_DATE;
                    return m_videoInput.getVideoInputData()->isSuperimposeDateSupported();
                case TA_Visual::SuperimposeName:
                    featureStr = TA_Visual::SUPERIMPOSE_NAME;
                    return m_videoInput.getVideoInputData()->isSuperimposeNameSupported();
                case TA_Visual::SuperimposeText:
                    featureStr = TA_Visual::SUPERIMPOSE_TEXT;
                    return m_videoInput.getVideoInputData()->isSuperimposeTextSupported();
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
    void SuperimposedTextControl::updateState( const TA_Base_Bus::VideoInputCorbaDef::VideoInputState& state )    
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "updateState" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent superimposed text control is NULL" );
        m_pdSuperimposedTextControl->updateState( state );

        LOG_FUNCTION_EXIT( SourceInfo, "updateState" );
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // SuperimposedTextControlCorbaDef Methods
    //
    ///////////////////////////////////////////////////////////////////////
	
    //
    // isDateSuperimposed
    // 
    bool SuperimposedTextControl::isDateSuperimposed()
    {
        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent superimposed text control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( "isDateSuperimposed" );
		
		try
		{
        	return m_pdSuperimposedTextControl->isDateSuperimposed();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
    }

    
    //
    // superimposeDate
    //
	void SuperimposedTextControl::superimposeDate( bool isDateSuperimposed, const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "superimposeDate" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent superimposed text control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( "Superimpose Date" );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_videoInput.getVideoInputData(), TA_Base_Bus::aca_CONTROL_VIDEO_INPUT, "Superimpose Date" );
        m_videoInput.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::SuperimposeDate );
        m_videoInput.verifyInService();

		try
		{
        	m_pdSuperimposedTextControl->superimposeDate( isDateSuperimposed );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
        
        LOG_FUNCTION_EXIT( SourceInfo, "superimposeDate" );
    }
	

    //
    // isNameSuperimposed
    //
    bool SuperimposedTextControl::isNameSuperimposed()
    {
        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent superimposed text control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( "isNameSuperimposed" );

		try
		{
        	return m_pdSuperimposedTextControl->isNameSuperimposed();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
    }


    //
    // superimposeName
    //
	void SuperimposedTextControl::superimposeName( bool isNameSuperimposed, const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "superimposeName" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent superimposed text control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( "Superimpose Name" );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_videoInput.getVideoInputData(), TA_Base_Bus::aca_CONTROL_VIDEO_INPUT, "Superimpose Name" );
        m_videoInput.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::SuperimposeName );
        m_videoInput.verifyInService();

		try
		{
        	m_pdSuperimposedTextControl->superimposeName( isNameSuperimposed );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}

        LOG_FUNCTION_EXIT( SourceInfo, "superimposeName" );
   }
   

    //
    // getSuperimposedText
    //
    char* SuperimposedTextControl::getSuperimposedText()
    {
        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent superimposed text control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( "getSuperimposedText" );
		
		try
		{
        	return CORBA::string_dup( m_pdSuperimposedTextControl->getSuperimposedText().c_str() );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
    }

    
    //
    // superimposeText
    //
    void SuperimposedTextControl::superimposeText( const char* text, const char* sessionId )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "superimposeText" );

        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent superimposed text control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( "Superimpose Text" );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_videoInput.getVideoInputData(), TA_Base_Bus::aca_CONTROL_VIDEO_INPUT, "Superimpose Text" );
        m_videoInput.verifyUserHoldsLock( sessionId );
        verifyFeatureIsSupported( TA_Visual::SuperimposeText );
        std::string textStr = text;
        if ( textStr.size() > m_pdSuperimposedTextControl->getMaximumTextLength() )
        {
            std::stringstream error;
            error << "The text is too long to be displayed. The maximum number of characters allowed is ";
            error << getMaximumTextLength();
            throw TA_Base_Bus::VideoSwitchAgentException( error.str().c_str() );
        }
        // FUTURE TODO Check Inappropriate words library.
        m_videoInput.verifyInService();

		try
		{
        	m_pdSuperimposedTextControl->superimposeText( textStr );
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}

        LOG_FUNCTION_EXIT( SourceInfo, "superimposeText" );
    }


    //
    // getMaximumTextLength
    //
    unsigned short SuperimposedTextControl::getMaximumTextLength()
    {
        TA_ASSERT( hasInitialised(), "The object handling the protocol dependent superimposed text control is NULL" );

        AgentModeMonitor::getInstance().verifyInControlMode( "getMaximumTextLength" );

		try
		{
        	return m_pdSuperimposedTextControl->getMaximumTextLength();
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Private Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // hasInitialised
    //
    bool SuperimposedTextControl::hasInitialised() const
    {
        return ( 0 != m_pdSuperimposedTextControl.get() );
    }


    //
    // verifyFeatureIsSupported
    //
    void SuperimposedTextControl::verifyFeatureIsSupported( TA_Visual::ESuperimposedTextFeature feature )
    {
        if ( !isFeatureSupported( feature ) )
        {
            std::stringstream error;
            switch( feature )
            {
                case TA_Visual::SuperimposeDate:
                    error << TA_Visual::SUPERIMPOSE_DATE;
                case TA_Visual::SuperimposeName:
                    error << TA_Visual::SUPERIMPOSE_NAME;
                case TA_Visual::SuperimposeText:
                    error << TA_Visual::SUPERIMPOSE_TEXT;
                default:
                    error << "Unknown";
            }        
            error << " feature is not supported.";
            throw TA_Base_Bus::VideoSwitchAgentException( error.str().c_str() );
        }
    }

} // TA_IRS_App


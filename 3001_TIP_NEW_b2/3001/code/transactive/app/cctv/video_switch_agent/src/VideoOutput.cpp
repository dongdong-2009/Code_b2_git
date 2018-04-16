/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/src/VideoOutput.cpp $
  * @author:   Katherine Thomson
  * @version:  $Revision: #15 $
  *
  * Last modification: $DateTime: 2013/12/09 18:11:47 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles all the common functions of video outputs.
  * Implements the IVideoOutput interface. 
  * Uses the IPDVideoOutput interface to perform the protocol
  * dependent sections of the control functions.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <sstream>
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
#include <string.h>

namespace TA_IRS_App
{

    //
    // VideoOutput
    //
    VideoOutput::VideoOutput( TA_Base_Core::IEntityDataPtr entityData )
        :   VideoSwitchResource( entityData ),
            m_videoOutputData( boost::dynamic_pointer_cast<TA_Base_Core::VideoOutput>(entityData) ), 
            m_pdVideoOutput( 0 )
    {
        // Store the database data.

        if ( 0 == m_videoOutputData.get() )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::INVALID_CONFIG,
                "The provided entity data is not VideoOutput data." ) );
        }

        // Create an object to handle the protocol dependent work for the VideoOutput.

        m_pdVideoOutput = std::auto_ptr< IPDVideoOutput >( PDInterfaceFactory::createVideoOutput( m_videoOutputData ) );
        if ( 0 == m_pdVideoOutput.get() )
        {
            // m_videoOutputData is cleaned up by GenericAgent, so we don't have to clean it up here.

            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDVideoOutput." ) );
        }
    }

    
    //
    // ~VideoOutput
    //
    VideoOutput::~VideoOutput()
    {
        // If an exception is thrown here, there is nothing we can do, 
        // but we should make sure no exceptions are thrown out of this destructor.

        try
        {
            // Do not delete this object because it is owned by GenericAgent.

            //m_videoOutputData = 0;
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

    ///////////////////////////////////////////////////////////////////////
    //
    // State Update Methods
    //
    ///////////////////////////////////////////////////////////////////////

    
    //
    // updateState
    //
    void VideoOutput::updateState( const TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState& state )
    {
        FUNCTION_ENTRY("updateState" );

        TA_ASSERT( 0 != m_pdVideoOutput.get(), "The object handling the protocol dependent video output tasks is NULL" );
        m_pdVideoOutput->updateState( state );

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
        catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
        }
    }


    //
    // getVideoOutputGroupName
    //
    char* VideoOutput::getVideoOutputGroupName()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();

        TA_ASSERT( 0 != m_videoOutputData.get(), "The video output database object has not been set." );

        try
        {
            return CORBA::string_dup( m_videoOutputData->getVideoOutputGroupName().c_str() );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }    
        return 0;
    }


    //
    // getCurrentVideoInputKey
    //
    unsigned long VideoOutput::getCurrentVideoInputKey()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();

        TA_ASSERT( 0 != m_pdVideoOutput.get(), "The object handling the protocol dependent video output tasks is NULL" );

        try
        {
            return m_pdVideoOutput->getCurrentVideoInputKey();
        }
        catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
        }
    }

	bool VideoOutput::isQuadInput()
	{
		TA_Base_Bus::GenericAgent::ensureControlMode();

		TA_ASSERT( 0 != m_pdVideoOutput.get(), "The object handling the protocol dependent video output tasks is NULL" );

		try
		{
			return m_pdVideoOutput->isQuadInput() ;
		}
		catch( TA_Base_Core::VideoSwitchAgentInternalException& vsiex )
		{
			throw TA_Base_Bus::VideoSwitchAgentException( vsiex.what() );
		}
	}

	TA_Base_Bus::VideoOutputCorbaDef::QuadCameras VideoOutput::getQuadCamreas()
	{
		TA_Base_Bus::GenericAgent::ensureControlMode();

		TA_ASSERT( 0 != m_pdVideoOutput.get(), "The object handling the protocol dependent video output tasks is NULL" );

		return m_pdVideoOutput->getQuadCameras() ;
	}
    //
    // requestSwitchOfInput
    //
    void VideoOutput::requestSwitchOfInput( unsigned long videoInputEntityKey, 
                                            const char* sessionId, bool isDemand )
    {
		FUNCTION_ENTRY("requestSwitchOfInput");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "requestSwitchOfInput, intputEntity:%lu", videoInputEntityKey);
        TA_ASSERT( 0 != m_pdVideoOutput.get(), "The object handling the protocol dependent video output tasks is NULL" );
        VideoInput* videoInput;
        if ( videoInputEntityKey != 0 )
        {
		    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Get internal video input");
            videoInput = &(SwitchManager::getInstance().getVideoInputInternal( videoInputEntityKey ));
        }

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::REQUEST_SWITCH_OF_INPUT );

		if(sessionId!=NULL)//NULL means called by self
		{
            if(strlen(sessionId) != 0)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin verify rights");
                m_userPrivilegeVerifier.verifyRights( sessionId, m_videoOutputData, TA_Base_Bus::aca_CONTROL_MONITOR_VIDEO_ASSIGNMENT, "Switch Monitor Video Input" );
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "End verify rights");
            }
		}
	    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Send request to hardware");
        m_pdVideoOutput->requestSwitchOfInput( videoInputEntityKey, isDemand );
        FUNCTION_EXIT;
    }

    //
    // requestSwitchOfInput
    //
    void VideoOutput::assignQuad( unsigned long cameraW,unsigned long cameraX,unsigned long cameraY,unsigned long cameraZ, 
                                            const char* sessionId, bool isDemand )
    {
        FUNCTION_ENTRY( "assignQuad" );

        TA_ASSERT( 0 != m_pdVideoOutput.get(), "The object handling the protocol dependent video output tasks is NULL" );


        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::REQUEST_SWITCH_OF_INPUT );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_videoOutputData, TA_Base_Bus::aca_CONTROL_MONITOR_VIDEO_ASSIGNMENT, "Switch Monitor Video Input" );
 
        // Verify that the user holds the lock for the given input.  
        // This will throw an exception as documented in the comment for this method.
 
        m_pdVideoOutput->assignQuad( cameraW,cameraX,cameraY,cameraZ, isDemand );
        FUNCTION_EXIT;
    }

	bool VideoOutput::mappingTrainBorneMonitor(const char* sessionId, bool isDemand)
	{
		FUNCTION_ENTRY( "mappingTrainBorneMonitor" );
		return m_pdVideoOutput->mappingTrainBorneMonitor(isDemand);
		FUNCTION_EXIT;
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

    //
    // update
    //
    void VideoOutput::update( const TA_Base_Core::ConfigUpdateDetails& updateEvent )
    {
        TA_ASSERT( 0 != m_videoOutputData.get(), "The database object for the VideoOutput has not been initialised." );
        m_videoOutputData->invalidate();
        // FUTURE TODO When adding online updates - may need to notify PD object.
        // FUTURE TODO If you update your corba name you have to reregister and update m_myCorbaName.
    }
} // TA_IRS_App


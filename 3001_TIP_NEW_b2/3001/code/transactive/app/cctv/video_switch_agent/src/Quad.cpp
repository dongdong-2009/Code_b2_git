/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/src/Quad.cpp $
  * @author:  Robert Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include "app/cctv/video_switch_agent/src/Quad.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDQuad.h"
#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"
#include "app/cctv/video_switch_agent/src/VisualAuditMessageSender.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{

    Quad::Quad( TA_Base_Core::IEntityDataPtr entityData )
    : VideoInput( entityData )
    , m_quadEntityData( boost::dynamic_pointer_cast<TA_Base_Core::Quad>( entityData ) )
    , m_pdQuad( NULL )
    , m_entityKey( entityData->getKey() )
    {
		if ( m_quadEntityData.get() == NULL )
		{
			TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException(
                      TA_Base_Core::VideoSwitchAgentInternalException::INVALID_CONFIG,
                      "Failed to cast entity data to Quad data" ) );
        }

		m_pdQuad = std::auto_ptr< IPDQuad >( PDInterfaceFactory::createQuad( m_quadEntityData ) );

		activateServantAtInit();
    }


    Quad::~Quad()
	{
		// Nothing yet
		deactivateServantAtClose();
    }


    TA_Base_Bus::QuadCorbaDef::QuadState Quad::getQuadState()
	{
		AgentModeMonitor::getInstance().verifyInControlMode( "getQuadState" );
        if ( m_pdQuad.get() == NULL )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( "Internal error: protocol dependent component not initialised" );
        }
        return m_pdQuad->getQuadState();
	}


	bool Quad::areInputsSwitchable()
	{
		AgentModeMonitor::getInstance().verifyInControlMode( "areInputsSwitchable" );
        if ( m_pdQuad.get() == NULL )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( "Internal error: protocol dependent component not initialised" );
        }
        return m_pdQuad->areInputsSwitchable();
	}


	void Quad::requestSwitchOfInput( TA_Base_Bus::QuadCorbaDef::EQuadSegment segment, unsigned long videoInputEntityKey,
                           	         const char* sessionId, bool isDemand )
	{
		TA_ASSERT( areInputsSwitchable(), "Quad inputs cannot be switched" );

		// verify control mode
		AgentModeMonitor::getInstance().verifyInControlMode( "requestSwitchOfInput" );
		// verify user privileges
        m_userPrivilegeVerifier.verifyRights( sessionId, m_quadEntityData, TA_Base_Bus::aca_CONTROL_QUAD_VIDEO_ASSIGNMENTS, "Switch Quad Inputs" );

        if ( m_pdQuad.get() == NULL )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( "Internal error: protocol dependent component not initialised" );
        }
		// pass request to protocol dependent class
        m_pdQuad->requestSwitchOfInput( segment, videoInputEntityKey, isDemand );
	}


	void Quad::requestSwitchOfInputs( unsigned long topLeftVideoInputEntityKey, unsigned long topRightVideoInputEntityKey,
                            		  unsigned long bottomLeftVideoInputEntityKey, unsigned long bottomRightVideoInputEntityKey,
                            		  const char* sessionId, bool isDemand )
	{
		TA_ASSERT( areInputsSwitchable(), "Quad inputs cannot be switched" );

		// verify control mode
		AgentModeMonitor::getInstance().verifyInControlMode( "requestSwitchOfInputs" );
		// verify user privileges
        m_userPrivilegeVerifier.verifyRights( sessionId, m_quadEntityData, TA_Base_Bus::aca_CONTROL_QUAD_VIDEO_ASSIGNMENTS, "Switch Quad Inputs" );

        if ( m_pdQuad.get() == NULL )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( "Internal error: protocol dependent component not initialised" );
        }

		// pass request to protocol dependent class
        m_pdQuad->requestSwitchOfInputs( topLeftVideoInputEntityKey, topRightVideoInputEntityKey, bottomLeftVideoInputEntityKey,
										 bottomRightVideoInputEntityKey, isDemand );

        try
        {
           // VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::ActionSuccess, m_entityKey, sessionId, "Switch Quad Inputs", m_quadEntityData->getName() );
        }
        catch ( ... )
        {
        }
	}
	

    void Quad::setQuadDescription( const char* newDescription, const char* sessionId )
	{
		//not used
    }	
	//
    // updateState
    //
    void Quad::updateState( const TA_Base_Bus::QuadCorbaDef::QuadState& state )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "updateState" );

        TA_ASSERT( 0 != m_pdQuad.get(), "The object handling the protocol dependent Quad tasks is NULL" );
        m_pdQuad->updateState( state );

        LOG_FUNCTION_EXIT( SourceInfo, "updateState" );
    }


	void Quad::start()
	{
		VideoInput::start();
	}

	void Quad::stop()
	{
		deactivateServant();
		VideoInput::stop();
	}

	void Quad::update( const TA_Base_Core::ConfigUpdateDetails& updateEvent )
	{
		TA_ASSERT( m_quadEntityData.get() != NULL, "Entity data has not been initialised" );
		m_quadEntityData->invalidate();

		VideoInput::update( updateEvent );
	}


	void Quad::remove()
	{
        try
        {
            SwitchManager::getInstance().removeQuad( m_entityKey );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to remove Quad from SwitchManager." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to remove Quad from SwitchManager." );
        }

        stop();
        VideoInput::remove();
	}


	void Quad::activateServantAtInit()
	{
        try
        {
            activateServantWithName( m_quadEntityData->getName() );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            // There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "A Quad cannot be activated." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            // There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "A Quad cannot be activated." );
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            // There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "TransactiveException", "A Quad cannot be activated." );
        }
        catch( ... )
        {
            // There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "A Quad cannot be activated." );
        }
	}


	void Quad::deactivateServantAtClose()
	{
		deactivateServant();
	}
}


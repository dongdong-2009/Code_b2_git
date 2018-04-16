/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSQuad.cpp $
  * @author:   Robert Young
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/08/21 08:52:08 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the protocol dependent sections of the general functions for a Quad
  * using the MODBUS protocol.
  * Inherits from the IPDQuad class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Quad and
  * that the Quad and the protocol support the required features.
  */

#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSQuad.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSSwitchManager.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/StateValidationCycle.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSCommunicationsHandler.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/SwitchQuadInputsCommand.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSHelper.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"

#include "core/alarm/src/AlarmHelperManager.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/types/VisualAlarms_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    //
    // MODBUSQuad
    //
    MODBUSQuad::MODBUSQuad( TA_Base_Core::QuadPtr QuadData )
    : m_QuadData( QuadData )
    {
        m_quadState.topLeftVideoInputKey = 0;
        m_quadState.topRightVideoInputKey = 0;
        m_quadState.bottomLeftVideoInputKey = 0;
        m_quadState.bottomRightVideoInputKey = 0;
        MODBUSSwitchManager::getInstance().addQuad( *this );
        //MODBUSSwitchManager::getInstance().getStateValidationCycle().addValidationItem( *this, false );
    }


    unsigned long MODBUSQuad::getKey()
    {
        return m_QuadData->getKey();
    }

    
    std::string MODBUSQuad::getAddress()
    {
        return m_QuadData->getAddress();
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // IPDQuad Methods
    //
    ///////////////////////////////////////////////////////////////////////

    //
    // getQuadState
    //
	TA_Base_Bus_GZL9::QuadCorbaDef::QuadState MODBUSQuad::getQuadState()
    {
        return m_quadState;
    }


    //
    // areInputsSwitchable
    //
    bool MODBUSQuad::areInputsSwitchable()
    {
        return false;
    }


    //
    // requestSwitchOfInput
    //
    void MODBUSQuad::requestSwitchOfInput( TA_Base_Bus_GZL9::QuadCorbaDef::EQuadSegment segment,
                                          unsigned long videoInputEntityKey,
                                          bool isDemand )
    {
        // TODO
    }


    //
    // requestSwitchOfInputs
    //
    void MODBUSQuad::requestSwitchOfInputs( unsigned long topLeftVideoInputEntityKey,
                                           unsigned long topRightVideoInputEntityKey,
                                           unsigned long bottomLeftVideoInputEntityKey,
                                           unsigned long bottomRightVideoInputEntityKey,
                                           bool isDemand )
    {
        // isDemand is not used for this protocol.
		/*
        try
        {
            MODBUSCommunicationsHandler::RequestCommandSharedPtr command = MODBUSCommunicationsHandler::RequestCommandSharedPtr( new SwitchQuadInputsCommand( topLeftVideoInputEntityKey, 
                topRightVideoInputEntityKey, bottomLeftVideoInputEntityKey, bottomRightVideoInputEntityKey, *this ) );
            if ( 0 == command.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to create SwitchQuadInputsCommand object" );
                LOGMORE( SourceInfo, "Failed to send Switch command." );
                throw TA_Base_Bus_GZL9::VideoSwitchAgentException( "Failed to send Switch command." );
            }

            MODBUSSwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( command, true );

            command->waitForCompletion();

            if ( IAgentRequestCommand::SUCCEEDED != command->getCompletionState() )
            {
                std::string error;
                if ( IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState() )
                {
                    error = "UE-090047"; 
                }
                else
                {
                    error = "UE-090048";
                }
                throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.c_str() );
            }
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send Switch command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( "UE-010028" );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send Switch command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( "UE-010028" );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Switch command." );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
        }        
		*/
    }


    //
    // notifyInputsSwitchedToQuad
    //
    void MODBUSQuad::notifyInputsSwitchedToQuad( unsigned long topLeftVideoInputEntityKey, unsigned long topRightVideoInputEntityKey,
                                        unsigned long bottomLeftVideoInputEntityKey, unsigned long bottomRightVideoInputEntityKey  )
    {
		/*
		if(m_quadState.topLeftVideoInputKey != topLeftVideoInputEntityKey ||
        m_quadState.topRightVideoInputKey != topRightVideoInputEntityKey ||
        m_quadState.bottomLeftVideoInputKey != bottomLeftVideoInputEntityKey ||
        m_quadState.bottomRightVideoInputKey != bottomRightVideoInputEntityKey)
		{
			m_quadState.topLeftVideoInputKey = topLeftVideoInputEntityKey;
			m_quadState.topRightVideoInputKey = topRightVideoInputEntityKey;
			m_quadState.bottomLeftVideoInputKey = bottomLeftVideoInputEntityKey;
			m_quadState.bottomRightVideoInputKey = bottomRightVideoInputEntityKey;

			try
			{
				StateUpdateBroadcaster::getInstance().notifyQuadStateUpdate( m_QuadData->getKey() );
			}
			catch( const TA_Base_Core::DatabaseException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
					"Failed to notify the agent's clients that a switch of quad inputs has occurred." );
			}
			catch( const TA_Base_Core::DataException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
					"Failed to notify the agent's clients that a switch of quad inputs has occurred." );
			}	
		}
		*/
    }


    //
    // updateState
    //
    void MODBUSQuad::updateState( const TA_Base_Bus_GZL9::QuadCorbaDef::QuadState& state )
    {
        // TODO
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IStateValidationItem Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // validateState
    //
    void MODBUSQuad::validateState()
    {
		/*
        MODBUSCommunicationsHandler::RequestCommandSharedPtr statusCmd = 
            MODBUSCommunicationsHandler::RequestCommandSharedPtr( TA_MODBUS::createRequestQuadStatusCommand( *this ) );
        MODBUSSwitchManager::getInstance().getCommunicationsHandler()->submitRequestCommand( statusCmd, false );
        statusCmd->waitForCompletion();
		*/
    }

} // namespace TA_IRS_App

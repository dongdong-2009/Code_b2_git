/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSVideoInput.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Handles the protocol dependent sections of the operations for a VideoInput
  * using the MODBUS protocol. 
  * Inherits from IPDVideoInput. Before the methods
  * in this interface are called, the user privileges have been checked and
  * that the protocol supports the required features.
  */

#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSVideoInput.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

namespace TA_IRS_App
{
    //
    // MODBUSVideoInput
    //
    MODBUSVideoInput::MODBUSVideoInput( TA_Base_Core::VideoInputPtr videoInputData )
        : m_videoInputData( videoInputData ),
          m_lockOwner( "" )
    {
        // Do nothing.
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // State Update Methods
    //
    ///////////////////////////////////////////////////////////////////////
       
    
    //
    // notifyVideoInputStateUpdated
    //
    void MODBUSVideoInput::notifyVideoInputStateUpdated()
    {            
        try
        {
            StateUpdateBroadcaster::getInstance().notifyVideoInputStateUpdate( m_videoInputData->getKey() );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
                "Failed to notify the agent's clients that a VideoInput has changed state." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", 
                "Failed to notify the agent's clients that a VideoInput has changed state." );
        }	
    }


    //
    // updateState
    //
    void MODBUSVideoInput::updateState( const TA_Base_Bus_GZL9::VideoInputCorbaDef::VideoInputState& state )
    {
        m_lockOwner = state.sessionIdOfLockOwner;
        //m_videoInputData.setPublicViewingAllowed( state.isPublicViewingAllowed );
        m_videoInputData->setInService( state.isInService );
    }


	///////////////////////////////////////////////////////////////////////
	//
	// IPDVideoInput Methods
	//
	///////////////////////////////////////////////////////////////////////

    
    //
    // lock
    //
    void MODBUSVideoInput::lock( const std::string& sessionId )
    {
        m_lockOwner = sessionId;
        notifyVideoInputStateUpdated();
    }


    //
    // unlock
    //
    void MODBUSVideoInput::unlock( const std::string& sessionId )
    {
        m_lockOwner = "";
        notifyVideoInputStateUpdated();
    }


    //
    // overrideLock
    //
    void MODBUSVideoInput::overrideLock( const std::string& sessionId )
    {
        m_lockOwner = sessionId;
        notifyVideoInputStateUpdated();
    }


    //
    // getLockOwner
    //
    std::string MODBUSVideoInput::getLockOwner()
    {
        return m_lockOwner;
    }


    //
    // verifyUserHoldsLock
    //
	void MODBUSVideoInput::verifyUserHoldsLock( const std::string& sessionId )
    {
        return; // TODO REMOVE THIS
        
        if ( sessionId != m_lockOwner )
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( m_lockOwner.c_str() );
		}
	}


    //
    // isPublicViewingAllowed
    //
    bool MODBUSVideoInput::isPublicViewingAllowed()
    {
        return m_videoInputData->isPublicViewingAllowed();
    }


    //
    // setPublicViewingAllowed
    //
    void MODBUSVideoInput::setPublicViewingAllowed( bool isPublicViewingAllowed )
    {
        m_videoInputData->setPublicViewingAllowed( isPublicViewingAllowed );
        notifyVideoInputStateUpdated();
    }


    //
    // isInService
    //
    bool MODBUSVideoInput::isInService()
    {
        return m_videoInputData->isInService();
    }


    //
    // setInService
    //
    void MODBUSVideoInput::setInService( bool isInService )
    {
        m_videoInputData->setInService( isInService );
        notifyVideoInputStateUpdated();
    }


} // namespace TA_IRS_App


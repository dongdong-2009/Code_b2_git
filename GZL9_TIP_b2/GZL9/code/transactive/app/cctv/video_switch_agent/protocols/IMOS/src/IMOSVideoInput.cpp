/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/IMOS/src/IMOSVideoInput.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Handles the protocol dependent sections of the operations for a VideoInput
  * using the IMOS protocol. 
  * Inherits from IPDVideoInput. Before the methods
  * in this interface are called, the user privileges have been checked and
  * that the protocol supports the required features.
  */

#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSVideoInput.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

namespace TA_IRS_App
{
    //
    // IMOSVideoInput
    //
    IMOSVideoInput::IMOSVideoInput( TA_Base_Core::VideoInputPtr videoInputData )
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
    void IMOSVideoInput::notifyVideoInputStateUpdated()
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
    void IMOSVideoInput::updateState( const TA_Base_Bus_GZL9::VideoInputCorbaDef::VideoInputState& state )
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
    void IMOSVideoInput::lock( const std::string& sessionId )
    {
        m_lockOwner = sessionId;
        notifyVideoInputStateUpdated();
    }


    //
    // unlock
    //
    void IMOSVideoInput::unlock( const std::string& sessionId )
    {
        m_lockOwner = "";
        notifyVideoInputStateUpdated();
    }


    //
    // overrideLock
    //
    void IMOSVideoInput::overrideLock( const std::string& sessionId )
    {
        m_lockOwner = sessionId;
        notifyVideoInputStateUpdated();
    }


    //
    // getLockOwner
    //
    std::string IMOSVideoInput::getLockOwner()
    {
        return m_lockOwner;
    }


    //
    // verifyUserHoldsLock
    //
	void IMOSVideoInput::verifyUserHoldsLock( const std::string& sessionId )
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
    bool IMOSVideoInput::isPublicViewingAllowed()
    {
        return m_videoInputData->isPublicViewingAllowed();
    }


    //
    // setPublicViewingAllowed
    //
    void IMOSVideoInput::setPublicViewingAllowed( bool isPublicViewingAllowed )
    {
        m_videoInputData->setPublicViewingAllowed( isPublicViewingAllowed );
        notifyVideoInputStateUpdated();
    }


    //
    // isInService
    //
    bool IMOSVideoInput::isInService()
    {
        return m_videoInputData->isInService();
    }


    //
    // setInService
    //
    void IMOSVideoInput::setInService( bool isInService )
    {
        m_videoInputData->setInService( isInService );
        notifyVideoInputStateUpdated();
    }


} // namespace TA_IRS_App


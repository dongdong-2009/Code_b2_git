/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/COECamera.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Handles the protocol dependent sections of the general functions for a Camera
  * using the COE protocol.
  * Inherits from the IPDCamera class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Camera and
  * that the Camera and the protocol support the required features.
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/COECamera.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    //
    // COECamera
    //
    COECamera::COECamera( TA_Base_Core::CameraPtr cameraData )
        : m_cameraData( cameraData ),
          m_isInAlarmState( false )
    {
		COESwitchManager::getInstance().addCamera(*this);
    }
          

    //
    // getKey
    //
    unsigned long COECamera::getKey()
    {
        return m_cameraData->getKey(); 
    }


    //
    // getParsedAddress
    //
    std::pair< unsigned short, unsigned short > COECamera::getParsedAddress()
    {
        std::pair< unsigned short, unsigned short > retVal;

        std::string camAddress = m_cameraData->getAddress();
        std::string::size_type split = camAddress.find( ':' );
        std::istringstream camStn( camAddress.substr( 0, split ) );
        split++;
        std::istringstream camID( camAddress.substr( split, std::string::npos ) );
    
        if ( ( false == (camStn >> retVal.first && camStn.eof()) ) || ( false == (camID >> retVal.second && camID.eof()) ) )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, "Invalid value set to Camera Address" ) );
        }

        if ( ( retVal.first > 0xff) || ( retVal.second > 0xffff) )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, "Invalid value set to Camera Address" ) );
        }

        return retVal;
    }
    
    ///////////////////////////////////////////////////////////////////////
    //
    // State Update Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // notifyIsInAlarmState
    //
    void COECamera::notifyIsInAlarmState( bool isInAlarmState )
    {
        // Update the stored active preset index and notify the agent's clients.
        
        if(m_isInAlarmState != isInAlarmState)
		{
			m_isInAlarmState = isInAlarmState;
			

			try
			{
				StateUpdateBroadcaster::getInstance().notifyCameraStateUpdate( m_cameraData->getKey() );
			}
			catch( const TA_Base_Core::DatabaseException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
					"Failed to notify the agent's clients that the alarm state of a Camera (name unknown) has changed." );
			}
			catch( const TA_Base_Core::DataException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
				LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", 
					"Failed to notify the agent's clients that the alarm state of a Camera (name unknown) has changed." );
			}
		}
    }


    //
    // updateState
    //
    void COECamera::updateState( const TA_Base_Bus::CameraCorbaDef::CameraState& state )
    {
        m_isInAlarmState = state.isInAlarmState;
    }
	

    ///////////////////////////////////////////////////////////////////////
	//
	// IPDCamera Methods
	//
	///////////////////////////////////////////////////////////////////////


    //
    // isInAlarmState
    //
    bool COECamera::isInAlarmState()
    {
        return m_isInAlarmState;
    }

} // namespace TA_IRS_App

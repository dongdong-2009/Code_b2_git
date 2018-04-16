/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/IMOS/src/IMOSCamera.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #5 $
  *
  * Last modification: $DateTime: 2012/08/22 11:34:16 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the protocol dependent sections of the general functions for a Camera
  * using the IMOS protocol.
  * Inherits from the IPDCamera class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Camera and
  * that the Camera and the protocol support the required features.
  */

#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSCamera.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSSwitchManager.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSCommunicationsHandler.h"
//#include "app/cctv/video_switch_agent/protocols/IMOS/src/PTZCommand.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"

#include "bus/cctv_gzl9/video_switch_agent/IDL/src/CameraCorbaDef.h"

#if defined(sparc)
#include "app/cctv/video_switch_agent/protocols/imos_sdk_sparc/include/mw_ptzlib_pub.h"
#elif defined(WIN32)
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/mw_ptzlib_pub.h"
#endif

namespace
{
	enum
	{
	    ZOOMIN_STOP,
	    ZOOMIN,
	    ZOOMOUT_STOP,
	    ZOOMOUT,

	    UP_STOP,
	    UP,
	    DOWN_STOP,
	    DOWN,

	    RIGHT_STOP,
	    RIGHT,
	    LEFT_STOP,
	    LEFT,

	    LEFTUP_STOP,
	    LEFTUP,
	    LEFTDOWN_STOP,
	    LEFTDOWN,

	    RIGHTUP_STOP,
	    RIGHTUP,
	    RIGHTDOWN_STOP,
	    RIGHTDOWN,
		SPEEDSET //set speed
	};

	PTZ_CTRL_COMMAND_S ptzCommands[] =
	{
		{MW_PTZ_ZOOMTELESTOP, 0, 0, 0},
		{MW_PTZ_ZOOMTELE, 0, 0, 0},
		{MW_PTZ_ZOOMWIDESTOP, 0, 0, 0},
		{MW_PTZ_ZOOMWIDE, 0, 0, 0},

		{MW_PTZ_TILTUPSTOP, 0, 0, 0},
		{MW_PTZ_TILTUP, 0, 0, 0},
		{MW_PTZ_TILTDOWNSTOP, 0, 0, 0},
		{MW_PTZ_TILTDOWN, 0, 0, 0},

		{MW_PTZ_PANRIGHTSTOP, 0, 0, 0},
		{MW_PTZ_PANRIGHT, 0, 0, 0},
		{MW_PTZ_PANLEFTSTOP, 0, 0, 0},
		{MW_PTZ_PANLEFT, 0, 0, 0},

		{MW_PTZ_LEFTUPSTOP, 0, 0, 0},
		{MW_PTZ_LEFTUP, 0, 0, 0},
		{MW_PTZ_LEFTDOWNSTOP, 0, 0, 0},
		{MW_PTZ_LEFTDOWN, 0, 0, 0},

		{MW_PTZ_RIGHTUPSTOP, 0, 0, 0},
		{MW_PTZ_RIGHTUP, 0, 0, 0},
		{MW_PTZ_RIGHTDOWNSTOP, 0, 0, 0},
		{MW_PTZ_RIGHTDOWN, 0, 0, 0},
		{MW_PTZ_SPEEDSET, 0, 0, 0}
	};
}

namespace TA_IRS_App
{
    //
    // IMOSCamera
    //
    IMOSCamera::IMOSCamera( TA_Base_Core::CameraPtr cameraData )
        : m_cameraData( cameraData ),
          m_isInAlarmState( false ),
		  m_currentPanSpeed(1),
		  m_currentTiltSpeed(1),
		  m_moveState(TA_Base_Bus_GZL9::CameraCorbaDef::STOP)
    {
        IMOSSwitchManager::getInstance().addCamera( *this );
    }
          

    //
    // getKey
    //
    unsigned long IMOSCamera::getKey()
    {
        return m_cameraData->getKey(); 
    }

	std::string IMOSCamera::getName()
	{
		return m_cameraData->getName();
	}

	std::string IMOSCamera::getAddress()
	{
		return m_cameraData->getAddress();
	}
    ///////////////////////////////////////////////////////////////////////
    //
    // State Update Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // notifyIsInAlarmState
    //
    void IMOSCamera::notifyIsInAlarmState( bool isInAlarmState )
    {
        // Update the stored active preset index and notify the agent's clients.
        
        if(m_isInAlarmState != isInAlarmState)
		{
			m_isInAlarmState = isInAlarmState;
			

			try
			{
				StateUpdateBroadcaster::getInstance().notifyCameraStateUpdate( m_cameraData->getKey());
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
    void IMOSCamera::updateState( const TA_Base_Bus_GZL9::CameraCorbaDef::CameraState& state )
    {
        m_isInAlarmState = state.isInAlarmState;
		m_currentPanSpeed = state.panSpeed;
		m_currentTiltSpeed = state.tiltSpeed;
		m_moveState = state.currentMoveState;
    }
	

    ///////////////////////////////////////////////////////////////////////
	//
	// IPDCamera Methods
	//
	///////////////////////////////////////////////////////////////////////


    //
    // isInAlarmState
    //
    bool IMOSCamera::isInAlarmState()
    {
        return m_isInAlarmState;
    }


	TA_Base_Bus_GZL9::CameraCorbaDef::CameraState IMOSCamera::getCameraState()
	{
		TA_Base_Bus_GZL9::CameraCorbaDef::CameraState state;
		state.isInAlarmState = m_isInAlarmState;
		state.panSpeed = m_currentPanSpeed;
		state.tiltSpeed = m_currentTiltSpeed; 
		state.currentMoveState = m_moveState; 
		return state;
	}

	void IMOSCamera::CameraPan(::CORBA::UShort movement, ::CORBA::UShort speed)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"IMOSCamera::CameraPan be called.--(%d)(%d)(%d)",getKey(),(int)movement,(int)speed );
		try
		{
			// get camera address(resource id in imos)
			std::string address = getAddress();
			do 
			{
				if(0 == movement)
				{
					if (TA_Base_Bus_GZL9::CameraCorbaDef::STOP == m_moveState) // camera not in moving now
					{
						return;
					}
					else if (TA_Base_Bus_GZL9::CameraCorbaDef::LEFT == m_moveState)
					{
						IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[LEFT_STOP]);
					}
					else if (TA_Base_Bus_GZL9::CameraCorbaDef::RIGHT == m_moveState)
					{
						IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[RIGHT_STOP]);
					}
					else
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Camera not in pan state");
						break;
					}
					setMoveState(TA_Base_Bus_GZL9::CameraCorbaDef::STOP);
					break;
				}
				if(speed != m_currentPanSpeed)
				{
					PTZ_CTRL_COMMAND_S s = ptzCommands[SPEEDSET];
					s.ulPTZCmdPara1 = speed; 
					s.ulPTZCmdPara2 = m_currentTiltSpeed; 
					IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &s);
					setPanSpeed(speed);
				}
				if (1 == movement) //move left
				{
					IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[LEFT]);
					setMoveState(TA_Base_Bus_GZL9::CameraCorbaDef::LEFT);
				}
				else // move right
				{
					IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[RIGHT]);
					setMoveState(TA_Base_Bus_GZL9::CameraCorbaDef::RIGHT);
				}
			} while (0);
			// if state change , send message
			if(isStateChanged())
			{
				resetStateChange();
				StateUpdateBroadcaster::getInstance().notifyCameraStateUpdate(m_cameraData->getKey(), true);
			}
		}
		catch (const TA_Base_Core::DatabaseException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send PTZ(Pan) command." );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
		}
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send PTZ(Pan) command." );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
		}
		catch (const TA_Base_Core::VideoSwitchAgentInternalException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send PTZ(pan) command." );
			LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
		}
	}

	void IMOSCamera::CameraTilt(::CORBA::UShort movement, ::CORBA::UShort speed)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"IMOSCamera::CameraTilt be called.--(%d)(%d)(%d)",getKey(),(int)movement,(int)speed );
		try
		{
			// get camera address(resource id in imos)
			std::string address = getAddress();
			do 
			{
				if(0 == movement)
				{
					if (TA_Base_Bus_GZL9::CameraCorbaDef::STOP == m_moveState) // camera not in moving now
					{
						return;
					}
					else if (TA_Base_Bus_GZL9::CameraCorbaDef::UP == m_moveState)
					{
						IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[UP_STOP]);
					}
					else if (TA_Base_Bus_GZL9::CameraCorbaDef::DOWN == m_moveState)
					{
						IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[DOWN_STOP]);
					}
					else
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Camera not in tilt state");
						break;
					}
					setMoveState(TA_Base_Bus_GZL9::CameraCorbaDef::STOP);
					break;
				}
				if(speed != m_currentTiltSpeed)
				{
					PTZ_CTRL_COMMAND_S s = ptzCommands[SPEEDSET];
					s.ulPTZCmdPara1 = m_currentPanSpeed; 
					s.ulPTZCmdPara2 = speed; 
					IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &s);
					setTiltSpeed(speed);
				}
				if (1 == movement) //move UP 
				{
					IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[UP]);
					setMoveState(TA_Base_Bus_GZL9::CameraCorbaDef::UP);
				}
				else // move DOWN 
				{
					IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[DOWN]);
					setMoveState(TA_Base_Bus_GZL9::CameraCorbaDef::DOWN);
				}
			} while (0);
			// if state change , send message
			if(isStateChanged())
			{
				resetStateChange();
				StateUpdateBroadcaster::getInstance().notifyCameraStateUpdate(m_cameraData->getKey(), true);
			}
		}
		catch (const TA_Base_Core::DatabaseException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send PTZ(Tilt) command." );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
		}
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send PTZ(Tilt) command." );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
		}
		catch (const TA_Base_Core::VideoSwitchAgentInternalException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send PTZ(Tilt) command." );
			LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
		}
	}

	void IMOSCamera::CameraZoom(::CORBA::UShort movement)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"IMOSCamera::CameraZoom be called.--(%d)(%d)",getKey(),(int)movement);
		try
		{
			// get camera address(resource id in imos)
			std::string address = getAddress();
			do 
			{
				if(0 == movement)
				{
					if (TA_Base_Bus_GZL9::CameraCorbaDef::STOP == m_moveState) // camera not in moving now
					{
						return;
					}
					else if (TA_Base_Bus_GZL9::CameraCorbaDef::ZOOMIN == m_moveState)
					{
						IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[ZOOMIN_STOP]);
					}
					else if (TA_Base_Bus_GZL9::CameraCorbaDef::ZOOMOUT == m_moveState)
					{
						IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[ZOOMOUT_STOP]);
					}
					else
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Camera not in tilt state");
						break;
					}
					setMoveState(TA_Base_Bus_GZL9::CameraCorbaDef::STOP);
				}
				else if (1 == movement) //zoom in
				{
					IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[ZOOMIN]);
					setMoveState(TA_Base_Bus_GZL9::CameraCorbaDef::ZOOMIN);
				}
				else // zoom out
				{
					IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[ZOOMOUT]);
					setMoveState(TA_Base_Bus_GZL9::CameraCorbaDef::ZOOMOUT);
				}
			} while (0);

			if(isStateChanged())
			{
				resetStateChange();
				StateUpdateBroadcaster::getInstance().notifyCameraStateUpdate(m_cameraData->getKey(), true);
			}
		}
		catch (const TA_Base_Core::DatabaseException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send PTZ(Zoom) command." );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
		}
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send PTZ(Zoom) command." );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
		}
		catch (const TA_Base_Core::VideoSwitchAgentInternalException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send PTZ command." );
			LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
		};
	}

	void IMOSCamera::CameraPanAndTilt(::CORBA::UShort pan, ::CORBA::UShort panspeed, ::CORBA::UShort tilt, ::CORBA::UShort tiltspeed)
	{
		try
		{
			// get camera address(resource id in imos)
			std::string address = getAddress();

			do 
			{
				TA_Base_Bus_GZL9::CameraCorbaDef::MoveState movement;
				if ( 1 == pan && 1 == tilt)
				{
					movement = TA_Base_Bus_GZL9::CameraCorbaDef::LEFTUP;
				}
				else if ( 1 == pan && 2 == tilt)
				{
					movement = TA_Base_Bus_GZL9::CameraCorbaDef::LEFTDOWN;
				}
				else if ( 2 == pan && 1 == tilt)
				{
					movement = TA_Base_Bus_GZL9::CameraCorbaDef::RIGHTUP;
				}
				else if ( 2 == pan && 2 == tilt)
				{
					movement = TA_Base_Bus_GZL9::CameraCorbaDef::RIGHTDOWN;
				}
				else
				{
					movement = TA_Base_Bus_GZL9::CameraCorbaDef::STOP;
				}

				if(TA_Base_Bus_GZL9::CameraCorbaDef::STOP == movement)
				{
					if (TA_Base_Bus_GZL9::CameraCorbaDef::STOP == m_moveState) // camera not in moving now
					{
						return;
					}
					else if (TA_Base_Bus_GZL9::CameraCorbaDef::LEFTUP == m_moveState)
					{
						IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[LEFTUP_STOP]);
					}
					else if (TA_Base_Bus_GZL9::CameraCorbaDef::LEFTDOWN == m_moveState)
					{
						IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[LEFTDOWN_STOP]);
					}
					else if (TA_Base_Bus_GZL9::CameraCorbaDef::RIGHTUP == m_moveState)
					{
						IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[RIGHTUP_STOP]);
					}
					else if (TA_Base_Bus_GZL9::CameraCorbaDef::RIGHTDOWN == m_moveState)
					{
						IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[RIGHTDOWN_STOP]);
					}
					else
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Camera not in pan&tilt state");
						break;
					}
					setMoveState(TA_Base_Bus_GZL9::CameraCorbaDef::STOP);
					break;
				}

				if(panspeed != m_currentPanSpeed || tiltspeed != m_currentTiltSpeed)
				{
					PTZ_CTRL_COMMAND_S s = ptzCommands[SPEEDSET];
					s.ulPTZCmdPara1 = panspeed;
					s.ulPTZCmdPara2 = tiltspeed;
					IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &s);
					setPanSpeed(panspeed);
					setTiltSpeed(tiltspeed);
				}
				if (TA_Base_Bus_GZL9::CameraCorbaDef::LEFTUP == movement) //move LEFTUP 
				{
					IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[LEFTUP]);
					setMoveState(TA_Base_Bus_GZL9::CameraCorbaDef::LEFTUP);
				}
				else if (TA_Base_Bus_GZL9::CameraCorbaDef::LEFTDOWN == movement) //move LEFTDOWN
				{
					IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[LEFTDOWN]);
					setMoveState(TA_Base_Bus_GZL9::CameraCorbaDef::LEFTDOWN);
				}
				else if (TA_Base_Bus_GZL9::CameraCorbaDef::RIGHTUP == movement) //move RIGHTUP
				{
					IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[RIGHTUP]);
					setMoveState(TA_Base_Bus_GZL9::CameraCorbaDef::RIGHTUP);
				}
				else // move RIGHTDOWN 
				{
					IMOSSwitchManager::getInstance().getCommunicationsHandler()->ptzCtrl(address, &ptzCommands[RIGHTDOWN]);
					setMoveState(TA_Base_Bus_GZL9::CameraCorbaDef::RIGHTDOWN);
				}
			} while (0);
			// if state change , send message
			if(isStateChanged())
			{
				resetStateChange();
				StateUpdateBroadcaster::getInstance().notifyCameraStateUpdate(m_cameraData->getKey(), true);
			}
	    }
		catch (const TA_Base_Core::DatabaseException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to send PTZ(Pan and Tilt) command." );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
		}
		catch (const TA_Base_Core::DataException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to send PTZ(Pan and Tilt) command." );
			LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
		}
		catch (const TA_Base_Core::VideoSwitchAgentInternalException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
			LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to send Pan and Tilt command." );
			LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException( e.what() );
		};
	}
} // namespace TA_IRS_App

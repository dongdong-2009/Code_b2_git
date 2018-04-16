#if !defined(IPDCamera_BDE6E42B_B294_4e78_BE77_3ECED963A0C1__INCLUDED_)
#define IPDCamera_BDE6E42B_B294_4e78_BE77_3ECED963A0C1__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/src/IPDCamera.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/08/13 14:55:34 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles all the protocol dependent common functions of Cameras.
  * This interface is used by the Camera class.
  */

#include <string>

#include "bus/cctv_gzl9/video_switch_agent/IDL/src/CameraCorbaDef.h"

namespace TA_IRS_App
{
    class IPDCamera
	{
	public:

		/**
		  * ~IPDCamera
		  *
		  * Standard destructor.
		  */

        virtual ~IPDCamera() {};
                  
        ///////////////////////////////////////////////////////////////////////
        //
        // State Update Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * updateState
          *
          * Updates the state of the Camera.  
          *
          * This is called when a VideoSwitchAgentStateUpdate message is received, 
          * so the agent should be in Monitor mode. Because this method is called 
          * when a message is received, there is not much we can do if the method 
          * fails, except log. 
          *
          * @param      const TA_Base_Bus_GZL9::CameraCorbaDef::CameraState& state
          *             The new state of the Camera.
          */

        virtual void updateState( const TA_Base_Bus_GZL9::CameraCorbaDef::CameraState& state ) = 0;
		
        ///////////////////////////////////////////////////////////////////////
		//
		// IPDCamera Methods
		//
		///////////////////////////////////////////////////////////////////////

        /**
          * isInAlarmState
          *
          * Identifies if the Camera is in an alarm state.
          * i.e. if the video signal from the Camera has been lost.
          *
          * @return     bool
          *             True if the Camera is in an alarm state.
          *             False otherwise.
          */

        virtual bool isInAlarmState() = 0;

		virtual TA_Base_Bus_GZL9::CameraCorbaDef::CameraState getCameraState() = 0;

		virtual void CameraPan( ::CORBA::UShort movement, ::CORBA::UShort speed)=0;   //movement: 0-stop,1-move left, 2-move right; speed: 1-low speed,2-high speed
		virtual void CameraTilt(::CORBA::UShort movement, ::CORBA::UShort speed)=0;   //movement: 0-stop,1-move up, 2-move down;speed:1-low speed,2-high speed
		virtual void CameraZoom( ::CORBA::UShort movement)=0;						  //movement: 0-stop,1-zoom in,2-zoom out
		virtual void CameraPanAndTilt( ::CORBA::UShort pan, ::CORBA::UShort panspeed, ::CORBA::UShort tilt, ::CORBA::UShort tiltspeed)=0;//pan: 0-stop,1-move left, 2-move right; panspeed: 1-low speed,2-high speed

	};

} // namespace TA_IRS_App

#endif // !defined(IPDCamera_BDE6E42B_B294_4e78_BE77_3ECED963A0C1__INCLUDED_)

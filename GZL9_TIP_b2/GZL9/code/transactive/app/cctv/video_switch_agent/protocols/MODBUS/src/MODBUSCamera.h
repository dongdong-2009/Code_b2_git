#if !defined(MODBUSCamera_BDE6E42B_B294_4e78_BE77_3ECED963A0C1__INCLUDED_)
#define MODBUSCamera_BDE6E42B_B294_4e78_BE77_3ECED963A0C1__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSCamera.h $
  * @author:   Rob Young
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/08/13 14:55:34 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the protocol dependent sections of the general functions for a Camera
  * using the MODBUS protocol.
  * Inherits from the IPDCamera class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Camera and
  * that the Camera and the protocol support the required features.
  */

#include <string>

#include "app/cctv/video_switch_agent/protocols/src/IPDCamera.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/CameraCorbaDef.h"
#include "core/data_access_interface/entity_access/src/Camera.h"

// Forward declarations.

namespace TA_IRS_App
{
    class MODBUSCamera : public IPDCamera
	{
	public:

   		/**
		  * MODBUSCamera
		  *
		  * Standard constructor.
          *
          * @param      Camera& cameraData
          *             The protocol independent Camera object.
		  */

        MODBUSCamera( TA_Base_Core::CameraPtr cameraData );

		/**
		  * ~MODBUSCamera
		  *
		  * Standard destructor.
		  */

        virtual ~MODBUSCamera() {};
                  
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

        virtual void updateState( const TA_Base_Bus_GZL9::CameraCorbaDef::CameraState& state );
		
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

        virtual bool isInAlarmState();


		virtual TA_Base_Bus_GZL9::CameraCorbaDef::CameraState getCameraState();

		virtual void CameraPan(::CORBA::UShort movement, ::CORBA::UShort speed);
		virtual void CameraTilt(::CORBA::UShort movement, ::CORBA::UShort speed);
		virtual void CameraZoom(::CORBA::UShort movement);
		virtual void CameraPanAndTilt(::CORBA::UShort pan, ::CORBA::UShort panspeed, ::CORBA::UShort tilt, ::CORBA::UShort tiltspeed);
        /**
          * notifyIsInAlarmState
          *
          * This is called when data is received from the hardware saying that
          * the video signal for a Camera has been lost or reestablished.  
          * This puts the Camera into/out of an alarm state.
          * Sends a message to notify agent clients that the video signal for 
          * a Camera has been lost or reestablished.
          *
          * @param      bool isInAlarmState
          *             True if the Camera is in an alarm state.
          *             False otherwise.
          */

        void notifyIsInAlarmState( bool isInAlarmState );

        unsigned long getKey();
		std::string getName();
		std::string getAddress();
		unsigned long getLocationKey();

    private:
        //
        // Disable copy constructor and assignment operator.
        //

        MODBUSCamera( const MODBUSCamera& theMODBUSCamera );
        MODBUSCamera& operator=( const MODBUSCamera& );

		bool isStateChanged()
		{
			return m_isStateChange; 
		}
		void resetStateChange()
		{
			m_isStateChange = false;
		}
		void setStateChange()
		{
			m_isStateChange = true;
		}
		void setPanSpeed(::CORBA::UShort speed)
		{
			m_currentPanSpeed = speed;
			setStateChange();
		}

		void setTiltSpeed(::CORBA::UShort speed)
		{
			m_currentTiltSpeed = speed;
			setStateChange();
		}

		void setMoveState(::TA_Base_Bus_GZL9::CameraCorbaDef::MoveState moveState)
		{
			m_moveState = moveState;
			setStateChange();
		}
        
        //
        // The configuration data object for this Camera.
        //

        TA_Base_Core::CameraPtr m_cameraData;

        //
        // Flag indicating if the video signal from the Camera has been lost.
        //

        bool m_isInAlarmState;

		::CORBA::UShort m_currentPanSpeed;
		::CORBA::UShort m_currentTiltSpeed;
		TA_Base_Bus_GZL9::CameraCorbaDef::MoveState m_moveState;
		bool m_isStateChange;

	};

} // namespace TA_IRS_App

#endif // !defined(MODBUSCamera_BDE6E42B_B294_4e78_BE77_3ECED963A0C1__INCLUDED_)

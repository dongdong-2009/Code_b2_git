#if !defined(COECamera_BDE6E42B_B294_4e78_BE77_3ECED963A0C1__INCLUDED_)
#define COECamera_BDE6E42B_B294_4e78_BE77_3ECED963A0C1__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Rob Young
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Handles the protocol dependent sections of the general functions for a Camera
  * using the COE protocol.
  * Inherits from the IPDCamera class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Camera and
  * that the Camera and the protocol support the required features.
  */

#include <string>

#include "app/cctv/video_switch_agent/protocols/src/IPDCamera.h"
#include "bus/cctv/video_switch_agent/IDL/src/CameraCorbaDef.h"
#include "core/data_access_interface/entity_access/src/Camera.h"
#include <boost/noncopyable.hpp>

// Forward declarations.

namespace TA_IRS_App
{
    class COECamera : public IPDCamera, boost::noncopyable
    {
        public:

            /**
              * COECamera
              *
              * Standard constructor.
              *
              * @param      Camera& cameraData
              *             The protocol independent Camera object.
              */

            COECamera(TA_Base_Core::CameraPtr cameraData);

            /**
              * ~COECamera
              *
              * Standard destructor.
              */

            virtual ~COECamera();

            ///////////////////////////////////////////////////////////////////////
            //
            // State Update Methods
            //
            ///////////////////////////////////////////////////////////////////////

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

            void notifyIsInAlarmState(bool isInAlarmState);

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
              * @param      const TA_Base_Bus::CameraCorbaDef::CameraState& state
              *             The new state of the Camera.
              */

            virtual void updateState(const TA_Base_Bus::CameraCorbaDef::CameraState& state);

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


            virtual unsigned long getKey();


            virtual std::pair< unsigned short, unsigned short > getParsedAddress();

            void ptzRequest(TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd cmd);

        private:
            //
            // The configuration data object for this Camera.
            //

            TA_Base_Core::CameraPtr m_cameraData;

    };

} // namespace TA_IRS_App

#endif // !defined(COECamera_BDE6E42B_B294_4e78_BE77_3ECED963A0C1__INCLUDED_)

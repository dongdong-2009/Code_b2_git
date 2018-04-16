#ifndef _ISWITCH_AGENT_SERVICE_H_FILE_
#define _ISWITCH_AGENT_SERVICE_H_FILE_

#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/VideoInput.h"
#include "bus/cctv/video_switch_agent/IDL/src/CameraCorbaDef.h"

namespace TA_IRS_App
{

    class ISwitchAgentService
    {

        public:
            ISwitchAgentService()
            {
            }
            virtual ~ISwitchAgentService()
            {
            }
            virtual void assignMonitor(Monitor* monitor,
                                       VideoInput* source) = 0;

            virtual  void assignQuad(Monitor* monitor,
                                     Camera* topLeftCam,
                                     Camera* topRightCam,
                                     Camera* bottomLeftCam,
                                     Camera* bottomRightCam) = 0;

            virtual void setSequenceConfig(Sequence* sequence,
                                           unsigned short dwellTime,
                                           const std::list< Camera* > cameraInputs) = 0;

            virtual void setSequenceDescription(Sequence* sequence, const std::string& newDescription) = 0;

            virtual void clearMonitor(Monitor* monitor) = 0;

            virtual  void updateSequence(Sequence* sequence) = 0;

            virtual void zoomCamera(Camera* camera, TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd zoomDirection) = 0;
            virtual void panCamera(Camera* camera, TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd panDirection) = 0;
            virtual void tiltCamera(Camera* camera, TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd tiltDirection) = 0;
            virtual void panAndTiltCamera(Camera* camera,
                                          TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd panDirection,
                                          TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd tiltDirection) = 0;
    };
}

#endif


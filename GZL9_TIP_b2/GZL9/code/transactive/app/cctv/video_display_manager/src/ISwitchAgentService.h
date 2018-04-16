#ifndef _ISWITCH_AGENT_SERVICE_H_FILE_
#define _ISWITCH_AGENT_SERVICE_H_FILE_

#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/Quad.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/VideoInput.h"

namespace TA_IRS_App
{

    class ISwitchAgentService
    {

        public:
            ISwitchAgentService( void ) {}

            /**
                   * assignMonitor
                   *
                   * Assigns an input to a monitor
                   *
                   * @param monitor  The monitor to assign to
                   * @param source   The source
                   *
                   * @exception  TA_Core::TransactiveException
                   *             If the assignment fails
                   *             the reason will be given so it can be formatted into an error messsage
                   */
            virtual void assignMonitor( Monitor* monitor,
                                        VideoInput* source ) = 0;

            /**
              * assignQuad
              *
              * Assigns cameras to a quad.
              *
              * @param quad             a pointer to the quad (not NULL)
              * @param topLeftCam       the new top left camera (or NULL to clear)
              * @param topRightCam      the new top right camera (or NULL to clear)
              * @param bottomLeftCam    the new bottom left camera (or NULL to clear)
              * @param bottomRightCam   the new bottom right camera (or NULL to clear)
              *
              * @exception  TA_Core::TransactiveException
              *             If the assignment fails
              *             the reason will be given so it can be formatted into an error messsage
              */
            virtual  void assignQuad( Quad* quad,
                                      Camera* topLeftCam,
                                      Camera* topRightCam,
                                      Camera* bottomLeftCam,
                                      Camera* bottomRightCam ) = 0;


            /**
              * setSequenceConfig
              *
              * Sets the dwell time and cameras for a sequence
              *
              * @param sequence     The sequence (not NULL)
              * @param dwellTime    the new dwelltime
              * @param cameraInputs the list of cameras (none NULL)
              *
              * @exception  TA_Core::TransactiveException
              *             If the assignment fails
              *             the reason will be given so it can be formatted into an error messsage
              */
            virtual void setSequenceConfig( Sequence* sequence,
                                            unsigned short dwellTime,
                                            const std::list< Camera* > cameraInputs,bool con ) = 0;

            virtual bool connectUserDefinedSeqToMonitor( Monitor* monitor,
                                                         Sequence* sequence ) = 0;

            virtual bool disconnectUserDefinedSeqToMonitor( Monitor* monitor ) = 0;
            /**
              * setSequenceDescription
              *
              * Sets the description for a sequence
              *
              * @param sequence         The sequence (not NULL)
              * @param newDescription   The new description
              *
              * @exception  TA_Core::TransactiveException
              *             If the assignment fails
              *             the reason will be given so it can be formatted into an error messsage
              */
            virtual void setSequenceDescription( Sequence* sequence,
                                                 const std::string& newDescription ) = 0;


            /**
              * clearMonitor
              *
              * clears the given monitor
              *
              * @param monitor  the monitor to clear (not NULL)
              *
              * @exception  TA_Core::TransactiveException
              *             If the assignment fails
              *             the reason will be given so it can be formatted into an error messsage
              */
            virtual void clearMonitor( Monitor* monitor ) = 0;





            /**
            * updateSequence
            *
            * This is called by the DisplayItemManager to update a sequences
            * details when it receives a config update.
            *
            * @param sequence The sequence to update (not NULL)
            */
            virtual  void updateSequence( Sequence* sequence ) = 0;

            /*
            * zoom Camera
              *
              * This is called by the switchmanagerPresenter  to zoom a camera
              *
              *
              * @param camera the camera object
              * @param in : zoom in ( 1) /zoom out (0)
            */
            virtual void zoomCamera ( Camera* camera, unsigned short zoomDirection ) =0;

            /*
             * pan Camera to the left or right
                   *
                   * This is called by the switchmanagerPresenter  to pan a camera
                   *
                   *
                   * @param camera the camera object
                   * @param left : left  ( 1) /right (0)
            */
            virtual void panCamera ( Camera* camera, unsigned short left, unsigned short speed ) =0 ;

            /*
             * tilt Camera to the up or down
                   *
                   * This is called by the switchmanagerPresenter  to tilt a camera
                   *
                   *
                   * @param camera the camera object
                   * @param left : u8p  ( 1) /down (0)
            */
            virtual void tiltCamera ( Camera* camera, unsigned short up,unsigned short speed )=0;

            /*
             * pan Camera  and tilt camera to up/down /left/right
                   *
                   * This is called by the switchmanagerPresenter  to pan and title a camera
                   *
                   *
                   * @param camera the camera object
                   * @param left : left  ( 1) /right (0)
                   * @param up : up (1) , down (0)
            */
            virtual void panAndTiltCamera ( Camera* camera, unsigned short left, unsigned short up,unsigned short panSpeed, unsigned short tiltSpeed )=0;


            /*
            stop the ptz command
            @ param Camera : the camera object
            */



        public:
            ~ISwitchAgentService( void ) {}
    };
}

#endif


#ifndef _ISWITCH_AGENT_SERVICE_H_FILE_
#define _ISWITCH_AGENT_SERVICE_H_FILE_
#pragma once


    #include "app/trains/TrainBorne_DisplayManager/DisplayItem/Camera.h"
    #include "app/trains/TrainBorne_DisplayManager/DisplayItem/Quad.h"
    #include "app/trains/TrainBorne_DisplayManager/DisplayItem/Sequence.h"
    #include "app/trains/TrainBorne_DisplayManager/DisplayItem/Monitor.h"
    #include "app/trains/TrainBorne_DisplayManager/DisplayItem/VideoInput.h"

namespace TA_IRS_App 
{

class ISwitchAgentService
{
public:
	ISwitchAgentService(void){} 
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
   //    virtual void assignMonitor( Monitor* monitor,
    //                        VideoInput* source ) = 0;
	   virtual void assignMonitor( std::string monitorName,
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

		virtual bool connectUserDefinedSeqToMonitor(Monitor* monitor,
								Sequence* sequence ) = 0;

		virtual bool disconnectUserDefinedSeqToMonitor(Monitor* monitor) = 0;
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
        virtual bool clearMonitor( Monitor* monitor , VideoInput * input ) = 0;


        virtual bool clearMonitor(std::string monitorName,VideoInput * input ) = 0;


          /**
          * updateSequence
          *
          * This is called by the DisplayItemManager to update a sequences
          * details when it receives a config update.
          *
          * @param sequence The sequence to update (not NULL)
          */
       virtual  void updateSequence(Sequence* sequence) = 0;



        /*
		  stop the ptz command
		  @ param Camera : the camera object
		*/
		


public:
	~ISwitchAgentService(void){} 
};
}
#endif


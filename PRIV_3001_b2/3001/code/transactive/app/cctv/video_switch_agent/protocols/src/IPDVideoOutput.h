#if !defined(IPDVideoOutput_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
#define IPDVideoOutput_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/src/IPDVideoOutput.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Handles the protocol dependent sections of the operations for a VideoOutput.  
  * This interface is used by the VideoOutput class. Before the methods
  * in this interface are called, the user privileges have been checked and
  * that the protocol supports the required features.
  */

#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputCorbaDef.h"

namespace TA_IRS_App
{
    class IPDVideoOutput
    {
    public:

        /**
          * ~IPDVideoOutput
          *
          * Standard destructor.
          */

        virtual ~IPDVideoOutput() {};

        ///////////////////////////////////////////////////////////////////////
        //
        // State Update Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * updateState
          *
          * Updates the state of the VideoOutput.  
          *
          * This is called when a VideoSwitchAgentStateUpdate message is received, 
          * so the agent should be in Monitor mode. Because this method is called 
          * when a message is received, there is not much we can do if the method 
          * fails, except log. 
          *
          * @param      const TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState& state
          *             The new state of the VideoOutput.
          */

        virtual void updateState( const TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState& state ) = 0;
      
        ///////////////////////////////////////////////////////////////////////
        //
        // IPDVideoOutput Methods to be overridden
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * getCurrentVideoInputKey
          *
          * Get the database (entity) key of the VideoInput currently switched to this VideoOutput.  
          * 0 indicates no VideoInput is siwtched to this VideoOutput.
          *
          * @return     unsigned long
          *             The database (entity) key of the VideoInput for this VideoOutput device or
          *             0 if there is no associated input.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual unsigned long getCurrentVideoInputKey() = 0;

        
		/**
          * getVideoOutputState
          *
          * returns the entire state of this video output
          *
          * @return     VideoOutputState objhect asscociated with this videoOutput
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState* getVideoOutputState() = 0;

		virtual bool isQuadInput() = 0;
		virtual TA_Base_Bus::VideoOutputCorbaDef::QuadCameras getQuadCameras() = 0;
		/**
          * requestSwitchOfInput
          *
          * Requests that the hardware switches the named VideoInput to this VideoOutput.
          *
          * @param      unsigned long videoInputEntityKey
          *             The entity key of the VideoInput to switch.
          * @param      bool isDemand = false
          *             If false => If the VideoInput is locked (hardware locking), the 
          *                         switch will not occur. Also known as a polite request.
          *             If true  => The switch will occur even if the hardware is locked.
          * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. A polite request was made and the VideoInput is switch (hardware) locked.
          *             2. The VideoInput key is not recognised.
          *             3. There is a communications failure with the hardware.
          */

        virtual void requestSwitchOfInput( unsigned long videoInputEntityKey, 
                                           bool isDemand ) = 0;
		virtual void assignQuad( unsigned long cameraW,unsigned long cameraX,unsigned long cameraY,unsigned long cameraZ, 
			bool isDemand ) = 0;
       /**
          * mappingTrainBorneMonitor
          *
          *Inform VSS this monitor will be mapping as TrainBone monitor
          *
          * @return     boolean
          *             true if success
          *             otherwise false
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. There is a communications failure with the hardware.
          */

        virtual bool mappingTrainBorneMonitor(bool isDemand)=0;

    }; // IPDVideoOutput

} // TA_IRS_App
    
#endif // !defined(IPDVideoOutput_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)

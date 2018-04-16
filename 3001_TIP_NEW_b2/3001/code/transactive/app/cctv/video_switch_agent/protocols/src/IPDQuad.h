#ifndef IPDQUAD_H
#define IPDQUAD_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/src/IPDQuad.h $
 * @author:  Jade Welton
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 * Handles the protocol dependent sections of the operations for a Quad.
 *
 */

#include "bus/cctv/video_switch_agent/IDL/src/QuadCorbaDef.h"

namespace TA_IRS_App
{
	class IPDQuad
	{

	public:

		virtual ~IPDQuad() {};


         /**
          * getQuadState
          *
          * Gets the current state of this Quad.
          * Inherited from QuadCorbaDef.
          *
          * @return     QuadState
          *             The current state of this quad
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The database cannot be accessed.
          *             2. The data cannot be retrieved.
          *             3. The retrieved data is invalid.
          *             4. The agent is in Monitor mode.
          */
        virtual TA_Base_Bus::QuadCorbaDef::QuadState getQuadState() = 0;


        /**
          * areInputsSwitchable
          *
          * Indicates whether the inputs to this quad be switched.
          * Inherited from QuadCorbaDef.
          *
          * @return     boolean
          *             True if the inputs can be switched,
          *             False otherwise.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if the agent is in Monitor mode.
          */
        virtual bool areInputsSwitchable() = 0;


        /**
          * requestSwitchOfInput
          *
          * Requests switch of input for one of the Quad segments.
          * Inherited from QuadCorbaDef.
          *
          * @param      enum EQuadSegment
          *             Display segment corresponding to channel that
          *             is to be switched.
          * @param      unsigned long videoInputEntityKey
          *             The entity key of the VideoInput to switch.
          * @param      boolean isDemand = false
          *             If false => If the VideoInput is locked (hardware locking), the
          *                         switch will not occur. Also known as a polite request.
          *             If true  => The switch will occur even if the hardware is locked.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The VideoInput or the protocol does not support this operation.
          *             2. A polite request was made and the VideoInput is switch (hardware) locked.
          *             3. The VideoInput key is not recognised.
          *             4. The agent is in Monitor mode.
          *             5. The user does not have sufficient privileges for this operation.
          *             6. There is a communications failure with the hardware.
          */
        virtual void requestSwitchOfInput( TA_Base_Bus::QuadCorbaDef::EQuadSegment segment, unsigned long videoInputEntityKey, bool isDemand ) = 0;


		 /**
          * requestSwitchOfInputs
          *
          * Requests switch of inputs for the four segments.
          * Inherited from QuadCorbaDef.
          *
          * @param      unsigned long topLeftVideoInputEntityKey
          *             The entity key of the VideoInput to switch
          *             to channel for top-left quad segment.
          * @param      unsigned long topRightVideoInputEntityKey
          *             The entity key of the VideoInput to switch
          *             to channel for top-right quad segment.
          * @param      unsigned long bottomLeftVideoInputEntityKey
          *             The entity key of the VideoInput to switch
          *             to channel for bottom-left quad segment.
          * @param      unsigned long bottomRightVideoInputEntityKey
          *             to channel for bottom-right quad segment.
          * @param      boolean isDemand
          *             If false => If the VideoInput is locked (hardware locking), the
          *                         switch will not occur. Also known as a polite request.
          *             If true  => The switch will occur even if the hardware is locked.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The VideoInput or the protocol does not support this operation.
          *             2. A polite request was made and the VideoInput is switch (hardware) locked.
          *             3. The VideoInput key is not recognised.
          *             4. The agent is in Monitor mode.
          *             5. The user does not have sufficient privileges for this operation.
          *             6. There is a communications failure with the hardware.
          */
        virtual void requestSwitchOfInputs( unsigned long topLeftVideoInputEntityKey, unsigned long topRightVideoInputEntityKey,
                                            unsigned long bottomLeftVideoInputEntityKey, unsigned long bottomRightVideoInputEntityKey,
                                            bool isDemand ) = 0;


        /**
          * updateState
          *
          * Updates the state of the Quad.  
          *
          * This is called when a VideoSwitchAgentStateUpdate message is received, 
          * so the agent should be in Monitor mode. Because this method is called 
          * when a message is received, there is not much we can do if the method 
          * fails, except log. 
          *
          * @param      const TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState& state
          *             The new state of the VideoOutput.
          */
        virtual void updateState( const TA_Base_Bus::QuadCorbaDef::QuadState& state ) = 0;

	};
}

#endif // IPDQUAD_H

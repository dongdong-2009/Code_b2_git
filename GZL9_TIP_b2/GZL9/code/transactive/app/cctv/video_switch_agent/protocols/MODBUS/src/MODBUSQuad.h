#ifndef MODBUSQUAD_H
#define MODBUSQUAD_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSQuad.h $
  * @author:   Rob Young
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/08/21 08:52:08 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the protocol dependent sections of the general functions for a Quad
  * using the MODBUS protocol.
  * Inherits from the IPDQuad class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Quad and
  * that the Quad and the protocol support the required features.
  */

#include <string>
#include <memory>

#include "app/cctv/video_switch_agent/protocols/src/IPDQuad.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/IStateValidationItem.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/QuadCorbaDef.h"

#include "core/alarm/src/AlarmHelper.h"
#include "core/data_access_interface/entity_access/src/Quad.h"

namespace TA_IRS_App
{
    class MODBUSQuad : public IPDQuad//, public virtual IStateValidationItem
    {
    public:

        /**
          * MODBUSQuad
          *
          * Standard constructor.
          *
          * @param      Quad& QuadData
          *             The protocol independent Quad object.
          *             This object is owned by GenericAgent, so it should not be deleted.
          */

        MODBUSQuad( TA_Base_Core::QuadPtr QuadData );

        /**
          * ~MODBUSQuad
          *
          * Standard destructor.
          */

        virtual ~MODBUSQuad() {};

        virtual unsigned long getKey();

        virtual std::string getAddress();

        ///////////////////////////////////////////////////////////////////////
        //
        // IPDQuad Methods
        //
        ///////////////////////////////////////////////////////////////////////

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
        virtual TA_Base_Bus_GZL9::QuadCorbaDef::QuadState getQuadState();


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
        virtual bool areInputsSwitchable();


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
        virtual void requestSwitchOfInput( TA_Base_Bus_GZL9::QuadCorbaDef::EQuadSegment segment, unsigned long videoInputEntityKey, bool isDemand );


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
                                            bool isDemand );


        /**
          * notifyInputsSwitchedToQuad
          *
          * This is called when data is received from the hardware saying that the quads
          * inputs have been switched, so the agent should be in Control mode.
          * Sends a message to notify agent clients that this quads inputs have
          * been switched.
          *
          * @param      unsigned long topLeftVideoInputEntityKey
          *             The entity key now assigned to channel for top-left quad segment.
          * @param      unsigned long topRightVideoInputEntityKey
          *             The entity key now assigned to channel for top-right quad segment.
          * @param      unsigned long bottomLeftVideoInputEntityKey
          *             The entity key now assigned to channel for bottom-left quad segment.
          * @param      unsigned long bottomRightVideoInputEntityKey
          *             The entity key now assigned to channel for bottom-right quad segment.
          */
        void notifyInputsSwitchedToQuad( unsigned long topLeftVideoInputEntityKey, unsigned long topRightVideoInputEntityKey,
                                            unsigned long bottomLeftVideoInputEntityKey, unsigned long bottomRightVideoInputEntityKey  );


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
          * @param      const TA_Base_Bus_GZL9::QuadCorbaDef::QuadState& state
          *             The new state of the Quad.
          */
        void updateState( const TA_Base_Bus_GZL9::QuadCorbaDef::QuadState& state );


        ///////////////////////////////////////////////////////////////////////
        //
        // IStateValidationItem Methods
        //
        ///////////////////////////////////////////////////////////////////////


        /** 
          * validateState
          *
          * Callback for StateValidationCycle to let this item know its time
          * to validate its state.
          */
        virtual void validateState();

    private:
        //
        // Disable copy constructor and assignment operator.
        //

        MODBUSQuad( const MODBUSQuad& theMODBUSQuad );
        MODBUSQuad& operator=( const MODBUSQuad& );
        
        //
        // The configuration data object for this Quad.
        //

        TA_Base_Core::QuadPtr m_QuadData;

        TA_Base_Bus_GZL9::QuadCorbaDef::QuadState m_quadState;    // The current state of this Quad.

    };

} // namespace TA_IRS_App

#endif // MODBUSQUAD_H

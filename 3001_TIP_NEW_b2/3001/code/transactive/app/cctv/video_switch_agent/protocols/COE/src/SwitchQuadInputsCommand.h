#ifndef SWITCH_QUAD_INPUTS_COMMAND_H
#define SWITCH_QUAD_INPUTS_COMMAND_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/SwitchQuadInputsCommand.h $
  * @author:  Robert Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Object for requesting inputs be switched to a quad and for
  * processing the response.
  *
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h"

// Forward declarations.

namespace TA_IRS_App
{
    class COEVideoOutput;
}

namespace TA_IRS_App
{
    class SwitchQuadInputsCommand : public AbstractCOERequestCommand
    {
    public:

        /**
          * SwitchQuadInputsCommand
          *
          * Constructs the command to send to the switching hardware regarding
          * the switch of an input to an output.
          *
          * @param      unsigned long videoInputEntityKey
          *             The entity key of the VideoInput to switch.
          * @param      const std::string& videoInputAddress
          *             The address of the VideoInput to switch.
          * @param      COEVideoOutput& output
          *             The VideoOutput to switch to.
          *
          * @exception  DatabaseException
          *             Thrown if the database cannot be contacted.
          * @exception  DataException
          *             Thrown if the data cannot be retrieved or is invalid.
          */
        SwitchQuadInputsCommand( unsigned long topLeftVideoInput, unsigned long topRightVideoInput, unsigned long bottomLeftVideoInput, unsigned long bottomRightVideoInput,  COEVideoOutput& monitor );
       
        
        /**
          * ~SwitchQuadInputsCommand
          *
          * Standard destructor.
          */
        virtual ~SwitchQuadInputsCommand() {};


        /**
          * processResponse
          *
          * Processes the response to this command that has been received from
          * the switching hardware.
          *
          * @return     bool
          *             True if the response was valid.
          *             False otherwise.
          *
          * @param      DataVector& data
          *             The COE to process.  This will have the checksum removed.
          */
        virtual bool processResponse( const std::vector< unsigned char >& data );

    private:

        unsigned char m_expectedResponseCmd;

        //
        // Disable copy constructor and assignment operator.
        //

        SwitchQuadInputsCommand( const SwitchQuadInputsCommand& theSwitchQuadInputsCommand );
        SwitchQuadInputsCommand& operator=( const SwitchQuadInputsCommand& );

        //
        // The key of the VideoInput object and the VideoOutput object that will be switched.
        //

        unsigned long m_topLeftVideoInput;
        unsigned long m_topRightVideoInput;
        unsigned long m_bottomLeftVideoInput;
        unsigned long m_bottomRightVideoInput;
        COEVideoOutput& m_monitor;
    };
}

#endif // SWITCH_QUAD_INPUTS_COMMAND_H

#ifndef SWITCH_INPUT_TO_OUTPUT_COMMAND_H
#define SWITCH_INPUT_TO_OUTPUT_COMMAND_H

/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/SwitchInputToOutputCommand.h $
 * @author:  Rob Young
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * Object for requesting an input be switched to an output and for
 * processing the response.
 */

#include "app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h"

// Forward declarations.

namespace TA_IRS_App
{
    class COEVideoOutput;
}

namespace TA_IRS_App
{
    class SwitchInputToOutputCommand : public AbstractCOERequestCommand
    {
    public:
        /**
          * SwitchInputToOutputCommand
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
        SwitchInputToOutputCommand( unsigned long videoInputEntityKey, COEVideoOutput& output );


        /**
          * ~SwitchInputToOutputCommand
          *
          * Standard destructor.
          */
        virtual ~SwitchInputToOutputCommand() {};


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

		void buildCommand();

    private:

        void buildCamAndMonCmd();
     //   void buildQuadAndMonCmd();
     //   void buildBVSStageAndMonCmd();
        void buildSeqAndMonCmd();
        void buildClearMonCmd();

        unsigned char m_expectedResponseCmd;

        //
        // Disable copy constructor and assignment operator.
        //

        SwitchInputToOutputCommand( const SwitchInputToOutputCommand& theSwitchInputToOutputCommand );
        SwitchInputToOutputCommand& operator=( const SwitchInputToOutputCommand& );

        //
        // The key of the VideoInput object and the VideoOutput object that will be switched.
        //

        unsigned long m_videoInputEntityKey;
        COEVideoOutput& m_videoOutput;
    };
}

#endif // SWITCH_INPUT_TO_OUTPUT_COMMAND_H

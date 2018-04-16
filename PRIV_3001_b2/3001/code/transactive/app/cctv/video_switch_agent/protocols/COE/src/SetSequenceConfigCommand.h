#ifndef SET_SEQUENCE_CONFIG_COMMAND_H
#define SET_SEQUENCE_CONFIG_COMMAND_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/SetSequenceConfigCommand.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Object for requesting new sequence configuration and processing the response.
  *
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h"

// Forward declarations.

namespace TA_IRS_App
{
    class COESequence;
}

namespace TA_IRS_App
{
    class SetSequenceConfigCommand : public AbstractCOERequestCommand
    {
    public:

        /**
          * SetSequenceConfigCommand
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
        SetSequenceConfigCommand( unsigned short dwellTime, const std::vector< unsigned long >& videoInputs, COESequence& sequence );


        /**
          * ~SetSequenceConfigCommand
          *
          * Standard destructor.
          */
        virtual ~SetSequenceConfigCommand() {};


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

        //
        // Disable copy constructor and assignment operator.
        //

        SetSequenceConfigCommand( const SetSequenceConfigCommand& theSetSequenceConfigCommand );
        SetSequenceConfigCommand& operator=( const SetSequenceConfigCommand& );

        //
        // The key of the VideoInput object and the VideoOutput object that will be switched.
        //
        
        unsigned char m_expectedResponseCmd;
        unsigned short m_dwellTime;
        std::vector< unsigned long > m_videoInputs;
        COESequence& m_sequence;
    };
}

#endif // SET_SEQUENCE_CONFIG_COMMAND_H

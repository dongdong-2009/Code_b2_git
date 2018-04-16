#ifndef MAPPING_TRAINBORNE_COMMAND_H
#define MAPPING_TRAINBORNE_COMMAND_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/SwitchQuadInputsCommand.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
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
    class MappingTrainBorneMonitorCommand : public AbstractCOERequestCommand
    {
    public:

        /**
          * MappingTrainBorneMonitorCommand
          *
          * Constructs the command to send to the switching hardware regarding
          * the switch of an input to an output.
          *
          * @param      COEVideoOutput& output
          *             The VideoOutput to switch to.
          *
          * @exception  DatabaseException
          *             Thrown if the database cannot be contacted.
          * @exception  DataException
          *             Thrown if the data cannot be retrieved or is invalid.
          */
        MappingTrainBorneMonitorCommand(  COEVideoOutput& monitor);
       
        
        /**
          * ~SwitchQuadInputsCommand
          *
          * Standard destructor.
          */
        virtual ~MappingTrainBorneMonitorCommand() {};


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

        MappingTrainBorneMonitorCommand( const MappingTrainBorneMonitorCommand& theMappingTrainBorneMonitorCommand );
        MappingTrainBorneMonitorCommand& operator=( const MappingTrainBorneMonitorCommand& );

        //
        // The key of the VideoInput object and the VideoOutput object that will be switched.
        //
        COEVideoOutput& m_monitor;
    };
}

#endif // MAPPING_TRAINBORNE_COMMAND_H

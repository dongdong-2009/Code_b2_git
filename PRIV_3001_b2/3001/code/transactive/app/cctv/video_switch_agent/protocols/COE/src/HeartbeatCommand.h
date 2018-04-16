#ifndef HEARTBEAT_COMMAND_H
#define HEARTBEAT_COMMAND_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/HeartbeatCommand.h $
  * @author:  Zhang Hongzhi
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/12/18 16:15:14 $
  * Last modified by:  $Author: hongzhi.zhang $
  *
  * Object for heartbeat to VSS
  * processing the response.
  *
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h"

// Forward declarations.

namespace TA_IRS_App
{
    class COECommunicationsHandler;
}

namespace TA_IRS_App
{
    class HeartbeatCommand : public AbstractCOERequestCommand
    {
    public:

        /** 
          * HeartbeatCommand
          *
          * description
          *
          * @param communicationsChannelIndex
          * @param 
          *
          */
        HeartbeatCommand( unsigned int communicationsChannelIndex, COECommunicationsHandler& communicationsHandler );
       
        
        /**
          * ~HeartbeatCommand
          *
          * Standard destructor.
          */
        virtual ~HeartbeatCommand() {};


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

        HeartbeatCommand( const HeartbeatCommand& theHeartbeatCommand );
        HeartbeatCommand& operator=( const HeartbeatCommand& );

        //
        // Private Members
        //

        unsigned int m_communicationsChannelIndex;
        COECommunicationsHandler& m_communicationsHandler;

        static const unsigned short NUM_HEARTBEAT_COMMAND_DATA_BYTES;
    };
}

#endif // SUMMARY_POLLING_COMMAND_H

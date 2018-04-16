#ifndef SUMMARY_POLLING_COMMAND_H
#define SUMMARY_POLLING_COMMAND_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/SummaryPollingCommand.h $
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
    class COECommunicationsHandler;
}

namespace TA_IRS_App
{
    class SummaryPollingCommand : public AbstractCOERequestCommand
    {
    public:

        /** 
          * SummaryPollingCommand
          *
          * description
          *
          * @param communicationsChannelIndex
          * @param 
          *
          */
        SummaryPollingCommand( unsigned int communicationsChannelIndex, COECommunicationsHandler& communicationsHandler );
       
        
        /**
          * ~SummaryPollingCommand
          *
          * Standard destructor.
          */
        virtual ~SummaryPollingCommand() {};


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

        SummaryPollingCommand( const SummaryPollingCommand& theSummaryPollingCommand );
        SummaryPollingCommand& operator=( const SummaryPollingCommand& );

        //
        // Helper Functions
        //

        /** 
          * processSummaryPollingData
          *
          * description
          *
          * @param 
          */
        void processSummaryPollingData( unsigned char summaryPollingData );

        //
        // Private Members
        //

        unsigned int m_communicationsChannelIndex;
        COECommunicationsHandler& m_communicationsHandler;

        static const unsigned short NUM_POLLING_COMMAND_DATA_BYTES;
        static const unsigned char SWITCH_ACTIVE_IDLE_OK_BITMASK;
        static const unsigned char SWITCH_ACTIVE_IDLE_FAILED_BITMASK;
        static const unsigned char SWITCH_IDLE_BITMASK;
		//19444
		static const unsigned char DVR_SUMMARY_ALARM_BITMASK;	
		//19444
        static const unsigned char ANCILLARY_SUMMARY_ALARM_BITMASK;
        static const unsigned char CAMERA_SUMMARY_ALARM_BITMASK;
        static const unsigned char TRANSMISSION_SUMMARY_ALARM_BITMASK;
        static const unsigned char SYSTEM_SUMMARY_ALARM_BITMASK;
    };
}

#endif // SUMMARY_POLLING_COMMAND_H

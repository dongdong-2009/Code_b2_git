#if !defined RECEIVED_DATA_PROCESSOR_H
#define RECEIVED_DATA_PROCESSOR_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/ReceivedDataProcessor.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include <map>
#include <vector>

#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"

namespace TA_IRS_App
{
    class IHardwareNotificationCommand;
}

namespace TA_IRS_App
{
    class ReceivedDataProcessor
    {
    public:

        /** 
          * ReceivedDataProcessor
          *
          * description
          *
          */
        ReceivedDataProcessor();


        /** 
          * ~ReceivedDataProcessor
          *
          * description
          *
          */
        virtual ~ReceivedDataProcessor() {};

        
        /** 
          * validatePacketHeader
          *
          * description
          *
          * @param packetLength
          * @param commandByte
          * @param 
          *
          * @return 
          *
          */
        bool validatePacketHeader( unsigned int packetLength, unsigned char command, unsigned char error );

        
        /** 
          * createHardwareNotificationCommand
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        IHardwareNotificationCommand* createHardwareNotificationCommand( const std::vector< unsigned char >& notificationData );

    private:
        
        //
        // private types
        //
        struct ResponsePacketLengthRange
        {
            unsigned int minResponsePacketLength;
            unsigned int maxResponsePacketLength;
        };
        typedef std::map< unsigned char, ResponsePacketLengthRange > CommandToLengthRange;
        typedef std::map< unsigned char, TA_COE::HardwareNotificationCommandCreator > CommandToNotificationCommandCreator;

        //
        // Disable copy constructor and assignment operator.
        //

        ReceivedDataProcessor( const ReceivedDataProcessor& theReceivedDataProcessor );
        ReceivedDataProcessor& operator=( const ReceivedDataProcessor& theReceivedDataProcessor );

        //
        // private members
        //
        
        CommandToLengthRange m_commandToLengthRange;
        CommandToNotificationCommandCreator m_commandToNotificationCommandCreator;
    };

} // TA_IRS_App

#endif // RECEIVED_DATA_PROCESSOR_H

#if !defined RECEIVED_DATA_PROCESSOR_H
#define RECEIVED_DATA_PROCESSOR_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/ReceivedDataProcessor.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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

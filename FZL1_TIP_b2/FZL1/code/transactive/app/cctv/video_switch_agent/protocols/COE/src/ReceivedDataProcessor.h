#if !defined RECEIVED_DATA_PROCESSOR_H
#define RECEIVED_DATA_PROCESSOR_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/WXL1_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/COE/src/ReceivedDataProcessor.h $
  * @author:  Robert Young
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/07/30 17:42:30 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */

#include <map>
#include <vector>
#include <boost/noncopyable.hpp>
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"

namespace TA_IRS_App
{
    class ReceivedDataProcessor : boost::noncopyable
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
            virtual ~ReceivedDataProcessor();


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
            bool validatePacketHeader(unsigned int packetLength, unsigned char command, unsigned char error);


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

            //
            // private members
            //

            CommandToLengthRange m_commandToLengthRange;
    };

} // TA_IRS_App

#endif // RECEIVED_DATA_PROCESSOR_H

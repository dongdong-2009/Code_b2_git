/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Robert Young
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/ReceivedDataProcessor.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    //
    // Constructor
    //
    ReceivedDataProcessor::ReceivedDataProcessor()
    {
        FUNCTION_ENTRY("Constructor");
        const TA_COE::ReceivedDataProcessingInfo* processingInfo = &TA_COE::RECEIVED_DATA_PROCESSING_INFO_ARRAY[ 0 ];
        for(int i = 0; i < TA_COE::RECEIVED_DATA_PROCESSING_INFO_ARRAY_SIZE; ++i)
        {
            ResponsePacketLengthRange range = { (*processingInfo).minResponsePacketLength, (*processingInfo).maxResponsePacketLength };
            m_commandToLengthRange[(*processingInfo).command ] = range;
            processingInfo++;
        }
        FUNCTION_EXIT;
    }


    ReceivedDataProcessor::~ReceivedDataProcessor()
    {
    }
    //
    // validatePacketHeader
    //
    bool ReceivedDataProcessor::validatePacketHeader(unsigned int packetLength, unsigned char command, unsigned char error)
    {
        CommandToLengthRange::iterator it = m_commandToLengthRange.find(command);
        if(it != m_commandToLengthRange.end())
        {
            //
            // Check the Error byte, validating length against acceptable range according to error code.
            //
            unsigned int minAcceptableLength = (*it).second.minResponsePacketLength;
            unsigned int maxAcceptableLength = (*it).second.maxResponsePacketLength;
            switch(error)
            {
                case TA_COE::OK:
                    return ((packetLength >= minAcceptableLength) && (packetLength <= maxAcceptableLength)) ;
                case TA_COE::NOT_IMPLEMENTED:
                    return (packetLength <= maxAcceptableLength);
                case TA_COE::PROHIBITED:
                    return (packetLength <= maxAcceptableLength);
                case TA_COE::BAD_LENGTH:
                    return (packetLength <= maxAcceptableLength);
                case TA_COE::BAD_COMMAND:
                    return (packetLength <= maxAcceptableLength);
                case TA_COE::BUSY:
                    return (packetLength <= maxAcceptableLength);
                default:
                    break;
            }
        }
        return false;
    }
} // TA_IRS_App


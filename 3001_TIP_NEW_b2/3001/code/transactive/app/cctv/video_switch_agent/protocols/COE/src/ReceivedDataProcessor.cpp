/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/ReceivedDataProcessor.cpp $
  * @author:  Robert Young
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/04/18 14:09:13 $
  * Last modified by:  $Author: huang.wenbo $
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
        FUNCTION_ENTRY( "Constructor" );

        const TA_COE::ReceivedDataProcessingInfo* processingInfo = &TA_COE::RECEIVED_DATA_PROCESSING_INFO_ARRAY[ 0 ];
        unsigned int numIterations( 0 );
        for ( ; numIterations < TA_COE::RECEIVED_DATA_PROCESSING_INFO_ARRAY_SIZE; numIterations++ )
        {
            ResponsePacketLengthRange range = { ( *processingInfo ).minResponsePacketLength, ( *processingInfo ).maxResponsePacketLength };
            m_commandToLengthRange[ ( *processingInfo ).command ] = range;
            if ( 0 != ( *processingInfo ).creator )
            {
                m_commandToNotificationCommandCreator[ ( *processingInfo ).command ] = ( *processingInfo ).creator;
            }
            processingInfo++;
        }
        FUNCTION_EXIT;
    }


    //
    // validatePacketHeader
    //
    bool ReceivedDataProcessor::validatePacketHeader( unsigned int packetLength, unsigned char command, unsigned char error )
    {
        FUNCTION_ENTRY( "validatePacketHeader" );

        CommandToLengthRange::iterator commandToRangeIt = m_commandToLengthRange.find( command );
        if ( commandToRangeIt != m_commandToLengthRange.end() )
        {
            //
            // Check the Error byte, validating length against acceptable range according to error code.
            //
            unsigned int minAcceptableLength = ( *commandToRangeIt ).second.minResponsePacketLength;
            unsigned int maxAcceptableLength = ( *commandToRangeIt ).second.maxResponsePacketLength;

            FUNCTION_EXIT; // all paths of switch return.
            switch ( error )
            {
            case TA_COE::OK:
                return ( ( packetLength >= minAcceptableLength ) && ( packetLength <= maxAcceptableLength ) ) ;
            case TA_COE::NOT_IMPLEMENTED:
                return ( packetLength <= maxAcceptableLength );
            case TA_COE::PROHIBITED:
                return ( packetLength <= maxAcceptableLength );
            case TA_COE::BAD_LENGTH:
                return ( packetLength <= maxAcceptableLength );
            case TA_COE::BAD_COMMAND:
                return ( packetLength <= maxAcceptableLength );
            case TA_COE::BUSY:
                return ( packetLength <= maxAcceptableLength );
            default:
                return false;
            }
        }

        FUNCTION_EXIT;
        return false;
    }


    //
    // createHardwareNotificationCommand
    //
    IHardwareNotificationCommand* ReceivedDataProcessor::createHardwareNotificationCommand( const std::vector< unsigned char >& notificationData )
    {
        if ( TA_COE::COMMAND_BYTE < notificationData.size() )
        {
            CommandToNotificationCommandCreator::iterator commandToCreatorIt = m_commandToNotificationCommandCreator.find( notificationData[ TA_COE::COMMAND_BYTE ] );
            if ( m_commandToNotificationCommandCreator.end() != commandToCreatorIt )
            {
                return ( *commandToCreatorIt ).second( notificationData );
            }
        }

        // else no corresponding HardwareNotificationCommand to be created.
        return 0;
    }

} // TA_IRS_App


/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/ReceivedDataProcessor.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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

        // Place in try/catch so we can log debug error if COEHelper constants 
        // inaccurate and get unexpected exception. 
        // (i.e TA_COE::RECEIVED_DATA_PROCESSING_INFO_ARRAY_SIZE != actual fixed C-Array length)
        try 
        {
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
        }
        catch ( ... )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught unknown exception trying to populate ReceivedDataProcessor maps from static C Array" );
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


/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/RecordingUnitStatusCommand.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/RecordingUnitStatusCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COERecordingUnit.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <sstream>

namespace TA_IRS_App
{
    const unsigned short RecordingUnitStatusCommand::NUM_RECORDING_UNIT_STATUS_REQUEST_DATA_BYTES = 1;
    const unsigned short RecordingUnitStatusCommand::RECORDING_UNIT_ID_INDEX = TA_COE::FIRST_DATA_BYTE;
    const unsigned short RecordingUnitStatusCommand::RECORDING_UNIT_STATUS_BYTE_INDEX = TA_COE::FIRST_DATA_BYTE + 1;
    const unsigned char RecordingUnitStatusCommand::NOT_RECORDING_CODE = 0x01;
    const unsigned char RecordingUnitStatusCommand::REAL_TIME_RECORDING_CODE = 0x02;
    const unsigned char RecordingUnitStatusCommand::TIME_LAPSE_RECORDING_CODE = 0x03;

    //
    // Const strings for logging info
    //
    const char* RecordingUnitStatusCommand::NOTIFICATION_FOR_UNKNOWN_RECORDING_UNIT = "Hardware reported Status for unconfigured RecordingUnit Id: %d.";
    const char* RecordingUnitStatusCommand::INVALID_RECORDING_UNIT_STATUS           = "Hardware reported RecordingUnit Status not defined by COE protocol: %d.";
    const char* RecordingUnitStatusCommand::BAD_NOTIFICATION_DATA                   = "Asynchronous RecordingUnit Status Notification Packet has error.";
    const char* RecordingUnitStatusCommand::INSUFFICENT_LENGTH                      = "Received packet is of insufficient length to specify Recording Unit Status.";


    //
    // RecordingUnitStatusCommand (INotificationCommand intended constructor)
    //
    RecordingUnitStatusCommand::RecordingUnitStatusCommand( const std::vector< unsigned char >& notificationData )
        : AbstractCOERequestCommand(), m_recordingUnit( 0 ), m_recordingUnitId( 0 ), m_notificationData( notificationData )
    {
        //
        // Do nothing else until processNotification() is called.
        //
    }


    //
    // RecordingUnitStatusCommand (IAgentRequestCommand intended constructor)
    //
    RecordingUnitStatusCommand::RecordingUnitStatusCommand( COERecordingUnit& recordingUnit )
        : AbstractCOERequestCommand(), m_recordingUnit( &recordingUnit), m_recordingUnitId( 0 )
    {
        m_data.erase( m_data.begin(), m_data.end() );
        
        m_recordingUnitId = TA_COE::getIdFromAddress( m_recordingUnit->getAddress() );

        unsigned short packetLength = TA_COE::PACKET_HEADER_LENGTH + NUM_RECORDING_UNIT_STATUS_REQUEST_DATA_BYTES;
        unsigned int totalPacketLength = ( packetLength + TA_COE::NUM_LENGTH_BYTES );
        std::vector< char > commandPacket( totalPacketLength, TA_COE::OK );
        TA_COE::setPacketLength( packetLength, commandPacket[ TA_COE::LSB_LENGTH_BYTE ], commandPacket[ TA_COE::MSB_LENGTH_BYTE ] );

        commandPacket[ TA_COE::COMMAND_BYTE ] = TA_COE::REC_UNIT_STATUS_COMMAND;
        commandPacket[ RECORDING_UNIT_ID_INDEX ] = ( m_recordingUnitId % TA_COE::BYTE_HEX_CEILING );

        m_data.insert( m_data.end(), commandPacket.begin(), commandPacket.end() );
    }


    //
    // processResponse
    //
    bool RecordingUnitStatusCommand::processResponse( const std::vector< unsigned char >& data )
    {
        FUNCTION_ENTRY( "processResponse" );
        
        // Should never happen, but for consistency, make sure this object wasn't created using
        // notification command constructor (and hence is not expecting response), then this method
        // called.
        if ( 0 == m_recordingUnit )
        {
            return false;
        }

        if ( processGenericResponsePacketAttributes( data, TA_COE::REC_UNIT_STATUS_COMMAND, TA_COE::REC_UNIT_STATUS_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::REC_UNIT_STATUS_MAX_RESPONSE_PACKET_LENGTH ) )
        {
            if ( RECORDING_UNIT_ID_INDEX < data.size() )
            {
                if ( m_recordingUnitId == TA_COE::getIdFromByte( data[ RECORDING_UNIT_ID_INDEX ] ) )
                {
                    // If error code was OK, process the monitor status
                    // data.
                    if ( SUCCEEDED == m_completionState )
                    {
                        processRecordingUnitStatusData( data );
                    }
                    // Regardless of whether the error code showed OK, return true
                    // as can be sure response was for this request command.
                    return true;
                }
            }
            // Can't verify response is for the RecordingUnit the request when out for,
            // so return false and set state back to incomplete (as this is set whenever
            // processGenericResponsePacketAttributes returns true).
            m_completionState = COMMAND_LIFECYCLE_INCOMPLETE;
            return false;
        }
        else
        {
            // Not even valid RecordingUnit Status packet, so return false.
            return false;
        }

        FUNCTION_EXIT;
    }


    //
    // processNotification
    //
    void RecordingUnitStatusCommand::processNotification()
    {
        // Asynchronous notifications of device status are exactly the same as
        // responses to status requests, so here make use of AbstractRequestCommand 
        // helper to validate size of packet and parse error byte.
        if ( processGenericResponsePacketAttributes( m_notificationData, TA_COE::REC_UNIT_STATUS_COMMAND, TA_COE::REC_UNIT_STATUS_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::REC_UNIT_STATUS_MAX_RESPONSE_PACKET_LENGTH ) )
        {
            // If the error byte was OK and the packet valid, set the recordingUnitId member and use the helper to process the
            // RecordingUnit Status data.
            if ( SUCCEEDED == m_completionState )
            {
                m_recordingUnitId = TA_COE::getIdFromByte( m_notificationData[ RECORDING_UNIT_ID_INDEX ] );
                m_recordingUnit = COESwitchManager::getInstance().getRecordingUnitFromId( m_recordingUnitId );
                if ( 0 != m_recordingUnit )
                {
                    processRecordingUnitStatusData( m_notificationData );
                }
                else
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, NOTIFICATION_FOR_UNKNOWN_RECORDING_UNIT, m_recordingUnitId );
                }
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, BAD_NOTIFICATION_DATA );
            }
        }
    }

    
    //
    // processSequenceInfoData
    //
    void RecordingUnitStatusCommand::processRecordingUnitStatusData( const std::vector< unsigned char >& data )
    {
        TA_ASSERT( 0 != m_recordingUnit, "m_recordingUnit is null pointer!" );

        if ( RECORDING_UNIT_STATUS_BYTE_INDEX < data.size() )
        {
            switch ( data[ RECORDING_UNIT_STATUS_BYTE_INDEX ] )
            {
            case NOT_RECORDING_CODE:
                m_recordingUnit->notifyRecordingSpeedSet( 0 );
                break;
            case REAL_TIME_RECORDING_CODE:
                m_recordingUnit->notifyRecordingSpeedSet( 1 );
                break;
            case TIME_LAPSE_RECORDING_CODE:
                m_recordingUnit->notifyRecordingSpeedSet( 2 );
                break;
            default:
                m_completionState = RECEIVED_ERRONEOUS_RESPONSE;
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, INVALID_RECORDING_UNIT_STATUS, data[ RECORDING_UNIT_STATUS_BYTE_INDEX ] );
                break;
            }
        }
        else
        {
            m_completionState = RECEIVED_ERRONEOUS_RESPONSE;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, INSUFFICENT_LENGTH );
        }
    }
 
}

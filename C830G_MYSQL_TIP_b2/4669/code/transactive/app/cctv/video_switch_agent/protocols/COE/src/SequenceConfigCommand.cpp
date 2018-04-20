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
  * <description>
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/SequenceConfigCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECamera.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESequence.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <sstream>
#include <vector>

namespace TA_IRS_App
{
    const unsigned short SequenceConfigCommand::NUM_SEQUENCE_INFO_REQUEST_DATA_BYTES = 1;
    const unsigned short SequenceConfigCommand::SEQUENCE_ID_INDEX = TA_COE::FIRST_DATA_BYTE;
    const unsigned short SequenceConfigCommand::DWELL_TIME_INDEX = TA_COE::FIRST_DATA_BYTE + 1;
    const unsigned short SequenceConfigCommand::FIRST_CAM_ID_INDEX = TA_COE::FIRST_DATA_BYTE + 2;
    const unsigned short SequenceConfigCommand::FIRST_CAM_LOCATION_INDEX = TA_COE::FIRST_DATA_BYTE + 4;
    const unsigned short SequenceConfigCommand::NUM_CAM_FIELDS = 3;

    //
    // Const strings for logging info
    //
    const char* SequenceConfigCommand::UNKNOWN_CAMERA_ASSIGNED           = "Hardware reported Sequence population with unconfigured Camera Id: %d:%d.";
    const char* SequenceConfigCommand::NOTIFICATION_FOR_UNKNOWN_SEQUENCE = "Hardware reported Info for unconfigured Sequence Id: %d.";
    const char* SequenceConfigCommand::BAD_NOTIFICATION_DATA             = "Asynchronous Sequence Info Notification Packet has error.";
    const char* SequenceConfigCommand::UNEXPECTED_NUM_CAM_BYTES          = "Hardware reported Sequence Info with uneven number of Camera data bytes.";

    //
    // SequenceConfigCommand (INotificationCommand intended constructor)
    //
    SequenceConfigCommand::SequenceConfigCommand( const std::vector< unsigned char >& notificationData )
        : AbstractCOERequestCommand(), m_sequence( 0 ), m_sequenceId( 0 ), m_notificationData( notificationData )
    {
        //
        // Do nothing else until processNotification() is called.
        //
    }


    //
    // SequenceConfigCommand (IAgentRequestCommand intended constructor)
    //
    SequenceConfigCommand::SequenceConfigCommand( COESequence& sequence )
        : AbstractCOERequestCommand(), m_sequence( &sequence ), m_sequenceId( 0 )
    {
        m_data.erase( m_data.begin(), m_data.end() );
        
        m_sequenceId = TA_COE::getIdFromAddress( m_sequence->getAddress() );

        unsigned short packetLength = TA_COE::PACKET_HEADER_LENGTH + NUM_SEQUENCE_INFO_REQUEST_DATA_BYTES;
        unsigned int totalPacketLength = ( packetLength + TA_COE::NUM_LENGTH_BYTES );
        std::vector< char > commandPacket( totalPacketLength, TA_COE::OK );
        TA_COE::setPacketLength( packetLength, commandPacket[ TA_COE::LSB_LENGTH_BYTE ], commandPacket[ TA_COE::MSB_LENGTH_BYTE ] );

        commandPacket[ TA_COE::COMMAND_BYTE ] = TA_COE::SEQUENCE_INFO_COMMAND;
        commandPacket[ SEQUENCE_ID_INDEX ] = ( m_sequenceId % TA_COE::BYTE_HEX_CEILING );

        m_data.insert( m_data.end(), commandPacket.begin(), commandPacket.end() );
    }


    //
    // processResponse
    //
    bool SequenceConfigCommand::processResponse( const std::vector< unsigned char >& data )
    {
        FUNCTION_ENTRY( "processResponse" );
        
        // Should never happen, but for consistency, make sure this object wasn't created using
        // notification command constructor (and hence is not expecting response), then this method
        // called.
        if ( 0 == m_sequence )
        {
            return false;
        }

        if ( processGenericResponsePacketAttributes( data, TA_COE::SEQUENCE_INFO_COMMAND, TA_COE::SEQUENCE_INFO_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::SEQUENCE_INFO_MAX_RESPONSE_PACKET_LENGTH ) )
        {
            if ( SEQUENCE_ID_INDEX < data.size() )
            {
                if ( m_sequenceId == TA_COE::getIdFromByte( data[ SEQUENCE_ID_INDEX ] ) )
                {
                    // If error code was OK, process the monitor status
                    // data.
                    if ( SUCCEEDED == m_completionState )
                    {
                        processSequenceInfoData( data );
                    }
                    // Regardless of whether the error code showed OK, return true
                    // as can be sure response was for this request command.
                    return true;
                }
            }
            // Can't verify response is for the Sequence the request when out for,
            // so return false and set state back to incomplete (as this was set whenever
            // processGenericResponsePacketAttributes returns true).
            m_completionState = COMMAND_LIFECYCLE_INCOMPLETE;
            return false;
        }
        else
        {
            // Not valid Sequence Info packet, so return false.
            return false;
        }

        FUNCTION_EXIT;
    }


    //
    // processNotification
    //
    void SequenceConfigCommand::processNotification()
    {
        // Asynchronous notifications of device status are exactly the same as
        // responses to status requests, so here make use of AbstractRequestCommand 
        // helper to validate size of packet and parse error byte.
        if ( processGenericResponsePacketAttributes( m_notificationData, TA_COE::SEQUENCE_INFO_COMMAND, TA_COE::SEQUENCE_INFO_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::SEQUENCE_INFO_MAX_RESPONSE_PACKET_LENGTH ) )
        {
            // If the error byte was OK, set the sequenceId member and use the helper to process the 
            // Sequence Info data.
            if ( SUCCEEDED == m_completionState )
            {
                m_sequenceId = TA_COE::getIdFromByte( m_notificationData[ SEQUENCE_ID_INDEX ] );
                m_sequence = COESwitchManager::getInstance().getSequenceFromId( m_sequenceId );
                if ( 0 != m_sequence )
                {
                    processSequenceInfoData( m_notificationData );
                }
                else
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, NOTIFICATION_FOR_UNKNOWN_SEQUENCE, m_sequenceId );
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
    void SequenceConfigCommand::processSequenceInfoData( const std::vector< unsigned char >& data )
    {
        TA_ASSERT( 0 != m_sequence, "m_sequence is null pointer!" );

        std::vector< unsigned long > cameraKeys;

        if ( DWELL_TIME_INDEX < data.size() )
        {
            unsigned short dwellTime = static_cast< unsigned short >( data[ DWELL_TIME_INDEX ] );
            unsigned int numBytesFromFirstCamField = ( data.size() - FIRST_CAM_ID_INDEX );            
            if ( 0 != ( numBytesFromFirstCamField % NUM_CAM_FIELDS ) )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, UNEXPECTED_NUM_CAM_BYTES );
            }            
            unsigned int idIndex( FIRST_CAM_ID_INDEX );
            unsigned int locationIndex( FIRST_CAM_LOCATION_INDEX );
            while ( ( idIndex < data.size() ) && ( locationIndex < data.size() ) )
            {
                cameraKeys.push_back( determineCameraKey( getCamIdFromByteIndex( data, idIndex ), data[ locationIndex ] ) );
                idIndex += NUM_CAM_FIELDS;
                locationIndex += NUM_CAM_FIELDS;
            }
            m_sequence->notifySequenceConfigUpdated( dwellTime, cameraKeys );
        }
        else
        {
            // Sequence is empty.           
            m_sequence->notifySequenceConfigUpdated( 0, cameraKeys );
        }
    }


    //
    // determineCameraKey
    //
    unsigned long SequenceConfigCommand::determineCameraKey( unsigned short idShort, unsigned char locationByte )
    {
        unsigned short location = TA_COE::getIdFromByte( locationByte );
        unsigned short id = idShort;
        
        // Determine camera.
        COECamera* camera = 0;
        camera = COESwitchManager::getInstance().getCameraFromLocationAndId( location, id);
        
        // If Camera with this location and id is not configured, log warning.
        if ( 0 == camera )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, UNKNOWN_CAMERA_ASSIGNED, location, id );
            return 0;
        }
        else
        {
            return camera->getKey();
        }
    }

	unsigned short SequenceConfigCommand::getCamIdFromByteIndex(const std::vector<unsigned char>& data, unsigned short index)
	{
		unsigned short camId = 0;
		camId += data[index]; //LSB
		camId += data[index + 1]*0x0100; //MSB
		return camId;
	}
 
}

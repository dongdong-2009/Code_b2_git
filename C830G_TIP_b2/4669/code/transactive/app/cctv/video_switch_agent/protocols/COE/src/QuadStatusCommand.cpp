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

#include "app/cctv/video_switch_agent/protocols/COE/src/QuadStatusCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECamera.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEQuad.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <sstream>

namespace TA_IRS_App
{
    const unsigned short QuadStatusCommand::NUM_QUAD_STATUS_REQUEST_DATA_BYTES = 1;
    const unsigned short QuadStatusCommand::QUAD_ID_INDEX = TA_COE::FIRST_DATA_BYTE;
    const unsigned short QuadStatusCommand::FIRST_CAM_LOCATION_INDEX = TA_COE::FIRST_DATA_BYTE + 1;
    const unsigned short QuadStatusCommand::FIRST_CAM_ID_INDEX = TA_COE::FIRST_DATA_BYTE + 2;
    const unsigned short QuadStatusCommand::SECOND_CAM_LOCATION_INDEX = TA_COE::FIRST_DATA_BYTE + 4;
    const unsigned short QuadStatusCommand::SECOND_CAM_ID_INDEX = TA_COE::FIRST_DATA_BYTE + 5;
    const unsigned short QuadStatusCommand::THIRD_CAM_LOCATION_INDEX = TA_COE::FIRST_DATA_BYTE + 7;
    const unsigned short QuadStatusCommand::THIRD_CAM_ID_INDEX = TA_COE::FIRST_DATA_BYTE + 8;
    const unsigned short QuadStatusCommand::FOURTH_CAM_LOCATION_INDEX = TA_COE::FIRST_DATA_BYTE + 10;
    const unsigned short QuadStatusCommand::FOURTH_CAM_ID_INDEX = TA_COE::FIRST_DATA_BYTE + 11;

    //
    // Const strings for logging info
    //
    const char* QuadStatusCommand::UNKNOWN_CAMERA_ASSIGNED       = "Hardware reported Quad assignment with unconfigured Camera Id: %d:%d.";
    const char* QuadStatusCommand::NOTIFICATION_FOR_UNKNOWN_QUAD = "Hardware reported Status for unconfigured Quad Id: %d.";
    const char* QuadStatusCommand::INSUFFICENT_LENGTH            = "Received packet is of insufficient length to specify all Quad assignments";
    const char* QuadStatusCommand::BAD_NOTIFICATION_DATA         = "Asynchronous Quad Status Notification Packet has error.";

    //
    // QuadStatusCommand (INotificationCommand intended constructor)
    //
    QuadStatusCommand::QuadStatusCommand( const std::vector< unsigned char >& notificationData )
        : AbstractCOERequestCommand(), m_quad( 0 ), m_quadId( 0 ), m_notificationData( notificationData )
    {
        //
        // Do nothing else until processNotification() is called.
        //
    }


    //
    // QuadStatusCommand (IAgentRequestCommand intended constructor)
    //
    QuadStatusCommand::QuadStatusCommand( COEQuad& quad )
        : AbstractCOERequestCommand(), m_quad( &quad ), m_quadId( 0 )
    {
        m_data.erase( m_data.begin(), m_data.end() );
        
        m_quadId = TA_COE::getIdFromAddress( m_quad->getAddress() );

        unsigned short packetLength = TA_COE::PACKET_HEADER_LENGTH + NUM_QUAD_STATUS_REQUEST_DATA_BYTES;
        unsigned int totalPacketLength = ( packetLength + TA_COE::NUM_LENGTH_BYTES );
        std::vector< char > commandPacket( totalPacketLength, TA_COE::OK );
        TA_COE::setPacketLength( packetLength, commandPacket[ TA_COE::LSB_LENGTH_BYTE ], commandPacket[ TA_COE::MSB_LENGTH_BYTE ] );

        commandPacket[ TA_COE::COMMAND_BYTE ] = TA_COE::QUAD_STATUS_COMMAND;
        commandPacket[ QUAD_ID_INDEX ] = ( m_quadId % TA_COE::BYTE_HEX_CEILING );

        m_data.insert( m_data.end(), commandPacket.begin(), commandPacket.end() );
    }


    //
    // processResponse
    //
    bool QuadStatusCommand::processResponse( const std::vector< unsigned char >& data )
    {
        FUNCTION_ENTRY( "processResponse" );
        
        // Should never happen, but for consistency, make sure this object wasn't created using
        // notification command constructor (and hence is not expecting response), then this method
        // called.
        if ( 0 == m_quad )
        {
            return false;
        }

        if ( processGenericResponsePacketAttributes( data, TA_COE::QUAD_STATUS_COMMAND, TA_COE::QUAD_STATUS_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::QUAD_STATUS_MAX_RESPONSE_PACKET_LENGTH ) )
        {
            if ( QUAD_ID_INDEX < data.size() )
            {
                if ( m_quadId == TA_COE::getIdFromByte( data[ QUAD_ID_INDEX ] ) )
                {
                    // If error code was OK, process the monitor status
                    // data.
                    if ( SUCCEEDED == m_completionState )
                    {
                        processQuadStatusData( data );
                    }
                    // Regardless of whether returned error or not, return true
                    // as can be sure response was for this request command.
                    return true;
                }
            }
            // Can't verify response is for the Quad the request when out for,
            // so return false and set state back to incomplete (as this was set whenever
            // processGenericResponsePacketAttributes returns true).
            m_completionState = COMMAND_LIFECYCLE_INCOMPLETE;
            return false;
        }
        else
        {
            // Not valid Quad Status packet, so return false.
            return false;
        }

        FUNCTION_EXIT;
    }


    //
    // processNotification
    //
    void QuadStatusCommand::processNotification()
    {
        // Asynchronous notifications of device status are exactly the same as
        // responses to status requests, so here make use of AbstractRequestCommand 
        // helpers to validate size of packet and parse error byte.
        if ( processGenericResponsePacketAttributes( m_notificationData, TA_COE::QUAD_STATUS_COMMAND, TA_COE::QUAD_STATUS_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::QUAD_STATUS_MAX_RESPONSE_PACKET_LENGTH ) )
        {
            // If the error byte was OK, set the monitorId member and use the helper to process the 
            // Monitor Status data.
            if ( SUCCEEDED == m_completionState )
            {
                m_quadId = TA_COE::getIdFromByte( m_notificationData[ QUAD_ID_INDEX ] );
                m_quad = COESwitchManager::getInstance().getQuadFromId( m_quadId );
                if ( 0 != m_quad )
                {
                    processQuadStatusData( m_notificationData );
                }
                else
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, NOTIFICATION_FOR_UNKNOWN_QUAD, m_quadId );
                }
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, BAD_NOTIFICATION_DATA );
            }
        }
    }

    
	unsigned short QuadStatusCommand::getCamIdFromByteIndex(const std::vector<unsigned char>& data, unsigned short index)
	{
		unsigned short camId = 0;
		camId += data[index]; //LSB
		camId += data[index + 1]*0x0100; //MSB
		return camId;
	}
    //
    // processQuadStatusData
    //
    void QuadStatusCommand::processQuadStatusData( const std::vector< unsigned char >& data )
    {
        TA_ASSERT( 0 != m_quad, "m_quad is null pointer!" );

        if ( FOURTH_CAM_ID_INDEX < data.size() )
        {
            m_quad->notifyInputsSwitchedToQuad( 
                determineCameraKey( data[ FIRST_CAM_LOCATION_INDEX ], getCamIdFromByteIndex(data, FIRST_CAM_ID_INDEX) ),
                determineCameraKey( data[ SECOND_CAM_LOCATION_INDEX ], getCamIdFromByteIndex(data, SECOND_CAM_ID_INDEX) ),
                determineCameraKey( data[ THIRD_CAM_LOCATION_INDEX ], getCamIdFromByteIndex(data, THIRD_CAM_ID_INDEX) ),
                determineCameraKey( data[ FOURTH_CAM_LOCATION_INDEX ], getCamIdFromByteIndex(data, FOURTH_CAM_ID_INDEX) )
                );
        }
        else
        {
            m_completionState = RECEIVED_ERRONEOUS_RESPONSE;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, INSUFFICENT_LENGTH );
        }
    }


    //
    // determineCameraKey
    //
    unsigned long QuadStatusCommand::determineCameraKey( unsigned char locationByte, unsigned short idShort )
    {
        unsigned short location = TA_COE::getIdFromByte( locationByte );
        unsigned short id = idShort;
        
        // COE Protocol uses cam location:id 0:0 to indicate no camera assigned to Quad Segment,
        // and this application uses input key of 0 to do the same.
        if ( ( location == 0 ) && ( id == 0 ) )
        {
            return 0;
        }

        // Else determine camera.
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
 
}

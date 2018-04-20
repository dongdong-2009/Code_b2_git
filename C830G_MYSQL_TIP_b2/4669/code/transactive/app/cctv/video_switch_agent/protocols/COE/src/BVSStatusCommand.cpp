/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/BVSStatusCommand.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Developed to handle BVS Status notification.  
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/BVSStatusCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"

#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECamera.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESequence.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEQuad.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
    const unsigned short BVSStatusCommand::NUM_BVS_STATUS_REQUEST_DATA_BYTES = 1;
    const unsigned short BVSStatusCommand::BVS_STATION_ID_INDEX              = TA_COE::FIRST_DATA_BYTE;
    const unsigned short BVSStatusCommand::BVS_VIDEO_STATUS_INDEX            = TA_COE::FIRST_DATA_BYTE + 1;

    // BVS return data
    const unsigned char BVSStatusCommand::WITH_VIDEO    = 0x01; // video present
    const unsigned char BVSStatusCommand::WITHOUT_VIDEO = 0x00; // no video present

    const char* BVSStatusCommand::BVS_VIDEO_NOT_PRESENT   = "Hardware reported that BVS has no video present at requested station Id %d.";
    const char* BVSStatusCommand::BVS_VIDEO_PRESENT       = "Hardware reported that BVS has a video present at requested station Id %d.";
    const char* BVSStatusCommand::INVALID_BVS_STATUS      = "Hardware reported unknown BVS Status notification";
    

    // BVSStatusCommand (INotificationCommand intended constructor)
    BVSStatusCommand::BVSStatusCommand( const std::vector< unsigned char >& notificationData )
        : m_station_id( 0 ), m_BVSStage( 0 ), m_notificationData( notificationData )
    {
        //
        // Do nothing else until processNotification() is called.
        //
    }

    // BVSStatusCommand (IAgentRequestCommand intended constructor)
    BVSStatusCommand::BVSStatusCommand( COEBVSStage& BVSStage )
        : AbstractCOERequestCommand(), m_BVSStage( &BVSStage ), m_station_id( 0 )
    {
        m_data.erase( m_data.begin(), m_data.end() );
        
        m_station_id = TA_COE::getIdFromAddress( m_BVSStage->getAddress() );

        unsigned short packetLength = TA_COE::PACKET_HEADER_LENGTH + NUM_BVS_STATUS_REQUEST_DATA_BYTES;
        unsigned int totalPacketLength = ( packetLength + TA_COE::NUM_LENGTH_BYTES );
        std::vector< char > commandPacket( totalPacketLength, TA_COE::OK );
        TA_COE::setPacketLength( packetLength, commandPacket[ TA_COE::LSB_LENGTH_BYTE ], commandPacket[ TA_COE::MSB_LENGTH_BYTE ] );

        commandPacket[ TA_COE::COMMAND_BYTE ] = TA_COE::BVS_STATUS_COMMAND;
        commandPacket[ BVS_STATION_ID_INDEX ] = ( m_station_id % TA_COE::BYTE_HEX_CEILING );

        m_data.insert( m_data.end(), commandPacket.begin(), commandPacket.end() );
    }

    // not currently used by anyone
    bool BVSStatusCommand::processResponse( const std::vector< unsigned char >& data )
    {
        FUNCTION_ENTRY( "processResponse" );

        if ( processGenericResponsePacketAttributes( data,  TA_COE::BVS_STATUS_COMMAND, TA_COE::BVS_STATUS_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::BVS_STATUS_MAX_RESPONSE_PACKET_LENGTH ) )
        {
            FUNCTION_EXIT;
            return true;   
        }
        else
        {
            FUNCTION_EXIT;
            return false;
        }
        
        FUNCTION_EXIT;
        return false;
    }

    void BVSStatusCommand::processNotification()
    {
        // Asynchronous notifications of device status are exactly the same as
        // responses to status requests, so here make use of AbstractRequestCommand 
        // helper to validate size of packet and parse error byte.
        if ( processGenericResponsePacketAttributes( m_notificationData, TA_COE::BVS_STATUS_COMMAND, TA_COE::BVS_STATUS_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::BVS_STATUS_MAX_RESPONSE_PACKET_LENGTH) )
        {
            if ( BVS_VIDEO_STATUS_INDEX < m_notificationData.size() )
            {
                 m_station_id = m_notificationData[ BVS_STATION_ID_INDEX ];

                // Message not needed by any clients, so just log it 
                if ( m_notificationData[ BVS_VIDEO_STATUS_INDEX ] == WITH_VIDEO )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,BVS_VIDEO_PRESENT, m_station_id);
                }
                else if ( m_notificationData[ BVS_VIDEO_STATUS_INDEX ] == WITHOUT_VIDEO )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, BVS_VIDEO_NOT_PRESENT, m_station_id);
                }
                else
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, INVALID_BVS_STATUS);
                }
            }            
        }
    }

}

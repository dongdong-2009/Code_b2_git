/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/HeartbeatCommand.cpp $
  * @author:  Zhang Hongzhi
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/12/18 14:09:13 $
  * Last modified by:  $Author: zhang.hongzhi $
  *
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/HeartbeatCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    const unsigned short HeartbeatCommand::NUM_HEARTBEAT_COMMAND_DATA_BYTES = 0;

    //
    // HeartbeatCommand
    //
    HeartbeatCommand::HeartbeatCommand( unsigned int communicationsChannelIndex, COECommunicationsHandler& communicationsHandler )
        : AbstractCOERequestCommand(), m_communicationsChannelIndex( communicationsChannelIndex ), m_communicationsHandler( communicationsHandler )
    {
        m_data.erase( m_data.begin(), m_data.end() );
        
        unsigned short packetLength = TA_COE::PACKET_HEADER_LENGTH + NUM_HEARTBEAT_COMMAND_DATA_BYTES;
        unsigned int totalPacketLength = ( packetLength + TA_COE::NUM_LENGTH_BYTES );
        std::vector< char > commandPacket( totalPacketLength, TA_COE::OK );
        TA_COE::setPacketLength( packetLength, commandPacket[ TA_COE::LSB_LENGTH_BYTE ], commandPacket[ TA_COE::MSB_LENGTH_BYTE ] );
        commandPacket[ TA_COE::COMMAND_BYTE ] = TA_COE::HEARTBEAT_COMMAND;

        m_data.insert( m_data.end(), commandPacket.begin(), commandPacket.end() );
    }


    //
    // processResponse
    //
    bool HeartbeatCommand::processResponse( const std::vector< unsigned char >& data )
    {
        FUNCTION_ENTRY( "processResponse" );

        if ( processGenericResponsePacketAttributes( data, TA_COE::HEARTBEAT_COMMAND, TA_COE::PACKET_HEADER_LENGTH,
            TA_COE::PACKET_HEADER_LENGTH ) )
        {
	        FUNCTION_EXIT;
            return true;         
        }
        else
        {
	        FUNCTION_EXIT;
            return false;
        }
    }

}

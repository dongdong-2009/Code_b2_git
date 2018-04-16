/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventAckCommand.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * <description>
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventAckCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECamera.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESequence.h"
//#include "app/cctv/video_switch_agent/protocols/COE/src/COEQuad.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <sstream>

namespace TA_IRS_App
{

    //
    // TriggeringEventAckCommand (IAgentRequestCommand intended constructor)
    //
    TriggeringEventAckCommand::TriggeringEventAckCommand( const TA_COE::AlarmState alarmState, unsigned long monitorId )
        : AbstractCOERequestCommand()
    {
		m_data.erase( m_data.begin(), m_data.end() );
        
        unsigned short packetLength = TA_COE::PACKET_HEADER_LENGTH + 3;
        unsigned int totalPacketLength = ( packetLength + TA_COE::NUM_LENGTH_BYTES );
        std::vector< char > commandPacket( totalPacketLength, TA_COE::OK );
        TA_COE::setPacketLength( packetLength, commandPacket[ TA_COE::LSB_LENGTH_BYTE ], commandPacket[ TA_COE::MSB_LENGTH_BYTE ] );

        commandPacket[ TA_COE::COMMAND_BYTE ] = TA_COE::TRIGGERING_ALARM_ACK_COMMAND;

		commandPacket[ TA_COE::FIRST_DATA_BYTE ] = static_cast<unsigned short>(alarmState.stationId) % TA_COE::BYTE_HEX_CEILING;
		commandPacket[ TA_COE::FIRST_DATA_BYTE +1 ] = static_cast<unsigned short>(alarmState.eventId) % TA_COE::BYTE_HEX_CEILING;
		commandPacket[ TA_COE::FIRST_DATA_BYTE +2 ] = static_cast<unsigned short>(monitorId) % TA_COE::BYTE_HEX_CEILING;

		m_data.insert( m_data.end(), commandPacket.begin(), commandPacket.end() );

    }


    //
    // processResponse
    //
    bool TriggeringEventAckCommand::processResponse( const std::vector< unsigned char >& data )
    {
        FUNCTION_ENTRY( "processResponse" );
		//theres not alot we can do except make sure the response is valid
        if ( processGenericResponsePacketAttributes( data,  TA_COE::TRIGGERING_ALARM_ACK_COMMAND, TA_COE::PACKET_HEADER_LENGTH,
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

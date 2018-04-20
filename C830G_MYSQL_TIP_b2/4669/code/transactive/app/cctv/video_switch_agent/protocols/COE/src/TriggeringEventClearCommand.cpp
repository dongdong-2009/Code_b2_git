/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventClearCommand.cpp $
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

#include "app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventClearCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECamera.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESequence.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEQuad.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <sstream>

namespace TA_IRS_App
{

    //
    // TriggeringEventClearCommand (IAgentRequestCommand intended constructor)
    //
    TriggeringEventClearCommand::TriggeringEventClearCommand( TA_COE::AlarmState alarmState )
        : AbstractCOERequestCommand()
    {
		m_data.erase( m_data.begin(), m_data.end() );
        
        unsigned short packetLength = TA_COE::PACKET_HEADER_LENGTH + 2;
        unsigned int totalPacketLength = ( packetLength + TA_COE::NUM_LENGTH_BYTES );
        std::vector< char > commandPacket( totalPacketLength, TA_COE::OK );
        TA_COE::setPacketLength( packetLength, commandPacket[ TA_COE::LSB_LENGTH_BYTE ], commandPacket[ TA_COE::MSB_LENGTH_BYTE ] );

        commandPacket[ TA_COE::COMMAND_BYTE ] = TA_COE::TRIGGERING_ALARM_CLEAR_COMMAND;

		commandPacket[ TA_COE::FIRST_DATA_BYTE ] = static_cast<unsigned short>(alarmState.stationId) % TA_COE::BYTE_HEX_CEILING;
		commandPacket[ TA_COE::FIRST_DATA_BYTE +1 ] = static_cast<unsigned short>(alarmState.eventId) % TA_COE::BYTE_HEX_CEILING;

		m_data.insert( m_data.end(), commandPacket.begin(), commandPacket.end() );

    }


    //
    // processResponse
    //
    bool TriggeringEventClearCommand::processResponse( const std::vector< unsigned char >& data )
    {
        FUNCTION_ENTRY( "processResponse" );
		//theres not alot we can do except make sure the response is valid
        if ( processGenericResponsePacketAttributes( data,  TA_COE::TRIGGERING_ALARM_CLEAR_COMMAND, TA_COE::PACKET_HEADER_LENGTH,
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

		FUNCTION_EXIT;
        return false;
    }
}

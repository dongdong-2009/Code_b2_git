/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  oliverk
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/SetStageSwitchModeCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{

    const char SetStageSwitchModeCommand::m_commandValue = 0x3E;


    SetStageSwitchModeCommand::SetStageSwitchModeCommand( unsigned long originatingStage,
                                                          unsigned long destinationStage,
                                                          bool allowSwitch )
    {
        FUNCTION_ENTRY( "SetStageSwitchModeCommand" );

        m_data.erase( m_data.begin(), m_data.end() );

        unsigned short packetLength = TA_COE::PACKET_HEADER_LENGTH + 3;
        unsigned int totalPacketLength = ( packetLength + TA_COE::NUM_LENGTH_BYTES );
        std::vector<char> commandPacket ( totalPacketLength, TA_COE::OK );
        TA_COE::setPacketLength( packetLength, commandPacket[TA_COE::LSB_LENGTH_BYTE], commandPacket[TA_COE::MSB_LENGTH_BYTE] );

        commandPacket[TA_COE::COMMAND_BYTE] = TA_COE::TRA;

        commandPacket[TA_COE::FIRST_DATA_BYTE] = static_cast<unsigned short>( originatingStage ) % TA_COE::BYTE_HEX_CEILING;
        commandPacket[TA_COE::FIRST_DATA_BYTE + 1] = static_cast<unsigned short>( destinationStage ) % TA_COE::BYTE_HEX_CEILING;
        commandPacket[TA_COE::FIRST_DATA_BYTE + 2] = static_cast<unsigned short>( allowSwitch );

        m_data.insert( m_data.end(), commandPacket.begin(), commandPacket.end() );

        FUNCTION_EXIT;
    }


    bool SetStageSwitchModeCommand::processResponse( const std::vector<unsigned char>& data )
    {
        FUNCTION_ENTRY( "processResponse" );

        //theres not alot we can do except make sure the response is valid
        if ( processGenericResponsePacketAttributes( data, TA_COE::TRA, TA_COE::PACKET_HEADER_LENGTH, TA_COE::PACKET_HEADER_LENGTH ) )
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

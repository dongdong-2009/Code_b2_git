/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/SummaryPollingCommand.cpp $
  * @author:  Robert Young
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/04/18 14:09:13 $
  * Last modified by:  $Author: huang.wenbo $
  *
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/SummaryPollingCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    const unsigned short SummaryPollingCommand::NUM_POLLING_COMMAND_DATA_BYTES = 0;
    const unsigned char SummaryPollingCommand::SWITCH_ACTIVE_IDLE_OK_BITMASK = 0x01;
    const unsigned char SummaryPollingCommand::SWITCH_ACTIVE_IDLE_FAILED_BITMASK = 0x02;
    const unsigned char SummaryPollingCommand::SWITCH_IDLE_BITMASK = 0x04;
	//TD19444
	const unsigned char SummaryPollingCommand::DVR_SUMMARY_ALARM_BITMASK = 0x20;	
	//TD19444
    const unsigned char SummaryPollingCommand::ANCILLARY_SUMMARY_ALARM_BITMASK= 0x08;
    const unsigned char SummaryPollingCommand::CAMERA_SUMMARY_ALARM_BITMASK = 0x10;
    //const unsigned char SummaryPollingCommand::TRANSMISSION_SUMMARY_ALARM_BITMASK = 0x40;
    const unsigned char SummaryPollingCommand::SYSTEM_SUMMARY_ALARM_BITMASK = 0x40;

    //
    // SummaryPollingCommand
    //
    SummaryPollingCommand::SummaryPollingCommand( unsigned int communicationsChannelIndex, COECommunicationsHandler& communicationsHandler )
        : AbstractCOERequestCommand(), m_communicationsChannelIndex( communicationsChannelIndex ), m_communicationsHandler( communicationsHandler )
    {
        m_data.erase( m_data.begin(), m_data.end() );
        
        unsigned short packetLength = TA_COE::PACKET_HEADER_LENGTH + NUM_POLLING_COMMAND_DATA_BYTES;
        unsigned int totalPacketLength = ( packetLength + TA_COE::NUM_LENGTH_BYTES );
        std::vector< char > commandPacket( totalPacketLength, TA_COE::OK );
        TA_COE::setPacketLength( packetLength, commandPacket[ TA_COE::LSB_LENGTH_BYTE ], commandPacket[ TA_COE::MSB_LENGTH_BYTE ] );
        commandPacket[ TA_COE::COMMAND_BYTE ] = TA_COE::SUMMARY_POLLING_COMMAND;

        m_data.insert( m_data.end(), commandPacket.begin(), commandPacket.end() );
    }


    //
    // processResponse
    //
    bool SummaryPollingCommand::processResponse( const std::vector< unsigned char >& data )
    {
        FUNCTION_ENTRY( "processResponse" );

        if ( processGenericResponsePacketAttributes( data, TA_COE::SUMMARY_POLLING_COMMAND, TA_COE::SUMMARY_POLLING_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::SUMMARY_POLLING_MAX_RESPONSE_PACKET_LENGTH ) )
        {
            if ( SUCCEEDED == m_completionState )
            {
                processSummaryPollingData( data[ TA_COE::FIRST_DATA_BYTE ] );
            }
	        FUNCTION_EXIT;
            return true;         
        }
        else
        {
	        FUNCTION_EXIT;
            return false;
        }
    }

    
    //
    // processSummaryPollingData
    //
    void SummaryPollingCommand::processSummaryPollingData( unsigned char summaryPollingData )
    {
        m_communicationsHandler.processCommunicationsChannelState( m_communicationsChannelIndex, 
            COECommunicationsHandler::ACTIVE_SWITCH_CHANNEL );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace,"summaryPollingData %x",summaryPollingData);
        bool systemSummaryAlarm = ( SYSTEM_SUMMARY_ALARM_BITMASK == ( summaryPollingData & SYSTEM_SUMMARY_ALARM_BITMASK ) );
        bool cameraSummaryAlarm = ( CAMERA_SUMMARY_ALARM_BITMASK == ( summaryPollingData & CAMERA_SUMMARY_ALARM_BITMASK ) );
        bool ancillarySummaryAlarm = ( ANCILLARY_SUMMARY_ALARM_BITMASK == ( summaryPollingData & ANCILLARY_SUMMARY_ALARM_BITMASK ) );
		bool dvrSummaryAlarm = (DVR_SUMMARY_ALARM_BITMASK == (summaryPollingData & DVR_SUMMARY_ALARM_BITMASK));
        m_communicationsHandler.processSummaryAlarmsState( systemSummaryAlarm,// transmissionSummaryAlarm, 
            cameraSummaryAlarm, ancillarySummaryAlarm, dvrSummaryAlarm );
    }
}

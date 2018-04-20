/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventCommand.cpp $
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

#include "app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECamera.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESequence.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEQuad.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <sstream>

namespace TA_IRS_App
{
	const unsigned short TriggeringEventCommand::LSB_NUMBER_OF_ALARMS_INDEX = TA_COE::FIRST_DATA_BYTE;
    const unsigned short TriggeringEventCommand::MSB_NUMBER_OF_ALARMS_INDEX = TA_COE::FIRST_DATA_BYTE + 1;
    const unsigned short TriggeringEventCommand::FIRST_ALARM_INDEX = TA_COE::FIRST_DATA_BYTE + 2;
    const char* TriggeringEventCommand::INSUFFICENT_LENGTH               = "Received packet is of insufficient length to specify assigned device type.";    
    const char* TriggeringEventCommand::BAD_NOTIFICATION_DATA            = "Asynchronous Monitor Status Notification Packet has error.";    

    //
    // TriggeringEventCommand (INotificationCommand intended constructor)
    //
    TriggeringEventCommand::TriggeringEventCommand( const std::vector< unsigned char >& notificationData )
        : AbstractCOERequestCommand(), m_notificationData( notificationData )
    {
        //
        // Do nothing else until processNotification() is called.
        //
    }


    //
    // TriggeringEventCommand (IAgentRequestCommand intended constructor)
    //
    TriggeringEventCommand::TriggeringEventCommand()
        : AbstractCOERequestCommand()
    {
        m_data.erase( m_data.begin(), m_data.end() );
        
        unsigned short packetLength = TA_COE::PACKET_HEADER_LENGTH;
        unsigned int totalPacketLength = ( packetLength + TA_COE::NUM_LENGTH_BYTES );
        std::vector< char > commandPacket( totalPacketLength, TA_COE::OK );
        TA_COE::setPacketLength( packetLength, commandPacket[ TA_COE::LSB_LENGTH_BYTE ], commandPacket[ TA_COE::MSB_LENGTH_BYTE ] );

        commandPacket[ TA_COE::COMMAND_BYTE ] = TA_COE::TRIGGERING_ALARMS_COMMAND;

		m_data.insert( m_data.end(), commandPacket.begin(), commandPacket.end() );
    }


    //
    // processResponse
    //
    bool TriggeringEventCommand::processResponse( const std::vector< unsigned char >& data )
    {
        FUNCTION_ENTRY( "processResponse" );

        if ( processGenericResponsePacketAttributes( data, TA_COE::TRIGGERING_ALARMS_COMMAND, TA_COE::TRIGGERING_ALARMS_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::TRIGGERING_ALARMS_MAX_RESPONSE_PACKET_LENGTH ) )
        {
            // If the error byte was OK, set the videoOutput member and use helper to process the 
            // Monitor Status data.
            if ( SUCCEEDED == m_completionState )
            {
		        if ( ( MSB_NUMBER_OF_ALARMS_INDEX < data.size() ) 
						&& (data.size() - MSB_NUMBER_OF_ALARMS_INDEX)%2 == 1)
				{    
					unsigned long numberOfAlarms = 0;
					numberOfAlarms = TA_COE::getIdFromByte(data[MSB_NUMBER_OF_ALARMS_INDEX]) * 0x0100;
					numberOfAlarms += TA_COE::getIdFromByte(data[LSB_NUMBER_OF_ALARMS_INDEX]);
					
					if(numberOfAlarms == ((data.size() - MSB_NUMBER_OF_ALARMS_INDEX) / 2))
					{

						TA_COE::AlarmStack alarmStack;
						
						for(unsigned long i=FIRST_ALARM_INDEX; (i+1)<=data.size(); i+=2)
						{
							TA_COE::AlarmState alarm;
							alarm.stationId = TA_COE::getIdFromByte(data[i]);
							alarm.eventId = TA_COE::getIdFromByte(data[i+1]);
							alarmStack.push_back(alarm);
						}

						COESwitchManager::getInstance().getCommunicationsHandler()->processAlarmStack( alarmStack );
					}
					else
					{
						//
						// Packet is too small for this assignment type.
						//
						m_completionState = RECEIVED_ERRONEOUS_RESPONSE;
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, INSUFFICENT_LENGTH );
					}
				}
				else
				{
					//
					// Packet is too small for this assignment type.
					//
					m_completionState = RECEIVED_ERRONEOUS_RESPONSE;
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, INSUFFICENT_LENGTH );
				}
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, BAD_NOTIFICATION_DATA );
            }
        }
		else
		{
			FUNCTION_EXIT;
			return false;
		}

		FUNCTION_EXIT;
		return true;
    }


    //
    // processNotification
    //
    void TriggeringEventCommand::processNotification()
    {
		// Asynchronous notifications of device status are exactly the same as
        // responses to status requests, so here make use of AbstractRequestCommand 
        // helper to validate size of packet and parse error byte.

		processResponse(m_notificationData);
	}
}

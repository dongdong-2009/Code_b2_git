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

#include "app/cctv/video_switch_agent/protocols/COE/src/MonitorStatusCommand.h"
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
    const unsigned short MonitorStatusCommand::NUM_MONITOR_STATUS_REQUEST_DATA_BYTES = 1;
    const unsigned short MonitorStatusCommand::MONITOR_ID_INDEX = TA_COE::FIRST_DATA_BYTE;
    const unsigned short MonitorStatusCommand::MODE_INDEX = TA_COE::FIRST_DATA_BYTE + 1;
    const unsigned short MonitorStatusCommand::DEVICE_ID_INDEX = TA_COE::FIRST_DATA_BYTE + 2;
    const unsigned short MonitorStatusCommand::DEVICE_LOCATION_INDEX = TA_COE::FIRST_DATA_BYTE + 3;
    const unsigned char MonitorStatusCommand::CAMERA_MODE = 0x01;
    const unsigned char MonitorStatusCommand::SEQUENCE_MODE = 0x02;
    const unsigned char MonitorStatusCommand::QUAD_MODE = 0x03;
    const unsigned char MonitorStatusCommand::TRAIN_MODE = 0x04;
    const unsigned char MonitorStatusCommand::ALARM_MODE = 0x05;
    const unsigned char MonitorStatusCommand::PARKED_MODE = 0x06;

    //
    // Const strings for logging info
    //
    const char* MonitorStatusCommand::UNKNOWN_CAMERA_ASSIGNED          = "Hardware reported Monitor assignment with unconfigured Camera Id: %d:%d.";
    const char* MonitorStatusCommand::UNKNOWN_SEQUENCE_ASSIGNED        = "Hardware reported Monitor assignment with unconfigured Sequence Id: %d.";
    const char* MonitorStatusCommand::UNKNOWN_QUAD_ASSIGNED            = "Hardware reported Monitor assignment with unconfigured Quad Id: %d.";
    const char* MonitorStatusCommand::UNKNOWN_BVS_STAGE_ASSIGNED       = "Hardware reported Monitor assignment with unconfigured BVSStage Id: %d.";
    const char* MonitorStatusCommand::NOTIFICATION_FOR_UNKNOWN_MONITOR = "Hardware reported Status for unconfigured Monitor Id: %d.";
    const char* MonitorStatusCommand::INSUFFICENT_LENGTH               = "Received packet is of insufficient length to specify assigned device type.";    
    const char* MonitorStatusCommand::INVALID_ASSIGNMENT_TYPE          = "Hardware reported assignment type not specified by the interface protocol: ";
    const char* MonitorStatusCommand::BAD_NOTIFICATION_DATA            = "Asynchronous Monitor Status Notification Packet has error.";    

    //
    // MonitorStatusCommand (INotificationCommand intended constructor)
    //
    MonitorStatusCommand::MonitorStatusCommand( const std::vector< unsigned char >& notificationData )
        : AbstractCOERequestCommand(), m_videoOutput( 0 ), m_monitorId( 0 ), m_notificationData( notificationData )
    {
        //
        // Do nothing else until processNotification() is called.
        //
    }


    //
    // MonitorStatusCommand (IAgentRequestCommand intended constructor)
    //
    MonitorStatusCommand::MonitorStatusCommand( COEVideoOutput& videoOutput )
        : AbstractCOERequestCommand(), m_videoOutput( &videoOutput ), m_monitorId( 0 )
    {
        m_data.erase( m_data.begin(), m_data.end() );
        
        m_monitorId = TA_COE::getIdFromAddress( m_videoOutput->getAddress() );

        unsigned short packetLength = TA_COE::PACKET_HEADER_LENGTH + NUM_MONITOR_STATUS_REQUEST_DATA_BYTES;
        unsigned int totalPacketLength = ( packetLength + TA_COE::NUM_LENGTH_BYTES );
        std::vector< char > commandPacket( totalPacketLength, TA_COE::OK );
        TA_COE::setPacketLength( packetLength, commandPacket[ TA_COE::LSB_LENGTH_BYTE ], commandPacket[ TA_COE::MSB_LENGTH_BYTE ] );

        commandPacket[ TA_COE::COMMAND_BYTE ] = TA_COE::MON_STATUS_COMMAND;
        commandPacket[ MONITOR_ID_INDEX ] = ( m_monitorId % TA_COE::BYTE_HEX_CEILING );

        m_data.insert( m_data.end(), commandPacket.begin(), commandPacket.end() );
    }


    //
    // processResponse
    //
    bool MonitorStatusCommand::processResponse( const std::vector< unsigned char >& data )
    {
        FUNCTION_ENTRY( "processResponse" );
        
        // Should never happen, but for consistency, make sure this object wasn't created using
        // notification command constructor (and hence is not expecting response), then this method
        // called.
        if ( 0 == m_videoOutput )
        {
            return false;
        }

        if ( processGenericResponsePacketAttributes( data, TA_COE::MON_STATUS_COMMAND, TA_COE::MON_STATUS_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::MON_STATUS_MAX_RESPONSE_PACKET_LENGTH ) )
        {
            if ( MONITOR_ID_INDEX < data.size() )
            {
                if ( m_monitorId == TA_COE::getIdFromByte( data[ MONITOR_ID_INDEX ] ) )
                {
                    // If error code was OK, process the monitor status
                    // data.
                    if ( SUCCEEDED == m_completionState )
                    {
                        processMonitorStatusData( data );
                    }
                    // Regardless of whether returned error or not, return true
                    // as can be sure response was for this request command.
                    return true;
                }
            }
            // Can't verify response is for the monitor the request when out for,
            // so return false and set state back to incomplete (as this was set whenever
            // processGenericResponsePacketAttributes returns true).
            m_completionState = COMMAND_LIFECYCLE_INCOMPLETE;
            return false;
        }
        else
        {
            // Not valid Monitor Status packet, so return false.
            return false;
        }

        FUNCTION_EXIT;
    }


    //
    // processNotification
    //
    void MonitorStatusCommand::processNotification()
    {
        // Asynchronous notifications of device status are exactly the same as
        // responses to status requests, so here make use of AbstractRequestCommand 
        // helper to validate size of packet and parse error byte.
        if ( processGenericResponsePacketAttributes( m_notificationData, TA_COE::MON_STATUS_COMMAND, TA_COE::MON_STATUS_MIN_RESPONSE_PACKET_LENGTH,
            TA_COE::MON_STATUS_MAX_RESPONSE_PACKET_LENGTH ) )
        {
            // If the error byte was OK, set the videoOutput member and use helper to process the 
            // Monitor Status data.
            if ( SUCCEEDED == m_completionState )
            {
                m_monitorId = TA_COE::getIdFromByte( m_notificationData[ MONITOR_ID_INDEX ] );
                m_videoOutput = COESwitchManager::getInstance().getVideoOutputFromId( m_monitorId );
                if ( 0 != m_videoOutput )
                {
                    processMonitorStatusData( m_notificationData );
                }
                else
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, NOTIFICATION_FOR_UNKNOWN_MONITOR, m_monitorId );
                }            
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, BAD_NOTIFICATION_DATA );
            }
        }
    }

    
    //
    // processMonitorStatusData
    //
    void MonitorStatusCommand::processMonitorStatusData( const std::vector< unsigned char >& data )
    {
        switch ( data[ MODE_INDEX ] )
        {
        case CAMERA_MODE:
            processCameraMode( data );
            break;
        case SEQUENCE_MODE:
            processSequenceMode( data );
            break;
        case QUAD_MODE:
            processQuadMode( data );
            break;
        case TRAIN_MODE:
            processTrainMode( data );
            break;
        case ALARM_MODE:
            processAlarmMode( data );
            break;
        case PARKED_MODE:
            processParkedMode( data );
            break;
        default:
            m_completionState = RECEIVED_ERRONEOUS_RESPONSE;
            std::stringstream warnMsgStr;
            warnMsgStr << INVALID_ASSIGNMENT_TYPE; 
            warnMsgStr << "0x" << std::hex << static_cast< unsigned short >( data[ MODE_INDEX ] ) << std::dec;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, warnMsgStr.str().c_str() );
            break;
        }
    }


    //
    // processCameraMode
    //
    void MonitorStatusCommand::processCameraMode( const std::vector< unsigned char >& data )
    {
        TA_ASSERT( 0 != m_videoOutput, "m_videoOutput is null pointer!" );

        if ( ( DEVICE_LOCATION_INDEX < data.size() ) && ( DEVICE_ID_INDEX < data.size() ) )
        {    
			unsigned short camIdLSB = TA_COE::getIdFromByte(data[TA_COE::FIRST_DATA_BYTE + 2]);
			unsigned short camIdMSB = TA_COE::getIdFromByte(data[TA_COE::FIRST_DATA_BYTE + 3]);
            unsigned short camLocation = TA_COE::getIdFromByte(data[TA_COE::FIRST_DATA_BYTE + 4]);
            unsigned short camId = (camIdMSB << 8) + camIdLSB;
            COECamera* camera = COESwitchManager::getInstance().getCameraFromLocationAndId( camLocation, camId );
            if ( 0 != camera )
            {
                m_videoOutput->notifyInputSwitchedToOutput( camera->getKey() );
            }
            else
            {
                //
                // The assigned camera is not configured.
                //
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, UNKNOWN_CAMERA_ASSIGNED, camLocation, camId );
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


    // 
    // processSequenceMode
    //
    void MonitorStatusCommand::processSequenceMode( const std::vector< unsigned char >& data )
    {
        TA_ASSERT( 0 != m_videoOutput, "m_videoOutput is null pointer!" );

        if ( DEVICE_ID_INDEX < data.size() )
        {    
            unsigned short sequenceId = TA_COE::getIdFromByte( data[ DEVICE_ID_INDEX ] );
            COESequence* sequence = COESwitchManager::getInstance().getSequenceFromId( sequenceId );
            if ( 0 != sequence )
            {
                m_videoOutput->notifyInputSwitchedToOutput( sequence->getKey() );
            }
            else
            {
                //
                // The assigned sequence is not configured.
                //
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, UNKNOWN_SEQUENCE_ASSIGNED, sequenceId );
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


    // 
    // processQuadMode
    //
    void MonitorStatusCommand::processQuadMode( const std::vector< unsigned char >& data )
    {
        TA_ASSERT( 0 != m_videoOutput, "m_videoOutput is null pointer!" );

        if ( DEVICE_ID_INDEX < data.size() )
        {    
            unsigned short quadId = TA_COE::getIdFromByte( data[ DEVICE_ID_INDEX ] );
            COEQuad* quad = COESwitchManager::getInstance().getQuadFromId( quadId );
            if ( 0 != quad )
            {
                m_videoOutput->notifyInputSwitchedToOutput( quad->getKey() );
            }
            else
            {
                //
                // The assigned quad is not configured.
                //
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, UNKNOWN_QUAD_ASSIGNED, quadId );
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


    // 
    // processTrainMode
    //
    void MonitorStatusCommand::processTrainMode( const std::vector< unsigned char >& data )
    {
        TA_ASSERT( 0 != m_videoOutput, "m_videoOutput is null pointer!" );

        if ( DEVICE_ID_INDEX < data.size() )
        {    
            unsigned short bvsStageId = TA_COE::getIdFromByte( data[ DEVICE_LOCATION_INDEX ] );
            COEBVSStage* bvsStage = COESwitchManager::getInstance().getBVSStageFromId( bvsStageId );
            if ( (0 != bvsStageId) && (bvsStage != NULL) )
            {
                m_videoOutput->notifyInputSwitchedToOutput( bvsStage->getKey() );
            }
            else
            {
                //
                // The assigned BVS Stage is not configured.
                //
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, UNKNOWN_BVS_STAGE_ASSIGNED, bvsStageId );
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


    // 
    // processAlarmMode
    //
    void MonitorStatusCommand::processAlarmMode( const std::vector< unsigned char >& data )
    {
        TA_ASSERT( 0 != m_videoOutput, "m_videoOutput is null pointer!" );

        if ( ( DEVICE_LOCATION_INDEX < data.size() ) && ( DEVICE_ID_INDEX < data.size() ) 
			&& (data.size() - DEVICE_ID_INDEX)%2 == 0)
        {    
			TA_COE::AlarmStack alarmStack;
			
			for(unsigned long i=DEVICE_ID_INDEX; (i+1)<data.size(); i+=2)
			{
				TA_COE::AlarmState alarm;
				alarm.eventId = TA_COE::getIdFromByte(data[i]);
				alarm.stationId = TA_COE::getIdFromByte(data[i+1]);
				alarmStack.push_back(alarm);
			}

            m_videoOutput->notifyOutputShowingAlarmStack( alarmStack );
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


    // 
    // processParkedMode
    //
    void MonitorStatusCommand::processParkedMode( const std::vector< unsigned char >& data )
    {
        TA_ASSERT( 0 != m_videoOutput, "m_videoOutput is null pointer!" );

        TA_ASSERT( 0 != m_videoOutput, "m_videoOutput is null pointer!" );

		m_videoOutput->notifyInputSwitchedToOutput( 0 );
    }

}

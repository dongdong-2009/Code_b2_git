/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/COEHelper.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Some constants and static functions to assist in the creation and processing of COE Protocol agent 
  * request and hardware notification commands.
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/src/IHardwareNotificationCommand.h"
#include "app/cctv/video_switch_agent/protocols/src/IAgentRequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/MonitorStatusCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/QuadStatusCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/RecordingUnitStatusCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SequenceConfigCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SetSequenceConfigCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SetRecordingUnitModeCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SwitchInputToOutputCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SwitchQuadInputsCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SummaryPollingCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventClearCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventAckCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/BVSStatusCommand.h" // Lorna TD#13212

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"

#include <sstream>

namespace TA_IRS_App
{
    namespace TA_COE
    {
        const char* BAD_ADDRESS_ERROR_MSG = "Invalid value set to Address";

        /////////////////////////////
        ////  Utility Functions  ////
        /////////////////////////////

        //
        // getPacketLength
        //
        unsigned short getPacketLength( unsigned char leastSigByte, unsigned char mostSigByte )
        {
            return ( (mostSigByte * BYTE_HEX_CEILING) + leastSigByte );
        }


        //
        // setPacketLength
        //
        void setPacketLength( unsigned short packetLength, char& leastSigByte, char& mostSigByte )
        {
            leastSigByte = ( packetLength % BYTE_HEX_CEILING );
            mostSigByte = ( packetLength / BYTE_HEX_CEILING );
        }


        //
        // getIDFromAddress
        //
        unsigned short getIdFromAddress( const std::string& address )
        {
            unsigned short Id;
            std::istringstream addressIStream( address ); 
            if ( false == (addressIStream >> Id && addressIStream.eof()) )
            {
                TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                    TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, 
                    "BAD_ADDRESS_ERROR_MSG" ) );            
            }
            
            return Id;
        }


        //
        // getIdFromByte
        //
        unsigned short getIdFromByte( unsigned char idByte )
        {
            return static_cast< unsigned short >( idByte );
        }


        //////////////////////////////////////
        ////  Command Creation Functions  ////
        //////////////////////////////////////


        //
        // createSwitchInputToOutputCommand
        //
        IAgentRequestCommand* createSwitchInputToOutputCommand( unsigned long videoInputEntityKey, COEVideoOutput& output )
        {
            return new SwitchInputToOutputCommand( videoInputEntityKey, output );
        }

       
        // 
        // createSwitchQuadInputsCommand
        //
        IAgentRequestCommand* createSwitchQuadInputsCommand( unsigned long topLeftVideoInput, unsigned long topRightVideoInput, unsigned long bottomLeftVideoInput, unsigned long bottomRightVideoInput, COEQuad& quad )
        {
            return new SwitchQuadInputsCommand( topLeftVideoInput, topRightVideoInput, bottomLeftVideoInput, bottomRightVideoInput, quad );
        }

        
        //
        // createSetSequenceConfigCommand
        //
        IAgentRequestCommand* createSetSequenceConfigCommand( unsigned short dwellTime, const std::vector< unsigned long >& videoInputs, COESequence& sequence )
        {
            return new SetSequenceConfigCommand( dwellTime, videoInputs, sequence );
        }


        // 
        // createSetRecordingUnitModeCommand
        //
        IAgentRequestCommand* createSetRecordingUnitModeCommand( unsigned short recordingSpeedIndex, COERecordingUnit& recordingUnit )
        {
            return new SetRecordingUnitModeCommand( recordingSpeedIndex, recordingUnit );
        }


        //
        // createSummaryPollingCommand
        //
        IAgentRequestCommand* createSummaryPollingCommand( unsigned int communicationsChannelIndex, COECommunicationsHandler& communicationsHandler )
        {
            return new SummaryPollingCommand( communicationsChannelIndex, communicationsHandler );
        }


        // 
        // createRequestMonitorStatusCommand
        //
        IAgentRequestCommand* createRequestMonitorStatusCommand( COEVideoOutput& videoOutput )
        {
            return new MonitorStatusCommand( videoOutput );
        }


        //
        // createMonitorStatusNotificationCommand
        // 
        IHardwareNotificationCommand* createMonitorStatusNotificationCommand( const std::vector< unsigned char >& notificationData )
        {
            return new MonitorStatusCommand( notificationData );
        }


        // 
        // createRequestQuadStatusCommand
        //
        IAgentRequestCommand* createRequestQuadStatusCommand( COEQuad& quad )
        {
            return new QuadStatusCommand( quad );
        }


        // 
        // createQuadStatusNotificationCommand
        //
        IHardwareNotificationCommand* createQuadStatusNotificationCommand( const std::vector< unsigned char >& notificationData )
        {
            return new QuadStatusCommand( notificationData );
        }


        // 
        // createRequestSequenceConfigCommand
        //
        IAgentRequestCommand* createRequestSequenceConfigCommand( COESequence& sequence )
        {
            return new SequenceConfigCommand( sequence );
        }


        // 
        // createSequenceConfigNotificationCommand
        //
        IHardwareNotificationCommand* createSequenceConfigNotificationCommand( const std::vector< unsigned char >& notificationData )
        {
            return new SequenceConfigCommand( notificationData );
        }


        // 
        // createRequestRecordingUnitStatusCommand
        //
        IAgentRequestCommand* createRequestRecordingUnitStatusCommand( COERecordingUnit& recordingUnit )
        {
            return new RecordingUnitStatusCommand( recordingUnit );
        }


        // 
        // createRecordingUnitStatusNotificationCommand
        //
        IHardwareNotificationCommand* createRecordingUnitStatusNotificationCommand( const std::vector< unsigned char >& notificationData )
        {
            return new RecordingUnitStatusCommand( notificationData ); 
        }

        IAgentRequestCommand* createRequestTriggeringEventCommand()
		{
			return new TriggeringEventCommand();
		}

        IHardwareNotificationCommand* createTriggeringEventNotificationCommand( const std::vector< unsigned char >& notificationData )
		{
			return new TriggeringEventCommand( notificationData );
		}

        IAgentRequestCommand* createRequestTriggeringEventClearCommand(const TA_COE::AlarmState& alarmState)
		{
			return new TriggeringEventClearCommand(alarmState);
		}

        IAgentRequestCommand* createRequestTriggeringEventAckCommand(const TA_COE::AlarmState& alarmState, const unsigned long monitorId)
		{
			return new TriggeringEventAckCommand(alarmState, monitorId);
		}

        // ++ Lorna TD#13212
        IHardwareNotificationCommand* createBVSStatusNotificationCommand( const std::vector< unsigned char >& notificationData )
        {
            return new BVSStatusCommand(notificationData);
        }
        // ++ Lorna

    }
}

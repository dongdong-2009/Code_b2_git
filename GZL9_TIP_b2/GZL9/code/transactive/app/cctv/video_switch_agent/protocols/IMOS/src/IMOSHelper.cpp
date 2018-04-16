/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/IMOS/src/IMOSHelper.cpp $
  * @author:  Robert Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/08/13 14:55:34 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Some constants and static functions to assist in the creation and processing of IMOS Protocol agent 
  * request and hardware notification commands.
  */

#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSHelper.h"
/*
#include "app/cctv/video_switch_agent/protocols/src/IHardwareNotificationCommand.h"
#include "app/cctv/video_switch_agent/protocols/src/IAgentRequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/MonitorStatusCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/QuadStatusCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/RecordingUnitStatusCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/SequenceConfigCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/SetSequenceConfigCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/SetRecordingUnitModeCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/SwitchInputToOutputCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/SwitchQuadInputsCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/SummaryPollingCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/TriggeringEventCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/TriggeringEventClearCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/TriggeringEventAckCommand.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/BVSStatusCommand.h" // Lorna TD#13212
*/

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"

#include <sstream>

namespace TA_IRS_App
{
    namespace TA_IMOS
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
            //return ( (mostSigByte * BYTE_HEX_CEILING) + leastSigByte );
			return 0;
        }


        //
        // setPacketLength
        //
        void setPacketLength( unsigned short packetLength, char& leastSigByte, char& mostSigByte )
        {
			/*
            leastSigByte = ( packetLength % BYTE_HEX_CEILING );
            mostSigByte = ( packetLength / BYTE_HEX_CEILING );
			*/
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
        IAgentRequestCommand* createSwitchInputToOutputCommand( unsigned long videoInputEntityKey, IMOSVideoOutput& output )
        {
            //return new SwitchInputToOutputCommand( videoInputEntityKey, output );
			return NULL;
        }

       
        // 
        // createSwitchQuadInputsCommand
        //
        IAgentRequestCommand* createSwitchQuadInputsCommand( unsigned long topLeftVideoInput, unsigned long topRightVideoInput, unsigned long bottomLeftVideoInput, unsigned long bottomRightVideoInput, IMOSQuad& quad )
        {
            //return new SwitchQuadInputsCommand( topLeftVideoInput, topRightVideoInput, bottomLeftVideoInput, bottomRightVideoInput, quad );
			return NULL;
        }

        
        //
        // createSetSequenceConfigCommand
        //
        IAgentRequestCommand* createSetSequenceConfigCommand( unsigned short dwellTime, const std::vector< unsigned long >& videoInputs, IMOSSequence& sequence,bool con )
        {
			LOG0( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"IMOSHelper::createSetSequenceConfigCommand ");
            //return new SetSequenceConfigCommand( dwellTime, videoInputs, sequence,con );
			return NULL;
        }


        // 
        // createSetRecordingUnitModeCommand
        //
        IAgentRequestCommand* createSetRecordingUnitModeCommand( unsigned short recordingSpeedIndex, IMOSRecordingUnit& recordingUnit )
        {
            //return new SetRecordingUnitModeCommand( recordingSpeedIndex, recordingUnit );
			return NULL;
        }


        //
        // createSummaryPollingCommand
        //
        IAgentRequestCommand* createSummaryPollingCommand( unsigned int communicationsChannelIndex, IMOSCommunicationsHandler& communicationsHandler )
        {
            //return new SummaryPollingCommand( communicationsChannelIndex, communicationsHandler );
			return NULL;
        }


        // 
        // createRequestMonitorStatusCommand
        //
        IAgentRequestCommand* createRequestMonitorStatusCommand( IMOSVideoOutput& videoOutput )
        {
            //return new MonitorStatusCommand( videoOutput );
			return NULL;
        }


        //
        // createMonitorStatusNotificationCommand
        // 
        IHardwareNotificationCommand* createMonitorStatusNotificationCommand( const std::vector< unsigned char >& notificationData )
        {
            //return new MonitorStatusCommand( notificationData );
			return NULL;
        }


        // 
        // createRequestQuadStatusCommand
        //
        IAgentRequestCommand* createRequestQuadStatusCommand( IMOSQuad& quad )
        {
            //return new QuadStatusCommand( quad );
			return NULL;
        }


        // 
        // createQuadStatusNotificationCommand
        //
        IHardwareNotificationCommand* createQuadStatusNotificationCommand( const std::vector< unsigned char >& notificationData )
        {
            //return new QuadStatusCommand( notificationData );
			return NULL;
        }


        // 
        // createRequestSequenceConfigCommand
        //
        IAgentRequestCommand* createRequestSequenceConfigCommand( IMOSSequence& sequence )
        {
            //return new SequenceConfigCommand( sequence );
			return NULL;
        }


        // 
        // createSequenceConfigNotificationCommand
        //
        IHardwareNotificationCommand* createSequenceConfigNotificationCommand( const std::vector< unsigned char >& notificationData )
        {
            //return new SequenceConfigCommand( notificationData );
			return NULL;
        }


        // 
        // createRequestRecordingUnitStatusCommand
        //
        IAgentRequestCommand* createRequestRecordingUnitStatusCommand( IMOSRecordingUnit& recordingUnit )
        {
            //return new RecordingUnitStatusCommand( recordingUnit );
			return NULL;
        }


        // 
        // createRecordingUnitStatusNotificationCommand
        //
        IHardwareNotificationCommand* createRecordingUnitStatusNotificationCommand( const std::vector< unsigned char >& notificationData )
        {
            //return new RecordingUnitStatusCommand( notificationData ); 
			return NULL;
        }

        IAgentRequestCommand* createRequestTriggeringEventCommand()
		{
			//return new TriggeringEventCommand();
			return NULL;
		}

        IHardwareNotificationCommand* createTriggeringEventNotificationCommand( const std::vector< unsigned char >& notificationData )
		{
			//return new TriggeringEventCommand( notificationData );
			return NULL;
		}

        IAgentRequestCommand* createRequestTriggeringEventClearCommand(const TA_IMOS::AlarmState& alarmState)
		{
			//return new TriggeringEventClearCommand(alarmState);
			return NULL;
		}

        IAgentRequestCommand* createRequestTriggeringEventAckCommand(const TA_IMOS::AlarmState& alarmState, const unsigned long monitorId)
		{
			//return new TriggeringEventAckCommand(alarmState, monitorId);
			return NULL;
		}

        // ++ Lorna TD#13212
        /*IHardwareNotificationCommand* createBVSStatusNotificationCommand( const std::vector< unsigned char >& notificationData )
        {
            return new BVSStatusCommand(notificationData);
        }*/
        // ++ Lorna

    }
}

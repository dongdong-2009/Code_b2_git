/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/COEHelper.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Some constants and static functions to assist in the creation and processing of COE Protocol agent 
  * request and hardware notification commands.
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/src/IHardwareNotificationCommand.h"
#include "app/cctv/video_switch_agent/protocols/src/IAgentRequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SetSequenceConfigCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SwitchInputToOutputCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SwitchQuadInputsCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/SummaryPollingCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventAckCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/HeartbeatCommand.h"

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
                TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR, "BAD_ADDRESS_ERROR_MSG" ) );            
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
			SwitchInputToOutputCommand* command = new SwitchInputToOutputCommand( videoInputEntityKey, output );
			command->buildCommand();
			return command;
        }

       
        // 
        // createSwitchQuadInputsCommand
        //
        IAgentRequestCommand* createSwitchQuadInputsCommand( unsigned long topLeftVideoInput, unsigned long topRightVideoInput, unsigned long bottomLeftVideoInput, unsigned long bottomRightVideoInput, COEVideoOutput& monitor )
        {
            return new SwitchQuadInputsCommand( topLeftVideoInput, topRightVideoInput, bottomLeftVideoInput, bottomRightVideoInput, monitor );
        }

        
        //
        // createSetSequenceConfigCommand
        //
        IAgentRequestCommand* createSetSequenceConfigCommand( unsigned short dwellTime, const std::vector< unsigned long >& videoInputs, COESequence& sequence )
        {
            return new SetSequenceConfigCommand( dwellTime, videoInputs, sequence );
        }


        //
        // createSummaryPollingCommand
        //
        IAgentRequestCommand* createSummaryPollingCommand( unsigned int communicationsChannelIndex, COECommunicationsHandler& communicationsHandler )
        {
            return new SummaryPollingCommand( communicationsChannelIndex, communicationsHandler );
        }

		
		//
		// createHeartbeatCommand
		//
		IAgentRequestCommand* createHeartbeatCommand( unsigned int communicationsChannelIndex, COECommunicationsHandler& communicationsHandler )
		{
			return new HeartbeatCommand( communicationsChannelIndex, communicationsHandler );
		}


        IAgentRequestCommand* createRequestTriggeringEventCommand()
		{
			return new TriggeringEventCommand();
		}

        IHardwareNotificationCommand* createTriggeringEventNotificationCommand( const std::vector< unsigned char >& notificationData )
		{
			return new TriggeringEventCommand( notificationData );
		}

        IAgentRequestCommand* createRequestTriggeringEventAckCommand(const TA_COE::AlarmState& alarmState, const unsigned long monitorId)
		{
			return new TriggeringEventAckCommand(alarmState, monitorId);
		}
    }
}

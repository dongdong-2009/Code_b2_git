#ifndef COE_HELPER_H
#define COE_HELPER_H

/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  Rob Young
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 * 
 * Some constants and static functions to assist in the creation and processing of COE Protocol agent 
 * request and hardware notification commands.
 */

#include <string>
#include <vector>

namespace TA_IRS_App
{
    class IHardwareNotificationCommand;
    class IAgentRequestCommand;
    class COECommunicationsHandler;
    class COEVideoOutput;
    class COESequence;
    class COEQuad;
    class COERecordingUnit;
}

namespace TA_IRS_App
{
    
	namespace TA_COE
    {
    
		struct AlarmState
		{
			unsigned long eventId;
			unsigned long stationId;

			bool operator<(const AlarmState& rhs) const
			{
				if(stationId != rhs.stationId)
				{
					return (stationId < rhs.stationId);
				}
				else 
				{
					return (eventId < rhs.eventId);
				}
			};

			bool operator==(const AlarmState& rhs) const
			{
				return (eventId == rhs.eventId && 
							stationId==rhs.stationId);
			};

		};

		typedef std::vector< AlarmState > AlarmStack;


		///////////////////////////////////////////////
        ////  COE Packet Format Details Constants  ////
        ///////////////////////////////////////////////

        enum EPacketFormat
        {
            LSB_LENGTH_BYTE = 0,
            MSB_LENGTH_BYTE = 1,
            ERROR_BYTE      = 2,
            COMMAND_BYTE    = 3,
            FIRST_DATA_BYTE = 4
        };

        const unsigned int NUM_LENGTH_BYTES = 2;
        const unsigned int PACKET_HEADER_LENGTH = 2; // Not including length bytes themselves
        const unsigned int MAXIMUM_PACKET_LENGTH = 65535;
        const unsigned short BYTE_HEX_CEILING = 0x0100;

        ////////////////////////////////
        ////  ERROR Response Codes  ////
        ////////////////////////////////
        
        const unsigned char OK              = 0x00;
        const unsigned char NOT_IMPLEMENTED = 0x01;
        const unsigned char PROHIBITED      = 0x02;
        const unsigned char BAD_LENGTH      = 0x03;
        const unsigned char BAD_COMMAND     = 0x04;
        const unsigned char BUSY            = 0x05;

        /////////////////////////
        ////  CLEAR command  ////
        /////////////////////////

        const unsigned char CLEAR_COMMAND = 0x06;
        const unsigned int CLEAR_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;
        const unsigned int CLEAR_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;

        ////////////////////////////////////////////
        ////  SET RECORDING UNIT SPEED command  ////
        ////////////////////////////////////////////

        const unsigned char REC_UNIT_MODE_COMMAND = 0x12;
        const unsigned int REC_UNIT_MODE_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;
        const unsigned int REC_UNIT_MODE_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;
        
        ////////////////////////////////////
        ////  PROGRAM SEQUENCE command  ////
        ////////////////////////////////////

        const unsigned char PROG_SEQ_COMMAND = 0x17;
        const unsigned int PROG_SEQ_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;
        const unsigned int PROG_SEQ_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;

        ////////////////////////////////
        ////  PROGRAM QUAD command  ////
        ////////////////////////////////

        const unsigned char PROG_QUAD_COMMAND = 0x18;
        const unsigned int PROG_QUAD_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;
        const unsigned int PROG_QUAD_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;

        //////////////////////////////////////
        ////  CAMERA AND MONITOR command  ////
        //////////////////////////////////////

        const unsigned char CAM_AND_MON_COMMAND = 0x21;
        const unsigned int CAM_AND_MON_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;
        const unsigned int CAM_AND_MON_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;

        ////////////////////////////////////////
        ////  SEQUENCE AND MONITOR command  ////
        ////////////////////////////////////////

        const unsigned char SEQ_AND_MON_COMMAND = 0x22;
        const unsigned int SEQ_AND_MON_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;
        const unsigned int SEQ_AND_MON_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;

        /////////////////////////////////////////////
        ////  CONTROL SEQUENCE PLAYBACK command  ////
        /////////////////////////////////////////////

        const unsigned char CTRL_SEQ_COMMAND = 0x23;
        const unsigned int CTRL_SEQ_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;
        const unsigned int CTRL_SEQ_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;

        ////////////////////////////////////
        ////  QUAD AND MONITOR command  ////
        ////////////////////////////////////

        const unsigned char QUAD_AND_MON_COMMAND = 0x24;
        const unsigned int QUAD_AND_MON_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;
        const unsigned int QUAD_AND_MON_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;

        /////////////////////////////////////
        ////  TRAIN AND MONITOR command  ////
        /////////////////////////////////////

        const unsigned char TRAIN_AND_MON_COMMAND = 0x25;
        const unsigned int TRAIN_AND_MON_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;
        const unsigned int TRAIN_AND_MON_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;

        ///////////////////////////////////
        ////  TRAIN ALARM ACK command  ////
        ///////////////////////////////////

        const unsigned char TRAIN_ALARM_ACK_COMMAND = 0x26;
        const unsigned int TRAIN_ALARM_ACK_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;
        const unsigned int TRAIN_ALARM_ACK_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;

        /////////////////////////////////////
        ////  TRAIN ALARM CLEAR command  ////
        /////////////////////////////////////

        const unsigned char TRAIN_ALARM_CLEAR_COMMAND = 0x27;
        const unsigned int TRAIN_ALARM_CLEAR_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;
        const unsigned int TRAIN_ALARM_CLEAR_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;

        //////////////////////////////
        ////  BVS STATUS command  ////
        //////////////////////////////

        const unsigned char BVS_STATUS_COMMAND = 0x32;
        const unsigned int BVS_STATUS_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH + 2;
        const unsigned int BVS_STATUS_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH + 2;

        ///////////////////////////////
        ////  QUAD STATUS command  ////
        ///////////////////////////////

        const unsigned char QUAD_STATUS_COMMAND = 0x35;
        const unsigned int QUAD_STATUS_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH + 13;
        const unsigned int QUAD_STATUS_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH + 13;

        /////////////////////////////////
        ////  SEQUENCE INFO command  ////
        /////////////////////////////////

        const unsigned char SEQUENCE_INFO_COMMAND = 0x36;
        const unsigned int SEQUENCE_INFO_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH + 1;
        const unsigned int SEQUENCE_INFO_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH + 195;

        ///////////////////////////////////	
        ////  REC UNIT STATUS command  ////
        ///////////////////////////////////

        const unsigned char REC_UNIT_STATUS_COMMAND = 0x37;
        const unsigned int REC_UNIT_STATUS_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH + 2;
        const unsigned int REC_UNIT_STATUS_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH + 2;

        //////////////////////////////////
        ////  MONITOR STATUS command  ////
        //////////////////////////////////

        const unsigned char MON_STATUS_COMMAND = 0x38;
        const unsigned int MON_STATUS_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH + 2;
        const unsigned int MON_STATUS_MAX_RESPONSE_PACKET_LENGTH = MAXIMUM_PACKET_LENGTH;

        ///////////////////////////////////
        ////  SUMMARY POLLING command  ////
        ///////////////////////////////////

        const unsigned char SUMMARY_POLLING_COMMAND = 0x3a;
        const unsigned int SUMMARY_POLLING_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH + 1;
        const unsigned int SUMMARY_POLLING_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH + 1;

        /////////////////////////////////////
        ////  TRIGGERING ALARMS command  ////
        /////////////////////////////////////

        const unsigned char TRIGGERING_ALARMS_COMMAND = 0x3b;
        const unsigned int TRIGGERING_ALARMS_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH + 2;
        const unsigned int TRIGGERING_ALARMS_MAX_RESPONSE_PACKET_LENGTH = MAXIMUM_PACKET_LENGTH;

        ////////////////////////////////////////
        ////  TRIGGERING ALARM ACK command  ////
        ////////////////////////////////////////

        const unsigned char TRIGGERING_ALARM_ACK_COMMAND = 0x3c;
        const unsigned int TRIGGERING_ALARM_ACK_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;
        const unsigned int TRIGGERING_ALARM_ACK_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;

        //////////////////////////////////////////
        ////  TRIGGERING ALARM CLEAR command  ////
        //////////////////////////////////////////

        const unsigned char TRIGGERING_ALARM_CLEAR_COMMAND = 0x3d;
        const unsigned int TRIGGERING_ALARM_CLEAR_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;
        const unsigned int TRIGGERING_ALARM_CLEAR_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;

        const unsigned char TRA = 0x3e;
        const unsigned int SETSTAGE_SWITCH_MODE_MIN_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;
        const unsigned int SETSTAGE_SWITCH_MODE_MAX_RESPONSE_PACKET_LENGTH = PACKET_HEADER_LENGTH;
        /////////////////////////////
        ////  Utility Functions  ////
        /////////////////////////////

        /** 
          * getPacketLength
          *
          * description
          *
          * @param leastSigByte
          * @param mostSigByte
          *
          * @return 
          */
        unsigned short getPacketLength( unsigned char leastSigByte, unsigned char mostSigByte );


        /** 
          * setPacketLength
          *
          * description
          *
          * @param packetLength
          * @param leastSigByte
          * @param 
          */
        void setPacketLength( unsigned short packetLength, char& leastSigByte, char& mostSigByte );

        
        /** 
          * getIDFromAddress
          *
          * description
          *
          * @param 
          *
          * @return
          *
          * @exception
          */
        unsigned short getIdFromAddress( const std::string& address );


        /** 
          * getIDFromByte
          *
          * description
          *
          * @param 
          *
          * @return
          *
          * @exception
          */
        unsigned short getIdFromByte( unsigned char idByte );
       
   
        //////////////////////////////////////
        ////  Command Creation Functions  ////
        //////////////////////////////////////

   
        /** 
          * createSwitchInputToOutputCommand
          *
          * description
          *
          * @param videoInputEntityKey
          * @param 
          *
          * @return 
          */
        IAgentRequestCommand* createSwitchInputToOutputCommand( unsigned long videoInputEntityKey, COEVideoOutput& output );

       
        /** 
          * createSwitchQuadInputsCommand
          *
          * description
          *
          * @param topLeftVideoInput
          * @param topRightVideoInput
          * @param bottomLeftVideoInput
          * @param bottomRightVideoInput
          * @param 
          *
          * @return
          */
        IAgentRequestCommand* createSwitchQuadInputsCommand( unsigned long topLeftVideoInput, unsigned long topRightVideoInput, unsigned long bottomLeftVideoInput, unsigned long bottomRightVideoInput, COEQuad& quad );

        
        /** 
          * createSetSequenceConfigCommand
          *
          * description
          *
          * @param dwellTime
          * @param videoInputs
          * @param 
          *
          * @return 
          */
        IAgentRequestCommand* createSetSequenceConfigCommand( unsigned short dwellTime, const std::vector< unsigned long >& videoInputs, COESequence& sequence );


        /** 
          * createSetRecordingUnitModeCommand
          *
          * description
          *
          * @param recordingSpeedIndex
          * @param 
          *
          * @return 
          *
          */
        IAgentRequestCommand* createSetRecordingUnitModeCommand( unsigned short recordingSpeedIndex, COERecordingUnit& recordingUnit );


        /** 
          * createSummaryPollingCommand
          *
          * description
          *
          * @param communicationsChannelIndex
          * @param 
          *
          * @return 
          */
        IAgentRequestCommand* createSummaryPollingCommand( unsigned int communicationsChannelIndex, COECommunicationsHandler& communicationsHandler );


        /** 
          * createRequestMonitorStatusCommand
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        IAgentRequestCommand* createRequestMonitorStatusCommand( COEVideoOutput& videoOutput );


        /** 
          * createMonitorStatusNotificationCommand
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        IHardwareNotificationCommand* createMonitorStatusNotificationCommand( const std::vector< unsigned char >& notificationData );


        /** 
          * createRequestQuadStatusCommand
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        IAgentRequestCommand* createRequestQuadStatusCommand( COEQuad& quad );


        /** 
          * createQuadStatusNotificationCommand
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        IHardwareNotificationCommand* createQuadStatusNotificationCommand( const std::vector< unsigned char >& notificationData );


        /** 
          * createTriggeringAlarmNotificationCommand
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        IAgentRequestCommand* createRequestTriggeringEventCommand();


        /** 
          * createTriggeringAlarmNotificationCommand
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        IHardwareNotificationCommand* createTriggeringEventNotificationCommand( const std::vector< unsigned char >& notificationData );


        /** 
          * createTriggeringAlarmClearNotificationCommand
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        IAgentRequestCommand* createRequestTriggeringEventClearCommand( const AlarmState& alarmState );


        /** 
          * createTriggeringAlarmClearNotificationCommand
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        IAgentRequestCommand* createTriggeringEventAckCommand( const AlarmState& alarmState, unsigned long monitorId );


        /** 
          * createRequestSequenceConfigCommand
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        IAgentRequestCommand* createRequestSequenceConfigCommand( COESequence& sequence );


        /** 
          * createQSequenceConfigNotificationCommand
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        IHardwareNotificationCommand* createSequenceConfigNotificationCommand( const std::vector< unsigned char >& notificationData );


        /** 
          * createRequestRecordingUnitStatusCommand
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        IAgentRequestCommand* createRequestRecordingUnitStatusCommand( COERecordingUnit& recordingUnit );


        /** 
          * createRecordingUnitStatusNotificationCommand
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        IHardwareNotificationCommand* createRecordingUnitStatusNotificationCommand( const std::vector< unsigned char >& notificationData );


        // ++ Lorna TD#13212
        /** 
          * createBVSStatusNotificationCommand
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        IHardwareNotificationCommand* createBVSStatusNotificationCommand( const std::vector< unsigned char >& notificationData );
        // ++ Lorna


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////  Array used to map incoming data against valid packet lengths and NotificationCommandCreator functions.         ////
        ////  Note - if NotificationCommandCreator not applicable for command type (i.e. request only) then set creator      ////
        ////  to null. Note, this static array is used by ReceivedDataProcessor to build an STL map. The purpose of using    ////
        ////  a static array here (instead of hardcoding it in the processor class) was to avoid hardcoding protocol related ////
        ////  information in any file other than this.                                                                       ////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        typedef IHardwareNotificationCommand* ( *HardwareNotificationCommandCreator )( const std::vector< unsigned char >& notificationData );
        struct ReceivedDataProcessingInfo
        {
            unsigned char command;
            unsigned int minResponsePacketLength;
            unsigned int maxResponsePacketLength;
            HardwareNotificationCommandCreator creator;
        };

        const unsigned int RECEIVED_DATA_PROCESSING_INFO_ARRAY_SIZE = 20;
        const ReceivedDataProcessingInfo RECEIVED_DATA_PROCESSING_INFO_ARRAY[ RECEIVED_DATA_PROCESSING_INFO_ARRAY_SIZE ] =
        {
            { CLEAR_COMMAND,
              CLEAR_MIN_RESPONSE_PACKET_LENGTH, 
              CLEAR_MAX_RESPONSE_PACKET_LENGTH, 
              0 },

            { REC_UNIT_MODE_COMMAND,
              REC_UNIT_MODE_MIN_RESPONSE_PACKET_LENGTH, 
              REC_UNIT_MODE_MAX_RESPONSE_PACKET_LENGTH, 
              0 },

            { PROG_SEQ_COMMAND,
              PROG_SEQ_MIN_RESPONSE_PACKET_LENGTH, 
              PROG_SEQ_MAX_RESPONSE_PACKET_LENGTH, 
              0 },

            { PROG_QUAD_COMMAND,
              PROG_QUAD_MIN_RESPONSE_PACKET_LENGTH, 
              PROG_QUAD_MAX_RESPONSE_PACKET_LENGTH, 
              0 },

            { CAM_AND_MON_COMMAND,
              CAM_AND_MON_MIN_RESPONSE_PACKET_LENGTH, 
              CAM_AND_MON_MAX_RESPONSE_PACKET_LENGTH, 
              0 },

            { SEQ_AND_MON_COMMAND,
              SEQ_AND_MON_MIN_RESPONSE_PACKET_LENGTH, 
              SEQ_AND_MON_MAX_RESPONSE_PACKET_LENGTH, 
              0 },

            { CTRL_SEQ_COMMAND,
              CTRL_SEQ_MIN_RESPONSE_PACKET_LENGTH, 
              CTRL_SEQ_MAX_RESPONSE_PACKET_LENGTH, 
              0 },

            { QUAD_AND_MON_COMMAND,
              QUAD_AND_MON_MIN_RESPONSE_PACKET_LENGTH, 
              QUAD_AND_MON_MAX_RESPONSE_PACKET_LENGTH, 
              0 },

            { TRAIN_AND_MON_COMMAND,
              TRAIN_AND_MON_MIN_RESPONSE_PACKET_LENGTH, 
              TRAIN_AND_MON_MAX_RESPONSE_PACKET_LENGTH, 
              0 },

            { TRAIN_ALARM_ACK_COMMAND,
              TRAIN_ALARM_ACK_MIN_RESPONSE_PACKET_LENGTH, 
              TRAIN_ALARM_ACK_MAX_RESPONSE_PACKET_LENGTH, 
              0 },

            { TRAIN_ALARM_CLEAR_COMMAND,
              TRAIN_ALARM_CLEAR_MIN_RESPONSE_PACKET_LENGTH, 
              TRAIN_ALARM_CLEAR_MAX_RESPONSE_PACKET_LENGTH, 
              0 },

            { BVS_STATUS_COMMAND,
              BVS_STATUS_MIN_RESPONSE_PACKET_LENGTH, 
              BVS_STATUS_MAX_RESPONSE_PACKET_LENGTH, 
              // ++  Lorna TD#13212 
              // 0 }, Lorna
              createBVSStatusNotificationCommand}, 
              // ++ Lorna
             
            { QUAD_STATUS_COMMAND,
              QUAD_STATUS_MIN_RESPONSE_PACKET_LENGTH, 
              QUAD_STATUS_MAX_RESPONSE_PACKET_LENGTH, 
              createQuadStatusNotificationCommand },
              
            { SEQUENCE_INFO_COMMAND,
              SEQUENCE_INFO_MIN_RESPONSE_PACKET_LENGTH, 
              SEQUENCE_INFO_MAX_RESPONSE_PACKET_LENGTH, 
              createSequenceConfigNotificationCommand },
              
            { REC_UNIT_STATUS_COMMAND,
              REC_UNIT_STATUS_MIN_RESPONSE_PACKET_LENGTH, 
              REC_UNIT_STATUS_MAX_RESPONSE_PACKET_LENGTH, 
              createRecordingUnitStatusNotificationCommand },

            { MON_STATUS_COMMAND,
              MON_STATUS_MIN_RESPONSE_PACKET_LENGTH, 
              MON_STATUS_MAX_RESPONSE_PACKET_LENGTH, 
              createMonitorStatusNotificationCommand },

            { SUMMARY_POLLING_COMMAND,
              SUMMARY_POLLING_MIN_RESPONSE_PACKET_LENGTH, 
              SUMMARY_POLLING_MAX_RESPONSE_PACKET_LENGTH, 
              0 },

            { TRIGGERING_ALARMS_COMMAND,
              TRIGGERING_ALARMS_MIN_RESPONSE_PACKET_LENGTH, 
              TRIGGERING_ALARMS_MAX_RESPONSE_PACKET_LENGTH, 
              createTriggeringEventNotificationCommand },

            { TRIGGERING_ALARM_ACK_COMMAND,
              TRIGGERING_ALARM_ACK_MIN_RESPONSE_PACKET_LENGTH, 
              TRIGGERING_ALARM_ACK_MAX_RESPONSE_PACKET_LENGTH, 
              0 },

            { TRIGGERING_ALARM_CLEAR_COMMAND,
              TRIGGERING_ALARM_CLEAR_MIN_RESPONSE_PACKET_LENGTH, 
              TRIGGERING_ALARM_CLEAR_MAX_RESPONSE_PACKET_LENGTH, 
              0 }
        };

    }
}

#endif // COE_HELPER_H

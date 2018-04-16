/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_simulator/src/CMTypes.h $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Defines the SDTS protocol message types
  */

#ifndef CMTYPES_H
#define CMTYPES_H
#include <vector>
#include "app/radio/radio_simulator/TrainSimulatorLib/src/IRadioCommsObserver.h"

//#include "app/radio/radio_simulator/CommandProcessorLib/src/HeartBeatCommandProcessor.h"

enum Command
{
	OFF = 0,
	ON = 1
};

enum CMPDUType
{
    CMPDUTYPE_DATA_REQ                  = 0x00,
    CMPDUTYPE_REPORT_RESP               = 0x01,
    CMPDUTYPE_REGISTER_REQ              = 0x02,
    CMPDUTYPE_REGISTER_RESP             = 0x03,
    CMPDUTYPE_BROADCAST                 = 0x04
};

enum CMMessageFormat
{
	CM_SDTS_TL_UNITDATA_INDEX                 = 1,
	CM_PDUTYPE_INDEX_POSITION                 = 2,
	ADAPTATION_LENGTH_POSITION                = 2,
	TL_LENGTH								  = 2,
	CM_SDTS_DATA_SRCADD_INDEX                 = 3,
	CM_REG_PASSNUMBER_INDEX_POSITION          = 3,
	CM_SDTS_TL_DATA_INDEX                     = 6,
	CM_SDTS_DATA_DSTADD_INDEX                 = 6,
	CM_REG_ISSI_INDEX_POSITION                = 7,
	CM_SDTS_DATA_PI_INDEX                     = 9,
	CM_REG_REGISTRATIONTYPE_INDEX_POSITION    = 10,
	CM_SDTS_DATA_FLAGS_INDEX                  = 10,
	CM_REGISTER_REQUEST_LENGTH_SIZE           = 11,
	CM_SDTS_DATA_MSGREF_INDEX                 = 11,
	CM_SDTS_DATA_AREASEL_INDEX                = 12,
	CM_SDTS_DATA_TLLENGTH_INDEX               = 13,
	CM_SDTS_DATA_LENGTH                       = 15,
	CM_SDTS_BROADCAST_TLLENGTH_INDEX_POSITION = 17,
	CM_SDTS_BRAODCAST_LENGTH                  = 19,

	USER_DATA_INDEX                           = 22,
	USER_DATA_COMMAND_INDEX                   = 22,
	USER_DATA_FLAG                            = 7,
	CRC_LENGTH                                = 2
};

enum CMProtocolIdentifier
{
    CMPROTOCOLIDENTIFIER_TEXTMSG        = 130,
    CMPROTOCOLIDENTIFIER_TRAINBORNEMSG  = 192,
    CMPROTOCOLIDENTIFIER_LONGTEXTMSG    = 123       // TODO: not yet determined
};

enum CMChannelSelectionMask
{
    CMCHANNELSELECTIONMASK_MCCH         = 0x01,
    CMCHANNELSELECTIONMASK_PDCH         = 0x02,
    CMCHANNELSELECTIONMASK_GROUP        = 0x04,
    CMCHANNELSELECTIONMASK_RESERVED     = 0XF8
};

enum CMServiceSelection
{
    CMSERVICESELECTION_INDIVIDUAL       = 0x00,
    CMSERVICESELECTION_GROUP            = 0x01
};

enum CMRegistrationStatus
{
    CMREGISTERSTATUS_ADDRESSREGISTERED	= 0X00,
    CMREGISTERSTATUS_DUPLICATEADDRESS	= 0X01,
    CMREGISTERSTATUS_INVALIDADDRESS	= 0X02,
    CMREGISTERSTATUS_REGISTRATIONNOTAVAILABLE	= 0X03,
    CMREGISTERSTATUS_INVALIDPASSNUMBER	= 0X04
};

enum TLDeliveryStatus
{
    TLDELIVERYSTATUS_NREQ               = 0x00,
    TLDELIVERYSTATUS_RECEIVE_REQ        = 0x01,
    TLDELIVERYSTATUS_CONSUME_REQ        = 0x02,
    TLDELIVERYSTATUS_RECON_REQ          = 0X03
};

// Delivery status codes
enum CMDeliveryStatus
{
	CMDELIVERYSTATUS_SUCCESS					= 0x00,
    CMDELIVERYSTATUS_NET_OVERLOAD               = 0x40,
    CMDELIVERYSTATUS_SERVICE_DISABLED           = 0x41,
    CMDELIVERYSTATUS_SERVICE_TEMP_NAVAIL        = 0x42,
    CMDELIVERYSTATUS_DEST_NAUTHORIZED           = 0x44,
    CMDELIVERYSTATUS_UKNOWN_DEST                = 0x45,
    CMDELIVERYSTATUS_GROUP_ADDR_W_INDIVIDUAL    = 0x47,
    CMDELIVERYSTATUS_NREACHABLE                 = 0x4A,
    CMDELIVERYSTATUS_NREGISTERED                = 0x4B,
    CMDELIVERYSTATUS_DEST_QUEUE_FULL            = 0x4C,
    CMDELIVERYSTATUS_MSG_TOO_LONG               = 0x4D,
    CMDELIVERYSTATUS_DEST_NSUPPORT_SDS          = 0x4E,
    CMDELIVERYSTATUS_DEST_NCONNECTED            = 0x4F,
    CMDELIVERYSTATUS_PROTOCOL_NSUPPORTED        = 0x50,
    CMDELIVERYSTATUS_DEST_NACCEPTING_SDS        = 0x53,
    CMDELIVERYSTATUS_DEST_ADM_PROHIBITED        = 0x56,
    CMDELIVERYSTATUS_UNKNOWN_SUBSCRIBER         = 0x58,
    CMDELIVERYSTATUS_DEST_NREACHABLE_FAILED     = 0x8D,
    CMDELIVERYSTATUS_CORRUPT_DISCARDED          = 0x8E
};

enum CMRegistrationType
{
    CMREGISTRATIONTYPE_DEFAULT = 0x00
};

enum CMRepeatRequest
{
    CMREPEATREQ_NREPEAT = 0x00,
    CMREPEATREQ_REPEAT  = 0x01
};

enum CMSDSType
{
    CMSDSTYPE_1         = 0x00,
    CMSDSTYPE_2         = 0x01,
    CMSDSTYPE_3         = 0x02,
    CMSDSTYPE_4         = 0x04
};

enum TLPDUType
{
    TLPDUTYPE_DATA        = 0x00,
    TLPDUTYPE_REPORT      = 0x01,
	TLPDUTYPE_UNITDATA    = 0x02,
	TLPDUTYPE_SHORTREPORT = 0x03
};


enum TextCodingScheme
{
	TEXTCODING_ISO_88591 =  0x01, // ISO8859-1 - 8 bits
	TEXTCODING_ISO_UCS2  =  0x1A  //ISO/IEC 10646-1 UCS2 - 16 bits char
};


enum CommandType
{
	PA_LIVE_ANNOUNCEMENT = 0x0B,
	PA_PRERECORDED_ANNOUNCEMENT = 0x0C,
	PA_LIVE_RESET = 0x0E,
    PA_PRERECORDED_RESET = 0x0F,
	PA_CONTINUE = 0x12,
	PA_COMMAND_RECEIVED = 0x13,
	READY_FOR_LIVE_ANNOUNCEMENT = 0x14,
	REQUEST_FOR_PA_RESET = 0x16,
	TRAIN_PA_MESSAGE_COMPLETE = 0x18,
	TTIS_FREE_TEXT_MESSAGE = 0x1F,
	TTIS_PRESTORED_MESSAGE = 0x20,
	TTIS_LIBRARY_DOWNLOAD = 0x21,
	TTIS_MESSAGE_RESET = 0x23,
	END_OF_TTIS_DOWNLOAD = 0x27,
	TTIS_COMMAND_RECEIVED = 0x2A,
	TTIS_LIBRARY_UPGRADE = 0x2B,
	PEC_ANSWER = 0x33,
	PEC_RESET = 0x34,
	PEC_ACTIVATED = 0x35,
	PEC_ANSWERED_BY_DRIVER = 0x36,
	PEC_COMMAND_RECEIVED = 0x37,
	READY_FOR_PEC_CONVERSATION = 0x38,
	PEC_RESET_RECEIVED = 0x3B,
	PEC_HOLD = 0x3C,
	ALARM_TABLE = 0x3D,
	CHANGEOVER_STATUS = 0x41,
	REQUEST_FOR_OCC_CALL = 0x50,
	BAD_COMMAND = 0x42,
	TEST_CALL = 0x47,
	TEST_CALL_RESULT = 0x48,
	TETRA_ISCS_MODE = 0x49,
	CHANGE_AREA = 0x4B,
	CHANGE_AREA_RECEIVED = 0x4D,
	INVALID_COMMAND
};

enum ORIGINAL_COMMAND1
{
	OC1_PA_LIVE_ANNOUNCEMENT = 0,
	OC1_PA_PRERECORDED_ANNOUNCEMENT = 1,
    OC1_DVA_PA_RESET = 3,
	OC1_LIVE_PA_RESET = 4
};

enum ORIGINAL_COMMAND2
{
	OC2_TTIS_FREE_TEXT_MESSAGE = 0,
	OC2_TTIS_PRESTORED_MESSAGE = 1,
	OC2_TTIS_LIBRARY_DOWNLOAD = 2,
	OC2_TTIS_LIBRARY_UPGRADE = 3,
	OC2_TTIS_MESSAGE_RESET = 4
};

enum ORIGINAL_COMMAND3
{
	OC3_BAD_CHECKSUM = 1,
	OC3_UNKNOWN_COMMAND = 2,
	OC3_BAD_PARAMETERS = 3,
	OC3_UNEXPECTED_COMMAND = 4
};

enum ERROR_CODE
{
	BAD_CHECKSUM = 1,
	UNKNOWN_COMMAND = 2,
	BAD_PARAMETERS = 3,
	UNEXPECTED_COMMAND = 4
};

class SDSMessageData 
{
public:
	typedef std::vector<unsigned char> VMessage;
	SDSMessageData(const VMessage data,VMessage resp)
	{
		m_Response = resp;
		m_Data = data;
	}
	~SDSMessageData()
	{
		m_Data.clear();
	}
	inline char getCommandType() { return m_Data[USER_DATA_COMMAND_INDEX]; }
	inline VMessage getResponse() const { return m_Response; }
	inline VMessage getData() const { return m_Data; }
private:
	VMessage m_Data;
	VMessage m_Response;
};

typedef std::vector<unsigned char> SOCKETDATA;

struct TrainHeartBeatInfo
{
	unsigned int TrainID;
	unsigned int DelayTime;
	std::string StatusInfo;
};

struct HeartBeatInfo
{
	std::vector<int> TrainID;
	std::vector<int> TimeDelay;
	std::vector<std::string> TrainStatus;
	int CommsStatus;
};

#endif

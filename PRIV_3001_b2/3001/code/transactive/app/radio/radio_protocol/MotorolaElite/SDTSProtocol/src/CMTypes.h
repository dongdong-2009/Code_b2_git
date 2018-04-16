/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMTypes.h $
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


enum CMPDUType
{
    CMPDUTYPE_DATA_REQ                  = 0x00,
    CMPDUTYPE_REPORT_RESP               = 0x01,
    CMPDUTYPE_REGISTER_REQ              = 0x02,
    CMPDUTYPE_REGISTER_RESP             = 0x03,
    CMPDUTYPE_BROADCAST                 = 0x04
};

enum CMProtocolIdentifier
{
    CMPROTOCOLIDENTIFIER_TEXTMSG        = 0xFF,		// not used
    CMPROTOCOLIDENTIFIER_TRAINBORNEMSG  = 130,
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

enum TLDeliveryStatus
{
    TLDELIVERYSTATUS_CODINGSCHEME       = 0x51,
    TLDELIVERYSTATUS_MEMORYFULL         = 0x52,
    TLDELIVERYSTATUS_RECEIVED           = 0x00,
    TLDELIVERYSTATUS_CONSUMED           = 0X02
};

// Delivery status codes
enum CMDeliveryStatus
{
	CMDELIVERYSTATUS_NONE						= 0x00,
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
    CMSDSTYPE_4         = 0x03,
	CMSDSTYPE_5         = 0x04,
	CMSDSTYPE_6         = 0x05,
	CMSDSTYPE_7         = 0x06,
	CMSDSTYPE_8         = 0x07
	
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

#endif

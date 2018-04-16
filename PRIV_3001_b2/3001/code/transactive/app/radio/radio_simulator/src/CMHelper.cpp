/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_simulator/src/CMHelper.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Helper methods to encode/decode SDTS messages
  */


#include "app/radio/radio_simulator/src/CMHelper.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
#include "RadioCommsObserver.h"

#include <ostream>
#include <sstream>
#include <math.h>

using namespace std;
using namespace TA_Base_Core;

vector<unsigned char> CMHelper::encodeSSI(unsigned int x)
{
    vector<unsigned char> ret;

	ret.push_back((x & 0xFF0000) >> 16);
	ret.push_back((x & 0xFF00) >> 8);
	ret.push_back((x & 0xFF));

    return ret;
}


unsigned int CMHelper::decodeSSI(const vector<unsigned char> &x)
{

	TA_ASSERT( x.size() == 3, "Missing SSI data");
	unsigned int ssi = (x[0] << 16) +(x[1] << 8) + x[2];

    return ssi;
}

string CMHelper::ssiToString(unsigned int x)
{
	ostringstream data;
	data << x;

    return data.str();
}

string CMHelper::ssiToString(const vector<unsigned char> &x)
{
	return ssiToString(decodeSSI(x));
}

std::vector<unsigned char> CMHelper::encodeCM_RegistrationType()
{
	 vector<unsigned char> ret(1,0x00);

    return ret;
}

std::vector<unsigned char> CMHelper::encodeCM_PDUType(CMPDUType x)
{
    vector<unsigned char> ret(1,x);

    return ret;
}


CMPDUType CMHelper::decodeCM_PDUType(const std::vector<unsigned char> &x)
{
	CMPDUType cmPduType;

	TA_ASSERT( x.size() == 1, "Missing CM PDU Type");
	switch(x[0])
	{
	case CMPDUTYPE_DATA_REQ:
		 cmPduType = CMPDUTYPE_DATA_REQ;
		 break;
	case CMPDUTYPE_REPORT_RESP:
		 cmPduType = CMPDUTYPE_REPORT_RESP;
		 break;
	case CMPDUTYPE_REGISTER_REQ:
		 cmPduType = CMPDUTYPE_REGISTER_REQ;
		 break;
	case CMPDUTYPE_REGISTER_RESP:
		 cmPduType = CMPDUTYPE_REGISTER_RESP;
		 break;
	case CMPDUTYPE_BROADCAST:   
		 cmPduType = CMPDUTYPE_BROADCAST;
		 break;
    default:
        {
            ostringstream oss;
            oss << "Bad PDU type:" << x[0];
            TA_THROW(TransactiveException(oss.str()));
        }

	}
	return cmPduType;
}

std::string CMHelper::CM_PDUTypeToString(CMPDUType x)
{
	ostringstream data;
	data << x;

    return data.str();
}

std::string CMHelper::CM_PDUTypeToString(const std::vector<unsigned char> &x)
{
    return CM_PDUTypeToString(decodeCM_PDUType(x));
	
}

std::vector<unsigned char> CMHelper::encodeCM_ProtocolIdentifier(CMProtocolIdentifier x)
{
    FUNCTION_ENTRY("encodeCM_ProtocolIdentifier");
    vector<unsigned char> ret(1,x);

    FUNCTION_EXIT;
    return ret;
}

CMProtocolIdentifier CMHelper::decodeCM_ProtocolIdentifier(const std::vector<unsigned char> &x)
{
	CMProtocolIdentifier pi;

	TA_ASSERT( x.size() == 1, "Missing Protocol Identifier");
	
	switch(x[0])
	{
	case CMPROTOCOLIDENTIFIER_TEXTMSG:
		pi = CMPROTOCOLIDENTIFIER_TEXTMSG;
		break;
	case CMPROTOCOLIDENTIFIER_TRAINBORNEMSG:
		pi = CMPROTOCOLIDENTIFIER_TRAINBORNEMSG;
		break;
	case CMPROTOCOLIDENTIFIER_LONGTEXTMSG:
		pi = CMPROTOCOLIDENTIFIER_LONGTEXTMSG;
        break;

    default:
        {
            ostringstream oss;
            oss << "Bad PI type:" << x[0];
            TA_THROW(TransactiveException(oss.str()));
        }

	}

	return pi;
}

std::string CMHelper::CM_ProtocolIdentifierToString(CMProtocolIdentifier x)
{
	ostringstream data;
	data << x;

    return data.str();
}

std::string CMHelper::CM_ProtocolIdentifierToString(const std::vector<unsigned char> &x)
{
	return CM_ProtocolIdentifierToString(decodeCM_ProtocolIdentifier(x));
}

std::vector<unsigned char> CMHelper::encodeCM_Flag(CMSDSType sdsType, CMServiceSelection select)
{
	unsigned char cmFlag = (sdsType << 5) + (select << 4);
	vector<unsigned char> ret(1,cmFlag);

    return ret;
}

CMSDSType CMHelper::decodeCM_SDSType(const std::vector<unsigned char> &x )
{
	TA_ASSERT( x.size() == 1, "Missing SDS Type");

	CMSDSType sdsType;
	unsigned char value = x[0] >> 5;

	switch(value)
	{
	case CMSDSTYPE_1: 
		 sdsType = CMSDSTYPE_1;
		 break;
	case CMSDSTYPE_2:
		 sdsType = CMSDSTYPE_2;
		 break;
	case CMSDSTYPE_3:
		 sdsType = CMSDSTYPE_3;
		 break;
	case CMSDSTYPE_4:
		 sdsType = CMSDSTYPE_4;
         break;

    default:
        {
            ostringstream oss;
            oss << "Bad SDS type:" << value;
            TA_THROW(TransactiveException(oss.str()));
        }

	}
	return sdsType;
}


std::string CMHelper::CM_SDSTypeToString(CMSDSType x)
{
	ostringstream data;
	data << x;

    return data.str();
}

std::string CMHelper::CM_SDSTypeToString(const std::vector<unsigned char> &x)
{
    return CM_SDSTypeToString(decodeCM_SDSType(x));
}

CMServiceSelection CMHelper::decodeCM_ServiceSelection(const std::vector<unsigned char> &x)
{
	TA_ASSERT( x.size() == 1, "Missing Service Selection");

	CMServiceSelection oSelection;

	unsigned char value = (x[0]>>4) & 1;

	switch(value)
	{
	case 0x00:
        oSelection = CMSERVICESELECTION_INDIVIDUAL;
        break;
	case 0x01:
        oSelection = CMSERVICESELECTION_GROUP;
        break;

    default:
        {
            ostringstream oss;
            oss << "Bad Service Selection:" << value;
            TA_THROW(TransactiveException(oss.str()));
        }


	}
	return oSelection;
}

std::string CMHelper::CM_ServiceSelectionToString(CMServiceSelection x)
{
	ostringstream data;
	data << x;

    return data.str();
}


std::string CMHelper::CM_ServiceSelectionToString(const std::vector<unsigned char> &x)
{
    return CM_ServiceSelectionToString(decodeCM_ServiceSelection(x));
}


std::vector<unsigned char> CMHelper::encodeCM_ChannelSelectionMask(CMChannelSelectionMask x)
{
    FUNCTION_ENTRY("encodeCM_ChannelSelectionMask");
    vector<unsigned char> ret(1,x);


    FUNCTION_EXIT;
    return ret;
}

CMChannelSelectionMask CMHelper::decodeCM_ChannelSelectionMask(const std::vector<unsigned char> &x)
{
	TA_ASSERT( x.size() == 1, "Missing Channel Selection Mask");

	CMChannelSelectionMask oChannel;

	switch(x[0])
	{
	case CMCHANNELSELECTIONMASK_MCCH:
		 oChannel = CMCHANNELSELECTIONMASK_MCCH;
		 break;
	case CMCHANNELSELECTIONMASK_PDCH:
		 oChannel = CMCHANNELSELECTIONMASK_PDCH;
		 break;
	case CMCHANNELSELECTIONMASK_GROUP:
		 oChannel = CMCHANNELSELECTIONMASK_GROUP;
		 break;
	case CMCHANNELSELECTIONMASK_RESERVED:
		 oChannel = CMCHANNELSELECTIONMASK_RESERVED;
         break;

    default:
        {
            ostringstream oss;
            oss << "Bad Channel Selection:" << x[0];
            TA_THROW(TransactiveException(oss.str()));
        }

	}
	return oChannel;
}

std::string CMHelper::CM_ChannelSelectionMaskToString(CMChannelSelectionMask x)
{
	ostringstream data;
	data << x;

    return data.str();
}


std::string CMHelper::CM_ChannelSelectionMaskToString(const std::vector<unsigned char> &x)
{
    return CM_ChannelSelectionMaskToString(decodeCM_ChannelSelectionMask(x));
}


std::vector<unsigned char> CMHelper::encodeCM_RepeatRequest(CMRepeatRequest x)
{
    vector<unsigned char> ret(1,x);

    return ret;
}

CMRepeatRequest CMHelper::decodeCM_RepeatRequest(const std::vector<unsigned char> &x)
{
	TA_ASSERT( x.size() == 1, "Missing Repeat Request");
	
	CMRepeatRequest repReq;
	switch(x[0])
	{
	case 0: repReq = CMREPEATREQ_NREPEAT; break;
	case 1: repReq = CMREPEATREQ_REPEAT; break;

    default:
        {
            ostringstream oss;
            oss << "Bad Repeat Request:" << x[0];
            TA_THROW(TransactiveException(oss.str()));
        }

	}

	return repReq;
}

std::string CMHelper::CM_RepeatRequestToString(CMRepeatRequest x)
{
	ostringstream data;
	data << x;

    return data.str();

}
std::string CMHelper::CM_RepeatRequestToString(const std::vector<unsigned char> &x)
{
    return CM_RepeatRequestToString(decodeCM_RepeatRequest(x));
}

std::vector<unsigned char> CMHelper::encodeCM_DeliveryStatus(CMDeliveryStatus x)
{
    vector<unsigned char> ret(1,x);

    return ret;
}

std::vector<unsigned char> CMHelper::encodeCM_RegistrationStatus(CMRegistrationStatus x)
{
    vector<unsigned char> ret(1,x);

    return ret;
}

CMDeliveryStatus CMHelper::decodeCM_DeliveryStatus(const std::vector<unsigned char> &x)
{
	TA_ASSERT( x.size() == 1, "Missing CM Delivery Status");

	CMDeliveryStatus status;

	switch(x[0])
	{
	case CMDELIVERYSTATUS_NET_OVERLOAD:
		status = CMDELIVERYSTATUS_NET_OVERLOAD;
		break;
	case CMDELIVERYSTATUS_SERVICE_DISABLED: 
		status = CMDELIVERYSTATUS_SERVICE_DISABLED;
		break;		
	case CMDELIVERYSTATUS_SERVICE_TEMP_NAVAIL:
		status = CMDELIVERYSTATUS_SERVICE_TEMP_NAVAIL;
		break;
	case CMDELIVERYSTATUS_DEST_NAUTHORIZED:
		status = CMDELIVERYSTATUS_DEST_NAUTHORIZED;
		break;
	case CMDELIVERYSTATUS_UKNOWN_DEST:
		status = CMDELIVERYSTATUS_UKNOWN_DEST;
		break;
	case CMDELIVERYSTATUS_GROUP_ADDR_W_INDIVIDUAL:
		status = CMDELIVERYSTATUS_GROUP_ADDR_W_INDIVIDUAL;
		break;
	case CMDELIVERYSTATUS_NREACHABLE:
		status = CMDELIVERYSTATUS_NREACHABLE;
		break;
	case CMDELIVERYSTATUS_NREGISTERED:
		status = CMDELIVERYSTATUS_NREGISTERED;
		break;
	case CMDELIVERYSTATUS_DEST_QUEUE_FULL:
		status = CMDELIVERYSTATUS_DEST_QUEUE_FULL;
		break;
	case CMDELIVERYSTATUS_MSG_TOO_LONG:
		status = CMDELIVERYSTATUS_MSG_TOO_LONG;
		break;
	case CMDELIVERYSTATUS_DEST_NSUPPORT_SDS:
		status = CMDELIVERYSTATUS_DEST_NSUPPORT_SDS;
		break;
	case CMDELIVERYSTATUS_DEST_NCONNECTED:
		status = CMDELIVERYSTATUS_DEST_NCONNECTED;
		break;
	case CMDELIVERYSTATUS_PROTOCOL_NSUPPORTED:
		status = CMDELIVERYSTATUS_PROTOCOL_NSUPPORTED;
		break;
	case CMDELIVERYSTATUS_DEST_NACCEPTING_SDS:
		status = CMDELIVERYSTATUS_DEST_NACCEPTING_SDS;
		break;
	case CMDELIVERYSTATUS_DEST_ADM_PROHIBITED:
		status = CMDELIVERYSTATUS_DEST_ADM_PROHIBITED;
		break;
	case CMDELIVERYSTATUS_UNKNOWN_SUBSCRIBER:
		status = CMDELIVERYSTATUS_UNKNOWN_SUBSCRIBER;
		break;
	case CMDELIVERYSTATUS_DEST_NREACHABLE_FAILED:
		status = CMDELIVERYSTATUS_DEST_NREACHABLE_FAILED;
		break;
	case CMDELIVERYSTATUS_CORRUPT_DISCARDED:
		status = CMDELIVERYSTATUS_CORRUPT_DISCARDED;
        break;

    default:
        {
            ostringstream oss;
            oss << "Bad delivery status:" << x[0];
            TA_THROW(TransactiveException(oss.str()));
        }

	}
	
	return status;
}


std::string CMHelper::CM_DeliveryStatusToString(CMDeliveryStatus x)
{
	ostringstream data;
	data << x;

    return data.str();
}

std::string CMHelper::CM_DeliveryStatusToString(const std::vector<unsigned char> &x)
{

    return CM_DeliveryStatusToString(decodeCM_DeliveryStatus(x));
}


std::vector<unsigned char> CMHelper::encodeTL_PDUType(TLPDUType x)
{
    vector<unsigned char> ret;

	ret.push_back(x);

	return ret;
}

TLPDUType CMHelper::decodeTL_PDUType(const std::vector<unsigned char> &x)
{
	TA_ASSERT( x.size() == 1, "Missing TL PDU Type");

	TLPDUType type;

	switch(x[0])
	{
	case TLPDUTYPE_DATA: 
		type = TLPDUTYPE_DATA;
        break;
	case TLPDUTYPE_REPORT: 
		type = TLPDUTYPE_REPORT;
        break;
	case TLPDUTYPE_UNITDATA:
		type = TLPDUTYPE_UNITDATA;
        break;
	case TLPDUTYPE_SHORTREPORT:
		type = TLPDUTYPE_SHORTREPORT; 
        break;

    default:
        {
            ostringstream oss;
            oss << "Bad TL PDU type:" << x[0];
            TA_THROW(TransactiveException(oss.str()));
        }

	}
	return type;
}

std::string CMHelper::TL_PDUTypeToString(TLPDUType x)
{
	ostringstream data;
	data << x;

    return data.str();
}


std::string CMHelper::TL_PDUTypeToString(const std::vector<unsigned char> &x)
{
    return TL_PDUTypeToString(decodeTL_PDUType(x));
}


std::vector<unsigned char> CMHelper::encodeTL_DeliveryStatus(TLDeliveryStatus x)
{
    vector<unsigned char> ret(1,x);

	return ret;
}

TLDeliveryStatus CMHelper::decodeTL_DeliveryStatus(const std::vector<unsigned char> &x)
{
	TA_ASSERT( x.size() == 1, "Missing TL Delivery Status");

	TLDeliveryStatus status;

	switch(x[0])
	{
	case TLDELIVERYSTATUS_NREQ:
		status = TLDELIVERYSTATUS_NREQ;
		break;
	case TLDELIVERYSTATUS_RECEIVE_REQ:
		status = TLDELIVERYSTATUS_RECEIVE_REQ;
		break;
	case TLDELIVERYSTATUS_CONSUME_REQ:
		status = TLDELIVERYSTATUS_CONSUME_REQ;
		break;
	case TLDELIVERYSTATUS_RECON_REQ:
		status = TLDELIVERYSTATUS_RECON_REQ;
        break;

    default:
        {
            ostringstream oss;
            oss << "Bad Delivery Status:" << x[0];
            TA_THROW(TransactiveException(oss.str()));
        }

	}

	return status;
}

std::string CMHelper::TL_DeliveryStatusToString(TLDeliveryStatus x)
{
	ostringstream data;
	data << x;

    return data.str();
}

std::string CMHelper::TL_DeliveryStatusToString(const std::vector<unsigned char> &x)
{
    return TL_DeliveryStatusToString(decodeTL_DeliveryStatus(x));
}


std::vector<unsigned char> CMHelper::encodeTL_Flag(bool recFlag, bool conFlag, bool stoFlag)    
{
	unsigned char flag = (recFlag << 7) + (conFlag << 6) + (stoFlag << 5);

	vector<unsigned char> ret(1,flag);

	return ret;
}

bool CMHelper::decodeTL_REC(unsigned char x)
{
	bool flag = false;
		
	if ( x >> 7 )
		flag =true;

	return flag;
}

bool CMHelper::decodeTL_CON(unsigned char x)
{
	bool flag = ((x >> 6) & 1) == 1;

	return flag;
}

bool CMHelper::decodeTL_STO(unsigned char x)
{
	bool flag = ((x >> 5) & 1) == 1;

	return flag;	
}

std::vector<unsigned char> CMHelper::encode_u16( unsigned short x)
{
	std::vector<unsigned char> ret;

	ret.push_back((x & 0xFF00) >> 8);
	ret.push_back((x & 0xFF));
	
	return ret;
}

std::vector<unsigned char> CMHelper::encode_u24( unsigned long x)
{
	std::vector<unsigned char> ret;

	ret.push_back(static_cast<unsigned char>((x & 0xFF0000) >> 16));
	ret.push_back(static_cast<unsigned char>((x & 0xFF00) >> 8));
	ret.push_back(static_cast<unsigned char>((x & 0xFF)));

	return ret;
}

std::vector<unsigned char> CMHelper::encode_u32( unsigned long x)
{
	std::vector<unsigned char> ret;

	ret.push_back(static_cast<unsigned char>( (x & 0xFF000000) >> 24));
	ret.push_back(static_cast<unsigned char>( (x & 0xFF0000) >> 16));
	ret.push_back(static_cast<unsigned char>( (x & 0xFF00) >> 8));
	ret.push_back(static_cast<unsigned char>( (x & 0xFF)));

	return ret;
}

unsigned short CMHelper::decode_u16( const std::vector<unsigned char> &x)
{
	TA_ASSERT( x.size() <= 2, "Incorrect data input on unsigned short value");

	unsigned short value = static_cast<unsigned short>((x[0] << 8) + x[1]);
	
	return value;
}

unsigned long CMHelper::decode_u32( const std::vector<unsigned char> &x)
{
	TA_ASSERT( x.size() <= 4, "Incorrect data input on unsigned long value");

	unsigned long value = 0;

	std::vector<unsigned char>::const_reverse_iterator iter;

	int shift = 0;

	for ( iter = x.rbegin(); iter< x.rend(); ++iter)
	{
		if ( shift == 0)
		{
			value += *iter;
		}
		else
		{
			value += *iter << shift;
		}
		shift += 8;
	}
	return value;
}
//
//ostream& operator<<(ostream& os, CMPDUType dat)
//{
//    string sdat;
//    switch(dat)
//    {
//        case CMPDUTYPE_DATA_REQ:
//            sdat = "Data request";
//            break;
//
//        case CMPDUTYPE_REPORT_RESP:
//            sdat = "Report response";
//            break;
//
//        case CMPDUTYPE_REGISTER_REQ:
//            sdat = "Register request";
//            break;
//
//        case CMPDUTYPE_REGISTER_RESP:
//            sdat = "Register response";
//            break;
//
//        case CMPDUTYPE_BROADCAST:
//            sdat = "Broadcast";
//            break;
//        default:
//            sdat = "<UNKNOWN>";
//    }
//    
//	os << "[CMPDUType=(" << dat << ")" << sdat << "]";
//    return os;
//}
//
//ostream& operator<<(ostream& os, CMProtocolIdentifier dat)
//{
//    string sdat;
//    switch(dat)
//    {
//        case CMPROTOCOLIDENTIFIER_TEXTMSG:
//            sdat = "Text Message";
//            break;
//
//        case CMPROTOCOLIDENTIFIER_TRAINBORNEMSG:
//            sdat = "Trainborne Message";
//            break;
//
//        case CMPROTOCOLIDENTIFIER_LONGTEXTMSG:
//            sdat = "Long Text Message";
//            break;
//
//        default:
//            sdat = "<UNKNOWN>";
//    }
//    
//    os << "[CMProtocolIdentifier=(" << dat << ")" << sdat << "]";
//    
//    return os;
//}
//
//ostream& operator<<(ostream& os, CMChannelSelectionMask dat)
//{
//    string sdat;
//    switch(dat)
//    {
//        case CMCHANNELSELECTIONMASK_MCCH:
//            sdat = "MCCH";
//            break;
//
//        case CMCHANNELSELECTIONMASK_PDCH:
//            sdat = "PDCH";
//            break;
//
//        case CMCHANNELSELECTIONMASK_GROUP:
//            sdat = "Group";
//            break;
//
//        case CMCHANNELSELECTIONMASK_RESERVED:
//            sdat = "Reserved";
//            break;
//
//        default:
//            sdat = "<UNKNOWN>";
//    }
//
//    os << "[CMChannelSelectionMask=(" << dat << ")" << sdat << "]";
//    
//    return os;
//}
//
//ostream& operator<<(ostream& os, CMDeliveryStatus dat)
//{
//    string sdat;
//    switch(dat)
//    {
//        case CMDELIVERYSTATUS_NET_OVERLOAD:
//            sdat = "Network overload";
//            break;
//
//        case CMDELIVERYSTATUS_SERVICE_DISABLED:
//            sdat = "Service disabled";
//            break;
//
//        case CMDELIVERYSTATUS_SERVICE_TEMP_NAVAIL:
//            sdat = "Service temporary not available";
//            break;
//
//        case CMDELIVERYSTATUS_DEST_NAUTHORIZED:
//            sdat = "Destination not authorized";
//            break;
//
//        case CMDELIVERYSTATUS_UKNOWN_DEST:
//            sdat = "Unknown destination";
//            break;
//
//        case CMDELIVERYSTATUS_GROUP_ADDR_W_INDIVIDUAL:
//            sdat = "Group address";
//            break;
//
//        case CMDELIVERYSTATUS_NREACHABLE:
//            sdat = "Not reachable";
//            break;
//
//        case CMDELIVERYSTATUS_NREGISTERED:
//            sdat = "Not registered";
//            break;
//
//        case CMDELIVERYSTATUS_DEST_QUEUE_FULL:
//            sdat = "Destination queue full";
//            break;
//
//        case CMDELIVERYSTATUS_MSG_TOO_LONG:
//            sdat = "Message too long";
//            break;
//
//        case CMDELIVERYSTATUS_DEST_NSUPPORT_SDS:
//            sdat = "Destination does not support SDS";
//            break;
//
//        case CMDELIVERYSTATUS_DEST_NCONNECTED:
//            sdat = "Destination not connected";
//            break;
//
//        case CMDELIVERYSTATUS_PROTOCOL_NSUPPORTED:
//            sdat = "Protocol not supported";
//            break;
//
//        case CMDELIVERYSTATUS_DEST_NACCEPTING_SDS:
//            sdat = "Destination not accepting SDS";
//            break;
//
//        case CMDELIVERYSTATUS_DEST_ADM_PROHIBITED:
//            sdat = "Destination administratively prohibited";
//            break;
//
//        case CMDELIVERYSTATUS_UNKNOWN_SUBSCRIBER:
//            sdat = "Unknown subscriber";
//            break;
//
//        case CMDELIVERYSTATUS_DEST_NREACHABLE_FAILED:
//            sdat = "Destination not reachable, delivery failed";
//            break;
//
//        case CMDELIVERYSTATUS_CORRUPT_DISCARDED:
//            sdat = "Data corrupt, data discarded";
//            break;
//
//        default:
//            sdat = "<UNKNOWN>";    
//    }
//    os << "[CMDeliveryStatus=(" << dat << ")" << sdat << "]";
//    
//    return os;
//}
//
//ostream& operator<<(ostream& os, CMRegistrationType dat)
//{
//    string sdat;
//    switch(dat)
//    {
//    case CMREGISTRATIONTYPE_DEFAULT:
//        sdat = "Default";
//        break;
//    default:
//        sdat = "<UNKNOWN>";
//    }
//    os << "[CMRegistrationType=(" << dat << ")" << sdat << "]";
//    
//    return os;
//}
//
//ostream& operator<<(ostream& os, CMRepeatRequest dat)
//{
//    string sdat;
//    switch(dat)
//    {
//        case CMREPEATREQ_NREPEAT:
//            sdat = "No repeat request";
//            break;
//    
//        case CMREPEATREQ_REPEAT:
//            sdat = "Repeat request";
//            break;
//
//        default:
//            sdat = "<UNKNOWN>";
//    }
//    os << "[CMRepeatRequest=(" << dat << ")" << sdat << "]";
//    
//    return os;
//}
//
//ostream& operator<<(ostream& os, CMSDSType dat)
//{
//    string sdat;
//    switch(dat)
//    {
//        case CMSDSTYPE_1:
//            sdat = "Type 1";
//            break;
//    
//        case CMSDSTYPE_2:
//            sdat = "Type 2";
//            break;
//
//        case CMSDSTYPE_3:
//            sdat = "Type 3";
//            break;
//    
//        case CMSDSTYPE_4:
//            sdat = "Type 4";
//            break;
//
//        default:
//            sdat = "<UNKNOWN>";
//    }
//    os << "[CMSDSType=(" << dat << ")" << sdat << "]";
//    
//    return os;
//}
//
//ostream& operator<<(ostream& os, CMServiceSelection dat)
//{
//    string sdat;
//    switch(dat)
//    {
//        case CMSERVICESELECTION_INDIVIDUAL:
//            sdat = "Individual";
//            break;
//    
//        case CMSERVICESELECTION_GROUP:
//            sdat = "Group";
//            break;
//
//        default:
//            sdat = "<UNKNOWN>";
//    }
//    os << "[CMServiceSelection=(" << dat << ")" << sdat << "]";
//    
//    return os;
//}
//
//ostream& operator<<(ostream& os, TLDeliveryStatus dat)
//{
//    string sdat;
//    switch(dat)
//    {
//        case TLDELIVERYSTATUS_NREQ:
//            sdat = "No request";
//            break;
//    
//        case TLDELIVERYSTATUS_RECEIVE_REQ:
//            sdat = "Receive request";
//            break;
//
//        case TLDELIVERYSTATUS_CONSUME_REQ:
//            sdat = "Consume Request";
//            break;
//    
//        case TLDELIVERYSTATUS_RECON_REQ:
//            sdat = "Receive & consume Request";
//            break;
//
//        default:
//            sdat = "<UNKNOWN>";
//    }
//    os << "[TLDeliveryStatus=(" << dat << ")" << sdat << "]";
//  
//    return os;
//}
//
//ostream& operator<<(ostream& os, TLPDUType dat)
//{
//    string sdat;
//    switch(dat)
//    {
//        case TLPDUTYPE_DATA:
//            sdat = "Data";
//            break;
//    
//        case TLPDUTYPE_REPORT:
//            sdat = "Report";
//            break;
//
//        default:
//            sdat = "<UNKNOWN>";
//    }
//    os << "[TLPDUType=(" << dat << ")" << sdat << "]";
//
//    return os;
//}

unsigned int CMHelper::Hex2Int(char character, int pos)
{

	unsigned int num = 0;
	unsigned int result = 0;

	switch(character)
	{
	case '0':	num = 0;
				break;
	case '1':	num = 1;
				break;
	case '2':	num = 2;
				break;
	case '3':	num = 3;
				break;
	case '4':	num = 4;
				break;
	case '5':	num = 5;
				break;
	case '6':	num = 6;
				break;
	case '7':	num = 7;
				break;
	case '8':	num = 8;
				break;
	case '9':	num = 9;
				break;
	case 'a':
	case 'A':	num = 10;
				break;
	case 'b':	
	case 'B':	num = 11;
				break;
	case 'c':
	case 'C':	num = 12;
				break;
	case 'd':
	case 'D':	num = 13;
				break;
	case 'e':	
	case 'E':	num = 14;
				break;
	case 'f':	
	case 'F':	num = 15;
				break;
	}

	result = num * int(pow(16.0, pos));
			
	return result;
	
}

int CMHelper::getDigitNumber(unsigned long number)
{

	if(number==0)
		return 1;
	else
	{
		int counter = 1;
		for(int i=1; i<1000000000; i=i*10)
		{
			if((number/i)<10)
			{
				return counter;
			}
			else
				counter++;
		}
	}
	
	return 0;

}


static std::string toHex(int x)
{
	std::string ret;
	const std::string def = "0123456789ABCDEF";
	ret = ret + def[(x >> 4) & 0xF];
	ret = ret + def[x & 0xF];
	return ret;
}

std::string CMHelper::decode_cmData(const std::string& cmdValue)
{

	// user data
	std::string m_userData = "";

	for(unsigned int j = 0; j<cmdValue.size(); j++)
	{
		m_userData += toHex(cmdValue[j]);
	}

	return m_userData;
}

std::string CMHelper::decode_tlLength(const std::string& cmdValue)
{
	std::string m_tlLength = "";
	// TL Length
	int decLength = (cmdValue.size() + 7) * 8;

	m_tlLength = toHex((decLength >> 8) & 0xFF);
	m_tlLength += toHex(decLength & 0xFF);

//	cout << "user data = " << m_userData << endl;
//	cout << "length = " << m_tlLength << endl;
	return m_tlLength;
}



void CMHelper::buildCMheader(CMData& cmData, std::vector<unsigned char>& rawData)
	{

		//std::vector<unsigned char> rawData;

		std::vector<unsigned char> tempData;

		// PDU type 
		tempData = encodeCM_PDUType( cmData.getCMPduType() );
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

		// source ISSI
		tempData = encodeSSI( cmData.getSrcAddr() );
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

		// destination ISSI
		tempData = encodeSSI( cmData.getDstAddr() );
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

		//Protocol Identifier
		tempData = encodeCM_ProtocolIdentifier( cmData.getProtocolId() );
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

		// CM flags
		//unsigned char cmFlag = (getSDSType() << 5) + (getServiceSelection() << 4);
		vector<unsigned char> tmp(1,cmData.getFlags());
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

		// Message reference
		rawData.push_back(cmData.getMsgRef());

		// Area selection
		rawData.push_back(cmData.getAreaSel());

		// CM TL length will be assigned after TL PDUs created
		
		unsigned int nAdaptation = rawData.size() - 2;
		tempData = encode_u16(nAdaptation);
		rawData.insert( rawData.begin(), tempData.begin(), tempData.end() );
	}

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSHelper.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Helper methods to encode/decode SDTS messages
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSHelper.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"

#include "core/utilities/src/DebugUtil.h"
#include <sstream>
#include <iomanip> 

using namespace std;
using namespace TA_Base_Core;

vector<unsigned char> SDTSHelper::encodeSSI(unsigned int x)
{
    vector<unsigned char> ret;

	ret.push_back((x & 0xFF0000) >> 16);
	ret.push_back((x & 0xFF00) >> 8);
	ret.push_back((x & 0xFF));

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"SSI = %s", ssiToString(x).c_str());
    return ret;
}


unsigned int SDTSHelper::decodeSSI(const vector<unsigned char> &x)
{
	TA_ASSERT( x.size() == 3, "Missing SSI data");

	unsigned int ssi = (x[0] << 16) +(x[1] << 8) + x[2];
	
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"SSI = %s", ssiToString(ssi).c_str());
    return ssi;
}

string SDTSHelper::ssiToString(unsigned int x)
{
	ostringstream data;
	data << x;

    return data.str();
}

string SDTSHelper::ssiToString(const vector<unsigned char> &x)
{
	return ssiToString(decodeSSI(x));
}

std::vector<unsigned char> SDTSHelper::encodeCM_PDUType(CMPDUType x)
{
    vector<unsigned char> ret(1,x);

    return ret;
}


CMPDUType SDTSHelper::decodeCM_PDUType(const std::vector<unsigned char> &x)
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
            oss << "Bad PDU type:" << (int) x[0];
            TA_THROW(TransactiveException(oss.str()));
        }

	}
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"CM_PDU type = %s", CM_PDUTypeToString(cmPduType).c_str());

	return cmPduType;
}

std::string SDTSHelper::CM_PDUTypeToString(CMPDUType x)
{
	ostringstream data;
	data << x;
    return data.str();
}

std::string SDTSHelper::CM_PDUTypeToString(const std::vector<unsigned char> &x)
{
    return CM_PDUTypeToString(decodeCM_PDUType(x));
	
}

std::vector<unsigned char> SDTSHelper::encodeCM_ProtocolIdentifier(CMProtocolIdentifier x)
{
    vector<unsigned char> ret(1,x);

    return ret;
}

CMProtocolIdentifier SDTSHelper::decodeCM_ProtocolIdentifier(const std::vector<unsigned char> &x)
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
            oss << "Bad PI type:" << (int) x[0];
            TA_THROW(TransactiveException(oss.str()));
        }

	}
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Protocol Identifier = %s", CM_ProtocolIdentifierToString(pi).c_str());

	return pi;
}

std::string SDTSHelper::CM_ProtocolIdentifierToString(CMProtocolIdentifier x)
{
	ostringstream data;
	data << x;
    return data.str();
}

std::string SDTSHelper::CM_ProtocolIdentifierToString(const std::vector<unsigned char> &x)
{
	return CM_ProtocolIdentifierToString(decodeCM_ProtocolIdentifier(x));
}

std::vector<unsigned char> SDTSHelper::encodeCM_Flag(CMSDSType sdsType, CMServiceSelection select)
{
	unsigned char cmFlag = (sdsType << 5) + (select << 4);
	vector<unsigned char> ret(1,cmFlag);

    return ret;
}

CMSDSType SDTSHelper::decodeCM_SDSType(const std::vector<unsigned char> &x )
{
	TA_ASSERT( x.size() == 1, "Missing SDS Type");

	CMSDSType sdsType;
	unsigned char value = x[0] >> 5;
	sdsType = (CMSDSType) value;

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"SDS type= %s", CM_SDSTypeToString(sdsType).c_str());

	return sdsType;
}


std::string SDTSHelper::CM_SDSTypeToString(CMSDSType x)
{
	ostringstream data;
	data << x;
    return data.str();
}

std::string SDTSHelper::CM_SDSTypeToString(const std::vector<unsigned char> &x)
{
    return CM_SDSTypeToString(decodeCM_SDSType(x));
}

CMServiceSelection SDTSHelper::decodeCM_ServiceSelection(const std::vector<unsigned char> &x)
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
            oss << "Bad Service Selection:" << (int) value;
            TA_THROW(TransactiveException(oss.str()));
        }
	}

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Service Selection = %s", CM_ServiceSelectionToString(oSelection).c_str());
	return oSelection;
}

std::string SDTSHelper::CM_ServiceSelectionToString(CMServiceSelection x)
{
	ostringstream data;
	data << x;
    return data.str();
}


std::string SDTSHelper::CM_ServiceSelectionToString(const std::vector<unsigned char> &x)
{
    return CM_ServiceSelectionToString(decodeCM_ServiceSelection(x));
}


std::vector<unsigned char> SDTSHelper::encodeCM_ChannelSelectionMask(CMChannelSelectionMask x)
{
    FUNCTION_ENTRY("encodeCM_ChannelSelectionMask");
    //vector<unsigned char> ret(1,x);

	vector<unsigned char> ret = encode_u16(x);

    FUNCTION_EXIT;
    return ret;
}

CMChannelSelectionMask SDTSHelper::decodeCM_ChannelSelectionMask(const std::vector<unsigned char> &x)
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
			oss << "Bad Channel Selection:" << (int) x[0];
			TA_THROW(TransactiveException(oss.str()));
		}
	}

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Channel Selection = %s", CM_ChannelSelectionMaskToString(oChannel).c_str());
	return oChannel;
}

std::string SDTSHelper::CM_ChannelSelectionMaskToString(CMChannelSelectionMask x)
{
	ostringstream data;
	data << x;
    return data.str();
}


std::string SDTSHelper::CM_ChannelSelectionMaskToString(const std::vector<unsigned char> &x)
{
    return CM_ChannelSelectionMaskToString(decodeCM_ChannelSelectionMask(x));
}


std::vector<unsigned char> SDTSHelper::encodeCM_RepeatRequest(CMRepeatRequest x)
{
    vector<unsigned char> ret(1,x);

    return ret;
}

CMRepeatRequest SDTSHelper::decodeCM_RepeatRequest(const std::vector<unsigned char> &x)
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
				oss << "Bad Repeat Request:" << (int) x[0];
				TA_THROW(TransactiveException(oss.str()));
			}
	}

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Repeat Request = %s", CM_RepeatRequestToString(repReq).c_str());

	return repReq;
}

std::string SDTSHelper::CM_RepeatRequestToString(CMRepeatRequest x)
{
	ostringstream data;
	data << x;
    return data.str();

}
std::string SDTSHelper::CM_RepeatRequestToString(const std::vector<unsigned char> &x)
{
    return CM_RepeatRequestToString(decodeCM_RepeatRequest(x));
}


std::vector<unsigned char> SDTSHelper::encodeCM_RegistrationType(CMRegistrationType x)
{
    vector<unsigned char> ret(1, x);
    return ret;
}

CMRegistrationType SDTSHelper::decodeCM_RegistrationType(const std::vector<unsigned char> &x)
{
    TA_ASSERT(x.size() == 1, "Invalid registration type size");

    switch (x[0])
    {
		case CMREGISTRATIONTYPE_DEFAULT:
			return CMREGISTRATIONTYPE_DEFAULT;
		default:
		{
			ostringstream oss;
			oss << "Bad registration type:" << (int) x[0];
			TA_THROW(TransactiveException(oss.str()));
		}
    }
}

std::string SDTSHelper::CM_RegistrationTypeToString(CMRegistrationType x)
{
	ostringstream data;
	data << x;
    return data.str();
}

std::string SDTSHelper::CM_RegistrationTypeToString(const std::vector<unsigned char> &x)
{
    return CM_RegistrationTypeToString(decodeCM_RegistrationType(x));
}


std::vector<unsigned char> SDTSHelper::encodeCM_DeliveryStatus(CMDeliveryStatus x)
{
    vector<unsigned char> ret(1,x);
    return ret;
}

CMDeliveryStatus SDTSHelper::decodeCM_DeliveryStatus(const std::vector<unsigned char> &x)
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
            oss << "Bad delivery status:" << (int) x[0];
            TA_THROW(TransactiveException(oss.str()));
        }
	}

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"CM Delivery Status = %s", CM_DeliveryStatusToString(status).c_str());
	
	return status;
}

std::string SDTSHelper::CM_DeliveryStatusToString(CMDeliveryStatus x)
{
	ostringstream data;
	data << x;
    return data.str();
}

std::string SDTSHelper::CM_DeliveryStatusToString(const std::vector<unsigned char> &x)
{
    return CM_DeliveryStatusToString(decodeCM_DeliveryStatus(x));
}


std::vector<unsigned char> SDTSHelper::encodeTL_PDUType(TLPDUType x)
{
    vector<unsigned char> ret;

	ret.push_back(x);

	return ret;
}

TLPDUType SDTSHelper::decodeTL_PDUType(const std::vector<unsigned char> &x)
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
            oss << "Bad TL PDU type:" << (int) x[0];
            TA_THROW(TransactiveException(oss.str()));
        }
	}

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"TL_PDU type = %s", TL_PDUTypeToString(type).c_str());

	return type;
}

std::string SDTSHelper::TL_PDUTypeToString(TLPDUType x)
{
	ostringstream data;
	data << x;
    return data.str();
}


std::string SDTSHelper::TL_PDUTypeToString(const std::vector<unsigned char> &x)
{
    return TL_PDUTypeToString(decodeTL_PDUType(x));
}


std::vector<unsigned char> SDTSHelper::encodeTL_DeliveryStatus(TLDeliveryStatus x)
{
    vector<unsigned char> ret(1,x);

	return ret;
}

TLDeliveryStatus SDTSHelper::decodeTL_DeliveryStatus(const std::vector<unsigned char> &x)
{
	TA_ASSERT( x.size() == 1, "Missing TL Delivery Status");

	TLDeliveryStatus status;

	switch(x[0])
	{
	case 0x51:
		status = TLDELIVERYSTATUS_CODINGSCHEME;
		break;
	case 0x52:
		status = TLDELIVERYSTATUS_MEMORYFULL;
		break;
	case 0x00:
		status = TLDELIVERYSTATUS_RECEIVED;
		break;
	case 0X02:
		status = TLDELIVERYSTATUS_CONSUMED;
        break;

    default:
        {
            ostringstream oss;
            oss << "Bad Delivery Status:" << (int) x[0];
            TA_THROW(TransactiveException(oss.str()));
        }
	}
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"TL Delivery status = %s", TL_DeliveryStatusToString(status).c_str());

	return status;
}

std::string SDTSHelper::TL_DeliveryStatusToString(TLDeliveryStatus x)
{
	ostringstream data;
	data << x;
    return data.str();
}

std::string SDTSHelper::TL_DeliveryStatusToString(const std::vector<unsigned char> &x)
{
    return TL_DeliveryStatusToString(decodeTL_DeliveryStatus(x));
}


std::vector<unsigned char> SDTSHelper::encodeTL_Flag(bool recFlag, bool conFlag, bool stoFlag)    
{
	unsigned char flag = (recFlag << 7) + (conFlag << 6) + (stoFlag << 5);

	vector<unsigned char> ret(1,flag);

	return ret;
}

bool SDTSHelper::decodeTL_REC(unsigned char x)
{
	bool flag = false;
		
	if ( x >> 7 )
		flag =true;

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"REC = %d",flag);

	return flag;
}

bool SDTSHelper::decodeTL_CON(unsigned char x)
{
	bool flag = ((x >> 6) & 1) == 1;

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"CON = %d",flag);
	return flag;
}

bool SDTSHelper::decodeTL_STO(unsigned char x)
{
	bool flag = ((x >> 5) & 1) == 1;

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"STO = %d",flag);
	return flag;	
}

std::vector<unsigned char> SDTSHelper::encode_u16( unsigned short x)
{
	std::vector<unsigned char> ret;

	ret.push_back((x & 0xFF00) >> 8);
	ret.push_back((x & 0xFF));
	
	return ret;
}

std::vector<unsigned char> SDTSHelper::encode_u24( unsigned long x)
{
	std::vector<unsigned char> ret;

	ret.push_back(static_cast<unsigned char>((x & 0xFF0000) >> 16));
	ret.push_back(static_cast<unsigned char>((x & 0xFF00) >> 8));
	ret.push_back(static_cast<unsigned char>((x & 0xFF)));

	return ret;
}

std::vector<unsigned char> SDTSHelper::encode_u32( unsigned long x)
{
	std::vector<unsigned char> ret;

	ret.push_back(static_cast<unsigned char>( (x & 0xFF000000) >> 24));
	ret.push_back(static_cast<unsigned char>( (x & 0xFF0000) >> 16));
	ret.push_back(static_cast<unsigned char>( (x & 0xFF00) >> 8));
	ret.push_back(static_cast<unsigned char>( (x & 0xFF)));

	return ret;
}

unsigned short SDTSHelper::decode_u16( const std::vector<unsigned char> &x)
{
	TA_ASSERT( x.size() <= 2, "Incorrect data input on unsigned short value");

	unsigned short value = static_cast<unsigned short>((x[0] << 8) + x[1]);
	
	return value;
}

unsigned long SDTSHelper::decode_u32( const std::vector<unsigned char> &x)
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


void SDTSHelper::printData(const std::vector<unsigned char>& rawMessage, bool isReceived)
{
	ostringstream data;

	data<<(isReceived==true ? "-- Data Received from SDR --" : "-- Data Sent to SDR --") <<"\n"; 
	
	data << hex;

	int counter = 0;
	int number=0;  //ISCS log system can only support up to 2000 characters.
	std::vector<unsigned char>::const_iterator iter;
	
	for(iter=rawMessage.begin();iter!=rawMessage.end() && number<=2010;++iter)
	{
		data << setw(2) << setfill('0') << (int)(*iter) << ' ';
		number++;
		if (counter == 12)
		{
			data << "\n";
			counter = 0;
		}
		else
			counter++;
	}
	
	//LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"DATA = %s", data.str().c_str());		
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"DATA SIZE = %d , Data=%s",rawMessage.size(),data.str().c_str());		
}

ostream& operator<<(ostream& os, CMPDUType dat)
{
    string sdat;
    switch(dat)
    {
        case CMPDUTYPE_DATA_REQ:
            sdat = "Data request";
            break;

        case CMPDUTYPE_REPORT_RESP:
            sdat = "Report response";
            break;

        case CMPDUTYPE_REGISTER_REQ:
            sdat = "Register request";
            break;

        case CMPDUTYPE_REGISTER_RESP:
            sdat = "Register response";
            break;

        case CMPDUTYPE_BROADCAST:
            sdat = "Broadcast";
            break;
        default:
            sdat = "<UNKNOWN>";
    }
    
    os << sdat;
    return os;
}

ostream& operator<<(ostream& os, CMProtocolIdentifier dat)
{
    string sdat;
    switch(dat)
    {
        case CMPROTOCOLIDENTIFIER_TEXTMSG:
            sdat = "Text Message";
            break;

        case CMPROTOCOLIDENTIFIER_TRAINBORNEMSG:
            sdat = "Trainborne Message";
            break;

        case CMPROTOCOLIDENTIFIER_LONGTEXTMSG:
            sdat = "Long Text Message";
            break;

        default:
            sdat = "<UNKNOWN>";
    }
    
    os << sdat;
    return os;
}

ostream& operator<<(ostream& os, CMChannelSelectionMask dat)
{
    string sdat;
    switch(dat)
    {
        case CMCHANNELSELECTIONMASK_MCCH:
            sdat = "MCCH";
            break;

        case CMCHANNELSELECTIONMASK_PDCH:
            sdat = "PDCH";
            break;

        case CMCHANNELSELECTIONMASK_GROUP:
            sdat = "Group";
            break;

        case CMCHANNELSELECTIONMASK_RESERVED:
            sdat = "Reserved";
            break;

        default:
            sdat = "<UNKNOWN>";
    }

    os << sdat;
    return os;
}

ostream& operator<<(ostream& os, CMDeliveryStatus dat)
{
    string sdat;
    switch(dat)
    {
        case CMDELIVERYSTATUS_NET_OVERLOAD:
            sdat = "Network overload";
            break;

        case CMDELIVERYSTATUS_SERVICE_DISABLED:
            sdat = "Service disabled";
            break;

        case CMDELIVERYSTATUS_SERVICE_TEMP_NAVAIL:
            sdat = "Service temporary not available";
            break;

        case CMDELIVERYSTATUS_DEST_NAUTHORIZED:
            sdat = "Destination not authorized";
            break;

        case CMDELIVERYSTATUS_UKNOWN_DEST:
            sdat = "Unknown destination";
            break;

        case CMDELIVERYSTATUS_GROUP_ADDR_W_INDIVIDUAL:
            sdat = "Group address";
            break;

        case CMDELIVERYSTATUS_NREACHABLE:
            sdat = "Not reachable";
            break;

        case CMDELIVERYSTATUS_NREGISTERED:
            sdat = "Not registered";
            break;

        case CMDELIVERYSTATUS_DEST_QUEUE_FULL:
            sdat = "Destination queue full";
            break;

        case CMDELIVERYSTATUS_MSG_TOO_LONG:
            sdat = "Message too long";
            break;

        case CMDELIVERYSTATUS_DEST_NSUPPORT_SDS:
            sdat = "Destination does not support SDS";
            break;

        case CMDELIVERYSTATUS_DEST_NCONNECTED:
            sdat = "Destination not connected";
            break;

        case CMDELIVERYSTATUS_PROTOCOL_NSUPPORTED:
            sdat = "Protocol not supported";
            break;

        case CMDELIVERYSTATUS_DEST_NACCEPTING_SDS:
            sdat = "Destination not accepting SDS";
            break;

        case CMDELIVERYSTATUS_DEST_ADM_PROHIBITED:
            sdat = "Destination administratively prohibited";
            break;

        case CMDELIVERYSTATUS_UNKNOWN_SUBSCRIBER:
            sdat = "Unknown subscriber";
            break;

        case CMDELIVERYSTATUS_DEST_NREACHABLE_FAILED:
            sdat = "Destination not reachable, delivery failed";
            break;

        case CMDELIVERYSTATUS_CORRUPT_DISCARDED:
            sdat = "Data corrupt, data discarded";
            break;

        default:
            sdat = "<UNKNOWN>";    
    }
    os << sdat;
    return os;
}

ostream& operator<<(ostream& os, CMRegistrationType dat)
{
    string sdat;
    switch(dat)
    {
    case CMREGISTRATIONTYPE_DEFAULT:
        sdat = "Default";
        break;
    default:
        sdat = "<UNKNOWN>";
    }
    os << sdat;
    return os;
}

ostream& operator<<(ostream& os, CMRepeatRequest dat)
{
    string sdat;
    switch(dat)
    {
        case CMREPEATREQ_NREPEAT:
            sdat = "No repeat request";
            break;
    
        case CMREPEATREQ_REPEAT:
            sdat = "Repeat request";
            break;

        default:
            sdat = "<UNKNOWN>";
    }
    os << sdat;
    return os;
}

ostream& operator<<(ostream& os, CMSDSType dat)
{
    string sdat;
    switch(dat)
    {
        case CMSDSTYPE_1:
            sdat = "Type 1";
            break;
    
        case CMSDSTYPE_2:
            sdat = "Type 2";
            break;

        case CMSDSTYPE_3:
            sdat = "Type 3";
            break;
    
        case CMSDSTYPE_4:
            sdat = "Type 4";
            break;

        default:
            sdat = "<UNKNOWN>";
    }
    os << sdat;
    return os;
}

ostream& operator<<(ostream& os, CMServiceSelection dat)
{
    string sdat;
    switch(dat)
    {
        case CMSERVICESELECTION_INDIVIDUAL:
            sdat = "Individual";
            break;
    
        case CMSERVICESELECTION_GROUP:
            sdat = "Group";
            break;

        default:
            sdat = "<UNKNOWN>";
    }
    os << sdat;
    return os;
}

ostream& operator<<(ostream& os, TLDeliveryStatus dat)
{
    string sdat;
    switch(dat)
    {
        case TLDELIVERYSTATUS_CODINGSCHEME:
            sdat = "Coding scheme not supported";
            break;
    
        case TLDELIVERYSTATUS_MEMORYFULL:
            sdat = "Destination memory full";
            break;

        case TLDELIVERYSTATUS_RECEIVED:
            sdat = "Received by destination";
            break;
    
        case TLDELIVERYSTATUS_CONSUMED:
            sdat = "Consumed by destination";
            break;

        default:
            sdat = "<UNKNOWN>";
    }
    os << sdat;
    return os;
}

ostream& operator<<(ostream& os, TLPDUType dat)
{
    string sdat;
    switch(dat)
    {
        case TLPDUTYPE_DATA:
            sdat = "Data";
            break;
    
        case TLPDUTYPE_REPORT:
            sdat = "Report";
            break;

        default:
            sdat = "<UNKNOWN>";
    }
    os << sdat;
    return os;
}


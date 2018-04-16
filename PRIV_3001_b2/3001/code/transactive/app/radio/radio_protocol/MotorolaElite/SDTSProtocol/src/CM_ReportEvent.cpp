
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_ReportEvent.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_ReportEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSHelper.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;

namespace TA_IRS_App {

	const int CM_ReportEvent::CM_REPORT_STATUS_INDEX =  11;
	const int CM_ReportEvent::CM_REPORT_LENGTH = 12;

    CM_ReportEvent::CM_ReportEvent() : deliveryStatus(CMDELIVERYSTATUS_NONE)
	{
		FUNCTION_ENTRY("constructor");

		setCM_PDUType(CMPDUTYPE_REPORT_RESP);

		FUNCTION_EXIT;
	}

	CM_ReportEvent::~CM_ReportEvent()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	CMDeliveryStatus CM_ReportEvent::getDeliveryStatus()
	{
		FUNCTION_ENTRY("getDeliveryStatus");
    
		FUNCTION_EXIT;
		return deliveryStatus;

	}

	void CM_ReportEvent::setDeliveryStatus(CMDeliveryStatus status)
	{
		FUNCTION_ENTRY("setDeliveryStatus");
		
		deliveryStatus = status;
		
		FUNCTION_EXIT;
	}

	bool CM_ReportEvent::parseData(std::vector<unsigned char>& rawData)
	{
		FUNCTION_ENTRY("parseData");

		// Adaptation layer
		std::vector<unsigned char> vAdaptation(rawData.begin(), rawData.begin()+ADAPTATION_LENGTH);
        int adaptationLength = SDTSHelper::decode_u16(vAdaptation);

        if (adaptationLength != CM_REPORT_LENGTH - ADAPTATION_LENGTH)
        {
			TA_THROW(TA_Base_Core::TransactiveException("Invalid raw data size"));
            return false;
        }
     
		setAdaptationLength(adaptationLength);
		
		//CM PDU Type
		std::vector<unsigned char> vPDUtype(1, rawData[CM_PDUTYPE_INDEX]);
        CMPDUType pduType = SDTSHelper::decodeCM_PDUType(vPDUtype);
        if (pduType != CMPDUTYPE_REPORT_RESP)
        {
			TA_THROW(TA_Base_Core::TransactiveException("Invalid CM PDU Type"));
            return false;
        }

		setCM_PDUType(pduType);
			
		//source ISSI
		std::vector<unsigned char> vSourceSSI(rawData.begin()+CM_SOURCE_SSI_INDEX, rawData.begin()+CM_SOURCE_SSI_INDEX+3);
		setSourceSSI(SDTSHelper::decodeSSI(vSourceSSI));

		//dest ISSI
		std::vector<unsigned char> vDestSSI(rawData.begin()+CM_DESTINATION_SSI_INDEX, rawData.begin()+CM_DESTINATION_SSI_INDEX+3);
		setDestSSI(SDTSHelper::decodeSSI(vDestSSI));

		// Protocol Identifier
		std::vector<unsigned char> vPI(1, rawData[CM_PI_INDEX]);
		setProtocolId( SDTSHelper::decodeCM_ProtocolIdentifier(vPI) );

		// Message Reference, different from CM_BROADCAST/CM_DATA
		setMessageRef(rawData[CM_PI_INDEX + 1]);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Message Reference = %d",rawData[CM_PI_INDEX + 1]);		

		// CM Delivery Status
		std::vector<unsigned char> deliveryStat(1, rawData[CM_REPORT_STATUS_INDEX]);
		setDeliveryStatus(SDTSHelper::decodeCM_DeliveryStatus(deliveryStat));

		FUNCTION_EXIT;	
		return true;	
	}

}
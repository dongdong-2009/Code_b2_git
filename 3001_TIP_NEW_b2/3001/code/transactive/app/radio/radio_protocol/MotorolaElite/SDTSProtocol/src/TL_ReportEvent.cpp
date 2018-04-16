/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_ReportEvent.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_ReportEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon3.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSHelper.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;

namespace TA_IRS_App {

	const int TL_ReportEvent::TL_REPORT_CM_TL_LENGTH = 2;

    TL_ReportEvent::TL_ReportEvent() : TLCommon(TLPDUTYPE_REPORT), deliveryStatus(TLDELIVERYSTATUS_RECEIVED)
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	TL_ReportEvent::~TL_ReportEvent()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	unsigned int TL_ReportEvent::getTLDataLength()
	{
		FUNCTION_ENTRY("getTLDataLength");
		FUNCTION_EXIT;
        // data length is fixed 2 bytes (PDU Type + TL delivery status)
		return 2 * 8;
	}

	TLDeliveryStatus TL_ReportEvent::getDeliveryStatus()
	{
		FUNCTION_ENTRY("getDeliveryStatus");
		FUNCTION_EXIT;
		return deliveryStatus;
	}

	void TL_ReportEvent::setDeliveryStatus(TLDeliveryStatus status)
	{
		FUNCTION_ENTRY("setDeliveryStatus");

		deliveryStatus = status;
		FUNCTION_EXIT;
	}

	void TL_ReportEvent::buildMsg()
	{
		FUNCTION_ENTRY("buildMsg");

		CMCommon3* pCMHeader = getCMHeader();

		if ( pCMHeader != NULL )
		{
			std::vector<unsigned char> tlReport = pCMHeader->getRawMessage();

			// set CM TL length, it's already preallocated by the CMCommon3
			std::vector<unsigned char> vlength = SDTSHelper::encode_u16( getTLDataLength() );

			// TD14310 Huangxin -- corrected protocol
			//tlReport[tlReport.size() - 2] = vlength[0];
			//tlReport[tlReport.size() - 1] = vlength[1];
			tlReport.insert(tlReport.end(),vlength.begin(),vlength.end());
			// TD14310 Huangxin

			// TL PDU type
			std::vector<unsigned char> tempData = SDTSHelper::encodeTL_PDUType( getTL_PDUType() );
			tlReport.insert( tlReport.end(), tempData.begin(), tempData.end() );

			// Delivery Status
			tempData = SDTSHelper::encodeTL_DeliveryStatus( deliveryStatus );
			tlReport.insert( tlReport.end(), tempData.begin(), tempData.end() );

			// set Adaptation length, it's already preallocated by the CMCommon3
			unsigned int nAdaptation = tlReport.size() - 2;
			tempData = SDTSHelper::encode_u16(nAdaptation);
            tlReport[0] = tempData[0];
            tlReport[1] = tempData[1];

			setRawMessage(tlReport);			
		}

		FUNCTION_EXIT;
	}

	bool TL_ReportEvent::parseData(const std::vector<unsigned char>& rawData)
	{
		FUNCTION_ENTRY("parseData");

		std::vector<unsigned char> vPDUtype(1, rawData[TL_DATA_PDU_TYPE_INDEX]);
		TLPDUType pduType = SDTSHelper::decodeTL_PDUType(vPDUtype);
		if (pduType != TLPDUTYPE_REPORT)
		{
			TA_THROW(TA_Base_Core::TransactiveException(std::string("Invalid TL PDU Type")));
			return false;
		}	
		
		std::vector<unsigned char> vStatus(1, rawData[TL_DATA_PDU_TYPE_INDEX+1]);
		setDeliveryStatus(SDTSHelper::decodeTL_DeliveryStatus(vStatus));

		FUNCTION_EXIT;
        return true;
	}

}

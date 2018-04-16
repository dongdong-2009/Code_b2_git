/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_ShortReportEvent.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_ShortReportEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSHelper.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;

namespace TA_IRS_App {

    TL_ShortReportEvent::TL_ShortReportEvent() : 
                            TLCommon(TLPDUTYPE_SHORTREPORT), 
                            deliveryStatus(TLDELIVERYSTATUS_RECEIVED)
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	TL_ShortReportEvent::~TL_ShortReportEvent()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	TLDeliveryStatus TL_ShortReportEvent::getDeliveryStatus()
	{
		FUNCTION_ENTRY("getDeliveryStatus");
		FUNCTION_EXIT;
		return deliveryStatus;
	}

	void TL_ShortReportEvent::setDeliveryStatus(TLDeliveryStatus status)
	{
	    FUNCTION_ENTRY("setDeliveryStatus");
    
		deliveryStatus = status;
		FUNCTION_EXIT;
	}

	bool TL_ShortReportEvent::parseData(const std::vector<unsigned char>& rawData)
	{
		FUNCTION_ENTRY("parseData");

		std::vector<unsigned char> vPDUtype(1, rawData[TL_DATA_PDU_TYPE_INDEX]);
		TLPDUType pduType = SDTSHelper::decodeTL_PDUType(vPDUtype);
		if (pduType != TLPDUTYPE_SHORTREPORT)
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

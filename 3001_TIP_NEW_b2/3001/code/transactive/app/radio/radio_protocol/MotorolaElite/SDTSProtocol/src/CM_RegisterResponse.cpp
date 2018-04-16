
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_RegisterResponse.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_RegisterResponse.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSHelper.h"


namespace TA_IRS_App {

    const int CM_RegisterResponse::CM_REGISTRATION_STATUS_INDEX = 11;
    const int CM_RegisterResponse::CM_REGISTER_RESPONSE_LENGTH = 12;

	CM_RegisterResponse::CM_RegisterResponse() : m_registrationStatus(0)
	{
		FUNCTION_ENTRY("constructor");
        
        setCM_PDUType(CMPDUTYPE_REGISTER_RESP);
		
		FUNCTION_EXIT;
	}

	CM_RegisterResponse::~CM_RegisterResponse()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

    bool CM_RegisterResponse::parseData(const std::vector<unsigned char>& rawData)
    {
		FUNCTION_ENTRY("parseData");
		if (rawData.size() != CM_REGISTER_RESPONSE_LENGTH)
        {
            return false;
        }

		// Adaptation layer
		std::vector<unsigned char> vAdaptation(rawData.begin(), rawData.begin()+ADAPTATION_LENGTH);
        int adaptationLength = SDTSHelper::decode_u16(vAdaptation);

        if (adaptationLength != CM_REGISTER_RESPONSE_LENGTH - ADAPTATION_LENGTH)
        {
            return false;
        }
     
		setAdaptationLength(adaptationLength);
		
		//CM PDU Type
		std::vector<unsigned char> vPDUtype(1, rawData[CM_PDUTYPE_INDEX]);
        CMPDUType pduType = SDTSHelper::decodeCM_PDUType(vPDUtype);
        if (pduType != CMPDUTYPE_REGISTER_RESP)
        {
            return false;
        }

		setCM_PDUType(pduType);
	
		// Pass Number - 4 bytes
		std::vector<unsigned char> vPassNum(rawData.begin() + CM_PASS_NUMBER_INDEX, 
                                            rawData.begin() + CM_PASS_NUMBER_INDEX+4);
		setPassNumber(SDTSHelper::decode_u32(vPassNum));

		// ISSI - 3 bytes
		std::vector<unsigned char> vClientSSI(rawData.begin() + CM_CLIENT_SSI_INDEX, 
                                              rawData.begin() + CM_CLIENT_SSI_INDEX +3);
		setClientSSI(SDTSHelper::decodeSSI(vClientSSI));

		m_registrationStatus = rawData[CM_REGISTRATION_STATUS_INDEX];


		FUNCTION_EXIT;
        return true;
    }

	int CM_RegisterResponse::getRegistrationStatus()
	{
		FUNCTION_ENTRY("getRegistrationStatus");
		FUNCTION_EXIT;
		return m_registrationStatus;
	}

}

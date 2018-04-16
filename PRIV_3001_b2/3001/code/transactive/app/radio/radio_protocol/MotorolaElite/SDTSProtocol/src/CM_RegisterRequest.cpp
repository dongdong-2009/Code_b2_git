
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_RegisterRequest.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_RegisterRequest.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSHelper.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App {

    const int CM_RegisterRequest::CM_PASS_NUMBER_INDEX = 3;
    const int CM_RegisterRequest::CM_CLIENT_SSI_INDEX = 7;
    const int CM_RegisterRequest::CM_REGISTRATION_TYPE_INDEX = 10;

    CM_RegisterRequest::CM_RegisterRequest():
    m_passNumber(0), m_clientSSI(0), m_registrationType(CMREGISTRATIONTYPE_DEFAULT)
    {

    }

	CM_RegisterRequest::CM_RegisterRequest(unsigned long passNumber, unsigned long clientSSI, 
                                           CMRegistrationType regType) :
                            m_passNumber(passNumber), m_clientSSI(clientSSI),
                            m_registrationType(regType)
	{
		FUNCTION_ENTRY("constructor");

		setCM_PDUType(CMPDUTYPE_REGISTER_REQ);

		FUNCTION_EXIT;
	}

	CM_RegisterRequest::~CM_RegisterRequest()
	{
		FUNCTION_ENTRY("destructor");

		FUNCTION_EXIT;
	}


	unsigned long CM_RegisterRequest::getPassNumber()
	{
		FUNCTION_ENTRY("getPassNumber");
		FUNCTION_EXIT;
		return m_passNumber;
	}

	unsigned long CM_RegisterRequest::getClientSSI()
	{
		FUNCTION_ENTRY("getClientSSI");
		FUNCTION_EXIT;
		return m_clientSSI;
	}

	CMRegistrationType CM_RegisterRequest::getRegistrationType()
	{
		FUNCTION_ENTRY("getRegistrationType");
		FUNCTION_EXIT;
		return m_registrationType;
	}

	void CM_RegisterRequest::setPassNumber(unsigned long passNum)
	{
		FUNCTION_ENTRY("setPassNumber");

		m_passNumber = passNum;
		FUNCTION_EXIT;
	}

	void CM_RegisterRequest::setClientSSI(unsigned long clientSSI)
	{
		FUNCTION_ENTRY("setClientSSI");

		m_clientSSI = clientSSI;
		FUNCTION_EXIT;
	}

	void CM_RegisterRequest::setRegistrationType(CMRegistrationType regType)
	{
		FUNCTION_ENTRY("setRegistrationType");

		m_registrationType = regType;
		FUNCTION_EXIT;
	}

	void CM_RegisterRequest::buildMsg()
	{
		FUNCTION_ENTRY("buildMsg");

		std::vector<unsigned char> rawData;

		std::vector<unsigned char> tempData;

		// PDU type
		tempData = SDTSHelper::encodeCM_PDUType(getCM_PDUType()) ;
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

		// PassNumber
		tempData = SDTSHelper::encode_u32(m_passNumber);
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

		//ISSI
		tempData = SDTSHelper::encodeSSI( m_clientSSI );
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

		// Registration type
		tempData = SDTSHelper::encodeCM_RegistrationType(m_registrationType);
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );
        
        // adaptation length, 2 bytes at beginning
		unsigned int nAdaptation = rawData.size();
		tempData = SDTSHelper::encode_u16(nAdaptation);
		rawData.insert( rawData.begin(), tempData.begin(), tempData.end() );
        
        setAdaptationLength(nAdaptation);
		
		setRawMessage(rawData);

		FUNCTION_EXIT;
	}
}

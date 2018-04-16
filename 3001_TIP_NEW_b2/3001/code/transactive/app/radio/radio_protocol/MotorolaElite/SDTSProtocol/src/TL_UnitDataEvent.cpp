/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_UnitDataEvent.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_UnitDataEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSHelper.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon3.h"
#include "core/utilities/src/DebugUtil.h"



namespace TA_IRS_App {

	TL_UnitDataEvent::TL_UnitDataEvent() : TLCommon(TLPDUTYPE_UNITDATA)
	{
		FUNCTION_ENTRY("constructor");

		FUNCTION_EXIT;
	}

	TL_UnitDataEvent::~TL_UnitDataEvent()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}


	unsigned int TL_UnitDataEvent::getTLDataLength()
	{
		FUNCTION_ENTRY("getTLDataLength");
		FUNCTION_EXIT;
        // unit data length is 1 byte for PDU type + length of user data
		return (m_msgData.length() + 1) * 8;
	}


	std::string TL_UnitDataEvent::getMessageData()
	{
		FUNCTION_ENTRY("string TL_UnitDataEvent::getMessageData");
		FUNCTION_EXIT;
		return m_msgData;
	}

	void TL_UnitDataEvent::setMessageData(const std::string& userData)
	{
		FUNCTION_ENTRY("setMessageData");

		m_msgData = userData;

		FUNCTION_EXIT;
	}

	void TL_UnitDataEvent::buildMsg()
	{
		FUNCTION_ENTRY("buildMsg");

		CMCommon3* pCMHeader = getCMHeader();

		if ( pCMHeader != NULL )
		{
			std::vector<unsigned char> tlData = pCMHeader->getRawMessage();

			// set CM TL length, it's already preallocated by the CMCommon3
			std::vector<unsigned char> vlength = SDTSHelper::encode_u16( getTLDataLength() );
			tlData[tlData.size() - 2] = vlength[0];
			tlData[tlData.size() - 1] = vlength[1];

			// PDU type
			tlData.push_back( getTL_PDUType() );

			// User data
			for (unsigned int i=0; i < m_msgData.length(); ++i)
			{
				tlData.push_back( m_msgData[i] );
			}

			// set Adaptation length, it's already preallocated by the CMCommon3
			unsigned int nAdaptation = tlData.size() - 2;
			std::vector<unsigned char> tempData = SDTSHelper::encode_u16(nAdaptation);
            tlData[0] = tempData[0];
            tlData[1] = tempData[1];

			setRawMessage(tlData);
		}
		
		FUNCTION_EXIT;
	}
}

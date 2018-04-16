/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TLCommon.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TLCommon.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon3.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App {

	const int TLCommon::TL_DATA_PDU_TYPE_INDEX = 15;
	const int TLCommon::TL_BROADCAST_PDU_TYPE_INDEX = 19;

    TLCommon::TLCommon(TLPDUType pduType) : m_cmData(NULL), m_tlPDUType(pduType)
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	TLCommon::~TLCommon()
	{
		FUNCTION_ENTRY("destructor");
        
        delete m_cmData;
        m_cmData = NULL;

		FUNCTION_EXIT;
	}

	CMCommon3* TLCommon::getCMHeader()
	{
		FUNCTION_ENTRY("getCMHeader");
		FUNCTION_EXIT;
		return m_cmData;
	}

	void TLCommon::setCMHeader(CMCommon3* pCMCommon3)
	{
		FUNCTION_ENTRY("setCMHeader");

		m_cmData = pCMCommon3;
		FUNCTION_EXIT;
	}


	TLPDUType TLCommon::getTL_PDUType()
	{
		FUNCTION_ENTRY("getTL_PDUType");
		FUNCTION_EXIT;
		return m_tlPDUType;
	}
}

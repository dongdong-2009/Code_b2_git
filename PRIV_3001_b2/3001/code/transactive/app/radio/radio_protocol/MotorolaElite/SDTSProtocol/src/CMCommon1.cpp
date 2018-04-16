
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon1.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon1.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App {

	const int CMCommon1::CM_PDUTYPE_INDEX = 2;

	CMCommon1::CMCommon1()
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	CMCommon1::~CMCommon1()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	CMPDUType CMCommon1::getCM_PDUType()
	{
		FUNCTION_ENTRY("getCM_PDUType");
		FUNCTION_EXIT;

		return cmPDUType;
	}

	void CMCommon1::setCM_PDUType(CMPDUType pduType)
	{
		FUNCTION_ENTRY("setCM_PDUType");
    
		cmPDUType = pduType;
		FUNCTION_EXIT;
	}
}
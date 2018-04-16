
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon3.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon3.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App {

	const int CMCommon3::CM_FLAG_INDEX = 10;


	CMCommon3::CMCommon3() : cmSDSType(CMSDSTYPE_4),tlBitLength(0)
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	CMCommon3::~CMCommon3()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	CMServiceSelection CMCommon3::getServiceSelection()
	{
		FUNCTION_ENTRY("getServiceSelection");
		FUNCTION_EXIT;
		return cmServiceSelection;
	}

	CMSDSType CMCommon3::getSDSType()
	{
		FUNCTION_ENTRY("getSDSType");
		FUNCTION_EXIT;
		return cmSDSType;
	}

	unsigned short CMCommon3::getTLlength()
	{
		FUNCTION_ENTRY("getTLlength");
		FUNCTION_EXIT;
		return tlBitLength;
	}


	void CMCommon3::setServiceSelection(CMServiceSelection srvSelection)
	{
		FUNCTION_ENTRY("setServiceSelection");

		cmServiceSelection = srvSelection;
		FUNCTION_EXIT;
	}

	void CMCommon3::setSDSType(CMSDSType sdsType)
	{
		FUNCTION_ENTRY("setSDSType");

		cmSDSType = sdsType;
		FUNCTION_EXIT;
	}

	void CMCommon3::setTLlength(unsigned short length)
	{
		FUNCTION_ENTRY("setTLlength");

		tlBitLength = length;
		FUNCTION_EXIT;
	}
}
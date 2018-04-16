
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon2.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CMCommon2.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App {

    const int CMCommon2::CM_SOURCE_SSI_INDEX	  = 3;
    const int CMCommon2::CM_DESTINATION_SSI_INDEX = 6;
    const int CMCommon2::CM_PI_INDEX			  = 9;
    const int CMCommon2::CM_MESSAGE_REF_INDEX	  = 11;	// for CM-BROADCAST and CM-DATA only
			
	CMCommon2::CMCommon2() : sourceSSI(0),destSSI(0),cmMsgRef(0)
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	CMCommon2::~CMCommon2()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	unsigned long CMCommon2::getSourceSSI()
	{
	    FUNCTION_ENTRY("getSourceSSI");
		FUNCTION_EXIT;
		return sourceSSI;
	}

	unsigned long CMCommon2::getDestSSI()
	{
		FUNCTION_ENTRY("getDestSSI");
		FUNCTION_EXIT;
		return destSSI;
	}

	unsigned char CMCommon2::getMessageRef()
	{
		FUNCTION_ENTRY("getMessageRef");
		FUNCTION_EXIT;
		return cmMsgRef;
	}

	CMProtocolIdentifier CMCommon2::getProtocolId()
	{
		FUNCTION_ENTRY("getProtocolId");
		FUNCTION_EXIT;
		return cmProtocolIdentifier;
	}
	
	void CMCommon2::setMessageRef( unsigned char msgRef )
	{
		FUNCTION_ENTRY("setMessageRef");
		
		cmMsgRef = msgRef;

		FUNCTION_EXIT;
	}

	void CMCommon2::setSourceSSI( unsigned long srcSSI )
	{
		FUNCTION_ENTRY("setSourceSSI");

		sourceSSI = srcSSI;

		FUNCTION_EXIT;
	}

	void CMCommon2::setDestSSI(unsigned long dstSSI )
	{
		FUNCTION_ENTRY("setDestSSI");

		destSSI = dstSSI;

		FUNCTION_EXIT;	
	}

	void CMCommon2::setProtocolId( CMProtocolIdentifier pi)
	{
		FUNCTION_ENTRY("setProtocolId");

		cmProtocolIdentifier = pi;

		FUNCTION_EXIT;
	}
}
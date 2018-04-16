/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSRecvText.cpp $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */

#include "core/utilities/src/DebugUtil.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSRecvText.h"

namespace TA_IRS_App {

	SDSRecvText::SDSRecvText()
	{
		FUNCTION_ENTRY( "Constructor" );
		FUNCTION_EXIT;
	}


	SDSRecvText::~SDSRecvText()
	{
		FUNCTION_ENTRY( "Destructor" );
		FUNCTION_EXIT;
	}

	int SDSRecvText::call()
	{
		FUNCTION_ENTRY("call");

        // nothing to do

		FUNCTION_EXIT;

		return 0;
	}

}

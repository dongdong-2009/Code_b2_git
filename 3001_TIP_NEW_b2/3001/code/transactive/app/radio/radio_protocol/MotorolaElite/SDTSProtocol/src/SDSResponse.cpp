/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/RadioProtocol/MotorolaElite/SDTSProtocol/src/SDSResponse.cpp $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */

#include "core/utilities/src/DebugUtil.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSResponse.h"

namespace TA_IRS_App {

	SDSResponse::SDSResponse()
	{
		FUNCTION_ENTRY( "Constructor" );
		FUNCTION_EXIT;
	}


	SDSResponse::~SDSResponse()
	{
		FUNCTION_ENTRY( "Destructor" );
		FUNCTION_EXIT;
	}

	int SDSResponse::call()
	{
		FUNCTION_ENTRY("call");

        // nothing to do

		FUNCTION_EXIT;

		return 0;
	}

}

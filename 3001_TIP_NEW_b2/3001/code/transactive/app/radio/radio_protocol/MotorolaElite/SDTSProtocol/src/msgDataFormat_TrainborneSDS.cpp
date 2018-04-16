/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/msgDataFormat_TrainborneSDS.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/msgDataFormat_TrainborneSDS.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App {

	msgDataFormat_TrainborneSDS::msgDataFormat_TrainborneSDS()
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	msgDataFormat_TrainborneSDS::~msgDataFormat_TrainborneSDS()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	std::vector<unsigned char> msgDataFormat_TrainborneSDS::getSDSMessage()
	{
		return sdsMsg;
	}

	void msgDataFormat_TrainborneSDS::setSDSMessage(const std::vector<unsigned char>& data)
	{
		sdsMsg = data;
	}
}
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/bus/trains/TrainTransactionModel/src/TrainAgentException.cpp $
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 10:00:53 $
 * Last modified by:  $Author: builder $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/utilities/src/DebugUtil.h"
#include "bus/trains/TrainTransactionModel/src/TrainAgentException.h"

namespace TA_IRS_Bus
{
	using TA_Base_Core::TransactiveException;


	TrainAgentException::TrainAgentException() throw()
		:TransactiveException("")
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	}
	

	TrainAgentException::TrainAgentException(const std::string& msg) throw()
		:TransactiveException(msg)
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	}


	TrainAgentException::~TrainAgentException() throw()
	{
		FUNCTION_ENTRY("Destructor");
		FUNCTION_EXIT;
	}

	const char* TrainAgentException::what() const throw()
	{
		FUNCTION_ENTRY("what");
		FUNCTION_EXIT;
		return TransactiveException::what();
	}
}

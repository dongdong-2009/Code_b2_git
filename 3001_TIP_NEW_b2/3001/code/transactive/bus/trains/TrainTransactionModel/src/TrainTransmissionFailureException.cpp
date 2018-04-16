/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/bus/trains/TrainTransactionModel/src/TrainTransmissionFailureException.cpp $
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 10:00:53 $
 * Last modified by:  $Author: builder $
 * 
 * Thrown when a command cannot be sent to the train via the radio interface.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/utilities/src/DebugUtil.h"
#include "bus/trains/TrainTransactionModel/src/TrainTransmissionFailureException.h"
#include "bus/trains/TrainTransactionModel/src/IScheduledEventProcessor.h"

namespace TA_IRS_Bus
{
	TrainTransmissionFailureException::TrainTransmissionFailureException() throw()
		:TrainAgentException()
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	
	}

	TrainTransmissionFailureException::~TrainTransmissionFailureException() throw()
	{
		FUNCTION_ENTRY("Destructor");
		FUNCTION_EXIT;
	
	}


	TrainTransmissionFailureException::TrainTransmissionFailureException(const std::string& msg) throw()
		:TrainAgentException(msg)
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	
	}

	const char* TrainTransmissionFailureException::what() const throw()
	{
		FUNCTION_ENTRY("what");
		FUNCTION_EXIT;
		return TrainAgentException::what();
	
	}
}

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL9_TIP/GZL9/transactive/bus/bas/bas_agent_library/src/BasAgentLibraryException.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/02/27 17:12:34 $
  * Last modified by:  $Author: CM $
  *
  * This is used to get access to the pa agent.
  */

#if !defined(BAS_AGENT_LIBRARY_EXCEPTION_H)
#define BAS_AGENT_LIBRARY_EXCEPTION_H

#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_Bus
{
	class BasNotDeployStationException : public TA_Base_Core::TransactiveException
	{
	public:
		BasNotDeployStationException(const std::string& msg) throw()
			:TA_Base_Core::TransactiveException(msg)
		{
		}
	};

	class BasAgentInvalidException : public TA_Base_Core::TransactiveException
	{
	public:
		BasAgentInvalidException(const std::string& msg) throw()
			:TA_Base_Core::TransactiveException(msg)
		{
		}
	};
}

#endif
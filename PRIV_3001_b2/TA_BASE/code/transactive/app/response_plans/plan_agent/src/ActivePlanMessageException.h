#ifndef ACTIVEPLANMESSAGEEXCEPTION_H_INCLUDED
#define ACTIVEPLANMESSAGEEXCEPTION_H_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanMessageException.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  <description>
  */

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_App
{
	class ActivePlanMessageException : public TA_Base_Core::TransactiveException
	{
	public:
		ActivePlanMessageException() throw();
		ActivePlanMessageException(const std::string& msg) throw();
		virtual ~ActivePlanMessageException() throw();
	};
}; // TA_Base_App

#endif // ACTIVEPLANMESSAGEEXCEPTION_H_INCLUDED

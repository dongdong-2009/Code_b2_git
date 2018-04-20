#ifndef ACTIVEPLANMESSAGEEXCEPTION_H_INCLUDED
#define ACTIVEPLANMESSAGEEXCEPTION_H_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanMessageException.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */

namespace TA_Base_App
{
	class ActivePlanMessageException : public TA_Base_Core::PlanAgentException
	{
	public:
		ActivePlanMessageException() throw();
		ActivePlanMessageException(const std::string& msg) throw();
		virtual const char* what() const throw();
		virtual ~ActivePlanMessageException() throw();
	private:
		const std::string m_msg;
	};
}; // TA_Base_App

#endif // ACTIVEPLANMESSAGEEXCEPTION_H_INCLUDED

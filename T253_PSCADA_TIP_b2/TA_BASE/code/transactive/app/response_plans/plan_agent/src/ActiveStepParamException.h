#ifndef ACTIVESTEPPARAMEXCEPTION_H_INCLUDED
#define ACTIVESTEPPARAMEXCEPTION_H_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveStepParamException.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2013/07/17 15:33:29 $
  *  Last modified by:  $Author: CM $
  *
  *  <description>
  */

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_App
{
	class ActiveStepParamException : public TA_Base_Core::TransactiveException  
	{
	public:
		ActiveStepParamException() throw();
		ActiveStepParamException(const std::string& msg) throw();
		virtual ~ActiveStepParamException() throw();
	};
}; // TA_Base_App

#endif // ACTIVESTEPPARAMEXCEPTION_H_INCLUDED

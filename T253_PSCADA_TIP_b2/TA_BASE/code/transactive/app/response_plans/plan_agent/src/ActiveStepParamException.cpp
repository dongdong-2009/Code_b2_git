/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveStepParamException.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2013/07/17 15:33:29 $
  *  Last modified by:  $Author: CM $
  *
  *  Implementation of .
  */

#include "app/response_plans/plan_agent/src/ActiveStepParamException.h"

namespace TA_Base_App
{
    ActiveStepParamException::ActiveStepParamException() throw() : TransactiveException( "" )
	{
	}

	ActiveStepParamException::ActiveStepParamException(const std::string& msg) throw() : TransactiveException( msg )
	{
	}

	ActiveStepParamException::~ActiveStepParamException() throw()
	{
	}

} // TA_Base_App

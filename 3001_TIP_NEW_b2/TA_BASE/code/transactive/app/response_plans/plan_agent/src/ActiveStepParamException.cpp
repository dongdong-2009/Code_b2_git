/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveStepParamException.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
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

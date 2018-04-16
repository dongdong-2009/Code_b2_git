/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ThirdPartyLibException.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2013/07/17 15:33:29 $
  *  Last modified by:  $Author: CM $
  *
  *  Implementation of .
  */

#include "app/response_plans/plan_agent/src/ThirdPartyLibException.h"

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{
	ThirdPartyLibException::ThirdPartyLibException() throw() : TransactiveException( "" )
	{
	}

	ThirdPartyLibException::ThirdPartyLibException(const std::string& msg) throw() : TransactiveException( msg )
	{
	}
	
	ThirdPartyLibException::~ThirdPartyLibException() throw()
	{
	}

} // TA_Base_App

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ThirdPartyLibException.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
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

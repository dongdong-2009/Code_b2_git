/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ThirdPartyLibException.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{
	ThirdPartyLibException::ThirdPartyLibException() throw()
	{
	}

	ThirdPartyLibException::ThirdPartyLibException(const std::string& msg) throw() :
		m_msg(msg)
	{
	}
	
	const char* ThirdPartyLibException::what() const throw()
	{
		return( m_msg.c_str() );
	}
	
	ThirdPartyLibException::~ThirdPartyLibException() throw()
	{
	}

} // TA_Base_App

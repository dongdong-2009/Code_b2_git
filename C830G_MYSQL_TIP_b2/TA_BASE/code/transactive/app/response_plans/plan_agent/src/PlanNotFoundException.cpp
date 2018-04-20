/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanNotFoundException.cpp $
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
    PlanNotFoundException::PlanNotFoundException() throw() :
		m_msg("Plan not found"),
        m_errorCode(TA_Base_Core::PNFE_NODE_NOT_EXIST),
        m_planId(0),
        m_instanceId(0)
	{
	}

	PlanNotFoundException::PlanNotFoundException(const std::string& msg, 
                                const TA_Base_Core::EPlanNotFoundError error, 
                                const TA_Base_Core::NodeId planId /*=0*/, 
                                const unsigned long instanceId /*=0*/) throw() :
		m_msg(msg),
        m_errorCode(error),
        m_planId(planId),
        m_instanceId(instanceId)
	{
	}
	
	const char* PlanNotFoundException::what() const throw()
	{
		return( m_msg.c_str() );
	}
	
	PlanNotFoundException::~PlanNotFoundException() throw()
	{
	}

} // TA_Base_App

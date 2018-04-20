/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanReassignMessage.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

namespace TA_Base_App
{
	ActivePlanReassignMessage::ActivePlanReassignMessage(const std::string& session, const std::string& pmId) 
		: m_sessionId(session), m_pmId(pmId)
	{

	}

	void ActivePlanReassignMessage::execute( ActivePlan& activePlan ) const
	{
		activePlan.reassign(m_sessionId, m_pmId);
	}

} // TA_Base_App

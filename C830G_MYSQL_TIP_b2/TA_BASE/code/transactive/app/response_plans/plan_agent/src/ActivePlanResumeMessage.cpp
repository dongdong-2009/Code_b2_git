/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanResumeMessage.cpp $
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
	//lizettejl++ update class to include the resumeStep (DP-changes)
	ActivePlanResumeMessage::ActivePlanResumeMessage(const TA_Base_Core::StepNumber resumeStepPosition)
		: m_resumeStepPosition(resumeStepPosition)
	{
 
	}
	
	void ActivePlanResumeMessage::execute( ActivePlan& activePlan ) const
	{
		activePlan.resume(m_resumeStepPosition);
	}

} // TA_Base_App

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanResumeMessage.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  Implementation of .
  */

#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanResumeMessage.h"

namespace TA_Base_App
{
	ActivePlanResumeMessage::ActivePlanResumeMessage()
	{
	}
	
	void ActivePlanResumeMessage::execute( ActivePlan& activePlan ) const
	{
		activePlan.resume();
	}

} // TA_Base_App

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanState.cpp $
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
	ActivePlan::ENextStepExecuteMethod ActivePlanState::defaultExecuteNextStep( ActivePlan& activePlan ) const
	{
		return( activePlan.executeNextStepImpl() );
	}

	void ActivePlanState::defaultReassign( ActivePlan& activePlan, const std::string& sessionId, const std::string& pmId ) const
	{
		activePlan.reassignImpl(sessionId, pmId);
	}

	void ActivePlanState::defaultUnassign( ActivePlan& activePlan ) const
	{
	}
	
	void ActivePlanState::defaultCustomiseStep( ActivePlan& activePlan, const TA_Base_Core::StepCustomisableDetail& newDetail ) const
	{
		activePlan.customiseStepImpl( newDetail );
	}

	void ActivePlanState::changeState( ActivePlan& activePlan, ActivePlanState* state ) const
	{
		activePlan.changeState( state );
	}

} // TA_Base_App

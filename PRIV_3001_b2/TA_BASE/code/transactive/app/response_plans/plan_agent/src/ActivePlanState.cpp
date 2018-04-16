/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanState.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  Implementation of .
  */

#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanState.h"

namespace TA_Base_App
{
	int ActivePlanState::defaultExecuteNextStep( ActivePlan& activePlan ) const
	{
		return activePlan.executeNextStepImpl();
    }

    int ActivePlanState::defaultWaitCurStep( ActivePlan& activePlan ) const
    {
        return activePlan.waitCurStepImpl();
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

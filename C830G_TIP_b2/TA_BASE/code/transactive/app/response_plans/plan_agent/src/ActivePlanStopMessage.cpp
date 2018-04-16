/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanStopMessage.cpp $
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
    ActivePlanStopMessage::ActivePlanStopMessage(std::string& reason)
    {
        m_reason = reason;
    }

	void ActivePlanStopMessage::execute( ActivePlan& activePlan ) const
	{
        std::string temp = m_reason;
		activePlan.stop( temp );
	}

} // TA_Base_App

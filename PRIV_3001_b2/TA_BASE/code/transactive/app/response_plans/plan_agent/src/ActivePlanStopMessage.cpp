/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanStopMessage.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  Implementation of .
  */

#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanStopMessage.h"

namespace TA_Base_App
{
    ActivePlanStopMessage::ActivePlanStopMessage( const std::string& reason) : 
    m_reason( reason.c_str() )
    {
    }

	void ActivePlanStopMessage::execute( ActivePlan& activePlan ) const
	{
		activePlan.stop( m_reason );
	}

} // TA_Base_App

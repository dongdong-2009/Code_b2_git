/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanMessageBlock.cpp $
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
		ActivePlanMessageBlock::ActivePlanMessageBlock( const TA_Base_Core::ActivePlanId& plan,
													IActivePlanMessage* data ) :
		m_plan(plan),
		m_control(data)
    {
    }
	
	ActivePlanMessageBlock::~ActivePlanMessageBlock(void)
	{
		delete m_control;
	}

	IActivePlanMessage* ActivePlanMessageBlock::getControlMessage( void ) const
	{
		return m_control;
	}

} // TA_Base_App

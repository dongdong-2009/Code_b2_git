/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveActivePlanCheckStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

#if defined ( STEPTYPE_ACTIVE_PLAN_CHECK )
// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveActivePlanCheckStep::ActiveActivePlanCheckStep( const TA_Base_Core::StepDetail& stepDetail, 
									const TA_Base_Core::ActivePlanCheckParameters& params,
									IActivePlanFlow& flowControl ) :
		ActiveStep(stepDetail, flowControl),
		m_params(params),		
		m_flowControl(flowControl)
	{
	}

	TA_Base_Core::EActiveStepState ActiveActivePlanCheckStep::executeImpl( void )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "%s: checking for plan %lu", dumpContext().c_str(), m_params.plan );

		if( ActivePlanBulletinBoard::instance()->isActive( m_params.plan, m_flowControl.getPlan() ) )
		{
			m_flowControl.setNextStep( m_params.yesStep );
		}
		else
		{
			m_flowControl.setNextStep( m_params.noStep );
		}

		return TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
	}
} // TA_Base_App

#endif //  STEPTYPE_ACTIVE_PLAN_CHECK

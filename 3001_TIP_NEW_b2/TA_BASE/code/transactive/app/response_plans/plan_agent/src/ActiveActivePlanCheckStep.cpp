/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveActivePlanCheckStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2013/09/25 13:26:41 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_ACTIVE_PLAN_CHECK )
// namespace alias for Corba structures/types

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveActivePlanCheckStep.h"

namespace TA_Base_App
{

	ActiveActivePlanCheckStep::ActiveActivePlanCheckStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::ActivePlanCheckParameters& params, IActivePlanFlow& flowControl ) : ActiveStep(stepDetail, flowControl),
	m_params(params)
	{
	}

	TA_Base_Core::EActiveStepState ActiveActivePlanCheckStep::executeImpl( void )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s: checking for plan %lu", dumpContext().c_str(), m_params.plan );

        bool bCheckResult = ActivePlanBulletinBoard::instance()->isActive( m_params.plan, m_flowControl.getPlan() );

        if( !m_flowControl.setNextStep( bCheckResult ? m_params.yesStep : m_params.noStep ) )
		{
            return TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}

		return TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
	}
} // TA_Base_App

#endif //  STEPTYPE_ACTIVE_PLAN_CHECK

#include "app/response_plans/common/src/ProjectDefinition.h"
#if defined (STEPTYPE_PLAN_SET_TIMER)
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanSetTimerStep.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  <description>
  */

#include "ace/OS.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanSetTimerStep.h"

namespace TA_Base_App
{
    ActivePlanSetTimerStep::ActivePlanSetTimerStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::PlanSetTimerParameters& params, IActivePlanFlow& flowControl ) :
    ActiveStep(stepDetail, flowControl),
    m_params(params)
    {
    }

    TA_Base_Core::EActiveStepState ActivePlanSetTimerStep::executeImpl( void )
    {
        // Expected expired time
        const unsigned long ulNewTimer = ACE_OS::time() + m_params.time;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Plan set timer at [%d] with [%d] seconds", ulNewTimer, m_params.time );

        try
        {
            ActivePlanBulletinBoard::instance()->updateActivePlanTimer( m_flowControl.getPlan(), ulNewTimer );
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Exception", "Caught exception when updateActivePlanTimer" );
            return TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }

        m_flowControl.setNextStep();

        return TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
    }
}


#endif // STEPTYPE_PLAN_CHECK_TIMER
#include "app/response_plans/common/src/ProjectDefinition.h"
#if defined (STEPTYPE_PLAN_CHECK_TIMER)
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanCheckTimerStep.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2013/09/25 13:26:41 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include "ace/OS.h"
#include "ace/Time_Value.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanCheckTimerStep.h"

#pragma warning ( disable : 4250 )

namespace TA_Base_App
{
    ActivePlanCheckTimerStep::ActivePlanCheckTimerStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::PlanCheckTimerParameters& params, IActivePlanFlow& flowControl ) :
    ActiveStep(stepDetail, flowControl),
    m_params(params)
    {
    }

    TA_Base_Core::EActiveStepState ActivePlanCheckTimerStep::executeImpl( void )
    {
        // Time in timer and later then the time span of checking
        unsigned long ulActivePlanTimer = 0;

        try
        {
            ulActivePlanTimer = ActivePlanBulletinBoard::instance()->getActivePlanDetail( m_flowControl.getPlan() ).timer;
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Exception", "Caught exception when getActivePlanDetail" );
            return TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }

        ACE_Time_Value atvEnd( ulActivePlanTimer + m_params.timeSpan, 0 );
        ACE_Time_Value atvNow = ACE_OS::gettimeofday();

        bool bResult = ( atvNow > atvEnd );

        if ( bResult )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "Plan check timer success at [%d], expected end at [%d]", atvNow.sec(), atvEnd.sec() );
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "Plan check timer failed at [%d], expected end at [%d]", atvNow.sec(), atvEnd.sec() );
        }

        if ( !m_flowControl.setNextStep( bResult ? m_params.successStep : m_params.failureStep ))
        {
            return TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }

        return TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
    }
}


#endif // STEPTYPE_PLAN_CHECK_TIMER
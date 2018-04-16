
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveAbortStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #11 $
  *
  *  Last modification: $DateTime: 2015/01/08 17:33:12 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/PlanExecutor.h"
#include "app/response_plans/plan_agent/src/ActiveAbortStep.h"
#include "app/response_plans/common/src/StringConstants.h"

// namespace alias for Corba structures/types

namespace TA_Base_App
{

    ActiveAbortStep::ActiveAbortStep(const BasicParameter& detail, IActivePlan& plan) : ActiveStep( m_params, plan ),
    m_params( detail )
    {

    }

    bool ActiveAbortStep::executeImpl()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s: aborting", dumpContext().c_str());

        PlanExecutor::instance()->stop(m_iPlan.id(), RemarkPlanAbort);

        return true;
    }

    int ActiveAbortStep::waitImpl()
    {
        int nWaitRes = ActivePlanBulletinBoard::instance()->isActive(m_iPlan.id()) ? eAsyncWaitContinue : eWaitSucceed;

        static const ACE_Time_Value IdleTime(0, 200000);   // 200 millisecond
        ACE_OS::sleep(IdleTime);

        return nWaitRes;
    }

} // TA_Base_App

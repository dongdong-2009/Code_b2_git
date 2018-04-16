/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanStepPausedMessage.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanStepPausedMessage.h"
#include "app/response_plans/common/src/StringConstants.h"

namespace TA_Base_App
{

ActivePlanStepPausedMessage::ActivePlanStepPausedMessage(const unsigned long ulStepNum) :
m_ulStepNumber( ulStepNum )
{
}

void ActivePlanStepPausedMessage::execute( ActivePlan& activePlan ) const
{
    const TA_Base_Core::ActivePlanId& refPlanId = activePlan.id();

    if ( activePlan.allStepPaused( false ))
    {
        activePlan.planPaused();
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Plan [%lu, %lu] paused", refPlanId.plan, refPlanId.instance );
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Step %d of Plan [%lu, %lu] paused", 
        m_ulStepNumber, refPlanId.plan, refPlanId.instance );
}

} // TA_Base_App

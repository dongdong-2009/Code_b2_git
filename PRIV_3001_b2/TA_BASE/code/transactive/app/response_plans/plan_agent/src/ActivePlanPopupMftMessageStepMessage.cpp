/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanPopupMftMessageStepMessage.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  <description>
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_POPUP_MFT_MESSAGE )

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanPopupMftMessageStepMessage.h"

namespace TA_Base_App
{

ActivePlanPopupMftMessageStepMessage::ActivePlanPopupMftMessageStepMessage(
    const TA_Base_Core::CurrentPlanStepDetail& step, const int status) : 
m_status( status ),
m_srtStepId( step )
{
}

void ActivePlanPopupMftMessageStepMessage::execute( ActivePlan& activePlan ) const
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[MftMsg] Plan [%lu, %lu, %lu] update state to %d", 
        m_srtStepId.getNodeId(), m_srtStepId.getPlanInstance(), m_srtStepId.getStepNumber(), m_status );

    ActivePlanBulletinBoard::instance()->setStepCompletionStatus(m_srtStepId, m_status);
}

}

#endif //STEPTYPE_POPUP_MFT_MESSAGE

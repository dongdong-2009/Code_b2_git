/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanCompleteMessage.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2014/11/10 18:14:32 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanCompleteMessage.h"
#include "app/response_plans/common/src/StringConstants.h"

namespace TA_Base_App
{
	void ActivePlanCompleteMessage::execute( ActivePlan& activePlan ) const
    {
        activePlan.setIsActive( false );
        activePlan.complete();

        std::string strRemark = activePlan.anyPlanStepFailed() ? RemarkPlanCompletedWithFailedSteps : RemarkPlanCompletedSuccess;

        const TA_Base_Core::ActivePlanId& refPlanId = activePlan.id();

        ActivePlanBulletinBoard::instance()->updatePlanState( refPlanId, TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE, strRemark );

        if ( ActivePlanBulletinBoard::instance()->isControlMode() )
        {
            std::string session = activePlan.session();
            const std::string strPlanName = activePlan.name();
            const unsigned long ulInstanceNum = refPlanId.instance;

            AAMessageSender::instance()->submitAuditPlanCompleted( session, strPlanName, ulInstanceNum );

            //if (ActivePlanBulletinBoard::instance()->isGRCPlan(refPlanId))
            //{
            //    if (ActivePlanBulletinBoard::instance()->isManualPlan(refPlanId))
            //    {
            //        AAMessageSender::instance()->submitAuditManualGrcSucceed(strPlanName, ulInstanceNum);
            //    }
            //    else//automatic GRC
            //    {
            //        AAMessageSender::instance()->submitAuditAutoGrcSucceed(strPlanName, ulInstanceNum);
            //    }
            //}
            //else//DSS plans
            //{
            //}
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Plan [%lu, %lu] has completed", refPlanId.plan, refPlanId.instance );
	}

} // TA_Base_App

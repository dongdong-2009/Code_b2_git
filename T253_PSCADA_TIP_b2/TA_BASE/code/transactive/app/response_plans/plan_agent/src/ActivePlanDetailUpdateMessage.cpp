/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanDetailUpdateMessage.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2014/07/10 16:23:57 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanDetailUpdateMessage.h"

namespace TA_Base_App
{
    ActivePlanDetailUpdateMessage::ActivePlanDetailUpdateMessage(const TA_Base_Core::ActivePlanDetail& planDetail) : 
    m_detail ( planDetail )
    {
    }

    void ActivePlanDetailUpdateMessage::execute( ActivePlan& activePlan ) const
    {
        //Send appropriate updates so that ActivePlnaBulletinBoard can be updated.
        ActivePlanBulletinBoard::instance()->setUpdateId(m_detail.activeUpdateId);

        if ( TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE == m_detail.activePlanState )
        {
            ActivePlanBulletinBoard::instance()->cullStoppedPlan( m_detail.plan );
            return;
        }

        // Ensure plan is not active when its state is non-active
        if (( TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE == m_detail.activePlanState ) || 
            ( TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE == m_detail.activePlanState ))
        {
            activePlan.setIsActive(false);
        }

        // Update all fields of ActivePlanDetail since we don't know which update should be called
        ActivePlanBulletinBoard::instance()->updateActivePlanDetail( m_detail );
    }
}

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanPauseMessage.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2014/11/10 18:14:32 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanPauseMessage.h"

namespace TA_Base_App
{
    ActivePlanPauseMessage::ActivePlanPauseMessage()
    {
    }

	void ActivePlanPauseMessage::execute( ActivePlan& activePlan ) const
	{
        if ( activePlan.pause() )
        {
            if ( ActivePlanBulletinBoard::instance()->isControlMode() )
            {
                const std::string session = activePlan.session();
                const std::string strPlanName = activePlan.name();
                const unsigned long ulInstanceNum = activePlan.id().instance;
                //send this message for DSS plans only
                AAMessageSender::instance()->submitAuditPlanPause( session, strPlanName, ulInstanceNum );

                //Send State Update message for the stop plan
                TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData();

                pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

                TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate();
                psu->session = session.c_str();
                psu->plan = activePlan.id();
                pasUpdateData->update.pausePlan(psu);
                ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
            }
        }
	}

} // TA_Base_App

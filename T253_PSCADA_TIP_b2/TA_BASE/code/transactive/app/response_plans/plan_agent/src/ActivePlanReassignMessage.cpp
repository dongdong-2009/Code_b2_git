/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanReassignMessage.cpp $
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
#include "app/response_plans/plan_agent/src/ActivePlanReassignMessage.h"

namespace TA_Base_App
{
	ActivePlanReassignMessage::ActivePlanReassignMessage(const std::string& session) : 
    m_sessionId(session.c_str())
	{
	}

	void ActivePlanReassignMessage::execute( ActivePlan& activePlan ) const
    {
        activePlan.reassign( m_sessionId );

        ActivePlanBulletinBoard::instance()->updateOwner( activePlan.id(), m_sessionId );

        if ( ActivePlanBulletinBoard::instance()->isControlMode() )
        {
            std::string session = activePlan.session();
            const std::string strPlanName = activePlan.name();
            std::string operatorName = ActivePlanBulletinBoard::instance()->getOperatorName( activePlan.id() );
            const unsigned long ulInstanceNum = activePlan.id().instance;

            AAMessageSender::instance()->submitAuditControlAcquire( session, strPlanName, operatorName, ulInstanceNum );

            //Send State Update message for Taking control
            TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData();

            pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

            TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate();
            psu->session = m_sessionId.c_str();
            psu->plan = activePlan.id();
            pasUpdateData->update.takeControl(psu);
            ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
        }
	}

} // TA_Base_App

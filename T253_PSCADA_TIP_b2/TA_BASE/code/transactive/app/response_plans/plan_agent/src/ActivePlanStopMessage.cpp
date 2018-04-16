/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanStopMessage.cpp $
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
#include "app/response_plans/plan_agent/src/ActivePlanStopMessage.h"

namespace TA_Base_App
{
    ActivePlanStopMessage::ActivePlanStopMessage( const std::string& reason) : 
    m_reason( reason.c_str() )
    {
    }

	void ActivePlanStopMessage::execute( ActivePlan& activePlan ) const
	{
        activePlan.setIsActive( false );
        activePlan.stop();

        ActivePlanBulletinBoard::instance()->updatePlanState( activePlan.id(), TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE, m_reason );

        // Alarm id is reset by ActivePlanStepUpdateMessage in Monitor Mode
        if ( ActivePlanBulletinBoard::instance()->isControlMode() )
        {
            std::string session = ActivePlanBulletinBoard::instance()->getSession(activePlan.id());
            const std::string strPlanName = activePlan.name();
            const unsigned long ulInstanceNum = activePlan.id().instance;
            AAMessageSender::instance()->submitAuditPlanStop( session, strPlanName, ulInstanceNum );

            //Send State Update message for the stop plan
            TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData();

            pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

            TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate();
            psu->session = session.c_str();
            psu->plan = activePlan.id();
            psu->remark = m_reason.c_str();
            pasUpdateData->update.stopPlan(psu);
            ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);

            // Get step number to alarm id mapping
            std::map<unsigned long, std::string> mapAlarmIds = ActivePlanBulletinBoard::instance()->getAssocAlarmIds( activePlan.id() );

            // Close the alarm if alarm id is not empty
            for ( std::map<unsigned long, std::string>::iterator itLoop = mapAlarmIds.begin(); mapAlarmIds.end() != itLoop; ++itLoop )
            {
                if (!itLoop->second.empty() )
                {
                    AAMessageSender::instance()->closeAlarm(itLoop->second);

                    ActivePlanBulletinBoard::instance()->resetAssocAlarmId( activePlan.id(), itLoop->first );
                }
            }
        }
	}

} // TA_Base_App

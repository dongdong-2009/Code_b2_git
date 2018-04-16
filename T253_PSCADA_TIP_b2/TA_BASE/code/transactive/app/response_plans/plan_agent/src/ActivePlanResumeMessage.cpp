/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanResumeMessage.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2015/06/03 11:12:40 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/AAMessageSender.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanResumeMessage.h"
#include "app/response_plans/common/src/StringConstants.h"

namespace TA_Base_App
{
	ActivePlanResumeMessage::ActivePlanResumeMessage()
	{
	}
	
	void ActivePlanResumeMessage::execute( ActivePlan& activePlan ) const
    {
        if ( activePlan.allStepPaused( false ))
        {
            ActivePlanBulletinBoard::instance()->updatePlanState( activePlan.id(), TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE, RemarkPlanResumed );

            if ( ActivePlanBulletinBoard::instance()->isControlMode() )
            {
                std::string session = ActivePlanBulletinBoard::instance()->getSession(activePlan.id());

                //Send State Update message for the stop plan
                TA_Base_Core::PASUpdateData_var pasUpdateData = new TA_Base_Core::PASUpdateData();

                pasUpdateData->pasUpdateId = 0; //pasUpdateId to be filled by the sendStateUpdateMessage(PASUpdateData) method

                TA_Base_Core::PlanStateUpdate_var psu = new TA_Base_Core::PlanStateUpdate();
                psu->session = session.c_str();
                psu->plan = activePlan.id();
                pasUpdateData->update.resumePlan(psu);
                ActivePlanBulletinBoard::instance()->sendStateUpdateMessage(pasUpdateData);
            }
        }

        activePlan.resume();

        // Alarm id is reset by ActivePlanStepUpdateMessage in Monitor Mode
        if ( ActivePlanBulletinBoard::instance()->isControlMode() )
        {
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

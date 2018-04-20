/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanDecisionStepMessage.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */
#include "PlanAgent.h"

#if defined ( STEPTYPE_DECISION )

namespace TA_Base_App
{
    ActivePlanDecisionStepMessage::ActivePlanDecisionStepMessage(TA_Base_Core::EPlanStepCompletionStatus status)
        : m_status (status)
    {

    }

    void ActivePlanDecisionStepMessage::execute ( ActivePlan& activePlan ) const
    {

        ActiveStep* activeStep = activePlan.getCurrentStep();        
        TA_Base_Core::StepNumber stepNumber = activeStep->getStepNumber();

        ActiveDecisionStep * decisionStep = dynamic_cast <ActiveDecisionStep *> (activeStep);

        if ( decisionStep == NULL )
        {
            //Step failed
            activePlan.setInteractiveStepFailed();
            activeStep->processStepExecutionFailure();          
            return;
        }

		//DP-changes
	    //TA_Base_Core::StepNumber nextStepNumber = decisionStep->getNextDecisionStep(m_status);
 		switch(m_status)
		{
		case TA_Base_Core::PSCS_PAUSE_RESPONSE:
			{				
 				//pause active plan instance
				// send active plan update, indicating the plan has paused
				std::string remark = "Active plan has been paused by the operator.";
				ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
					stepNumber, TA_Base_Core::PAUSED_ACTIVE_STEP_STATE, remark);
			}
			break;
		case TA_Base_Core::PSCS_TIMEOUT:
			{
 				std::string remark = "Plan paused as it timed out.";	
				ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
					stepNumber, TA_Base_Core::PAUSED_ACTIVE_STEP_STATE, remark);				 
			}
			break;
		default:
			{
				TA_Base_Core::StepNumber nextStepNumber = decisionStep->getNextDecisionStep(m_status);
				if ( nextStepNumber == 0)
				{
					//Step failed
 					activePlan.setInteractiveStepFailed();
					activeStep->processStepExecutionFailure("Plan paused");          
					return;
				}
				
 				std::string remark = "Decision step executed";
				ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
					stepNumber, TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE, remark );
				
 				PlanExecutor::instance()->resume( activePlan.getPlan(), stepNumber);
				
				activePlan.setNextStep(nextStepNumber);
				activePlan.setInteractiveStep(false);
				break;
			}
		}	
		//++lizettejl
    }
}

#endif //STEPTYPE_DECISION

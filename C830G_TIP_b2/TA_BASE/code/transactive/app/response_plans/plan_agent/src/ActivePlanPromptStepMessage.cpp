/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanPromptStepMessage.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */
#include "PlanAgent.h"


namespace TA_Base_App
{
    ActivePlanPromptStepMessage::ActivePlanPromptStepMessage(TA_Base_Core::EPlanStepCompletionStatus status)
        : m_status (status)
    {

    }

    void ActivePlanPromptStepMessage::execute ( ActivePlan& activePlan ) const
    {
        ActiveStep* activeStep = activePlan.getCurrentStep();
        
        TA_Base_Core::StepNumber stepNumber = activeStep->getStepNumber();

		if( TA_Base_Core::PSCS_ACKNOWLEDGEMENT != m_status )            
	    {            
            activePlan.setInteractiveStepFailed();
				if (m_status == TA_Base_Core::PSCS_TIMEOUT)
				{
					activeStep->processStepExecutionFailure("Plan paused");
				}
				else
				{
					activeStep->processStepExecutionFailure();    		    
				}

            return;
	    }

		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
													 stepNumber,
                                                         TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE, "Step execution successful" );

        activePlan.setNextStep();
        activePlan.setInteractiveStep(false);
        
		//lizettejl++ DP-changes pass in the step number to resume execution
        PlanExecutor::instance()->resume( activePlan.getPlan(), activePlan.getStepNumber() );        
    }
}

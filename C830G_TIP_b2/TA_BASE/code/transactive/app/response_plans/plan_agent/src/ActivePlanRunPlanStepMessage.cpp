/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanRunPlanStepMessage.cpp $
  *  @author:  Bart Golab
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Generated when the result of a Run Plan interactive step execution request
  *  is received back from the Plan Manager.
  */
  
#include "PlanAgent.h"


namespace TA_Base_App
{
    ActivePlanRunPlanStepMessage::ActivePlanRunPlanStepMessage(TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error)
        : m_status(status), m_error(error)
    {

    }


    ActivePlanRunPlanStepMessage::~ActivePlanRunPlanStepMessage()
    {
    }


    void ActivePlanRunPlanStepMessage::execute( ActivePlan& activePlan ) const
    {
        ActiveRunPlanStep* activeStep = static_cast<ActiveRunPlanStep*>(activePlan.getCurrentStep());
        TA_ASSERT(activeStep != NULL, "Invalid current step");

		if ( TA_Base_Core::PSCS_SUCCESS != m_status )            
	    {            
            activePlan.setInteractiveStepFailed();
            activeStep->processStepExecutionFailure(m_error);    		    
            return;
	    }

        if (activeStep->m_params.waitForCompletion)
        {
            // There should be a pending instance of the child plan.
            TA_ASSERT(activeStep->m_childPlan.plan > 0, "Invalid child plan ID");
            TA_ASSERT(activeStep->m_childPlan.instance > 0, "Invalid child plan instance");

			// Must wait for the child plan to complete.
			activePlan.setCompletedInterlock(activeStep->m_childPlan);

		    ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
													     activeStep->getStepNumber(),
                                                         TA_Base_Core::LOADED_ACTIVE_STEP_STATE, 
                                                         activeStep->generateCompletionPendingRemark() );
        }
        else
        {
		    ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
													     activeStep->getStepNumber(),
                                                         TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE, 
                                                         "Step execution successful" );
        }

        activePlan.setNextStep();
        activePlan.setInteractiveStep(false);
        
		//lizettejl++ DP-changes pass in the step number to resume execution
        PlanExecutor::instance()->resume( activePlan.getPlan(), activePlan.getStepNumber() );        
    }
}

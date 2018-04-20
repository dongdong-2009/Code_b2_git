
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanLaunchSchematicStepMessage.cpp $
  *  @author:  Bart Golab
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Generated when the result of a Launch Schematic interactive step execution request
  *  is received back from the Plan Manager.
  */

#include "PlanAgent.h"


namespace TA_Base_App
{
    ActivePlanLaunchSchematicStepMessage::ActivePlanLaunchSchematicStepMessage(TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error)
        : m_status(status), m_error(error)
    {

    }


    ActivePlanLaunchSchematicStepMessage::~ActivePlanLaunchSchematicStepMessage()
    {
    }


    void ActivePlanLaunchSchematicStepMessage::execute( ActivePlan& activePlan ) const
    {        
        ActiveStep* activeStep = activePlan.getCurrentStep();
        TA_ASSERT(activeStep != NULL, "Invalid current step");

		if ( TA_Base_Core::PSCS_SUCCESS != m_status )            
	    {            
            activePlan.setInteractiveStepFailed();
            activeStep->processStepExecutionFailure(m_error);    		    
            return;
	    }

		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
													 activeStep->getStepNumber(),
                                                     TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE, "Step execution successful" );

        activePlan.setNextStep();
        activePlan.setInteractiveStep(false);
        
		//lizettejl++ DP-changes pass in the step number to resume execution
        PlanExecutor::instance()->resume( activePlan.getPlan(), activePlan.getStepNumber() );        
    }
}

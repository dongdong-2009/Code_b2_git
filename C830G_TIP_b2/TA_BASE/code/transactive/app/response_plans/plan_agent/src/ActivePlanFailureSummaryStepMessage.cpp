/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanFailureSummaryStepMessage.cpp $
  *  @author:  Lizette Lingo
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Generated when the result of a Failure summary dialog display
  *  is received back from the Plan Manager.
  */
  
#include "PlanAgent.h"


namespace TA_Base_App
{
    ActivePlanFailureSummaryStepMessage::ActivePlanFailureSummaryStepMessage(TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error)
        : m_status(status), m_error(error)
    {

    }


    ActivePlanFailureSummaryStepMessage::~ActivePlanFailureSummaryStepMessage()
    {
    }


    void ActivePlanFailureSummaryStepMessage::execute( ActivePlan& activePlan ) const
    {        
        ActiveStep* activeStep = activePlan.getCurrentStep();
		TA_Base_Core::StepNumber stepNumber = activeStep->getStepNumber();

        TA_ASSERT(activeStep != NULL, "Invalid current step");

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "ActivePlanFailureSummaryStepMessage");

		if ( TA_Base_Core::PSCS_SUCCESS != m_status )            
	    {            
            activePlan.setInteractiveStepFailed();
            activeStep->processStepExecutionFailure(m_error);    		    
            return;
	    }

		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
													 stepNumber,
                                                     TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE, 
													 "Step execution successful" );

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "ActivePlanBulletinBoard::instance()->update");

        PlanExecutor::instance()->resume( activePlan.getPlan(), stepNumber);
 
        activePlan.setNextStep();
        activePlan.setInteractiveStep(false);
        
    }
}

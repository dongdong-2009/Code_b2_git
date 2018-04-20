/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanLogStepMessage.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2011/03/29 13:47:47 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */

#include "PlanAgent.h"
#include <algorithm> 

namespace TA_Base_App
{
	ActivePlanLogStepMessage::ActivePlanLogStepMessage(TA_Base_Core::EPlanStepCompletionStatus status, const std::string& logMessage)
		: m_status(status), m_logMessage(logMessage)
	{
	}

	void ActivePlanLogStepMessage::execute ( ActivePlan& activePlan ) const
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

		std::string session = ActivePlanBulletinBoard::instance()->getSession(activePlan.getPlan());

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, 
			"LOG STEP:Operator (SessionId[%s]):Plan ID(Number:%u,Instance:%u) logged.(%s).", 
			session.c_str(), activePlan.getPlan().plan, activePlan.getPlan().instance, m_logMessage.c_str() );

		TA_Base_Core::DescriptionParameters descriptionParameters;

		//xufeng++ TD14927
		TA_Base_Core::NameValuePair pair0( "DSSOperatorActionValue", "SUCCESSFUL");
		descriptionParameters.push_back ( &pair0 );
		//++xufeng TD14927

        TA_Base_Core::NameValuePair pair1( "PlanName", TA_Base_Bus::PlanDataReader::instance()->getNodePath(activePlan.getPlan().plan ));
		descriptionParameters.push_back ( &pair1 );

        std::ostringstream tempStrm;
        tempStrm << activePlan.getPlan().instance;
        TA_Base_Core::NameValuePair pair2( "InstanceNumber", tempStrm.str());
		descriptionParameters.push_back ( &pair2 );

        // TD#3400 - Enable Event Viewer to display the message in a readable format. 
        // Remove this once Event Viewer has been modified to properly handle non-printable characters.
        std::string strippedLogMessage = removeLineFeed(m_logMessage);

        TA_Base_Core::NameValuePair pair3( "LogMessage", strippedLogMessage);
		descriptionParameters.push_back ( &pair3 );

		AAMessageSender::instance()->sendAuditMessage(session, TA_Base_Core::PlanAgentAudit::PlanOperatorsLog, descriptionParameters);

		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
													 stepNumber,
                                                     TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE, "Step execution successful" );

        activePlan.setNextStep();
        activePlan.setInteractiveStep(false);
        
		//lizettejl++ DP-changes pass in the step number to resume execution
        PlanExecutor::instance()->resume( activePlan.getPlan(), activePlan.getStepNumber() );
    }

    // TD#3400
    std::string ActivePlanLogStepMessage::removeLineFeed ( const std::string& message ) const
    {
        std::string log = message;

        std::replace(log.begin(), log.end(), '\n', ' ');
        std::string::iterator last = std::remove(log.begin(), log.end(), '\r');

        log.resize(last - log.begin());

        return log;
    }
}

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveFailureSummaryStep.cpp $
  *  @author:  Lizette Lingo
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

#if defined ( STEPTYPE_FAILURE_SUMMARY_STEP )

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveFailureSummaryStep::ActiveFailureSummaryStep( const TA_Base_Core::StepDetail& stepDetail, 
									const TA_Base_Core::FailureSummaryParameters& params,
									IActivePlanFlow& flowControl ) :
		ActiveStep(stepDetail, flowControl),
		m_params(params),		
		m_flowControl(flowControl)
	{
	}

	TA_Base_Core::EActiveStepState ActiveFailureSummaryStep::executeImpl( void )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Failure summary step %lu", getStepNumber() );
		
		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;

		m_remark = "Waiting for Operator's response";

		try
		{
			// get failed step data
			TA_Base_Core::FailedStepDetails failedSteps = ActivePlanBulletinBoard::instance()->getFailedSteps(m_flowControl.getPlan());

			// initialize structure to pass to Plan Manager
			TA_Base_Bus::IPlanManagerCorbaDef::CurrentPlanStepDetail failureSummaryDetail;
			failureSummaryDetail.activePlan = m_flowControl.getPlan();
			failureSummaryDetail.step = getStepNumber();
			failureSummaryDetail.data.failedSteps(failedSteps);

			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Failure summary step %lu", failureSummaryDetail.data._d());

			PlanExecutor::instance()->pauseForOperator( m_flowControl.getPlan() );

			// ask Plan Manager to display the Failure Summary dialog
			TA_Base_Bus::IPlanManagerCorbaDef_ptr planManager = NULL;
			try
			{
				ActivePlanBulletinBoard::instance()->getPlanManagerObjRef(m_flowControl.getPlan(), planManager);
				planManager->executeInteractiveStep(failureSummaryDetail);
			}
			catch ( const CORBA::Exception& e)
			{
				LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "TA_Base_Core::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str(), dumpContext().c_str() );
				m_remark = "Plan Manager is not registered";
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}
			catch ( ... )
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Error: Unregistered PlanManager is running the interactive plan.");
				m_remark = "Plan Manager is not registered";
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}

		}
		catch ( const CORBA::Exception& e)
		{
			LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "TA_Base_Core::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str(), dumpContext().c_str() );
			m_remark = "Plan Manager is not registered";
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch ( ... )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Error: Unregistered PlanManager is running the interactive plan.");
			m_remark = "Plan Manager is not registered";
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}

		return( stepState );
	}

} // TA_Base_App


#endif //  STEPTYPE_FAILURE_SUMMARY_STEP

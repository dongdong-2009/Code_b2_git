/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveRunPlanStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

#if defined ( STEPTYPE_RUN_PLAN )
// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveRunPlanStep::ActiveRunPlanStep( const TA_Base_Core::StepDetail& stepDetail, 
									const TA_Base_Core::RunPlanParameters& params,
									IActivePlanFlow& flowControl ) :
		ActiveStep(stepDetail, flowControl),
		m_params(params),		
		m_flowControl(flowControl)
	{
        m_childPlan.plan = m_childPlan.instance = 0;
	}

	TA_Base_Core::EActiveStepState ActiveRunPlanStep::executeImpl( void )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "%s: running plan id %lu", dumpContext().c_str(), m_params.plan );

		m_childPlan.plan = m_params.plan;
		m_childPlan.instance = 0;

	    std::string session = ActivePlanBulletinBoard::instance()->getSession( m_flowControl.getPlan() );
   	    std::string pmId = ActivePlanBulletinBoard::instance()->getPlanManagerId( m_flowControl.getPlan() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::UNDEFINED_ACTIVE_STEP_STATE;

		if( m_params.manualLaunch )
		{
            stepState = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;
            m_remark = "Waiting for Plan Controller to be launched";

            PlanExecutor::instance()->pauseForOperator( m_flowControl.getPlan() );

			TA_Base_Bus::IPlanManagerCorbaDef_ptr planManager = NULL;

			try
			{
				ActivePlanBulletinBoard::instance()->getPlanManagerObjRef(m_flowControl.getPlan(), planManager);

				if ( planManager != NULL )
				{
					if ( planManager->_non_existent() )
					{
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Reference to non existent Plan Manager");
						m_remark = "Plan Manager is not registered";
						stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;						
					}
					else
                    {
                        try
                        {
				            PlanExecutor::instance()->spawnPending( session, pmId, m_flowControl.getPlan(), m_childPlan );
                            std::ostringstream message;
                            message << "ActiveRunPlanStep: pending plan[" << m_childPlan.plan << ":" << m_childPlan.instance << "] "
                                    << " reserved by the Plan Agent for session id[" << session.c_str() << "]";
				            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.str().c_str());

			                TA_Base_Bus::IPlanManagerCorbaDef::CurrentPlanStepDetail detail;

			                detail.activePlan = m_flowControl.getPlan();
			                detail.step = getStepNumber();
                            detail.data.runPlan(m_childPlan);

						    planManager->executeInteractiveStep(detail);
                            m_flowControl.setInteractiveStep(true);
                        }
                        catch (const TA_Base_Bus::IPlanManagerCorbaDef::InteractiveStepExecuteError& isee)
                        {   
                            std::string message = "ActiveRunPlanStep: Exception caught IPlanManagerCorbaDef::InteractiveStepExecuteError. ";
                            message += isee.reason.in();
				            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str() );

                            m_remark = "Plan Controller for child plan unable to launch. ";
                            m_remark += isee.reason.in();
                            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;                            
                        }
			            catch (const PlanNotFoundException&)
			            {
				            m_remark = "Plan not found";
				            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			            }
			            catch ( ... )
			            {
				            m_remark = "Failed to run Plan";
				            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			            }
                    }
				}
				else
				{
					LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Error: Unregistered Plan Manager is running the interactive plan.");
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
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Error: Unregistered Plan Manager is running the interactive plan.");
				m_remark = "Plan Manager is not registered";
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}
            
		}
		else
		{
            stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;
            m_remark = "Step executed";

			try
			{
                // Run plan in control mode
                PlanExecutor::instance()->run( session, pmId, m_childPlan, false, false );
                std::ostringstream message;
                message << "ActiveRunPlanStep: Execution of the plan[" << m_childPlan.plan << ":" << m_childPlan.instance << "] "
                        << " started by the Plan Agent for session id[" << session.c_str() << "]";
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.str().c_str());

		        if( m_params.waitForCompletion )
		        {
			        // Tell the active plan that this plan needs to wait for the
			        // child plan to complete successfully (or fail if it does not).
			        m_flowControl.setCompletedInterlock( m_childPlan );
			        
			        stepState = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;

                    m_remark = generateCompletionPendingRemark();
		        }

		        m_flowControl.setNextStep();	//Increment the step

			}
			catch (const PlanNotFoundException&)
			{
				m_remark = "Plan not found";
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}
			catch ( const TA_Base_Core::PlanAgentException&)
			{                
				m_remark = "Failed to run Plan";
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}
			catch ( ... )
			{
				m_remark = "Failed to run Plan";
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}
		}

        return stepState;
	}

    std::string ActiveRunPlanStep::generateCompletionPendingRemark() const
    {
        // There should be a pending instance of the child plan.
        TA_ASSERT(m_childPlan.plan > 0, "Invalid child plan ID");
        TA_ASSERT(m_childPlan.instance > 0, "Invalid child plan instance");

        std::ostringstream remark;
        remark << "Awaiting completion of plan '" 
               << TA_Base_Bus::PlanDataReader::instance()->getNodePath(m_childPlan.plan) 
               << "' instance " << m_childPlan.instance;

        return remark.str();
    }

} // TA_Base_App


#endif // STEPTYPE_RUN_PLAN


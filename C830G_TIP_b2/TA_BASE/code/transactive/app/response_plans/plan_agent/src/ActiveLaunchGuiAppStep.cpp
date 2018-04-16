/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveLaunchGuiAppStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

#if defined ( STEPTYPE_LAUNCH_GUI_APP )

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

	ActiveLaunchGuiAppStep::ActiveLaunchGuiAppStep( const TA_Base_Core::StepDetail& stepDetail, 
											const TA_Base_Core::LaunchGuiAppParameters& params,
											IActivePlanFlow& flowControl ) :
		ActiveStep(stepDetail, flowControl),
		m_params(params),
		m_flowControl(flowControl)
	{
	}

	TA_Base_Core::EActiveStepState ActiveLaunchGuiAppStep::executeImpl( void )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Prompt step %lu", getStepNumber() );

		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::LOADED_ACTIVE_STEP_STATE;

		m_remark = "Waiting for application to be launched";

		try
		{

			TA_Base_Bus::IPlanManagerCorbaDef::CurrentPlanStepDetail detail;

			detail.activePlan = m_flowControl.getPlan();
			detail.step = getStepNumber();
            detail.data.launchGuiApp(m_params);

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
						// The object is invalid
					}
					else
                    {
                        try
                        {
						    planManager->executeInteractiveStep(detail);
                            m_flowControl.setInteractiveStep(true);
                        }
                        catch (const TA_Base_Bus::IPlanManagerCorbaDef::InteractiveStepExecuteError& isee)
                        {   
                            std::string message = "ActiveLaunchGuiAppStep: Exception caught IPlanManagerCorbaDef::InteractiveStepExecuteError. ";
                            message += isee.reason.in();
				            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str() );
				            m_remark = "Application unable to launch. ";
                            m_remark += isee.reason.in();
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
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Error: Unregistered PlanManager is running the interactive plan.");
				m_remark = "Plan Manager is not registered";
				stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
			}

		}
		catch( const TA_Base_Core::ObjectResolutionException& e )
		{
				LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "TA_Base_Core::ObjectResolutionException", e.what(), dumpContext().c_str() );
				m_remark = "Plan Manager is not registered";
				stepState =  TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( const CORBA::Exception &e )
		{
				LOG( SOURCE_INFO, EXCEPTION_DETAIL_LOG_TYPE, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str(), dumpContext().c_str() );
				stepState =  TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
				m_remark = "Plan Manager is not registered";
		}

		return( stepState );
	}
} // TA_Base_App


#endif //  STEPTYPE_LAUNCH_GUI_APP

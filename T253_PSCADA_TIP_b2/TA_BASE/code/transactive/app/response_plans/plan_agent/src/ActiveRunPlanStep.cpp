/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveRunPlanStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #21 $
  *
  *  Last modification: $DateTime: 2015/04/30 20:09:36 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "core/utilities/src/DebugUtil.h"
#include "bus/response_plans/common/src/RpcAgency.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/plan_step_plugin/internal/src/RunPlanUserParameter.h"
#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/PlanExecutor.h"
#include "app/response_plans/plan_agent/src/ActiveRunPlanStep.h"
#include "app/response_plans/common/src/StringConstants.h"
 
namespace TA_Base_App
{

	ActiveRunPlanStep::ActiveRunPlanStep( const BasicParameter& detail, IActivePlan& plan ) : ActiveStep(m_params, plan),
	m_params( detail )
	{
	}

	bool ActiveRunPlanStep::executeImpl()
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s: running plan id %lu", dumpContext().c_str(), m_params.plan );

        // Reset instance number
		m_childPlan.instance = 0u;

        std::string session = ActivePlanBulletinBoard::instance()->getSession( m_iPlan.id() );

        bool bExecSucceed = false;

		if( m_params.manualLaunch )
		{
            try
            {
			    PlanExecutor::instance()->spawnPending( session, m_iPlan.id(), m_childPlan );

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "Pending plan[%lu, %lu] reserved by the Plan Agent for session id[%s]", 
                    m_childPlan.plan, m_childPlan.instance, session.c_str() );

                RunPlanUserParameter stUserParam( m_childPlan.plan, m_childPlan.instance, m_childPlan.location );
                std::vector<std::string> vecParameters = stUserParam.serialize();

                bExecSucceed = TA_Base_Bus::RpcAgency::getInstance().syncSendReq( m_iPlan.userLocation(), m_refParameter.basic.type, 
                    m_iPlan.id().location, m_iPlan.id().plan, m_iPlan.id().instance, id(), m_iPlan.userWorkstation(), vecParameters );

                remark( bExecSucceed ? RemarkWaitPlanControllerLaunch : RemarkStepExecutionFailed, false );
            }
		    catch (const PlanNotFoundException& ex)
		    {
			    LOG_EXCEPTION_CATCH(SourceInfo, "PlanNotFoundException", ex.what());
			    remark( RemarkNoFoundPlan );
		    }
		    catch ( ... )
		    {
			    LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when run a plan");
			    remark( RemarkRunPlanFailed );
		    }
        }
		else
		{
			try
			{
                PlanExecutor::instance()->run( session, m_childPlan, false, false );

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Execution of the[%lu, %lu] started by the Plan Agent for session id[%s]",
                    m_childPlan.plan, m_childPlan.instance, session.c_str() );

		        if( m_params.waitForCompletion )
		        {
                    remark( generateCompletionPendingRemark() );
                }

                bExecSucceed = true;
			}
			catch (const PlanNotFoundException& ex)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "PlanNotFoundException", ex.what());
			    remark( RemarkNoFoundPlan );
			}
            catch ( const std::exception& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
			    remark( RemarkRunPlanFailed );
            }
			catch ( ... )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when Run Plan");
			    remark( RemarkRunPlanFailed );
			}
        }

        return bExecSucceed;
	}

    std::string ActiveRunPlanStep::generateCompletionPendingRemark() const
    {
        // There should be a pending instance of the child plan.
        std::ostringstream remark;
        remark << "Awaiting completion of plan '" << m_iPlan.name() << "' instance " << m_childPlan.instance;

        return remark.str();
    }

    int ActiveRunPlanStep::waitImpl()
    {
        int nWaitRes = eAsyncWaitContinue;
        if ( m_params.waitForCompletion )
        {
            // [ToDo] m_childPlan is not synchronized to Monitor Mode Agent
            int ePlanState = ActivePlanBulletinBoard::instance()->getPlanState( m_childPlan );
            switch ( ePlanState )
            {
            case TA_Base_Core::COMPLETED_ACTIVE_PLAN_STATE:
                nWaitRes = eWaitSucceed;
                break;
            case TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE:
            case TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE:
            case TA_Base_Core::PENDING_ACTIVE_PLAN_STATE:
                nWaitRes = eAsyncWaitContinue;
                break;
            case TA_Base_Core::UNDEFINED_ACTIVE_PLAN_STATE:
            case TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE:
            case TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE:
            {
                nWaitRes = eWaitFailed;
                remark( RemarkPlanNotCompleted );
                break;
            }
            default:
                nWaitRes = eWaitFailed;
                break;
            }
        }
        else if ( m_params.manualLaunch )
        {
            nWaitRes = m_iPlan.stepResponsed(id());
        }
        
        static const ACE_Time_Value IdleTime(0, 500000); // 500 millisecond
        ACE_OS::sleep(IdleTime);

        return nWaitRes;
    }

	void ActiveRunPlanStep::initImpl()
	{
        m_childPlan.plan = m_params.plan;
        m_childPlan.instance = 0u;

        m_refParameter.basic.timeout = m_params.waitForCompletion ? std::numeric_limits<time_t>::max() : ( m_params.manualLaunch ? 60u : 0u );
	}

} // TA_Base_App


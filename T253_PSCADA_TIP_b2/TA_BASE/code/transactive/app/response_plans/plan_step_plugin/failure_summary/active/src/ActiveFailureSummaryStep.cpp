#include "ActiveFailureSummaryStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/common/src/StringConstants.h"
#include "app/response_plans/plan_step_plugin/failure_summary/common/FailureSummaryUserParameter.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "bus/response_plans/common/src/RpcAgency.h"
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"

namespace TA_Base_App
{
    ActiveFailureSummaryStep::ActiveFailureSummaryStep(const BasicParameter& detail, IActivePlan& flowControl) : ActiveStep(m_params, flowControl),
    m_params(detail)
    {
    }

    ActiveFailureSummaryStep::~ActiveFailureSummaryStep()
    {
    }

    bool ActiveFailureSummaryStep::executeImpl()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Failure summary step %lu", id() );

        bool bExecSucceed = false;
        
		FailureSummaryUserParameter parameter;

        try
        {
			TA_Base_Core::ExecutionHistory_var executionHistory;
			TA_Base_Core::ExecutionHistory_out tmp(executionHistory);
            // Always same location
			static TA_Base_Bus::PlanAgentLibrary planAgent( m_iPlan.id().location );
			planAgent.getPlanExecutionHistory(m_iPlan.id(), tmp);
			if(executionHistory.operator->() == NULL)
			{
				return bExecSucceed;
			}

            for (CORBA::ULong i = 0; i < executionHistory->details.length(); ++i)
            {
                TA_Base_Core::ActiveStepDetail& stepDetail =  executionHistory->details[i];

                if (stepDetail.state == TA_Base_Core::FAILED_ACTIVE_STEP_STATE)
                {
					FailureStepDetail failedStep;
	                failedStep.pos =  stepDetail.pos;
	                failedStep.name = stepDetail.name;
	                failedStep.error = stepDetail.remark;
					parameter.failedSteps.push_back(failedStep);
                }
            }

            bExecSucceed = TA_Base_Bus::RpcAgency::getInstance().syncSendReq( m_iPlan.userLocation(), m_refParameter.basic.type,
                m_iPlan.id().location, m_iPlan.id().plan, m_iPlan.id().instance, id(), m_iPlan.userWorkstation(), parameter.serialize() );

            if ( bExecSucceed )
            {
                remark( RemarkWaitingForOperatorResponse, false );
            }
        }
        catch ( const CORBA::Exception& e)
        {
            LOG_EXCEPTION_DETAILS( SourceInfo, "TA_Base_Core::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
            remark( "Failed to retrieve failure summary" );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Caught unknown exception when getPlanExecutionHistory");
            remark( "Failed to retrieve failure summary" );
        }

        return bExecSucceed;
    }
    
    int ActiveFailureSummaryStep::waitImpl()
    {
        // Wait for operator response
        return m_iPlan.stepResponsed( id() );
    }

	void ActiveFailureSummaryStep::initImpl()
	{
        m_refParameter.basic.timeout = std::numeric_limits<time_t>::max();
	}
}
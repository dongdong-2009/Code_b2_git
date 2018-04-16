
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/response_plans/common/src/RpcAgency.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/plan_step_plugin/decision/common/DecisionResponseParameter.h"
#include "app/response_plans/plan_step_plugin/decision/active/src/ActiveDecisionStep.h"
#include "app/response_plans/common/src/StringConstants.h"

namespace TA_Base_App
{
    ActiveDecisionStep::ActiveDecisionStep( const BasicParameter& detail, IActivePlan& plan ) : ActiveStep( m_params, plan ),
    m_params(detail),
    m_nextStepId(0)
    {
    }

    ActiveDecisionStep::~ActiveDecisionStep()
    {
    }

    bool ActiveDecisionStep::executeImpl()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Decision step %lu", id());

        bool bExecSucceed = false;

        std::vector<std::string> vecParameters = m_params.serialize();

        bExecSucceed = TA_Base_Bus::RpcAgency::getInstance().syncSendReq( m_iPlan.userLocation(), m_refParameter.basic.type,
            m_iPlan.id().location, m_iPlan.id().plan, m_iPlan.id().instance, id(), m_iPlan.userWorkstation(), vecParameters );
            
        remark( bExecSucceed ? RemarkWaitingForOperatorResponse : RemarkStepExecutionFailed, false );

        return bExecSucceed;
    }

    int ActiveDecisionStep::waitImpl()
    {
        // Wait for operator response
        std::vector<std::string> vecResponse = m_iPlan.stepData( id() );

        if ( vecResponse.empty() )
        {
            return eAsyncWaitContinue;
        }

        DecisionResponseParameter responseParam( vecResponse );

        int nWaitStatus = eWaitFailed;

        switch ( responseParam.response )
        {
        case TA_Base_Core::PSCS_PAUSE_RESPONSE:
            remark( "Operator request pause" );
            nWaitStatus = eWaitFailed;
            break;
        case TA_Base_Core::PSCS_TIMEOUT:
            remark( "Operator has no response within timeout" );
            nWaitStatus = eWaitFailed;
            break;
        case TA_Base_Core::PSCS_YES_RESPONSE:
            m_nextStepId = m_params.yesStep;
            nWaitStatus = eWaitSucceed;
            break;
        case TA_Base_Core::PSCS_NO_RESPONSE:
            m_nextStepId = m_params.noStep;
            nWaitStatus = eWaitSucceed;
            break;
        default:
            break;
        }

        return nWaitStatus;
    }

	void ActiveDecisionStep::initImpl()
	{
        m_refParameter.basic.timeout = m_params.responseTimeout;
	}

	std::list<ActiveStep*> ActiveDecisionStep::getNextStep()
    {
        std::list<ActiveStep*> outgoingStepList;

		for( std::list<ActiveStep*>::iterator itLoop = m_lstOutgoingStep.begin(); itLoop != m_lstOutgoingStep.end(); ++itLoop )
		{
			if( m_nextStepId == (*itLoop)->id() )
			{
				outgoingStepList.push_back( *itLoop );
				break;
			}
		}

		return outgoingStepList;
	}

}
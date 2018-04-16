#include "ActivePlanCheckStep.h"
#include "PlanAgentAccess.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/common/src/StringConstants.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PlanAgentComms_MessageTypes.h"

namespace TA_Base_App
{
    ActivePlanCheckStep::ActivePlanCheckStep(const BasicParameter& detail, IActivePlan& flowControl)
		: ActiveStep(m_params, flowControl),
          m_params(detail),
		  m_nextStepId(0)
    {
    }

    ActivePlanCheckStep::~ActivePlanCheckStep()
    {
    }

    bool ActivePlanCheckStep::executeImpl()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s: checking for plan %lu", dumpContext().c_str(), m_params.plan );

		TA_Base_Core::ActivePlanDetails_var activePlanDetails = PlanAgentAccess::getInstance().getActivePlanDetails(m_params.plan, m_iPlan.id().location);
		bool bCheckResult = isActive(activePlanDetails, m_iPlan.id());
		m_nextStepId = bCheckResult ? m_params.yesStep : m_params.noStep;

		return true;
    }

	std::list<ActiveStep*> ActivePlanCheckStep::getNextStep()
	{
		std::list<ActiveStep*> outgoingStepList;
		std::list<ActiveStep*>::iterator itActiveStep = m_lstOutgoingStep.begin();
		for(; itActiveStep != m_lstOutgoingStep.end(); ++itActiveStep)
		{
			if(m_nextStepId == (*itActiveStep)->id())
			{
				outgoingStepList.push_back(*itActiveStep);
				break;
			}
		}
		return outgoingStepList;
	}

	bool ActivePlanCheckStep::isActive( const TA_Base_Core::ActivePlanDetails& activePlanDetails, const TA_Base_Core::ActivePlanId& ignoreInstance ) const
	{
		bool active = false;

		// check that it is not the instance of the plan that we
		// should be ignoring
		for (int nLoop = 0; nLoop < activePlanDetails.length(); nLoop++)
		{
			TA_Base_Core::ActivePlanDetail planDetail = activePlanDetails[nLoop];
			if( planDetail.plan.plan != ignoreInstance.plan )
			{
				active = isActive(planDetail.activePlanState);
			}
			else
			{
				if( planDetail.plan.instance != ignoreInstance.instance )
				{
					active = isActive(planDetail.activePlanState);
				}
			}

			if (active)
			{
				break;
			}
			
		}

		// No PlanNotFoundException as asking if a non-existant plan is active is a valid question.
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[P:%lu]: %s active.", m_params.plan, (active?"is":"is NOT") );

		return active;
	}

	bool ActivePlanCheckStep::isActive(const TA_Base_Core::EActivePlanState activePlanState) const
	{
		switch( activePlanState )
		{
		case TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE:
		case TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE:
			return true;

		default:
			break;
		}

		return false;
	}
}
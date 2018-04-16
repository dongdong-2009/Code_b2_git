#include "ActiveEvaluateExpressionStep.h"
#include "BooleanContextImpl.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/common/src/StringConstants.h"
//#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PlanAgentComms_MessageTypes.h"

namespace TA_Base_App
{
    ActiveEvaluateExpressionStep::ActiveEvaluateExpressionStep(const BasicParameter& detail, IActivePlan& flowControl)
        : ActiveStep(m_params, flowControl),
          m_params(detail),
		  m_nextStepId(0)
    {
    }

    ActiveEvaluateExpressionStep::~ActiveEvaluateExpressionStep()
    {
    }

    bool ActiveEvaluateExpressionStep::executeImpl()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Evaluate Expression step %lu", id());

        bool bExecSucceed = false;

        bool success = false;
        std::string expression = m_params.expression;
        std::auto_ptr<TA_Base_Bus::ExpressionEvaluator> expEval(new TA_Base_Bus::ExpressionEvaluator());
        std::auto_ptr<BooleanContextImpl> context(new BooleanContextImpl());

        try
        {
            //as we are only interested in boolean expressions
            //we use boolean context only
            expEval->setNewExpression(expression);
            expEval->addBooleanContext(context.get());
            success = expEval->evaluateBooleanExpression();

            bExecSucceed = true;
        }
        catch(TA_Base_Core::MathematicalEvaluationException& mee)
        {
            //should not get here as we do check for valid expression before running this step (plan)
			m_remark = RemarkInvalidExpress;
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_App::MathematicalEvaluationException", mee.what());
        }
        catch(...)
        {
			m_remark = RemarkInvalidStepParameter;
            LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Caught unknown exception when evaluateBooleanExpression");
        }

		if (bExecSucceed)
		{
			m_nextStepId = success ? m_params.successStep : m_params.failureStep;
		}

        return bExecSucceed;
    }

	std::list<ActiveStep*> ActiveEvaluateExpressionStep::getNextStep()
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
}
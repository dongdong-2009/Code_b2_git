#include "app/response_plans/plan_step_plugin/decision/active/src/DecisionStepComponent.h"
#include "app/response_plans/plan_step_plugin/decision/active/src/ActiveDecisionStep.h"
#include "app/response_plans/plan_step_plugin/decision/active/src/ActivePlanDecisionStepMessage.h"

namespace TA_Base_App
{


    ActiveStep* DecisionStepComponentImpl::createActiveStep(const BasicParameter& refDetail, TA_Base_App::IActivePlan& iPlan)
    {
        return new ActiveDecisionStep( refDetail, iPlan );
    }

    void DecisionStepComponentImpl::validateStepParameter(const std::vector<std::string>& parameters)
    {

    }

    IActiveStepMessage* DecisionStepComponentImpl::createPlanStepMessage(const unsigned long ulStepNum, const std::vector<std::string>& vecData)
    {
        return new TA_Base_App::ActivePlanDecisionStepMessage(ulStepNum, vecData);
    }

    DecisionStepComponentImpl::DecisionStepComponentImpl()
    {

    }

    DecisionStepComponentImpl::~DecisionStepComponentImpl()
    {

    }

}
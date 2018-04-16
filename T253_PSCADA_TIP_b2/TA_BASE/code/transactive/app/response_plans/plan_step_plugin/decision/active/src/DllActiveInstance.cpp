#include "app/response_plans/plan_step_plugin/decision/active/src/DecisionStepComponent.h"
#include "app/response_plans/plan_step_plugin/common/src/ExportDef.h"

extern "C" Plan_Export TA_Base_App::IStepComponent* createStepComponent()
{
    return TA_Base_App::DecisionStepComponent::instance();
}

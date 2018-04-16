#include "app/response_plans/plan_step_plugin/pa/train_pa/active/src/TrainPaStepComponent.h"
#include "app/response_plans/plan_step_plugin/common/src/ExportDef.h"

extern "C" Plan_Export TA_Base_App::IStepComponent* createStepComponent()
{
    return TA_Base_App::TrainPaStepComponent::instance();
}


#include "app/response_plans/plan_step_plugin/tis/station_tis/active/src/StationTisStepComponent.h"
#include "app/response_plans/plan_step_plugin/common/src/ExportDef.h"

extern "C" Plan_Export TA_Base_App::IStepComponent* createStepComponent()
{
    return TA_Base_App::StationTisStepComponent::instance();
}


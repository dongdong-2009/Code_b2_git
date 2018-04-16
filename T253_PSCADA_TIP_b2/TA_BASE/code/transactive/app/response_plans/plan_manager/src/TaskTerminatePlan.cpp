#include "app/response_plans/plan_manager/src/TaskTerminatePlan.h"

namespace TA_Base_App
{

const std::string TaskTerminatePlan::Name = "Terminate Plan";
const std::string TaskTerminatePlan::DisplayName = "Terminate Plan";
const std::string TaskTerminatePlan::Resource = "res/PlanActivityTerminatePlan.bmp";
const std::string ActivityIconResource = "res/PlanActivityIconTerminatePlan.bmp";

TaskTerminatePlan::TaskTerminatePlan() : BitmapActivity( ActivityIconResource, false )
{
}

TaskTerminatePlan::~TaskTerminatePlan()
{
}

void TaskTerminatePlan::serializeAddons( Stringification& ioSerilization )
{
}

}

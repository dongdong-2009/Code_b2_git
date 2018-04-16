#include "app/response_plans/plan_manager/src/TaskRunPlan.h"

namespace TA_Base_App
{

const std::string TaskRunPlan::Name = "Run Plan";
const std::string TaskRunPlan::DisplayName = "Run Plan";
const std::string TaskRunPlan::Resource = "res/PlanActivityRunPlan.bmp";
const std::string ActivityIconResource = "res/PlanActivityIconRunPlan.bmp";

TaskRunPlan::TaskRunPlan() : BitmapActivity( ActivityIconResource, false )
{
}

TaskRunPlan::~TaskRunPlan()
{
}

void TaskRunPlan::serializeAddons( Stringification& ioSerilization )
{
}

}

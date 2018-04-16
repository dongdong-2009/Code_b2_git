#include "TaskPlanCheck.h"

namespace TA_Base_App
{

const std::string TaskPlanCheck::Name = "PlanCheck";
const std::string TaskPlanCheck::DisplayName = "PlanCheck";
const std::string TaskPlanCheck::Resource = "res/PlanActivityCheckPlan.bmp";

TaskPlanCheck::TaskPlanCheck() : ServiceTask(),
m_oIcon( Resource )
{
}

TaskPlanCheck::~TaskPlanCheck()
{
}

void TaskPlanCheck::serializeAddons( Stringification& ioSerilization )
{

}

PresentEntity& TaskPlanCheck::getIcon()
{
    return m_oIcon;
}

std::string TaskPlanCheck::displayName()
{
	return DisplayName;
}

std::string TaskPlanCheck::name()
{
	return Name;
}

std::string TaskPlanCheck::resource()
{
	return Resource;
}

}


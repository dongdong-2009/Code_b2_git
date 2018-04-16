#include "TaskPlanSetTimer.h"

namespace TA_Base_App
{

const std::string TaskPlanSetTimer::Name = "PlanSetTimer";
const std::string TaskPlanSetTimer::DisplayName = "PlanSetTimer";
const std::string TaskPlanSetTimer::Resource = "res/PlanActivityPlanSetTimer.bmp";

TaskPlanSetTimer::TaskPlanSetTimer() : ServiceTask(),
m_oIcon( Resource )
{
}

TaskPlanSetTimer::~TaskPlanSetTimer()
{
}

void TaskPlanSetTimer::serializeAddons( Stringification& ioSerilization )
{

}

PresentEntity& TaskPlanSetTimer::getIcon()
{
    return m_oIcon;
}

std::string TaskPlanSetTimer::displayName()
{
	return DisplayName;
}

std::string TaskPlanSetTimer::name()
{
	return Name;
}

std::string TaskPlanSetTimer::resource()
{
	return Resource;
}

}


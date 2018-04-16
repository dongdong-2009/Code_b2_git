#include "TaskPlanCheckTimer.h"

namespace TA_Base_App
{

const std::string TaskPlanCheckTimer::Name = "PlanCheckTimer";
const std::string TaskPlanCheckTimer::DisplayName = "PlanCheckTimer";
const std::string TaskPlanCheckTimer::Resource = "res/PlanActivityPlanCheckTimer.bmp";

TaskPlanCheckTimer::TaskPlanCheckTimer() : ServiceTask(),
m_oIcon( Resource )
{
}

TaskPlanCheckTimer::~TaskPlanCheckTimer()
{
}

void TaskPlanCheckTimer::serializeAddons( Stringification& ioSerilization )
{

}

PresentEntity& TaskPlanCheckTimer::getIcon()
{
    return m_oIcon;
}

std::string TaskPlanCheckTimer::displayName()
{
	return DisplayName;
}

std::string TaskPlanCheckTimer::name()
{
	return Name;
}

std::string TaskPlanCheckTimer::resource()
{
	return Resource;
}

}


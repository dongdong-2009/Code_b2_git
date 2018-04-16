#include "app/response_plans/plan_step_plugin/decision/editor/src/TaskDecision.h"

namespace TA_Base_App
{

const std::string TaskDecision::Name = "Decision";
const std::string TaskDecision::DisplayName = "Decision";
const std::string TaskDecision::Resource = "res/PlanActivityDecision.bmp";

TaskDecision::TaskDecision() : ServiceTask(),
m_oIcon( Resource )
{
}

TaskDecision::~TaskDecision()
{
}

void TaskDecision::serializeAddons( Stringification& ioSerilization )
{

}

PresentEntity& TaskDecision::getIcon()
{
    return m_oIcon;
}

std::string TaskDecision::displayName()
{
	return DisplayName;
}

std::string TaskDecision::name()
{
	return Name;
}

std::string TaskDecision::resource()
{
	return Resource;
}

}


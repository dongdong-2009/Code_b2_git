#include "TaskPrompt.h"

namespace TA_Base_App
{

const std::string TaskPrompt::Name = "Prompt";
const std::string TaskPrompt::DisplayName = "Prompt";
const std::string TaskPrompt::Resource = "res/PlanActivityPrompt.bmp";

TaskPrompt::TaskPrompt() : ServiceTask(),
m_oIcon( Resource )
{
}

TaskPrompt::~TaskPrompt()
{
}

void TaskPrompt::serializeAddons( Stringification& ioSerilization )
{

}

PresentEntity& TaskPrompt::getIcon()
{
    return m_oIcon;
}

std::string TaskPrompt::displayName()
{
	return DisplayName;
}

std::string TaskPrompt::name()
{
	return Name;
}

std::string TaskPrompt::resource()
{
	return Resource;
}

}


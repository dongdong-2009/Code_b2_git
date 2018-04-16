#include "TaskExecuteScript.h"

namespace TA_Base_App
{

const std::string TaskExecuteScript::Name = "ExecuteScript";
const std::string TaskExecuteScript::DisplayName = "ExecuteScript";
const std::string TaskExecuteScript::Resource = "res/PlanActivityExecuteScript.bmp";

TaskExecuteScript::TaskExecuteScript() : ServiceTask(),
m_oIcon( Resource )
{
}

TaskExecuteScript::~TaskExecuteScript()
{
}

void TaskExecuteScript::serializeAddons( Stringification& ioSerilization )
{

}

PresentEntity& TaskExecuteScript::getIcon()
{
    return m_oIcon;
}

std::string TaskExecuteScript::displayName()
{
	return DisplayName;
}

std::string TaskExecuteScript::name()
{
	return Name;
}

std::string TaskExecuteScript::resource()
{
	return Resource;
}

}


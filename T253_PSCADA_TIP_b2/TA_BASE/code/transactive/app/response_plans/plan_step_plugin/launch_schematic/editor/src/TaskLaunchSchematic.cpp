#include "TaskLaunchSchematic.h"

namespace TA_Base_App
{

const std::string TaskLaunchSchematic::Name = "LaunchSchematic";
const std::string TaskLaunchSchematic::DisplayName = "LaunchSchematic";
const std::string TaskLaunchSchematic::Resource = "res/PlanActivityLaunchSchematic.bmp";

TaskLaunchSchematic::TaskLaunchSchematic() : ServiceTask(),
m_oIcon( Resource )
{
}

TaskLaunchSchematic::~TaskLaunchSchematic()
{
}

void TaskLaunchSchematic::serializeAddons( Stringification& ioSerilization )
{

}

PresentEntity& TaskLaunchSchematic::getIcon()
{
    return m_oIcon;
}

std::string TaskLaunchSchematic::displayName()
{
	return DisplayName;
}

std::string TaskLaunchSchematic::name()
{
	return Name;
}

std::string TaskLaunchSchematic::resource()
{
	return Resource;
}

}


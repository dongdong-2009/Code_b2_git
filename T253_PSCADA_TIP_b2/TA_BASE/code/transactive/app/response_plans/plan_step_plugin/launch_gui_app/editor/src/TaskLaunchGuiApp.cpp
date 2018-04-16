#include "TaskLaunchGuiApp.h"

namespace TA_Base_App
{

const std::string TaskLaunchGuiApp::Name = "LaunchGuiApp";
const std::string TaskLaunchGuiApp::DisplayName = "LaunchGuiApp";
const std::string TaskLaunchGuiApp::Resource = "res/PlanActivityLaunchGuiApp.bmp";

TaskLaunchGuiApp::TaskLaunchGuiApp() : ServiceTask(),
m_oIcon( Resource )
{
}

TaskLaunchGuiApp::~TaskLaunchGuiApp()
{
}

void TaskLaunchGuiApp::serializeAddons( Stringification& ioSerilization )
{

}

PresentEntity& TaskLaunchGuiApp::getIcon()
{
    return m_oIcon;
}

std::string TaskLaunchGuiApp::displayName()
{
	return DisplayName;
}

std::string TaskLaunchGuiApp::name()
{
	return Name;
}

std::string TaskLaunchGuiApp::resource()
{
	return Resource;
}

}


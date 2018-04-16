#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/editor/src/TaskAssignCameraToMonitor.h"

namespace TA_Base_App
{

const std::string TaskAssignCameraToMonitor::Name = "AssignCameraToMonitor";
const std::string TaskAssignCameraToMonitor::DisplayName = "AssignCameraToMonitor";
const std::string TaskAssignCameraToMonitor::Resource = "res/PlanActivityAssignCameraToMonitor.bmp";

TaskAssignCameraToMonitor::TaskAssignCameraToMonitor() : ServiceTask(),
m_oIcon( Resource )
{
}

TaskAssignCameraToMonitor::~TaskAssignCameraToMonitor()
{
}

void TaskAssignCameraToMonitor::serializeAddons( Stringification& ioSerilization )
{

}

PresentEntity& TaskAssignCameraToMonitor::getIcon()
{
    return m_oIcon;
}

std::string TaskAssignCameraToMonitor::categoryName()
{
	return CategroyName;
}

std::string TaskAssignCameraToMonitor::displayName()
{
	return DisplayName;
}

std::string TaskAssignCameraToMonitor::name()
{
	return Name;
}

std::string TaskAssignCameraToMonitor::resource()
{
	return Resource;
}

}


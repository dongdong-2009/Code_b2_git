#include "app/response_plans/plan_step_plugin/tis/station_tis/editor/src/TaskStationTis.h"


namespace TA_Base_App
{

const std::string TaskStationTis::Name = "Station Tis";
const std::string TaskStationTis::DisplayName = "Station Tis";
const std::string TaskStationTis::Resource = "res/PlanActivityStationTis.bmp";


TaskStationTis::TaskStationTis() : ServiceTask(),
m_oIcon(Resource)
{
}

TaskStationTis::~TaskStationTis()
{
}

void TaskStationTis::serializeAddons( Stringification& ioSerilization )
{
}

PresentEntity& TaskStationTis::getIcon()
{
	return m_oIcon;
}

std::string TaskStationTis::displayName()
{
	return DisplayName;
}

std::string TaskStationTis::name()
{
	return Name;
}

std::string TaskStationTis::resource()
{
	return Resource;
}

}
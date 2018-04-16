#include "app/response_plans/plan_step_plugin/pa/station_pa/editor/src/TaskStationPa.h"


namespace TA_Base_App
{

const std::string TaskStationPa::Name = "Station PA";
const std::string TaskStationPa::DisplayName = "Station PA";
const std::string TaskStationPa::Resource = "res/PlanActivityStationPA.bmp";

TaskStationPa::TaskStationPa() : ServiceTask(),
m_oIcon(Resource)
{
}

TaskStationPa::~TaskStationPa()
{
}

void TaskStationPa::serializeAddons( Stringification& ioSerilization )
{
}

PresentEntity& TaskStationPa::getIcon()
{
	return m_oIcon;
}

std::string TaskStationPa::displayName()
{
	return DisplayName;
}

std::string TaskStationPa::name()
{
	return Name;
}

std::string TaskStationPa::resource()
{
	return Resource;
}

}




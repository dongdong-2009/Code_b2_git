#include "app/response_plans/plan_step_plugin/data_point_set/editor/src/TaskDataPointSet.h"

namespace TA_Base_App
{

const std::string TaskDataPointSet::Name = "Datapoint Set";
const std::string TaskDataPointSet::DisplayName = "Datapoint Set";
const std::string TaskDataPointSet::Resource = "res//PlanActivityDataPointSet.bmp";

TaskDataPointSet::TaskDataPointSet() : ServiceTask(),
m_oIcon( Resource )
{
}

TaskDataPointSet::~TaskDataPointSet()
{
}

void TaskDataPointSet::serializeAddons( Stringification& ioSerilization )
{

}

PresentEntity& TaskDataPointSet::getIcon()
{
    return m_oIcon;
}

std::string TaskDataPointSet::categoryName()
{
	return CategroyName;
}

std::string TaskDataPointSet::displayName()
{
	return DisplayName;
}

std::string TaskDataPointSet::name()
{
	return Name;
}

std::string TaskDataPointSet::resource()
{
	return Resource;
}

}


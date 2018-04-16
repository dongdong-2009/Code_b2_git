#include "TaskDataPointCheck.h"

namespace TA_Base_App
{

const std::string TaskDataPointCheck::Name = "Datapoint Check";
const std::string TaskDataPointCheck::DisplayName = "Datapoint Check";
const std::string TaskDataPointCheck::Resource = "res//PlanActivityDataPointCheck.bmp";

TaskDataPointCheck::TaskDataPointCheck() : ServiceTask(),
m_oIcon( Resource )
{
}

TaskDataPointCheck::~TaskDataPointCheck()
{
}

void TaskDataPointCheck::serializeAddons( Stringification& ioSerilization )
{

}

PresentEntity& TaskDataPointCheck::getIcon()
{
    return m_oIcon;
}

std::string TaskDataPointCheck::categoryName()
{
	return CategroyName;
}

std::string TaskDataPointCheck::displayName()
{
	return DisplayName;
}

std::string TaskDataPointCheck::name()
{
	return Name;
}

std::string TaskDataPointCheck::resource()
{
	return Resource;
}

}


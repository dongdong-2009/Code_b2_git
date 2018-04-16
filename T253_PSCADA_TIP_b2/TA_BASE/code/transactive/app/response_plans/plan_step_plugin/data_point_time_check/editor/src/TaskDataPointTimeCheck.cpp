#include "TaskDataPointTimeCheck.h"

namespace TA_Base_App
{

const std::string TaskDataPointTimeCheck::Name = "DataPointTimeCheck";
const std::string TaskDataPointTimeCheck::DisplayName = "DataPointTimeCheck";
const std::string TaskDataPointTimeCheck::Resource = "res/PlanActivityDataPointTimeCheck.bmp";

TaskDataPointTimeCheck::TaskDataPointTimeCheck() : ServiceTask(),
m_oIcon( Resource )
{
}

TaskDataPointTimeCheck::~TaskDataPointTimeCheck()
{
}

void TaskDataPointTimeCheck::serializeAddons( Stringification& ioSerilization )
{

}

PresentEntity& TaskDataPointTimeCheck::getIcon()
{
    return m_oIcon;
}

std::string TaskDataPointTimeCheck::displayName()
{
	return DisplayName;
}

std::string TaskDataPointTimeCheck::name()
{
	return Name;
}

std::string TaskDataPointTimeCheck::resource()
{
	return Resource;
}

}


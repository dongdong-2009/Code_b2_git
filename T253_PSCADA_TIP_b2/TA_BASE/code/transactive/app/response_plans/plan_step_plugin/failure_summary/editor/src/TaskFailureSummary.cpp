#include "TaskFailureSummary.h"

namespace TA_Base_App
{

const std::string TaskFailureSummary::Name = "FailureSummary";
const std::string TaskFailureSummary::DisplayName = "FailureSummary";
const std::string TaskFailureSummary::Resource = "res/PlanActivityFailureSummary.bmp";

TaskFailureSummary::TaskFailureSummary() : ServiceTask(),
m_oIcon( Resource )
{
}

TaskFailureSummary::~TaskFailureSummary()
{
}

void TaskFailureSummary::serializeAddons( Stringification& ioSerilization )
{

}

PresentEntity& TaskFailureSummary::getIcon()
{
    return m_oIcon;
}

std::string TaskFailureSummary::displayName()
{
	return DisplayName;
}

std::string TaskFailureSummary::name()
{
	return Name;
}

std::string TaskFailureSummary::resource()
{
	return Resource;
}

}


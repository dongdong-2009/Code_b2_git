#include "TaskJump.h"

namespace TA_Base_App
{

const std::string TaskJump::Name = "Jump";
const std::string TaskJump::DisplayName = "Jump";
const std::string TaskJump::Resource = "res/PlanActivityJump.bmp";

TaskJump::TaskJump() : ServiceTask(),
m_oIcon( Resource )
{
}

TaskJump::~TaskJump()
{
}

void TaskJump::serializeAddons( Stringification& ioSerilization )
{

}

PresentEntity& TaskJump::getIcon()
{
    return m_oIcon;
}

std::string TaskJump::displayName()
{
	return DisplayName;
}

std::string TaskJump::name()
{
	return Name;
}

std::string TaskJump::resource()
{
	return Resource;
}

}


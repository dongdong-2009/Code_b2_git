#include "TaskShutdownSystem.h"


namespace TA_Base_App
{

const std::string TaskShutdownSystem::Name = "Shutdown System";
const std::string TaskShutdownSystem::DisplayName = "Shutdown System";
const std::string TaskShutdownSystem::Resource = "res/PlanActivityShutdownSystem.bmp";

TaskShutdownSystem::TaskShutdownSystem() : ServiceTask(),
m_oIcon(Resource)
{
}

TaskShutdownSystem::~TaskShutdownSystem()
{
}

void TaskShutdownSystem::serializeAddons( Stringification& ioSerilization )
{
}

PresentEntity& TaskShutdownSystem::getIcon()
{
	return m_oIcon;
}

std::string TaskShutdownSystem::displayName()
{
	return DisplayName;
}

std::string TaskShutdownSystem::name()
{
	return Name;
}

std::string TaskShutdownSystem::resource()
{
	return Resource;
}

}
#include "TaskPopupMftMessage.h"

namespace TA_Base_App
{

const std::string TaskPopupMftMessage::Name = "PopupMftMessage";
const std::string TaskPopupMftMessage::DisplayName = "PopupMftMessage";
const std::string TaskPopupMftMessage::Resource = "res/PlanActivityPopupMftMessage.bmp";

TaskPopupMftMessage::TaskPopupMftMessage() : ServiceTask(),
m_oIcon( Resource )
{
}

TaskPopupMftMessage::~TaskPopupMftMessage()
{
}

void TaskPopupMftMessage::serializeAddons( Stringification& ioSerilization )
{

}

PresentEntity& TaskPopupMftMessage::getIcon()
{
    return m_oIcon;
}

std::string TaskPopupMftMessage::displayName()
{
	return DisplayName;
}

std::string TaskPopupMftMessage::name()
{
	return Name;
}

std::string TaskPopupMftMessage::resource()
{
	return Resource;
}

}


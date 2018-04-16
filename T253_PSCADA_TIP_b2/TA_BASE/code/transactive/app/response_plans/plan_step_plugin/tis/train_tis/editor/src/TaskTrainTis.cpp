#include "app/response_plans/plan_step_plugin/tis/train_tis/editor/src/TaskTrainTis.h"


namespace TA_Base_App
{

const std::string TaskTrainTis::Name = "TrainTis";
const std::string TaskTrainTis::DisplayName = "TrainTis";
const std::string TaskTrainTis::Resource = "res/PlanActivityTrainTis.bmp";

TaskTrainTis::TaskTrainTis() : ServiceTask(),
m_oIcon(Resource)
{
}

TaskTrainTis::~TaskTrainTis()
{
}

void TaskTrainTis::serializeAddons( Stringification& ioSerilization )
{
}

PresentEntity& TaskTrainTis::getIcon()
{
	return m_oIcon;
}

std::string TaskTrainTis::displayName()
{
	return DisplayName;
}

std::string TaskTrainTis::name()
{
	return Name;
}

std::string TaskTrainTis::resource()
{
	return Resource;
}

}
#include "app/response_plans/plan_step_plugin/pa/train_pa/editor/src/TaskTrainPa.h"


namespace TA_Base_App
{

const std::string TaskTrainPa::Name = "TrainPa";
const std::string TaskTrainPa::DisplayName = "TrainPa";
const std::string TaskTrainPa::Resource = "res/PlanActivityTrainPa.bmp";

TaskTrainPa::TaskTrainPa() : ServiceTask(),
m_oIcon(Resource)
{
}

TaskTrainPa::~TaskTrainPa()
{
}

void TaskTrainPa::serializeAddons( Stringification& ioSerilization )
{
}

PresentEntity& TaskTrainPa::getIcon()
{
	return m_oIcon;
}

std::string TaskTrainPa::displayName()
{
	return DisplayName;
}

std::string TaskTrainPa::name()
{
	return Name;
}

std::string TaskTrainPa::resource()
{
	return Resource;
}

}
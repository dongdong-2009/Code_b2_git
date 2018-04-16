#include "TaskEvaluateExpression.h"

namespace TA_Base_App
{

const std::string TaskEvaluateExpression::Name = "EvaluateExpression";
const std::string TaskEvaluateExpression::DisplayName = "EvaluateExpression";
const std::string TaskEvaluateExpression::Resource = "res/PlanActivityEvaluateExpression.bmp";

TaskEvaluateExpression::TaskEvaluateExpression() : ServiceTask(),
m_oIcon( Resource )
{
}

TaskEvaluateExpression::~TaskEvaluateExpression()
{
}

void TaskEvaluateExpression::serializeAddons( Stringification& ioSerilization )
{

}

PresentEntity& TaskEvaluateExpression::getIcon()
{
    return m_oIcon;
}

std::string TaskEvaluateExpression::displayName()
{
	return DisplayName;
}

std::string TaskEvaluateExpression::name()
{
	return Name;
}

std::string TaskEvaluateExpression::resource()
{
	return Resource;
}

}


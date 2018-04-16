#include "ActiveLaunchGuiAppStep.h"
#include "ActivePlanLaunchGuiAppStepMessage.h"
#include "LaunchGuiAppStepComponent.h"

namespace TA_Base_App
{


    ActiveStep* LaunchGuiAppStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveLaunchGuiAppStep(detail, flowControl);
    }

    void LaunchGuiAppStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    IActiveStepMessage* LaunchGuiAppStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLaunchGuiAppStepMessage(ulStepNum, vecData);
    }

    LaunchGuiAppStepComponentImpl::LaunchGuiAppStepComponentImpl()
    {

    }

    LaunchGuiAppStepComponentImpl::~LaunchGuiAppStepComponentImpl()
    {

    }

}
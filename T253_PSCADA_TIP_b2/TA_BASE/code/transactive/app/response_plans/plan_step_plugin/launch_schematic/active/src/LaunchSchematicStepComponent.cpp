#include "LaunchSchematicStepComponent.h"
#include "ActiveLaunchSchematicStep.h"
#include "ActivePlanLaunchSchematicStepMessage.h"

namespace TA_Base_App
{


    ActiveStep* LaunchSchematicStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveLaunchSchematicStep(detail, flowControl);
    }

    void LaunchSchematicStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    IActiveStepMessage* LaunchSchematicStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLaunchSchematicStepMessage(ulStepNum, vecData);
    }

    LaunchSchematicStepComponentImpl::LaunchSchematicStepComponentImpl()
    {

    }

    LaunchSchematicStepComponentImpl::~LaunchSchematicStepComponentImpl()
    {

    }

}
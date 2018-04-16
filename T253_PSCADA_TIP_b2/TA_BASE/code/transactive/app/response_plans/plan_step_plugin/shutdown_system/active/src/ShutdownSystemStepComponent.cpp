#include "ShutdownSystemStepComponent.h"
#include "ActiveShutdownSystemStep.h"

namespace TA_Base_App
{


    ActiveStep* ShutdownSystemStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveShutdownSystemStep(detail, flowControl);
    }

    void ShutdownSystemStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    /*IActiveStepMessage* ShutdownSystemStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    ShutdownSystemStepComponentImpl::ShutdownSystemStepComponentImpl()
    {

    }

    ShutdownSystemStepComponentImpl::~ShutdownSystemStepComponentImpl()
    {

    }

}
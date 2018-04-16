#include "JumpStepComponent.h"
#include "ActiveJumpStep.h"

namespace TA_Base_App
{


    ActiveStep* JumpStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveJumpStep(detail, flowControl);
    }

    void JumpStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    /*IActiveStepMessage* JumpStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    JumpStepComponentImpl::JumpStepComponentImpl()
    {

    }

    JumpStepComponentImpl::~JumpStepComponentImpl()
    {

    }

}
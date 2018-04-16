#include "VmsBlankoutStepComponent.h"
#include "ActiveVmsBlankoutStep.h"

namespace TA_Base_App
{


    ActiveStep* VmsBlankoutStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveVmsBlankoutStep(detail, flowControl);
    }

    void VmsBlankoutStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    /*IActiveStepMessage* VmsBlankoutStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    VmsBlankoutStepComponentImpl::VmsBlankoutStepComponentImpl()
    {

    }

    VmsBlankoutStepComponentImpl::~VmsBlankoutStepComponentImpl()
    {

    }

}
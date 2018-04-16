#include "NullStepComponent.h"
#include "ActiveNullStep.h"

namespace TA_Base_App
{


    ActiveStep* NullStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveNullStep(detail, flowControl);
    }

    void NullStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    /*IActiveStepMessage* NullStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    NullStepComponentImpl::NullStepComponentImpl()
    {

    }

    NullStepComponentImpl::~NullStepComponentImpl()
    {

    }

}
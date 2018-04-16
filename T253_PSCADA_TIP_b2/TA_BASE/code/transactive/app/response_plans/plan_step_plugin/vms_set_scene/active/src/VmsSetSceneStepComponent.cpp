#include "VmsSetSceneStepComponent.h"
#include "ActiveVmsSetSceneStep.h"

namespace TA_Base_App
{


    ActiveStep* VmsSetSceneStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveVmsSetSceneStep(detail, flowControl);
    }

    void VmsSetSceneStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    /*IActiveStepMessage* VmsSetSceneStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    VmsSetSceneStepComponentImpl::VmsSetSceneStepComponentImpl()
    {

    }

    VmsSetSceneStepComponentImpl::~VmsSetSceneStepComponentImpl()
    {

    }

}
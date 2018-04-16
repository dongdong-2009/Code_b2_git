#include "PromptStepComponent.h"
#include "ActivePromptStep.h"
#include "ActivePlanPromptStepMessage.h"

namespace TA_Base_App
{


    ActiveStep* PromptStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActivePromptStep(detail, flowControl);
    }

    void PromptStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    IActiveStepMessage* PromptStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanPromptStepMessage(ulStepNum, vecData);
    }

    PromptStepComponentImpl::PromptStepComponentImpl()
    {

    }

    PromptStepComponentImpl::~PromptStepComponentImpl()
    {

    }

}
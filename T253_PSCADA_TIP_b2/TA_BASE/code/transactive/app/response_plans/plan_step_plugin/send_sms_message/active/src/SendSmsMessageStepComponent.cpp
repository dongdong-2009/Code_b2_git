#include "SendSmsMessageStepComponent.h"
#include "ActiveSendSmsMessageStep.h"

namespace TA_Base_App
{


    ActiveStep* SendSmsMessageStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveSendSmsMessageStep(detail, flowControl);
    }

    void SendSmsMessageStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    /*IActiveStepMessage* JumpStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    SendSmsMessageStepComponentImpl::SendSmsMessageStepComponentImpl()
    {

    }

    SendSmsMessageStepComponentImpl::~SendSmsMessageStepComponentImpl()
    {

    }

}
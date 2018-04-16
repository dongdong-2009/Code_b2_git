#include "PopupMftMessageStepComponent.h"
#include "ActivePopupMftMessageStep.h"
#include "ActivePlanPopupMftMessageStepMessage.h"

namespace TA_Base_App
{


    ActiveStep* PopupMftMessageStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActivePopupMftMessageStep(detail, flowControl);
    }

    void PopupMftMessageStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    IActiveStepMessage* PopupMftMessageStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanPopupMftMessageStepMessage(ulStepNum, vecData);
    }

    PopupMftMessageStepComponentImpl::PopupMftMessageStepComponentImpl()
    {

    }

    PopupMftMessageStepComponentImpl::~PopupMftMessageStepComponentImpl()
    {

    }

}
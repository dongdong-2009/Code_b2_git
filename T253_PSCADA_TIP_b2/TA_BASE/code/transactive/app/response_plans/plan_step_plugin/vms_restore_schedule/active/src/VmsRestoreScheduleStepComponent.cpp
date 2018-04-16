#include "VmsRestoreScheduleStepComponent.h"
#include "ActiveVmsRestoreScheduleStep.h"

namespace TA_Base_App
{


    ActiveStep* VmsRestoreScheduleStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveVmsRestoreScheduleStep(detail, flowControl);
    }

    void VmsRestoreScheduleStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    /*IActiveStepMessage* VmsRestoreScheduleStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    VmsRestoreScheduleStepComponentImpl::VmsRestoreScheduleStepComponentImpl()
    {

    }

    VmsRestoreScheduleStepComponentImpl::~VmsRestoreScheduleStepComponentImpl()
    {

    }

}
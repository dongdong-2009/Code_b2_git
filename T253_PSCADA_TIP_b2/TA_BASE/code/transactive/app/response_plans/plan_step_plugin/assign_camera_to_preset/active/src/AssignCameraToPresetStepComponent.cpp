#include "app/response_plans/plan_step_plugin/assign_camera_to_preset/active/src/AssignCameraToPresetStepComponent.h"
#include "app/response_plans/plan_step_plugin/assign_camera_to_preset/active/src/ActiveAssignCameraToPresetStep.h"

namespace TA_Base_App
{


    ActiveStep* AssignCameraToPresetStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveAssignCameraToPresetStep(detail, flowControl);
    }

    void AssignCameraToPresetStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    /*IActiveStepMessage* AssignCameraToPresetStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    AssignCameraToPresetStepComponentImpl::AssignCameraToPresetStepComponentImpl()
    {

    }

    AssignCameraToPresetStepComponentImpl::~AssignCameraToPresetStepComponentImpl()
    {

    }

}
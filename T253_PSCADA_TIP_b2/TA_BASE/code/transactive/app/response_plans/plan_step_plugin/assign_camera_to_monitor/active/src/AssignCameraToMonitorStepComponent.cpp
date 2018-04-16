
#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/active/src/AssignCameraToMonitorStepComponent.h"
#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/active/src/ActiveAssignCameraToMonitorStep.h"
//#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/active/src/ActivePlanLogStepMessage.h"

namespace TA_Base_App
{


    ActiveStep* AssignCameraToMonitorComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveAssignCameraToMonitorStep(detail, flowControl);
    }

    void AssignCameraToMonitorComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {
    }

    /*IActiveStepMessage* AssignCameraToMonitorComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    AssignCameraToMonitorComponentImpl::AssignCameraToMonitorComponentImpl()
    {
    }

    AssignCameraToMonitorComponentImpl::~AssignCameraToMonitorComponentImpl()
    {
    }

}

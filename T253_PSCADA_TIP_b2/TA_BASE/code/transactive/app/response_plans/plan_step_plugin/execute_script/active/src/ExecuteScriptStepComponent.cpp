#include "ExecuteScriptStepComponent.h"
#include "ActiveExecuteScriptStep.h"
//#include "ActivePlanLogStepMessage.h"

namespace TA_Base_App
{


    ActiveStep* ExecuteScriptStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveExecuteScriptStep(detail, flowControl);
    }

    void ExecuteScriptStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    /*IActiveStepMessage* ExecuteScriptStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    ExecuteScriptStepComponentImpl::ExecuteScriptStepComponentImpl()
    {

    }

    ExecuteScriptStepComponentImpl::~ExecuteScriptStepComponentImpl()
    {

    }

}
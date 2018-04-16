#include "app/response_plans/plan_step_plugin/pa/train_pa/active/src/TrainPaStepComponent.h"
#include "app/response_plans/plan_step_plugin/pa/train_pa/active/src/ActiveTrainPaStep.h"

namespace TA_Base_App
{


    ActiveStep* TrainPaStepComponentImpl::createActiveStep(const BasicParameter& refDetail, TA_Base_App::IActivePlan& iPlan)
    {
        return new TA_Base_App::ActiveTrainPaStep(refDetail, iPlan);
    }

    void TrainPaStepComponentImpl::validateStepParameter(const std::vector<std::string>& parameters)
    {

    }

    /*IActiveStepMessage* TrainPaStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    TrainPaStepComponentImpl::TrainPaStepComponentImpl()
    {

    }

    TrainPaStepComponentImpl::~TrainPaStepComponentImpl()
    {

    }

}
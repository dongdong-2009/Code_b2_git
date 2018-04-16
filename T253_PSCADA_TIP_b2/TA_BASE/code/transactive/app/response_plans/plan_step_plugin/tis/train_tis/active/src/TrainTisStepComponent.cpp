#include "TrainTisStepComponent.h"
#include "ActiveTrainTisStep.h"

namespace TA_Base_App
{


    ActiveStep* TrainTisStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveTrainTisStep(detail, flowControl);
    }

    void TrainTisStepComponentImpl::validateStepParameter(const std::vector<std::string>& parameters)
    {

    }

    /*IActiveStepMessage* TrainTisStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    TrainTisStepComponentImpl::TrainTisStepComponentImpl()
    {

    }

    TrainTisStepComponentImpl::~TrainTisStepComponentImpl()
    {

    }

}
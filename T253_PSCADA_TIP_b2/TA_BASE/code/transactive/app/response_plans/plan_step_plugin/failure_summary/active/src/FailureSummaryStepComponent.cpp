#include "FailureSummaryStepComponent.h"
#include "ActiveFailureSummaryStep.h"
#include "ActivePlanFailureSummaryStepMessage.h"

namespace TA_Base_App
{


    ActiveStep* FailureSummaryStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveFailureSummaryStep(detail, flowControl);
    }

    void FailureSummaryStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    IActiveStepMessage* FailureSummaryStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
		return new ActivePlanFailureSummaryStepMessage(ulStepNum, vecData);
    }

    FailureSummaryStepComponentImpl::FailureSummaryStepComponentImpl()
    {

    }

    FailureSummaryStepComponentImpl::~FailureSummaryStepComponentImpl()
    {

    }

}
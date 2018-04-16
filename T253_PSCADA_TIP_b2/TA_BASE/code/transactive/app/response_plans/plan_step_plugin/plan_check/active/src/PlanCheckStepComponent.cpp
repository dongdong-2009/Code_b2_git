#include "PlanCheckStepComponent.h"
#include "ActivePlanCheckStep.h"
//#include "app/response_plans/plan_steps/plan_check_step/src/ActivePlanLogStepMessage.h"

namespace TA_Base_App
{


    ActiveStep* PlanCheckStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActivePlanCheckStep(detail, flowControl);
    }

    void PlanCheckStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {
		/*
                    const TA_Base_Core::ActivePlanCheckParameters& refParams = stepParameters->activePlanCheck();

                    if (!(refParams.plan) || !TA_Base_Bus::PlanDataReader::instance()->existsNode(refParams.plan))
                    {
                        message << "Step " << m+1 << " - Parameter 'Plan Path' : " << " No plan has been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PLAN_PATH, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if (TA_Base_Core::PLAN_NODE != TA_Base_Bus::PlanDataReader::instance()->getNodeType(refParams.plan))
                    {
                        message << "Step " << m+1 << " - Parameter 'Plan Path' : " << " No plan has been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_PLAN_PATH, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if (getStepNumber(plan, refParams.yesStep) == 0)
                    {
                        message << "Step " << m+1 << " - Parameter 'Yes Step' : " << " The step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_YES, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }

                    if (getStepNumber(plan, refParams.noStep) == 0)
                    {
                        message << "Step " << m+1 << " - Parameter 'No Step' : " << " The step has not been specified";
                        TA_THROW(TA_Base_Core::PlanStepDetailException(message.str(), TA_Base_Core::PSDT_NO, TA_Base_Core::PSDE_UNSPECIFIED, m+1 ));
                    }
					*/
    }

    /*IActiveStepMessage* PlanCheckStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    PlanCheckStepComponentImpl::PlanCheckStepComponentImpl()
    {

    }

    PlanCheckStepComponentImpl::~PlanCheckStepComponentImpl()
    {

    }

}
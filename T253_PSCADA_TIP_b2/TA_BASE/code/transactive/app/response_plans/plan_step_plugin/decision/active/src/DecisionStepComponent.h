#ifndef __DECISION_STEP_COMPONENT_H_
#define __DECISION_STEP_COMPONENT_H_

#include <ace/RW_Mutex.h>
#include <ace/Singleton.h>
#include "app/response_plans/plan_step_plugin/common_active/src/StepComponent.h"



namespace TA_Base_App
{
    class IActivePlan;

    class DecisionStepComponentImpl: public StepComponent
    {

    public:
        friend class ACE_Singleton<DecisionStepComponentImpl, ACE_Recursive_Thread_Mutex>;

        virtual ActiveStep* createActiveStep(const BasicParameter& refDetail, TA_Base_App::IActivePlan& iPlan);

        virtual void validateStepParameter(const std::vector<std::string>& parameters);

        virtual IActiveStepMessage* createPlanStepMessage(const unsigned long ulStepNum, const std::vector<std::string>& vecData);

    private:
        DecisionStepComponentImpl();
        virtual ~DecisionStepComponentImpl();
        DecisionStepComponentImpl(const DecisionStepComponentImpl&);
        DecisionStepComponentImpl& operator=(const DecisionStepComponentImpl&);
    };

    typedef ACE_Singleton<DecisionStepComponentImpl, ACE_Recursive_Thread_Mutex> DecisionStepComponent;
}

#endif
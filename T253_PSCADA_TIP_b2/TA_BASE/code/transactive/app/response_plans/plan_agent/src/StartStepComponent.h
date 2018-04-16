#ifndef __START_STEP_COMPONENT_H_
#define __START_STEP_COMPONENT_H_

#include <ace/RW_Mutex.h>
#include <ace/Singleton.h>
#include "app/response_plans/plan_step_plugin/common_active/src/StepComponent.h"

namespace TA_Base_App
{
    class IActivePlan;

    class StartStepComponentImpl: public StepComponent
    {

    public:
        friend class ACE_Singleton<StartStepComponentImpl, ACE_Recursive_Thread_Mutex>;

        virtual ActiveStep* createActiveStep( const BasicParameter& detail, IActivePlan& plan );

        virtual void validateStepParameter(const std::vector<std::string>& parameters);

        //virtual IActiveStepMessage* createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& parameters );

    private:
        StartStepComponentImpl();
        virtual ~StartStepComponentImpl();
        StartStepComponentImpl(const StartStepComponentImpl&);
        StartStepComponentImpl& operator=(const StartStepComponentImpl&);
    };

    typedef ACE_Singleton<StartStepComponentImpl, ACE_Recursive_Thread_Mutex> StartStepComponent;
}

#endif
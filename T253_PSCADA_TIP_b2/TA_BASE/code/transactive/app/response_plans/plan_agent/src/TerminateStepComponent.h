#ifndef __TERMINATE_STEP_COMPONENT_H_
#define __TERMINATE_STEP_COMPONENT_H_

#include <ace/RW_Mutex.h>
#include <ace/Singleton.h>
#include "app/response_plans/plan_step_plugin/common_active/src/StepComponent.h"

namespace TA_Base_App
{
    class IActivePlan;

    class TerminateStepComponentImpl: public StepComponent
    {
    public:
        friend class ACE_Singleton<TerminateStepComponentImpl, ACE_Recursive_Thread_Mutex>;

        virtual ActiveStep* createActiveStep( const BasicParameter& detail, IActivePlan& plan );

        virtual void validateStepParameter(const std::vector<std::string>& parameters);

        //virtual IActiveStepMessage* createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& parameters );

    private:
        TerminateStepComponentImpl();
        virtual ~TerminateStepComponentImpl();
        TerminateStepComponentImpl(const TerminateStepComponentImpl&);
        TerminateStepComponentImpl& operator=(const TerminateStepComponentImpl&);
    };

    typedef ACE_Singleton<TerminateStepComponentImpl, ACE_Recursive_Thread_Mutex> TerminateStepComponent;
}

#endif
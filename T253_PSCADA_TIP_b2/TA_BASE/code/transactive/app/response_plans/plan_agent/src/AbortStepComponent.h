#ifndef __ABORT_STEP_COMPONENT_H_
#define __ABORT_STEP_COMPONENT_H_

#include <ace/RW_Mutex.h>
#include <ace/Singleton.h>
#include "app/response_plans/plan_step_plugin/common_active/src/StepComponent.h"

namespace TA_Base_App
{
    class IActivePlan;

    class AbortStepComponentImpl: public StepComponent
    {

    public:
        friend class ACE_Singleton<AbortStepComponentImpl, ACE_Recursive_Thread_Mutex>;

        virtual ActiveStep* createActiveStep( const BasicParameter& detail, IActivePlan& plan );

        virtual void validateStepParameter(const std::vector<std::string>& parameters);

        //virtual IActiveStepMessage* createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& parameters );
    private:
        AbortStepComponentImpl();
        virtual ~AbortStepComponentImpl();
        AbortStepComponentImpl(const AbortStepComponentImpl&);
        AbortStepComponentImpl& operator=(const AbortStepComponentImpl&);
    };

    typedef ACE_Singleton<AbortStepComponentImpl, ACE_Recursive_Thread_Mutex> AbortStepComponent;
}

#endif
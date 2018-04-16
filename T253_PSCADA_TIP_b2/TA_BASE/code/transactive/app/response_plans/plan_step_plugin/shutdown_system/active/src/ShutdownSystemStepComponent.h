#ifndef __SHUTDOWN_SYSTEM_STEP_COMPONENT_H_
#define __SHUTDOWN_SYSTEM_STEP_COMPONENT_H_

#include "ace/RW_Mutex.h"
#include "ace/Singleton.h"
#include "app/response_plans/plan_step_plugin/common_active/src/StepComponent.h"



namespace TA_Base_App
{
    class IActivePlan;

    class ShutdownSystemStepComponentImpl: public StepComponent
    {
        
    public:
        friend class ACE_Singleton<ShutdownSystemStepComponentImpl, ACE_Recursive_Thread_Mutex>;

        virtual ActiveStep* createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl);

        virtual void validateStepParameter( const std::vector<std::string>& parameters );


    private:
        ShutdownSystemStepComponentImpl();
        ~ShutdownSystemStepComponentImpl();
        ShutdownSystemStepComponentImpl( const ShutdownSystemStepComponentImpl& );
        ShutdownSystemStepComponentImpl& operator=( const ShutdownSystemStepComponentImpl& );
    };

    typedef ACE_Singleton<ShutdownSystemStepComponentImpl, ACE_Recursive_Thread_Mutex> ShutdownSystemStepComponent;
}

#endif
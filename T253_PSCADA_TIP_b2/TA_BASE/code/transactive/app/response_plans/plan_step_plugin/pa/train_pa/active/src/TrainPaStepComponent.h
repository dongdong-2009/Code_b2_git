#ifndef __TRAIN_PA_STEP_COMPONENT_H_
#define __TRAIN_PA_STEP_COMPONENT_H_

#include "ace/RW_Mutex.h"
#include "ace/Singleton.h"
#include "app/response_plans/plan_step_plugin/common_active/src/StepComponent.h"



namespace TA_Base_App
{
    class IActivePlan;

    class TrainPaStepComponentImpl: public StepComponent
    {
        
    public:
        friend class ACE_Singleton<TrainPaStepComponentImpl, ACE_Recursive_Thread_Mutex>;

        virtual ActiveStep* createActiveStep(const BasicParameter& refDetail, TA_Base_App::IActivePlan& iPlan);
        virtual void validateStepParameter(const std::vector<std::string>& parameters);

    private:
        TrainPaStepComponentImpl();
        ~TrainPaStepComponentImpl();
        TrainPaStepComponentImpl( const TrainPaStepComponentImpl& );
        TrainPaStepComponentImpl& operator=( const TrainPaStepComponentImpl& );
    };

    typedef ACE_Singleton<TrainPaStepComponentImpl, ACE_Recursive_Thread_Mutex> TrainPaStepComponent;
}

#endif
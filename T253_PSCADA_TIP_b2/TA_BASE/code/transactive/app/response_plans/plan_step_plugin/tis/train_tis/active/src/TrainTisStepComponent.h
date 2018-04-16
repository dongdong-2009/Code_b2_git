#ifndef __TRAIN_TIS_STEP_COMPONENT_H_
#define __TRAIN_TIS_STEP_COMPONENT_H_

#include "ace/RW_Mutex.h"
#include "ace/Singleton.h"
#include "app/response_plans/plan_step_plugin/common_active/src/StepComponent.h"



namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class IActivePlan;

    class TrainTisStepComponentImpl: public StepComponent
    {
        
    public:
        friend class ACE_Singleton<TrainTisStepComponentImpl, ACE_Recursive_Thread_Mutex>;

        virtual ActiveStep* createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl);
        virtual void validateStepParameter(const std::vector<std::string>& parameters);

    private:
        TrainTisStepComponentImpl();
        ~TrainTisStepComponentImpl();
        TrainTisStepComponentImpl( const TrainTisStepComponentImpl& );
        TrainTisStepComponentImpl& operator=( const TrainTisStepComponentImpl& );
    };

    typedef ACE_Singleton<TrainTisStepComponentImpl, ACE_Recursive_Thread_Mutex> TrainTisStepComponent;
}

#endif
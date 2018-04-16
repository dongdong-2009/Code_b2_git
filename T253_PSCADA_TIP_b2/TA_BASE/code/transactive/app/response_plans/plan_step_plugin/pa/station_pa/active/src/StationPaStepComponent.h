#ifndef __STATION_PA_STEP_COMPONENT_H_
#define __STATION_PA_STEP_COMPONENT_H_

#include <ace/RW_Mutex.h>
#include <ace/Singleton.h>
#include "app/response_plans/plan_step_plugin/common_active/src/StepComponent.h"



namespace TA_Base_App
{
    class IActivePlan;

    class StationPaStepComponentImpl: public StepComponent
    {
        
    public:
        friend class ACE_Singleton<StationPaStepComponentImpl, ACE_Recursive_Thread_Mutex>;

        virtual ActiveStep* createActiveStep(const BasicParameter& refDetail, TA_Base_App::IActivePlan& iPlan);
        virtual void validateStepParameter(const std::vector<std::string>& parameters);

    private:
        StationPaStepComponentImpl();
        ~StationPaStepComponentImpl();
        StationPaStepComponentImpl( const StationPaStepComponentImpl& );
        StationPaStepComponentImpl& operator=( const StationPaStepComponentImpl& );
    };

    typedef ACE_Singleton<StationPaStepComponentImpl, ACE_Recursive_Thread_Mutex> StationPaStepComponent;
}

#endif
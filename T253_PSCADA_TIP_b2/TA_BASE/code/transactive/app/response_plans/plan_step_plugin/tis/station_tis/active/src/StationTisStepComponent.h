#ifndef __STATION_TIS_STEP_COMPONENT_H_
#define __STATION_TIS_STEP_COMPONENT_H_

#include <ace/RW_Mutex.h>
#include <ace/Singleton.h>
#include "app/response_plans/plan_step_plugin/common_active/src/StepComponent.h"




namespace TA_Base_App
{
    class IActivePlan;

    class StationTisStepComponentImpl: public StepComponent
    {
        
    public:
        friend class ACE_Singleton<StationTisStepComponentImpl, ACE_Recursive_Thread_Mutex>;

        virtual ActiveStep* createActiveStep(const BasicParameter& refDetail, TA_Base_App::IActivePlan& iPlan);
        virtual void validateStepParameter(const std::vector<std::string>& parameters);

    private:
        StationTisStepComponentImpl();
        virtual ~StationTisStepComponentImpl();
        StationTisStepComponentImpl( const StationTisStepComponentImpl& );
        StationTisStepComponentImpl& operator=( const StationTisStepComponentImpl& );
    };

    typedef ACE_Singleton<StationTisStepComponentImpl, ACE_Recursive_Thread_Mutex> StationTisStepComponent;
}

#endif
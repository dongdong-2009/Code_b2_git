#ifndef __VMS_SET_SCENE_STEP_COMPONENT_H_
#define __VMS_SET_SCENE_STEP_COMPONENT_H_

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

    class VmsSetSceneStepComponentImpl: public StepComponent
    {
        
    public:
        friend class ACE_Singleton<VmsSetSceneStepComponentImpl, ACE_Recursive_Thread_Mutex>;

        virtual ActiveStep* createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl);

        virtual void validateStepParameter( const std::vector<std::string>& parameters );

        /*virtual IActiveStepMessage* createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData );*/

    private:
        VmsSetSceneStepComponentImpl();
        ~VmsSetSceneStepComponentImpl();
        VmsSetSceneStepComponentImpl( const VmsSetSceneStepComponentImpl& );
        VmsSetSceneStepComponentImpl& operator=( const VmsSetSceneStepComponentImpl& );
    };

    typedef ACE_Singleton<VmsSetSceneStepComponentImpl, ACE_Recursive_Thread_Mutex> VmsSetSceneStepComponent;
}

#endif
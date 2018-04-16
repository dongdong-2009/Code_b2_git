#ifndef ACTIVE_VMS_SET_SCENE_STEP_INCLUDED_H
#define ACTIVE_VMS_SET_SCENE_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"
#include "app/response_plans/plan_step_plugin/vms_set_scene/common/VmsSetSceneStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveVmsSetSceneStep: public ActiveStep
    {
    public:
        ActiveVmsSetSceneStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveVmsSetSceneStep();

    protected:
        virtual bool executeImpl();
    private:
        ActiveVmsSetSceneStep(const ActiveVmsSetSceneStep&);
        ActiveVmsSetSceneStep& operator=(const ActiveVmsSetSceneStep&);
    private:
        TA_Base_App::VmsSetSceneStepParameter m_params;

        //TA_Base_Core::CommsMessageSender* m_commsSender;

    };

}


#endif //ACTIVE_LOG_STEP_INCLUDED_H

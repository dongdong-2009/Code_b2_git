#ifndef ACTIVE_VMS_BLANK_OUT_STEP_INCLUDED_H
#define ACTIVE_VMS_BLANK_OUT_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"
#include "app/response_plans/plan_step_plugin/vms_blankout/common/VmsBlankoutStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveVmsBlankoutStep: public ActiveStep
    {
    public:
        ActiveVmsBlankoutStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveVmsBlankoutStep();

    protected:
        virtual bool executeImpl();
    private:
        ActiveVmsBlankoutStep(const ActiveVmsBlankoutStep&);
        ActiveVmsBlankoutStep& operator=(const ActiveVmsBlankoutStep&);
    private:
        TA_Base_App::VmsBlankoutStepParameter m_params;

        //TA_Base_Core::CommsMessageSender* m_commsSender;

    };

}


#endif //ACTIVE_LOG_STEP_INCLUDED_H

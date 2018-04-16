#ifndef ACTIVE_VMS_RESTORE_SCHEDULE_STEP_INCLUDED_H
#define ACTIVE_VMS_RESTORE_SCHEDULE_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"
#include "app/response_plans/plan_step_plugin/vms_restore_schedule/common/VmsRestoreScheduleStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveVmsRestoreScheduleStep: public ActiveStep
    {
    public:
        ActiveVmsRestoreScheduleStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveVmsRestoreScheduleStep();

    protected:
        virtual bool executeImpl();
    private:
        ActiveVmsRestoreScheduleStep(const ActiveVmsRestoreScheduleStep&);
        ActiveVmsRestoreScheduleStep& operator=(const ActiveVmsRestoreScheduleStep&);
    private:
        TA_Base_App::VmsRestoreScheduleStepParameter m_params;

        //TA_Base_Core::CommsMessageSender* m_commsSender;

    };

}


#endif //ACTIVE_LOG_STEP_INCLUDED_H

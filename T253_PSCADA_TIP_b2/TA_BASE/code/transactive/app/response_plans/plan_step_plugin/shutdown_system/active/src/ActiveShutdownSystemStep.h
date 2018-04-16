#ifndef ACTIVE_SHUTDOWN_SYSTEM_STEP_INCLUDED_H
#define ACTIVE_SHUTDOWN_SYSTEM_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"
#include "app/response_plans/plan_step_plugin/shutdown_system/common/ShutdownSystemStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveShutdownSystemStep: public ActiveStep
    {
    public:
        ActiveShutdownSystemStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveShutdownSystemStep();

    protected:
        virtual bool executeImpl();
    private:
        ActiveShutdownSystemStep(const ActiveShutdownSystemStep&);
        ActiveShutdownSystemStep& operator=(const ActiveShutdownSystemStep&);

        std::string getHostname();
    private:
        TA_Base_App::ShutdownSystemStepParameter m_params;

        //TA_Base_Core::CommsMessageSender* m_commsSender;

    };

}


#endif //ACTIVE_SHUTDOWN_SYSTEM_STEP_INCLUDED_H

#ifndef ACTIVE_SEND_SMS_MESSAGE_STEP_INCLUDED_H
#define ACTIVE_SEND_SMS_MESSAGE_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"
#include "app/response_plans/plan_step_plugin/send_sms_message/common/SendSmsMessageStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveSendSmsMessageStep: public ActiveStep
    {
    public:
        ActiveSendSmsMessageStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveSendSmsMessageStep();

    protected:
        virtual bool executeImpl();
    private:
        ActiveSendSmsMessageStep(const ActiveSendSmsMessageStep&);
        ActiveSendSmsMessageStep& operator=(const ActiveSendSmsMessageStep&);
    private:
        TA_Base_App::SendSmsMessageStepParameter m_params;

        //TA_Base_Core::CommsMessageSender* m_commsSender;

    };

}


#endif //ACTIVE_LOG_STEP_INCLUDED_H

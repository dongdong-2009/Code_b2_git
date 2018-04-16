#ifndef ACTIVE_POPUP_MFT_MESSAGE_STEP_INCLUDED_H
#define ACTIVE_POPUP_MFT_MESSAGE_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"
#include "app/response_plans/plan_step_plugin/popup_mft_message/common/PopupMftMessageStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/naming/src/NamedObject.h"
#include "bus/scada/common_library/IDL/src/IDataPointAccessCorbaDef.h"

namespace TA_Base_Core
{
    class CommsMessageSender;
	class StepDetail;
}

namespace TA_Base_App
{
    class ActivePopupMftMessageStep: public ActiveStep
    {
    public:
        ActivePopupMftMessageStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActivePopupMftMessageStep();

		virtual std::list<ActiveStep*> getNextStep();

    protected:
        virtual bool executeImpl();
        virtual int waitImpl();
    private:
        ActivePopupMftMessageStep(const ActivePopupMftMessageStep&);
        ActivePopupMftMessageStep& operator=(const ActivePopupMftMessageStep&);
    private:
        TA_Base_App::PopupMftMessageStepParameter m_params;

        //TA_Base_Core::CommsMessageSender* m_commsSender;
        TA_Base_Core::NamedObject<TA_Base_Bus::IDataPointAccessCorbaDef,
            TA_Base_Bus::IDataPointAccessCorbaDef_ptr,
            TA_Base_Bus::IDataPointAccessCorbaDef_var> m_dataPoint;

        void sendActiveStepUpdateMessage();

        unsigned short m_sendPopupMessageTryTimes;
        TA_Base_Core::CommsMessageSender* m_commsSender;

		unsigned long m_nextStepId;

    };

}


#endif //ACTIVE_POPUP_MFT_MESSAGE_STEP_INCLUDED_H

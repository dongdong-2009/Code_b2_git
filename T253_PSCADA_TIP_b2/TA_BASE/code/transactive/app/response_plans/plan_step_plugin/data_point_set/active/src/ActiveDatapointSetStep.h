#ifndef ACTIVE_DATA_POINT_SET_STEP_INCLUDED_H
#define ACTIVE_DATA_POINT_SET_STEP_INCLUDED_H

#include "core/timers/src/TimedWaitSemaphore.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_step_plugin/data_point_set/common/DatapointSetStepParameter.h"

namespace TA_Base_Core
{
	struct StepDetail;
	struct NameValuePair;
	class MessageType;
}

namespace TA_Base_App
{
    class ActiveDataPointSetStep: public ActiveStep, public TA_Base_Bus::IEntityUpdateEventProcessor
    {
    public:
        ActiveDataPointSetStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveDataPointSetStep();

        virtual void processEntityUpdateEvent(unsigned long entityKey , TA_Base_Bus::ScadaEntityUpdateType updateType);
    protected:
        virtual bool executeImpl();

        bool sendControl(const std::string& strSession, const std::string &strValue, const int waitMillSec);
        bool initialize(std::string& strSession, std::string &strValue, int& waitMillSec);
    private:
        ActiveDataPointSetStep(const ActiveDataPointSetStep&);
        ActiveDataPointSetStep& operator=(const ActiveDataPointSetStep&);

        void submitAuditManualGrcBadLaunch(const std::string& strEquipDesc, const std::string& strLcc);
        void submitAuditManualGrcSucceed(const std::string& strPlanName, const unsigned long ulInstanceNum);
        void submitAuditManualGrcBadReturn(const std::string& strEquipDesc, const std::string& strRcc);
        void submitAuditManualGrcFailed(const std::string& strSession,
                                        const std::string& strPlanName,
                                        const std::string& strReason,
                                        const unsigned long ulInstanceNum,
                                        const unsigned long ulStepNum);
        void sendAuditMessage(const std::string& session,
                              const TA_Base_Core::MessageType& auditMessageType,
                              const std::vector<TA_Base_Core::NameValuePair*>& description);
    private:
        TA_Base_App::DataPointSetStepParameter m_params;
        TA_Base_Core::TimedWaitSemaphore m_semaphore;
        TA_Base_Core::TimedWaitSemaphore m_agentConnectedSem;
        TA_Base_Bus::DataPointProxySmartPtr m_pDataPoint;
        bool m_bRccSucceed;
        int m_nControlType;
    };
}

#endif //ACTIVE_DATA_POINT_SET_STEP_INCLUDED_H

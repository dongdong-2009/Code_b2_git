#include <algorithm>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "ActivePlanPromptStepMessage.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "app/response_plans/common/src/PlanMessageExecutionCfg.h"
/*
#include "IAAMessageSender.h"
#include "core/message/types/PlanAgentAudit_MessageTypes.h"
#include "core/utilities/src/DataConversion.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/utilities/src/DebugUtil.h"
*/

namespace TA_Base_App
{

    ActivePlanPromptStepMessage::ActivePlanPromptStepMessage(const unsigned long ulStepNum, const std::vector<std::string>& vecData)
		: m_ulStepNum(ulStepNum),
		  m_responseParam(vecData)
    {
    }

    ActivePlanPromptStepMessage::~ActivePlanPromptStepMessage()
    {
    }

    void ActivePlanPromptStepMessage::execute( IActivePlan& activePlan, IMessageSender& refMsgSender ) const
    {
        const int nStatus = m_responseParam.response;

		int nWaitStatus = (TA_Base_Core::PSCS_ACKNOWLEDGEMENT == nStatus ? ActiveStep::eWaitSucceed : ActiveStep::eWaitFailed);

        activePlan.stepResponsed( m_ulStepNum, nWaitStatus );
    }
}

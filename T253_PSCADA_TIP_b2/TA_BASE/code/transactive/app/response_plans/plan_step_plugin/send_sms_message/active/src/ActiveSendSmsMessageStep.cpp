#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "ActiveSendSmsMessageStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/common/src/StringConstants.h"
/*
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PlanAgentComms_MessageTypes.h"
*/

namespace TA_Base_App
{
    ActiveSendSmsMessageStep::ActiveSendSmsMessageStep(const BasicParameter& detail, IActivePlan& flowControl)
		: ActiveStep(m_params, flowControl),
		  m_params(detail)
    {
    }

    ActiveSendSmsMessageStep::~ActiveSendSmsMessageStep()
    {
    }

    bool ActiveSendSmsMessageStep::executeImpl()
    {
        // Step replaced by new sequence flow
        return true;
    }
}
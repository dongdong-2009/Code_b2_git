#include "ActiveVmsRestoreScheduleStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/common/src/StringConstants.h"
//#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PlanAgentComms_MessageTypes.h"

namespace TA_Base_App
{
    ActiveVmsRestoreScheduleStep::ActiveVmsRestoreScheduleStep(const BasicParameter& detail, IActivePlan& flowControl)
		: ActiveStep(m_params, flowControl),
          m_params(detail)
    {
    }

    ActiveVmsRestoreScheduleStep::~ActiveVmsRestoreScheduleStep()
    {
    }

    bool ActiveVmsRestoreScheduleStep::executeImpl()
    {
        //LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s: jumping to step %lu", dumpContext().c_str(), m_params.step );

        // Step replaced by new sequence flow
        return true;
    }
    
}
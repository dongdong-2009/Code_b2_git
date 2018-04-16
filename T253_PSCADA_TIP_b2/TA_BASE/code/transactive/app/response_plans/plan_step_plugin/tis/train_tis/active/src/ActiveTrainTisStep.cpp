#include "ActiveTrainTisStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/common/src/StringConstants.h"
//#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"

namespace TA_Base_App
{
    ActiveTrainTisStep::ActiveTrainTisStep(const BasicParameter& detail, IActivePlan& flowControl)
		: ActiveStep(m_params, flowControl),
          m_params(detail)
    {
    }

    ActiveTrainTisStep::~ActiveTrainTisStep()
    {
    }

    bool ActiveTrainTisStep::executeImpl()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Train TIS step %lu", id() );

        bool bExecSucceed = false;

        bExecSucceed = true;

        return bExecSucceed;
    }
}
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/response_plans/common/src/RpcAgency.h"
#include "ActiveLaunchSchematicStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/common/src/StringConstants.h"

namespace TA_Base_App
{
    ActiveLaunchSchematicStep::ActiveLaunchSchematicStep(const BasicParameter& detail, IActivePlan& flowControl)
		: ActiveStep(m_params, flowControl),
	      m_params(detail)
    {
    }

    ActiveLaunchSchematicStep::~ActiveLaunchSchematicStep()
    {
    }

    bool ActiveLaunchSchematicStep::executeImpl()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Launch Schematic step %lu", id() );

        bool bExecSucceed = false;

        m_remark = RemarkWaitingForAppToBeLaunched;

        std::vector<std::string> vecParameters = m_params.serialize();

        bExecSucceed = TA_Base_Bus::RpcAgency::getInstance().syncSendReq( m_iPlan.userLocation(), m_refParameter.basic.type,
            m_iPlan.id().location, m_iPlan.id().plan, m_iPlan.id().instance, id(), m_iPlan.userWorkstation(), vecParameters );

        return bExecSucceed;
    }
    
    int ActiveLaunchSchematicStep::waitImpl()
    {
        // Wait for operator response
        return m_iPlan.stepResponsed( id() );
    }

	void ActiveLaunchSchematicStep::initImpl()
	{
        m_refParameter.basic.timeout = 60u;
	}
}
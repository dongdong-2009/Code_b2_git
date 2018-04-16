#include "ace/OS.h"
#include "ActivePlanSetTimerStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/common/src/StringConstants.h"
//#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PlanAgentComms_MessageTypes.h"
#include <boost/lexical_cast.hpp>

namespace TA_Base_App
{
    ActivePlanSetTimerStep::ActivePlanSetTimerStep(const BasicParameter& detail, IActivePlan& flowControl)
		: ActiveStep(m_params, flowControl),
          m_params(detail)
    {
    }

    ActivePlanSetTimerStep::~ActivePlanSetTimerStep()
    {
    }

    bool ActivePlanSetTimerStep::executeImpl()
    {
        bool bExecSucceed = false;
        // Expected expired time
        const unsigned long ulNewTimer = ACE_OS::time() + m_params.time;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Plan set timer at [%d] with [%d] seconds", ulNewTimer, m_params.time );

		setTime(ulNewTimer);
		bExecSucceed = true;

		return bExecSucceed;
    }

	void ActivePlanSetTimerStep::setTime( unsigned long newTimer )
	{
		m_iPlan.planSharedData("time", boost::lexical_cast<std::string>(newTimer));
	}

}
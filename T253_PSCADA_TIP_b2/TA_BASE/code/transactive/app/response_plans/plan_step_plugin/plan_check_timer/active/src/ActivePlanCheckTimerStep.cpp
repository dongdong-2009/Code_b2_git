#include "ace/OS.h"
#include "ActivePlanCheckTimerStep.h"
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
    ActivePlanCheckTimerStep::ActivePlanCheckTimerStep(const BasicParameter& detail, IActivePlan& flowControl)
		: ActiveStep(m_params, flowControl),
          m_params(detail),
		  m_nextStepId(0)
    {
    }

    ActivePlanCheckTimerStep::~ActivePlanCheckTimerStep()
    {
    }

    bool ActivePlanCheckTimerStep::executeImpl()
    {
        bool bExecSucceed = false;

        // Time in timer and later then the time span of checking
        unsigned long ulActivePlanTimer = 0;

        try
        {
			ulActivePlanTimer = getTime();
            bExecSucceed = true;
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Exception", "Caught exception when getActivePlanDetail" );
        }

		if (bExecSucceed)
		{
			ACE_Time_Value atvEnd( ulActivePlanTimer + m_params.timeSpan, 0 );
			ACE_Time_Value atvNow = ACE_OS::gettimeofday();

			bool bResult = ( atvNow > atvEnd );

			if ( bResult )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"Plan check timer success at [%d], expected end at [%d]", atvNow.sec(), atvEnd.sec() );
				m_nextStepId = m_params.successStep;
			}
			else
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"Plan check timer failed at [%d], expected end at [%d]", atvNow.sec(), atvEnd.sec() );
				m_nextStepId = m_params.failureStep;
			}
		}

        return bExecSucceed;
    }

	std::list<ActiveStep*> ActivePlanCheckTimerStep::getNextStep()
	{
		std::list<ActiveStep*> outgoingStepList;
		std::list<ActiveStep*>::iterator itActiveStep = m_lstOutgoingStep.begin();
		for(; itActiveStep != m_lstOutgoingStep.end(); ++itActiveStep)
		{
			if(m_nextStepId == (*itActiveStep)->id())
			{
				outgoingStepList.push_back(*itActiveStep);
				break;
			}
		}
		return outgoingStepList;
	}

	unsigned long ActivePlanCheckTimerStep::getTime()
	{
		return boost::lexical_cast<unsigned long>(m_iPlan.planSharedData("time"));
	}

}
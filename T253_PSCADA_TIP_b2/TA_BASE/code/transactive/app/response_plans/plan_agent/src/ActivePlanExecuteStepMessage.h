#ifndef ACTIVEPLANEXECUTESTEPMESSAGE_H_INCLUDED
#define ACTIVEPLANEXECUTESTEPMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanExecuteStepMessage.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

	//Class declaration
	class ActivePlanExecuteStepMessage : public IActivePlanMessage
	{
	// Public methods
	public:
        ActivePlanExecuteStepMessage( const TA_Base_Core::ActiveStepDetail& detail );
        ActivePlanExecuteStepMessage( const TA_Base_Core::ExecutionHistoryUpdate& historyUpdate );

		virtual void execute( ActivePlan& activePlan ) const;

	private:
		ActivePlanExecuteStepMessage& operator=( const ActivePlanExecuteStepMessage& );
		ActivePlanExecuteStepMessage( const ActivePlanExecuteStepMessage& );

        const TA_Base_Core::ActiveStepDetail m_detail;
        const TA_Base_Core::ExecutionHistoryUpdate m_historyUpdate;
        bool m_bUpdateHistory;
	}; // ActivePlanExecuteStepMessage
}; // TA_Base_App

#endif // ACTIVEPLANEXECUTESTEPMESSAGE_H_INCLUDED

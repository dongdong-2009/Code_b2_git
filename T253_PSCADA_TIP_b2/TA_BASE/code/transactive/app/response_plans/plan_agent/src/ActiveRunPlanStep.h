#ifndef ACTIVERUNPLANSTEP_H_INCLUDED
#define ACTIVERUNPLANSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveRunPlanStep.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #13 $
  *
  *  Last modification: $DateTime: 2015/03/19 14:48:01 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/internal/src/RunPlanParameter.h"

namespace TA_Base_App
{
	//Class declaration
	class ActiveRunPlanStep : public ActiveStep
	{
	// Public methods
	public:
        ActiveRunPlanStep( const BasicParameter& detail, IActivePlan& plan );

	protected:
		virtual void initImpl();
        virtual bool executeImpl();
        virtual int waitImpl();
        std::string generateCompletionPendingRemark() const;

	private:
		ActiveRunPlanStep& operator=( const ActiveRunPlanStep& );
		ActiveRunPlanStep( const ActiveRunPlanStep& );

		RunPlanParameter m_params;
		TA_Base_Core::ActivePlanId m_childPlan;
	}; // ActiveRunPlanStep
}; // TA_Base_App

#endif // ACTIVERUNPLANSTEP_H_INCLUDED
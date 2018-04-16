#ifndef ACTIVE_PLAN_FAILURE_SUMMARY_STEP_HEADER_INCLUDED
#define ACTIVE_PLAN_FAILURE_SUMMARY_STEP_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanFailureSummaryStepMessage.h $
  *  @author:  Lizette Lingo
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2014/10/24 16:32:43 $
  *  Last modified by:  $Author: huang.wenbo $
  *
  *  Generated when the result of a Failure summary dialog display
  *  is received back from the Plan Manager.
  */

#include <string>

#include "app/response_plans/plan_step_plugin/common_active/src/IActiveStepMessage.h"
#include "app/response_plans/plan_step_plugin/failure_summary/common/FailureSummaryResponseParameter.h"

namespace TA_Base_App
{
    class ActivePlan;

    class ActivePlanFailureSummaryStepMessage : public IActiveStepMessage
    {
    //public methods
    public:
        ActivePlanFailureSummaryStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData );
        virtual ~ActivePlanFailureSummaryStepMessage();
        
        virtual void execute( IActivePlan& activePlan, IMessageSender& refMsgSender ) const;
    private:
        ActivePlanFailureSummaryStepMessage& operator=( const ActivePlanFailureSummaryStepMessage& );
        ActivePlanFailureSummaryStepMessage( const ActivePlanFailureSummaryStepMessage& );
		unsigned long m_ulStepNum;
		FailureSummaryResponseParameter m_responseParam;
    };

}

#endif //ACTIVE_PLAN_FAILURE_SUMMARY_STEP_HEADER_INCLUDED
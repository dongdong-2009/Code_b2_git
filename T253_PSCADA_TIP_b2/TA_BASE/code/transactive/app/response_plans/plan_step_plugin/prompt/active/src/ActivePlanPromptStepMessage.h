#ifndef ACTIVE_PLAN_PROMPT_STEP_HEADER_INCLUDED
#define ACTIVE_PLAN_PROMPT_STEP_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanPromptStepMessage.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2014/10/24 16:32:43 $
  *  Last modified by:  $Author: huang.wenbo $
  *
  *  <description>
  */

#include "app/response_plans/plan_step_plugin/common_active/src/IActiveStepMessage.h"
#include "app/response_plans/plan_step_plugin/prompt/common/PromptStepResponseParameter.h"
#include <vector>

namespace TA_Base_App
{
    class ActivePlanPromptStepMessage : public IActiveStepMessage
    {
    public:
        //ActivePlanDecisionStepMessage(const PlanMessageExecutionResultMap& results);
        ActivePlanPromptStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData );
        virtual ~ActivePlanPromptStepMessage();
        virtual void execute( IActivePlan& activePlan, IMessageSender& refMsgSender ) const; // TODO:To be discarded
    private:
        ActivePlanPromptStepMessage(const ActivePlanPromptStepMessage&);
        ActivePlanPromptStepMessage& operator=(const ActivePlanPromptStepMessage&);
		unsigned long m_ulStepNum;
		PromptStepResponseParameter m_responseParam;
    };

}

#endif //ACTIVE_PLAN_PROMPT_STEP_HEADER_INCLUDED
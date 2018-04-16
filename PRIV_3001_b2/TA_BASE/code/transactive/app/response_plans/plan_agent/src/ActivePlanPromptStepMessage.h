#if defined ( STEPTYPE_PROMPT )

#ifndef ACTIVE_PLAN_PROMPT_STEP_HEADER_INCLUDED
#define ACTIVE_PLAN_PROMPT_STEP_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanPromptStepMessage.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  <description>
  */

#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

    class ActivePlanPromptStepMessage : public IActivePlanMessage
    {
    //public methods
    public:
        ActivePlanPromptStepMessage(const int status);

        virtual void execute( ActivePlan& activePlan ) const;

    private:
        ActivePlanPromptStepMessage& operator=( const ActivePlanPromptStepMessage& );
        ActivePlanPromptStepMessage( const ActivePlanPromptStepMessage& );

        const int m_status;
    };

}

#endif //ACTIVE_PLAN_PROMPT_STEP_HEADER_INCLUDED

#endif
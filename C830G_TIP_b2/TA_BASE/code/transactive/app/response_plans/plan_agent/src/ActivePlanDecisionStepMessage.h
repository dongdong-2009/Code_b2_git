#if defined ( STEPTYPE_DECISION )

#ifndef ACTIVE_PLAN_DECISION_STEP_HEADER_INCLUDED
#define ACTIVE_PLAN_DECISION_STEP_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanDecisionStepMessage.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */
namespace TA_Base_App
{
    class ActivePlanDecisionStepMessage : public IActivePlanMessage
    {
    //public methods
    public:
            ActivePlanDecisionStepMessage(TA_Base_Core::EPlanStepCompletionStatus status);

        virtual void execute( ActivePlan& activePlan ) const;
    
    private:
        ActivePlanDecisionStepMessage& operator=( const ActivePlanDecisionStepMessage& );
        ActivePlanDecisionStepMessage( const ActivePlanDecisionStepMessage& );

            TA_Base_Core::EPlanStepCompletionStatus m_status;
    };

}

#endif

#endif // STEPTYPE_DECISION

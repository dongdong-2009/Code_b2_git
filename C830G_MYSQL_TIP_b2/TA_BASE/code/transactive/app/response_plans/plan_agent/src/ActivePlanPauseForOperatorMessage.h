#ifndef ACTIVE_PLAN_PAUSE_FOR_OPERATOR_MESSAGE_HEADER_INCLUDED
#define ACTIVE_PLAN_PAUSE_FOR_OPERATOR_MESSAGE_HEADER_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanPauseForOperatorMessage.h $
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
	//Class declaration
	class ActivePlanPauseForOperatorMessage : public IActivePlanMessage
	{
	// Public methods
	public:
		ActivePlanPauseForOperatorMessage(){}

		virtual void execute( ActivePlan& activePlan ) const;
	
	private:
		ActivePlanPauseForOperatorMessage& operator=( const ActivePlanPauseForOperatorMessage& );
		ActivePlanPauseForOperatorMessage( const ActivePlanPauseForOperatorMessage& );

	}; // ActivePlanPauseMessage
}; // TA_Base_App

#endif // ACTIVE_PLAN_PAUSE_FOR_OPERATOR_MESSAGE_HEADER_INCLUDED

#ifndef ACTIVEPLANPAUSEMESSAGE_H_INCLUDED
#define ACTIVEPLANPAUSEMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanPauseMessage.h $
  *  @author:  Gregg Kirkpatrick
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
	class ActivePlanPauseMessage : public IActivePlanMessage
	{
	// Public methods
	public:
		ActivePlanPauseMessage(){}

		virtual void execute( ActivePlan& activePlan ) const;
	
	private:
		ActivePlanPauseMessage& operator=( const ActivePlanPauseMessage& );
		ActivePlanPauseMessage( const ActivePlanPauseMessage& );

	}; // ActivePlanPauseMessage
}; // TA_Base_App

#endif // ACTIVEPLANPAUSEMESSAGE_H_INCLUDED

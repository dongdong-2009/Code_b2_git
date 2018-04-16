#ifndef ACTIVEPLANCOMPLETEMESSAGE_H_INCLUDED
#define ACTIVEPLANCOMPLETEMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanCompleteMessage.h $
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
	class ActivePlanCompleteMessage : public IActivePlanMessage
	{
	// Public methods
	public:
		ActivePlanCompleteMessage(){}

		virtual void execute( ActivePlan& activePlan ) const;
	
	private:
		ActivePlanCompleteMessage& operator=( const ActivePlanCompleteMessage& );
		ActivePlanCompleteMessage( const ActivePlanCompleteMessage& );

	}; // ActivePlanCompleteMessage
}; // TA_Base_App

#endif // ACTIVEPLANCOMPLETEMESSAGE_H_INCLUDED

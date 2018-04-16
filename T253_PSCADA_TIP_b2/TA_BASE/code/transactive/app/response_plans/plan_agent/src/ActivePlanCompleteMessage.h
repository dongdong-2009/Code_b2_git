#ifndef ACTIVEPLANCOMPLETEMESSAGE_H_INCLUDED
#define ACTIVEPLANCOMPLETEMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanCompleteMessage.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2014/11/04 13:46:22 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

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

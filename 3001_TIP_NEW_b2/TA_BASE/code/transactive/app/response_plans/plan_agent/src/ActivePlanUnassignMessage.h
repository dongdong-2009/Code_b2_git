#ifndef ACTIVEPLANUNASSIGNMESSAGE_H_INCLUDED
#define ACTIVEPLANUNASSIGNMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanUnassignMessage.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

	//Class declaration
	class ActivePlanUnassignMessage : public IActivePlanMessage
	{
	// Public methods
	public:
		ActivePlanUnassignMessage(){}

		virtual void execute( ActivePlan& activePlan ) const;
	
	private:
		ActivePlanUnassignMessage& operator=( const ActivePlanUnassignMessage& );
		ActivePlanUnassignMessage( const ActivePlanUnassignMessage& );

	}; // ActivePlanUnassignMessage
}; // TA_Base_App

#endif // ACTIVEPLANUNASSIGNMESSAGE_H_INCLUDED

#ifndef ACTIVEPLANRESUMEMESSAGE_H_INCLUDED
#define ACTIVEPLANRESUMEMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanResumeMessage.h $
  *  @author:  Gregg Kirkpatrick
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

	//Class declaration
	class ActivePlanResumeMessage : public IActivePlanMessage
	{
	// Public methods
	public:
		ActivePlanResumeMessage();

		virtual void execute( ActivePlan& activePlan ) const;

	private:
		ActivePlanResumeMessage& operator=( const ActivePlanResumeMessage& );
		ActivePlanResumeMessage( const ActivePlanResumeMessage& );
	}; // ActivePlanResumeMessage
}; // TA_Base_App

#endif // ACTIVEPLANRESUMEMESSAGE_H_INCLUDED

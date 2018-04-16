#ifndef ACTIVEPLANSTEPPAUSEDMESSAGE_H_INCLUDED
#define ACTIVEPLANSTEPPAUSEDMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanStepPausedMessage.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include <string>

#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

	//Class declaration
	class ActivePlanStepPausedMessage : public IActivePlanMessage
	{
	// Public methods
	public:
        ActivePlanStepPausedMessage(const unsigned long ulStepNum);

		virtual void execute( ActivePlan& activePlan ) const;

	private:
		ActivePlanStepPausedMessage& operator=( const ActivePlanStepPausedMessage& );
		ActivePlanStepPausedMessage( const ActivePlanStepPausedMessage& );

        unsigned long m_ulStepNumber;
	}; // ActivePlanStepPausedMessage
}; // TA_Base_App

#endif // ACTIVEPLANSTEPPAUSEDMESSAGE_H_INCLUDED

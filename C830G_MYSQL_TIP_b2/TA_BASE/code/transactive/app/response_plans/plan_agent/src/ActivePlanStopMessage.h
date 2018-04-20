#ifndef ACTIVEPLANSTOPMESSAGE_H_INCLUDED
#define ACTIVEPLANSTOPMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanStopMessage.h $
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
	class ActivePlanStopMessage : public IActivePlanMessage
	{
	// Public methods
	public:
        ActivePlanStopMessage(std::string& reason);

		virtual void execute( ActivePlan& activePlan ) const;
	
	private:
		ActivePlanStopMessage& operator=( const ActivePlanStopMessage& );
		ActivePlanStopMessage( const ActivePlanStopMessage& );

        std::string m_reason;

	}; // ActivePlanStopMessage
}; // TA_Base_App

#endif // ACTIVEPLANSTOPMESSAGE_H_INCLUDED

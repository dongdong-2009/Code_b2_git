#ifndef ACTIVEPLANCUSTOMISESTEPMESSAGE_H_INCLUDED
#define ACTIVEPLANCUSTOMISESTEPMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanCustomiseStepMessage.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2011/03/29 13:47:47 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */

namespace TA_Base_App
{
	//Class declaration
	class ActivePlanCustomiseStepMessage : public IActivePlanMessage
	{
	// Public methods
	public:
        ActivePlanCustomiseStepMessage( const TA_Base_Core::StepCustomisableDetail& detail );
        ActivePlanCustomiseStepMessage( const TA_Base_Core::CustomisationHistoryUpdate& historyUpdate );

		virtual void execute( ActivePlan& activePlan ) const;

	private:

		ActivePlanCustomiseStepMessage& operator=( const ActivePlanCustomiseStepMessage& );
		ActivePlanCustomiseStepMessage( const ActivePlanCustomiseStepMessage& );

        const TA_Base_Core::StepCustomisableDetail m_detail;
        TA_Base_Core::CustomisationHistoryUpdate m_historyUpdate;
        bool m_bUpdateHistory;
	}; // ActivePlanCustomiseStepMessage
}; // TA_Base_App

#endif // ACTIVEPLANCUSTOMISESTEPMESSAGE_H_INCLUDED

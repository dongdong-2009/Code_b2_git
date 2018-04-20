#ifndef ACTIVEPLANRESUMEMESSAGE_H_INCLUDED
#define ACTIVEPLANRESUMEMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanResumeMessage.h $
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
	class ActivePlanResumeMessage : public IActivePlanMessage
	{
	// Public methods
	public:
		ActivePlanResumeMessage(const TA_Base_Core::StepNumber resumeStepPosition);

		virtual void execute( ActivePlan& activePlan ) const;
	
	private:
		ActivePlanResumeMessage& operator=( const ActivePlanResumeMessage& );
		ActivePlanResumeMessage( const ActivePlanResumeMessage& );

		TA_Base_Core::StepNumber m_resumeStepPosition; 	//lizettejl (DP-changes)
	}; // ActivePlanResumeMessage
}; // TA_Base_App

#endif // ACTIVEPLANRESUMEMESSAGE_H_INCLUDED

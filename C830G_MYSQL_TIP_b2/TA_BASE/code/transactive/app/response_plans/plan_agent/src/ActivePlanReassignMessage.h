#ifndef ACTIVEPLANREASSIGNMESSAGE_H_INCLUDED
#define ACTIVEPLANREASSIGNMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanReassignMessage.h $
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
	class ActivePlanReassignMessage : public IActivePlanMessage
	{
	// Public methods
	public:
        ActivePlanReassignMessage(const std::string& session, const std::string& pmId);

		virtual void execute( ActivePlan& activePlan ) const;
	
	private:
		ActivePlanReassignMessage& operator=( const ActivePlanReassignMessage& );
		ActivePlanReassignMessage( const ActivePlanReassignMessage& );

		std::string m_sessionId;
        std::string m_pmId;

	}; // ActivePlanReassignMessage
}; // TA_Base_App

#endif // ACTIVEPLANREASSIGNMESSAGE_H_INCLUDED

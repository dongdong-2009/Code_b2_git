#ifndef ACTIVEPLANREASSIGNMESSAGE_H_INCLUDED
#define ACTIVEPLANREASSIGNMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanReassignMessage.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2014/11/04 13:46:22 $
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
	class ActivePlanReassignMessage : public IActivePlanMessage
	{
	// Public methods
	public:
        ActivePlanReassignMessage(const std::string& session);

		virtual void execute( ActivePlan& activePlan ) const;

	private:
		ActivePlanReassignMessage& operator=( const ActivePlanReassignMessage& );
		ActivePlanReassignMessage( const ActivePlanReassignMessage& );

		std::string m_sessionId;

	}; // ActivePlanReassignMessage
}; // TA_Base_App

#endif // ACTIVEPLANREASSIGNMESSAGE_H_INCLUDED

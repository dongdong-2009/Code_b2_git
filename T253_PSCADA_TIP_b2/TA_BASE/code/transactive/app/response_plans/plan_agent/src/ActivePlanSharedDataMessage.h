#ifndef ACTIVEPLANSHAREDDATAMESSAGE_H_INCLUDED
#define ACTIVEPLANSHAREDDATAMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanSharedDataMessage.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

	//Class declaration
	class ActivePlanSharedDataMessage : public IActivePlanMessage
	{
	// Public methods
	public:
        ActivePlanSharedDataMessage( const TA_Base_Core::KeyValueSeq& detail );

		virtual void execute( ActivePlan& activePlan ) const;

	private:
		ActivePlanSharedDataMessage& operator=( const ActivePlanSharedDataMessage& );
		ActivePlanSharedDataMessage( const ActivePlanSharedDataMessage& );

        const TA_Base_Core::KeyValueSeq m_detail;
	}; // ActivePlanSharedDataMessage
}; // TA_Base_App

#endif // ACTIVEPLANSHAREDDATAMESSAGE_H_INCLUDED

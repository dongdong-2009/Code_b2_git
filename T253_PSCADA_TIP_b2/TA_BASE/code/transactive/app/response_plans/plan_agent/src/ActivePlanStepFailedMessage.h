#ifndef ACTIVEPLANSTEPFAILEDMESSAGE_H_INCLUDED
#define ACTIVEPLANSTEPFAILEDMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanStepFailedMessage.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include <string>

#include "app/response_plans/plan_step_plugin/common_active/src/IActiveStepMessage.h"
#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

	//Class declaration
	class ActivePlanStepFailedMessage : public IActiveStepMessage, public IActivePlanMessage
	{
	// Public methods
	public:
        ActivePlanStepFailedMessage( IActiveStepMessage* pStepMsg );
        ActivePlanStepFailedMessage( const std::string& reason, const unsigned long ulStepNum, const bool bIgnoreFailure );
        ~ActivePlanStepFailedMessage();

        virtual void execute( IActivePlan& activePlan, IMessageSender& refMsgSender ) const;
        virtual void execute( ActivePlan& activePlan ) const;
	
	private:
		ActivePlanStepFailedMessage& operator=( const ActivePlanStepFailedMessage& );
		ActivePlanStepFailedMessage( const ActivePlanStepFailedMessage& );

        std::string m_reason;
        unsigned long m_ulStepNumber;
        bool m_bIgnoreFailure;

        IActiveStepMessage* m_pActiveStepMsg;
	}; // ActivePlanStepFailedMessage
}; // TA_Base_App

#endif // ACTIVEPLANSTEPFAILEDMESSAGE_H_INCLUDED

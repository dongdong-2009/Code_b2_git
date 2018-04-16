#if defined ( STEPTYPE_LOG )

#ifndef ACTIVE_PLAN_LOG_MESSAGE_HEADER_INCLUDED
#define ACTIVE_PLAN_LOG_MESSAGE_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanLogStepMessage.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  <description>
  */

#include <string>

#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;

	class ActivePlanLogStepMessage : public IActivePlanMessage
	{
		//public methods
	public:
		ActivePlanLogStepMessage( const int status, const std::string& logMessage );
		virtual void execute ( ActivePlan& activePlan ) const;

	private:
		ActivePlanLogStepMessage& operator=( const ActivePlanLogStepMessage& );
		ActivePlanLogStepMessage( const ActivePlanLogStepMessage& );

        // TD#3400
        std::string removeLineFeed ( const std::string& message ) const;

		const int m_status;
		std::string m_logMessage;
	};
}
#endif //ACTIVE_PLAN_LOG_MESSAGE_HEADER_INCLUDED

#endif
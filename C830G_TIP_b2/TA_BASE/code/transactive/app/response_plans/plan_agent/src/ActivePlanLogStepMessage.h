#ifndef ACTIVE_PLAN_LOG_MESSAGE_HEADER_INCLUDED
#define ACTIVE_PLAN_LOG_MESSAGE_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanLogStepMessage.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */
namespace TA_Base_App
{
	class ActivePlanLogStepMessage : public IActivePlanMessage
	{
		//public methods
	public:
		ActivePlanLogStepMessage( TA_Base_Core::EPlanStepCompletionStatus status, const std::string& logMessage );
		virtual void execute ( ActivePlan& activePlan ) const;

	private:
		ActivePlanLogStepMessage& operator=( const ActivePlanLogStepMessage& );
		ActivePlanLogStepMessage( const ActivePlanLogStepMessage& );

        // TD#3400
        std::string removeLineFeed ( const std::string& message ) const;


		TA_Base_Core::EPlanStepCompletionStatus m_status;
		std::string m_logMessage;
	};
}
#endif //ACTIVE_PLAN_LOG_MESSAGE_HEADER_INCLUDED

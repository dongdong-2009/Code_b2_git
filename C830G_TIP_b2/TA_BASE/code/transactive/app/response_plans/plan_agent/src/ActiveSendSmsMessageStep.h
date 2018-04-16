/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveSendSmsMessageStep.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class is responsible for sending an SMS message as part of
  * a plans execution.
  *
  */

#if defined ( STEPTYPE_SEND_SMS_MESSAGE )

#ifndef ACTIVE_SEND_SMS_MESSAGE_STEP_HEADER_INCLUDED
#define ACTIVE_SEND_SMS_MESSAGE_STEP_HEADER_INCLUDED

#include "app/messaging/sms_agent/IDL/src/ISmsAgentCorbaDef.h"
#include "core/data_access_interface/entity_access/src/SmsAgentEntityData.h"

namespace TA_Base_App
{
	//Class declaration
	class ActiveSendSmsMessageStep : public ActiveStep
	{
	// Public methods
	public:

		ActiveSendSmsMessageStep( const TA_Base_Core::StepDetail& stepDetail,
						const TA_Base_Core::SendSmsMessageParameters& params,
						IActivePlanFlow& flowControl );
		
	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:

		ActiveSendSmsMessageStep& operator=( const ActiveSendSmsMessageStep& );
		ActiveSendSmsMessageStep( const ActiveSendSmsMessageStep& );

		const TA_Base_Core::SendSmsMessageParameters m_params;
		IActivePlanFlow& m_flowControl;
		
	}; // ActiveSendSmsMessageStep
}; // TA_Base_App

#endif // ACTIVE_SEND_SMS_MESSAGE_STEP_HEADER_INCLUDED

#endif //  STEPTYPE_SEND_SMS_MESSAGE

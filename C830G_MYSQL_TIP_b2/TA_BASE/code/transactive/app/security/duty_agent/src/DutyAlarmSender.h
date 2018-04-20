/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/security/duty_agent/src/DutyAlarmSender.h $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2011/06/20 18:29:56 $
  * Last modified by:  $Author: shurui.liu $
  *
  * Listens to FrontEndStateChangeSubject and sends messages for displaying to affected operators.
  *
  */

#ifndef DUTYALARMSENDER_H_INCLUDED
#define DUTYALARMSENDER_H_INCLUDED

namespace TA_Base_App
{
	class DutyAlarmSender : public TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::FrontEndStateChangeSubjectImpl>,
		                    public TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl>
	{
	public:
		DutyAlarmSender();

		virtual void update( TA_Base_Bus::FrontEndStateChangeSubjectImpl& frontEndStateChange, const TA_Base_Bus::DutyState& state );
		virtual void update( TA_Base_Bus::AgentTerminateSubjectImpl&  terminateUpdate );

	private:

		typedef TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::FrontEndStateChangeSubjectImpl> FrontEndStateParent;
		typedef TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl>  AgentTerminateParent;

		//TD18093,jianghp
		//TA_Base_Core::NonUniqueAlarmHelper& m_alarmHelper;
	};
}
#endif // DUTYALARMSENDER_H_INCLUDED

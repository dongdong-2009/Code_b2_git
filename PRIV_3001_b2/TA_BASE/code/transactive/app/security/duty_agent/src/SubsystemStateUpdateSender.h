/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/security/duty_agent/src/SubsystemStateUpdateSender.h $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Listens to PeerStateChangeSubject and sends updates to Duty Managers.
  *
  */

#ifndef SUBSYSTEMSTATEUUPDATESENDER_H_INCLUDED
#define SUBSYSTEMSTATEUUPDATESENDER_H_INCLUDED

namespace TA_Base_App
{
	class SubsystemStateUpdateSender : public TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::PeerStateChangeSubjectImpl>,
		                            public TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl>
	{
	public:
		SubsystemStateUpdateSender();

		virtual void update( TA_Base_Bus::PeerStateChangeSubjectImpl& peerStateChange, const TA_Base_Bus::DutyState & state );
		virtual void update( TA_Base_Bus::AgentTerminateSubjectImpl&  terminateUpdate );

	private:

		typedef TA_Base_Bus::SeDutyStateObserver<TA_Base_Bus::PeerStateChangeSubjectImpl> PeerStateParent;
		typedef TA_Base_Bus::SeObserver<TA_Base_Bus::AgentTerminateSubjectImpl>  AgentTerminateParent;

		std::auto_ptr<TA_Base_Core::CommsMessageSender> m_commsSender;
	};
}
#endif // SUBSYSTEMSTATEUUPDATESENDER_H_INCLUDED

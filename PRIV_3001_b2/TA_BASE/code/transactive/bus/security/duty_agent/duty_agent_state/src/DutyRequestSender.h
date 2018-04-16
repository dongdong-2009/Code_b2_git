/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/DutyRequestSender.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  Broadcasts duty requests containing a unique id. The response is received via the
  *  DutyResponseSubject.
  */
#ifndef DUTYREQUESTSENDER_H_INCLUDED
#define DUTYREQUESTSENDER_H_INCLUDED

namespace TA_Base_Bus
{
	class DutyRequestSender : public SeObserver<DutyResponseSubjectImpl>
	{
	public:
		
		DutyRequestSender::DutyRequestSender();

		virtual void update( DutyResponseSubjectImpl& responseSubject );

		void send( const DutyPrimitive& session, DutyState& state, DASType::SubsystemMap& subsToApply, 
			DASType::SubsystemMap& subsToCancel );

	private:

		DutyRequestSender( const DutyRequestSender& );
		DutyRequestSender& operator=( const DutyRequestSender& );
		void waitForResponses();
		void processResponse( const std::string& responseId, const IDutyAgentRespCorbaDef::EDutyResponse& response );
	
		void processAccept ( DutyAgentTypeCorbaDef::DutyRequest& request );
		void processDeny   ( DutyAgentTypeCorbaDef::DutyRequest& request );
		void processTimeout( DutyAgentTypeCorbaDef::DutyRequest& request );

		void sendRequests();

		DutyRequestConverter::RequestMap m_outstandingRequests;

		TA_Base_Core::Condition m_responseFlag;
		//ACE_Thread_Semaphore m_responseFlag;
		ACE_Thread_Mutex     m_sendLock;

		DutyState* m_state;
		DASType::SubsystemMap* m_subsToApply;
		DASType::SubsystemMap* m_subsToCancel;

		std::auto_ptr<TA_Base_Core::CommsMessageSender> m_commsSender;

	};
}
#endif //  DUTYREQUESTSENDER_H_INCLUDED

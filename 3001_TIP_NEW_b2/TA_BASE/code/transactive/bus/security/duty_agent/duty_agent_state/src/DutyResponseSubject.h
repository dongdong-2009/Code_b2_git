#ifndef DUTYRESPONSERECEIVER_H_INCLUDED
#define DUTYRESPONSERECEIVER_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/DutyResponseSubject.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  Publishes responses to duty requests.
  */

namespace TA_Base_Bus
{
	class DutyResponseSubjectImpl : public SeSubject<DutyResponseSubjectImpl>
	{
	public:

		friend class ACE_Singleton<DutyResponseSubjectImpl, ACE_Recursive_Thread_Mutex>;

		void update( const std::string& uniqueId, const IDutyAgentRespCorbaDef::EDutyResponse& response );

		void getResponse( std::string& uniqueId, IDutyAgentRespCorbaDef::EDutyResponse& response ) const;

	private:

		DutyResponseSubjectImpl() : m_response(IDutyAgentRespCorbaDef::DR_DENY) {}
		~DutyResponseSubjectImpl(){}

		// Don't want to copy a Singleton
		DutyResponseSubjectImpl( const DutyResponseSubjectImpl& );
		DutyResponseSubjectImpl& operator=( const DutyResponseSubjectImpl& );

		std::string m_uniqueId;
		IDutyAgentRespCorbaDef::EDutyResponse m_response;
	};

	typedef ACE_Singleton<DutyResponseSubjectImpl, ACE_Recursive_Thread_Mutex> DutyResponseSubject;

}

#endif // DUTYRESPONSERECEIVER_H_INCLUDED

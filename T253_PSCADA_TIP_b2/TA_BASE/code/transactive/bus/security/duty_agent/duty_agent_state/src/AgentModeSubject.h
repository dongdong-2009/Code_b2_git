#ifndef AGENTMODESUBJECT_H_INCLUDED
#define AGENTMODESUBJECT_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/AgentModeSubject.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2017/10/31 16:21:54 $
  *  Last modified by:  $Author: CM $
  *
  *  Publishes the Agent's mode.
  */

namespace TA_Base_Bus
{
	class AgentModeSubjectImpl : public SeSubject<AgentModeSubjectImpl>
	{
	public:

		friend class ACE_Singleton<AgentModeSubjectImpl, ACE_Recursive_Thread_Mutex>;

		void update( const std::string& mode );

		std::string getMode() const { return(m_mode); }

	private:

		AgentModeSubjectImpl(){}
		~AgentModeSubjectImpl(){}

		// Don't want to copy a Singleton
		AgentModeSubjectImpl( const AgentModeSubjectImpl& );
		AgentModeSubjectImpl& operator=( const AgentModeSubjectImpl& );

		std::string m_mode;
	};

	typedef ACE_Singleton<AgentModeSubjectImpl, ACE_Recursive_Thread_Mutex> AgentModeSubject;

}

#endif // AGENTMODESUBJECT_H_INCLUDED

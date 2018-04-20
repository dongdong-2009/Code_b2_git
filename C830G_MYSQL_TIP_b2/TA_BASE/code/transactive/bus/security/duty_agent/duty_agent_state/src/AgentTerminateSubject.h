#ifndef AGENTTERMINATESUBJECT_H_INCLUDED
#define AGENTTERMINATESUBJECT_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/AgentTerminateSubject.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Publishes Agent termination.
  */

namespace TA_Base_Bus
{
	class AgentTerminateSubjectImpl : public SeSubject<AgentTerminateSubjectImpl>
	{
	public:

		friend class ACE_Singleton<AgentTerminateSubjectImpl, ACE_Recursive_Thread_Mutex>;

		void update();

	private:

		AgentTerminateSubjectImpl(){}
		~AgentTerminateSubjectImpl(){}

		// Don't want to copy a Singleton
		AgentTerminateSubjectImpl( const AgentTerminateSubjectImpl& );
		AgentTerminateSubjectImpl& operator=( const AgentTerminateSubjectImpl& );
	};

	typedef ACE_Singleton<AgentTerminateSubjectImpl, ACE_Recursive_Thread_Mutex> AgentTerminateSubject;

}

#endif // AGENTTERMINATESUBJECT_H_INCLUDED

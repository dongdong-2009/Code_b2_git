#ifndef STATEUPDATESUBJECT_H_INCLUDED
#define STATEUPDATESUBJECT_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/StateUpdateSubject.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2011/06/20 18:29:56 $
  *  Last modified by:  $Author: shurui.liu $
  *
  *  Publishes state changes to interested observers. These state changes come from
  *  the control duty agent.
  *
  */

namespace TA_Base_Bus
{
	class StateUpdateSubjectImpl : public SeDutyStateSubject<StateUpdateSubjectImpl>
	{
	public:

		friend class ACE_Singleton<StateUpdateSubjectImpl, ACE_Recursive_Thread_Mutex>;

		void update( const DutyState& state );

		// DutyState* getStateUpdate() const { return(m_state.ts_object()); }

	private:

		StateUpdateSubjectImpl()  {}
		~StateUpdateSubjectImpl(){}

		// Don't want to copy a Singleton
		StateUpdateSubjectImpl( const StateUpdateSubjectImpl& );
		StateUpdateSubjectImpl& operator=( const StateUpdateSubjectImpl& );

		
	};

	typedef ACE_Singleton<StateUpdateSubjectImpl, ACE_Recursive_Thread_Mutex> StateUpdateSubject;

}

#endif // STATEUPDATESUBJECT_H_INCLUDED

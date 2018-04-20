/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/SessionOvrBeginMO.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  The ACE Method Object that executes the SessionOverrideBegin method.
  */

#ifndef SESSIONOVRBEGINMO_H_INCLUDED
#define SESSIONOVRBEGINMO_H_INCLUDED

namespace TA_Base_Bus
{
	class DutyPeerTask;
}

namespace TA_Base_Bus
{
	//Class declaration
	class SessionOvrBeginMO : public DutyPeerTaskMO
	{
	// Public methods
	public:
		SessionOvrBeginMO( DutyPeerTask& task, const ACE_Future<bool>& future,
			DutyState& state, const SessionInfo& session );

	protected:

		virtual void execute();

	private:

		SessionOvrBeginMO& operator=( const SessionOvrBeginMO& );
		SessionOvrBeginMO( const SessionOvrBeginMO& );

		DutyPeerTask& m_task;
		DutyState& m_state;
		const SessionInfo& m_session;
	}; 
} // TA_Base_Bus

#endif //SESSIONOVRBEGINMO_H_INCLUDED

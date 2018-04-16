/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/SessionOvrBeginMO.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
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

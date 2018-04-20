/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/SessionEndMO.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  The ACE Method Object that executes the SessionEnd method.
  */

#ifndef SESSIONENDMO_H_INCLUDED
#define SESSIONENDMO_H_INCLUDED

namespace TA_Base_Bus
{
	class DutyPeerTask;
}

namespace TA_Base_Bus
{
	//Class declaration
	class SessionEndMO : public DutyPeerTaskMO
	{
	// Public methods
	public:
		SessionEndMO( DutyPeerTask& task, const ACE_Future<bool>& future,
			DutyState& state, const SessionInfo& session, const DASType::RegionSet& regions );

	protected:

		virtual void execute();

	private:

		SessionEndMO& operator=( const SessionEndMO& );
		SessionEndMO( const SessionEndMO& );

		DutyPeerTask& m_task;
		DutyState& m_state;
		const SessionInfo& m_session;
		const DASType::RegionSet& m_regions;
	}; 
} // TA_Base_Bus

#endif //SESSIONENDMO_H_INCLUDED

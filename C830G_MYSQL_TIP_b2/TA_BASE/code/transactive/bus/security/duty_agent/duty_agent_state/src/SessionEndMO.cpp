/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/SessionEndMO.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  The ACE Method Object that executes the SessionEnd method.
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	SessionEndMO::SessionEndMO( DutyPeerTask& task, const ACE_Future<bool>& future,
		DutyState& state, const SessionInfo& session, const DASType::RegionSet& regions ) : DutyPeerTaskMO(future),
		m_task(task),
		m_state(state),
		m_session(session),
		m_regions(regions)

	{
	}

	void SessionEndMO::execute( void )
	{
		m_task.sessionEndImpl( m_state, m_session, m_regions );
	}
} // TA_Base_Bus

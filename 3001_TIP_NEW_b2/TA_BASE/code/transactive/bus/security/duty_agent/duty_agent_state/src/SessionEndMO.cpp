/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/SessionEndMO.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/06/06 17:21:08 $
  *  Last modified by:  $Author: jinmin.huang $
  *
  *  The ACE Method Object that executes the SessionEnd method.
  */

#include "bus/security/duty_agent/duty_agent_state/src/DutyAgentState.h"

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

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/ExecuteQueryMO.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  Implementation of ExecuteQueryMO.
  */

#include "bus/security/duty_agent/duty_agent_state/src/DutyAgentState.h"

namespace TA_Base_Bus
{
	ExecuteQueryMO::ExecuteQueryMO( DutyPeerTask& task, const ACE_Future<bool>& future,
		DutyState& state, IDutyQuery& query ) : DutyPeerTaskMO(future),
		m_task(task),
		m_state(state),
		m_query(query)

	{
	}

	void ExecuteQueryMO::execute( void )
	{
		m_task.executeQueryImpl( m_state, m_query );
	}

} // TA_Base_Bus

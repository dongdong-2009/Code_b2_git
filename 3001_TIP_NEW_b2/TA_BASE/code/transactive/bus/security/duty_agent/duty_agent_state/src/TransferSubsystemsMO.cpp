/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/TransferSubsystemsMO.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/06/06 17:21:08 $
  *  Last modified by:  $Author: jinmin.huang $
  *
  *  Implementation of TransferSubsystemsMO.
  */

#include "bus/security/duty_agent/duty_agent_state/src/DutyAgentState.h"

namespace TA_Base_Bus
{
	TransferSubsystemsMO::TransferSubsystemsMO( DutyPeerTask& task, const ACE_Future<bool>& future,
		DutyState& state, const TA_Base_Bus::SessionInfo& session, const DASType::SubsystemMap& subsystems ) : DutyPeerTaskMO(future),
		m_task(task),
		m_state(state),
		m_session(session),
		m_subsystems(subsystems)

	{
	}

	void TransferSubsystemsMO::execute( void )
	{
		m_task.transferImpl( m_state, m_session, m_subsystems );
	}

} // TA_Base_Bus

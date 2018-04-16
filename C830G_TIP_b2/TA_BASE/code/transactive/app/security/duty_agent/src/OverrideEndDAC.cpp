/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/security/duty_agent/src/OverrideEndDAC.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * The "reallocate" command.
 */

#include "DutyAgent.h"
#include "OverrideEndDAC.h"

namespace TA_Base_App
{
	OverrideEndDAC::OverrideEndDAC( TA_Base_Bus::DutyState& state, TA_Base_Bus::DutyPeerTaskManager& taskManager,
		const TA_Base_Bus::SessionInfo& session, const TA_Base_Bus::DASType::LocationSet& locSet ) :
		IDutyAgentCommand(state),
		m_state(state),
		m_taskManager(taskManager),
		m_session(session),
		m_locSet(locSet)
	{
	}

	void OverrideEndDAC::execute()
	{
		m_taskManager.sessionOverrideEnd( m_state, m_session, m_locSet );
		TA_Base_Bus::DutySessionCache::instance()->sessionOverrideEnd( m_session );
	}
}

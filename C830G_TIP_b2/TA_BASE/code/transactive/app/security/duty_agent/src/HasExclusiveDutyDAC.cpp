/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File$
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 * The "hasDuty" command.
 */

#include "DutyAgent.h"
#include "HasExclusiveDutyDAC.h"

namespace TA_Base_App
{
	HasExclusiveDutyDAC::HasExclusiveDutyDAC( TA_Base_Bus::DutyState& state, TA_Base_Bus::DutyPeerTaskManager& taskManager,
		const TA_Base_Bus::SessionInfo& session,	const TA_Base_Bus::DASType::LocationSet& locSet ) : IDutyAgentCommand(state),
		m_state(state),
		m_taskManager(taskManager),
		m_session(session),
		m_locSet(locSet)
	{
	}

	void HasExclusiveDutyDAC::execute()
	{
		TA_Base_Bus::DutySessionCache::instance()->cacheSession( m_session );
		m_taskManager.getExclusiveDuty( m_state, m_session, m_locSet );
	}
}

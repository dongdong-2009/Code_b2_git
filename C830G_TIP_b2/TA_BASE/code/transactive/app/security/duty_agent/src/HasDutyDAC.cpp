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
#include "HasDutyDAC.h"

namespace TA_Base_App
{
	HasDutyDAC::HasDutyDAC( TA_Base_Bus::DutyState& state, TA_Base_Bus::DutyPeerTaskManager& taskManager,
		const TA_Base_Bus::SessionInfo& session,	const TA_Base_Bus::DASType::DbKey& region,
		const TA_Base_Bus::DASType::DbKey& subsystem ) : IDutyAgentCommand(state),
		m_state(state),
		m_taskManager(taskManager),
		m_session(session),
		m_region(region),
		m_subsystem(subsystem)
	{
	}

	void HasDutyDAC::execute()
	{
		TA_Base_Bus::DutySessionCache::instance()->cacheSession( m_session );
		m_taskManager.getDuty( m_state, m_session, m_region, m_subsystem );
	}
}

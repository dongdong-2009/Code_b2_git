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
 * The "take(locations)" command.
 */

#include "DutyAgent.h"
#include "TransferRegionDAC.h"

namespace TA_Base_App
{
	
	TransferRegionDAC::TransferRegionDAC( TA_Base_Bus::DutyState& state, TA_Base_Bus::DutyPeerTaskManager& taskManager,
		const TA_Base_Bus::SessionInfo& session, const TA_Base_Bus::DASType::RegionSet& regions ) : IDutyAgentCommand(state),
		m_state(state),
		m_taskManager(taskManager),
		m_session(session),
		m_regions(regions)
	{
	}

	void TransferRegionDAC::execute()
	{
		TA_Base_Bus::DutySessionCache::instance()->cacheSession( m_session );
		m_taskManager.transfer( m_state, m_session, m_regions );
	}
}

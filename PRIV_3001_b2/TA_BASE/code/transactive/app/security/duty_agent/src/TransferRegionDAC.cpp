/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/PRIV_3001/TA_BASE/transactive/app/security/duty_agent/src/TransferRegionDAC.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 *
 * The "take(locations)" command.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/TransferRegionDAC.h"

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

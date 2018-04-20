/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/security/duty_agent/src/DegradeDAC.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * The "allocate" command.
 */

#include "DutyAgent.h"
#include "DegradeDAC.h"

namespace TA_Base_App
{
	DegradeDAC::DegradeDAC( TA_Base_Bus::DutyState& state, TA_Base_Bus::DutyPeerTaskManager& taskManager,
		const TA_Base_Bus::DASType::SubsystemSet& subsystems,
		const TA_Base_Bus::DASType::LocationSet& locSet ) : IDutyAgentCommand(state),
		m_state(state),
		m_taskManager(taskManager),
		m_subsystems(subsystems),
		m_locSet(locSet)
	{
	}

	void DegradeDAC::execute()
	{
		m_taskManager.degrade( m_state, m_subsystems, m_locSet );
	}
}

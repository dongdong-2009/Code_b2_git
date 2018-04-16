/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/PRIV_3001/TA_BASE/transactive/app/security/duty_agent/src/GetStateDAC.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 *
 * The "getState" command.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/GetStateDAC.h"

namespace TA_Base_App
{
	
	GetStateDAC::GetStateDAC( TA_Base_Bus::DutyState& state, TA_Base_Bus::DutyPeerTaskManager& taskManager,
		const TA_Base_Bus::DASType::LocationSet& locSet ) : IDutyAgentCommand(state),
		m_state(state),
		m_taskManager(taskManager),
		m_locSet(locSet)
	{
	}

	void GetStateDAC::execute()
	{
		m_taskManager.getState( m_state, m_locSet );
	}
}

#ifndef HASEXCLUSIVEDUTYDAC_H_INCLUDED
#define HASEXCLUSIVEDUTYDAC_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/PRIV_3001/TA_BASE/transactive/app/security/duty_agent/src/HasExclusiveDutyDAC.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 *
 * The "hasDuty" command.
 */

#include "app/security/duty_agent/src/IDutyAgentCommand.h"

namespace TA_Base_App
{
    class HasExclusiveDutyDAC : public TA_Base_App::IDutyAgentCommand
    {
    public:
	
		HasExclusiveDutyDAC( TA_Base_Bus::DutyState& state, TA_Base_Bus::DutyPeerTaskManager& taskManager,
			const TA_Base_Bus::SessionInfo& session, const TA_Base_Bus::DASType::LocationSet& locSet );

		virtual void execute();

	private:

		TA_Base_Bus::DutyState&                  m_state;
		TA_Base_Bus::DutyPeerTaskManager&        m_taskManager;
		const TA_Base_Bus::SessionInfo&                  m_session;
		const TA_Base_Bus::DASType::LocationSet& m_locSet;
    };
}

#endif // HASEXCLUSIVEDUTYDAC_H_INCLUDED

#ifndef GETTRANSFERABLESUBSYSTEMSDAC_H_INCLUDED
#define GETTRANSFERABLESUBSYSTEMSDAC_H_INCLUDED

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
 * The "getTransferableSubsystems" command.
 */

#include "IDutyAgentCommand.h"

namespace TA_Base_App
{
    class GetTransferableSubsystemsDAC : public IDutyAgentCommand
    {
    public:
	
		GetTransferableSubsystemsDAC( TA_Base_Bus::DutyState& state, TA_Base_Bus::DutyPeerTaskManager& taskManager,
			const TA_Base_Bus::SessionInfo& session,	const TA_Base_Bus::DASType::RegionSet& regSet );		

		virtual void execute();

	private:

		TA_Base_Bus::DutyState&                m_state;
		TA_Base_Bus::DutyPeerTaskManager&      m_taskManager;
		const TA_Base_Bus::SessionInfo&                m_session;
		const TA_Base_Bus::DASType::RegionSet& m_regSet;
    };
}

#endif // GETTRANSFERABLESUBSYSTEMSDAC_H_INCLUDED

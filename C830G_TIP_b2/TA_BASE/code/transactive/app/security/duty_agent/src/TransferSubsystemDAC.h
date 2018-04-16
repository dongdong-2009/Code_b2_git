#ifndef TRANSFERSUBSYSTEMDAC_H_INCLUDED
#define TRANSFERSUBSYSTEMDAC_H_INCLUDED

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
 * The "take(subsystems)" command.
 */

#include "IDutyAgentCommand.h"

namespace TA_Base_App
{
    class TransferSubsystemDAC : public IDutyAgentCommand
    {
    public:
	
		TransferSubsystemDAC( TA_Base_Bus::DutyState& state, TA_Base_Bus::DutyPeerTaskManager& taskManager,
			const TA_Base_Bus::SessionInfo& session, const TA_Base_Bus::DASType::SubsystemMap& subsystems );		

		virtual void execute();

	protected:
		virtual unsigned long getActionKey() const{ return(TA_Base_Bus::aca_DUTY_TRANSFER_SUBSYSTEM); }
		
	private:

		TA_Base_Bus::DutyState& m_state;
		TA_Base_Bus::DutyPeerTaskManager& m_taskManager;
		const TA_Base_Bus::SessionInfo& m_session;
		const TA_Base_Bus::DASType::SubsystemMap& m_subsystems;
    };
}

#endif // TRANSFERSUBSYSTEMDAC_H_INCLUDED

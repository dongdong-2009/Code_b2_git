#ifndef DEGRADEDAC_H_INCLUDED
#define DEGRADEDAC_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/security/duty_agent/src/DegradeDAC.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #3 $
 *
 * Last modification: $DateTime: 2012/06/06 17:21:08 $
 * Last modified by:  $Author: jinmin.huang $
 *
 * The "delegate" command.
 */

#include "app/security/duty_agent/src/IDutyAgentCommand.h"

namespace TA_Base_App
{
    class DegradeDAC : public IDutyAgentCommand
    {
    public:
	
		DegradeDAC( TA_Base_Bus::DutyState& state, TA_Base_Bus::DutyPeerTaskManager& taskManager, 
			const TA_Base_Bus::DASType::SubsystemSet& subsystems, const TA_Base_Bus::DASType::LocationSet& locSet );		

		virtual void execute();

	protected:
		virtual unsigned long getActionKey() const{ return(TA_Base_Bus::aca_DUTY_DEGRADE_SUBSYSTEMS); }
	private:
		DegradeDAC(const DegradeDAC& rhs);
		
	private:

		TA_Base_Bus::DutyState&                   m_state;
		TA_Base_Bus::DutyPeerTaskManager&         m_taskManager;
		const TA_Base_Bus::DASType::SubsystemSet& m_subsystems;
		const TA_Base_Bus::DASType::LocationSet&  m_locSet;
    };
}

#endif // DEGRADEDAC_H_INCLUDED

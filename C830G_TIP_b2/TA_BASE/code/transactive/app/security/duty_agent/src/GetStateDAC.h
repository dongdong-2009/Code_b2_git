#ifndef GETSTATEDAC_H_INCLUDED
#define GETSTATEDAC_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/security/duty_agent/src/GetStateDAC.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * The "getState" command.
 */

#include "IDutyAgentCommand.h"

namespace TA_Base_App
{
    class GetStateDAC : public IDutyAgentCommand
    {
    public:
	
		GetStateDAC( TA_Base_Bus::DutyState& state, TA_Base_Bus::DutyPeerTaskManager& taskManager,
			const TA_Base_Bus::DASType::LocationSet& locSet );

		virtual void execute();

	private:

		TA_Base_Bus::DutyState&                  m_state;
		TA_Base_Bus::DutyPeerTaskManager&        m_taskManager;
		const TA_Base_Bus::DASType::LocationSet& m_locSet;
    };
}

#endif // GETSTATEDAC_H_INCLUDED
